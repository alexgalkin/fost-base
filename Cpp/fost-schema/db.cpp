/*
    Copyright 1999-2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema.hpp"
#include <fost/db.hpp>
#include <fost/thread.hpp>
#include <fost/exception/unexpected_eof.hpp>


using namespace fostlib;


namespace {


    const setting< fostlib::string > c_defaultDriver( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"Default driver", L"ado", true );

#ifdef _DEBUG
#define LOGGING true
#else
#define LOGGING false
#endif
    const setting< bool > c_logConnect( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"LogConnect", LOGGING, true );
    const setting< bool > c_logRead( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"LogRead", LOGGING, true );
    const setting< bool > c_logWrite( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"LogWrite", LOGGING, true );
    const setting< bool > c_logFailure( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"LogFailure", true, true );
#undef LOGGING

    const setting< long > c_countCommandTimeout( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"CountCommandTimeout", 120, true );
    const setting< long > c_readCommandTimeout( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"ReadCommandTimeout", 3600, true );
    const setting< long > c_writeCommandTimeout( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"WriteCommandTimeout", 15, true );


    library< const dbinterface * > &g_interfaces() {
        static library< const dbinterface * > interfaces;
        return interfaces;
    }

    fostlib::string driver_name( const fostlib::string &d ) {
        string p( partition( d, L";" ).first );
        string::size_type s( p.find( L'/' ) );
        if ( s == string::npos )
            return string();
        else
            return string( p, 0, s );
    }
    fostlib::string dsn( const fostlib::string &d ) {
        fostlib::string n( driver_name( d ) );
        if ( n.empty() )
            return d;
        else
            return d.substr( n.length() + 1 );
    }
    fostlib::string driver( const fostlib::string &read, const fostlib::nullable< fostlib::string > &write ) {
        fostlib::string rd( driver_name( read ) ), wd( driver_name( write.value( read ) ) );
        if ( rd != wd )
            throw fostlib::exceptions::data_driver( L"Read and write drivers not the same", rd, wd );
        if ( rd.empty() )
            return c_defaultDriver.value();
        else
            return rd;
    }


}


fostlib::exceptions::data_driver::data_driver( const string&m, const string &d ) throw ()
: exception( m ) {
    m_info << L"Driver: " << d << std::endl;
}
fostlib::exceptions::data_driver::data_driver( const string &m, const string &d1, const string &d2 ) throw ()
: exception( m ) {
    m_info << L"Driver 1: " << d1 << L"\nDriver 2: " << d2 << std::endl;
}
wliteral const fostlib::exceptions::data_driver::message() const throw () {
    return L"Problem with database driver.";
}


fostlib::exceptions::transaction_fault::transaction_fault( const string &error ) throw ()
: exception( error ) {
}
wliteral const fostlib::exceptions::transaction_fault::message() const throw () {
    return L"Fault in transaction handling.";
}


/*
    DBInterface
*/


const dbinterface &fostlib::dbinterface::connection( const string&read, const nullable< string > &write ) {
    if ( g_interfaces().find( driver( read, write ) ).empty() )
        throw exceptions::data_driver( L"No driver found", driver( read, write ) );
    return **g_interfaces().find( driver( read, write ) ).begin();
}


fostlib::dbinterface::dbinterface( const string &name ) {
    g_interfaces().add( name, this );
}


fostlib::dbinterface::read::read( dbconnection &d )
: m_connection( d ) {
}


fostlib::dbinterface::read::~read() {
}


fostlib::dbinterface::write::write( dbinterface::read &r )
: m_connection( r.m_connection ), m_reader( r ) {
}


fostlib::dbinterface::write::~write() {
}


fostlib::dbinterface::recordset::recordset( const dbinterface &dbi, const string &c )
: command( c ) {
}


fostlib::dbinterface::recordset::~recordset() {
}


/*
    DBConnection
*/


const setting< bool > fostlib::dbconnection::c_commitCount( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"Commit count", true, true );
const setting< long > fostlib::dbconnection::c_commitCountDomain( L"/fost-base/Cpp/fost-schema/db.cpp", L"Database", L"Commit count id", 9, true );


fostlib::dbconnection::dbconnection( const fostlib::string &r )
: readDSN( dsn( r ) ), m_readOnly( true ),
        m_interface( dbinterface::connection( r, null ) ), m_transaction( NULL ) {
    m_connection = m_interface.reader( *this );
}


fostlib::dbconnection::dbconnection( const fostlib::string &r, const fostlib::string &w )
: readDSN( dsn( r ) ), writeDSN( dsn( w ) ), m_readOnly( false ),
        m_interface( dbinterface::connection( r, w ) ), m_transaction( NULL ) {
    m_connection = m_interface.reader( *this );
}


fostlib::dbconnection::~dbconnection()
try{
} catch ( ... ) {
    try {
        if ( setting< bool >::value( L"Database", L"LogFailure" ) ) {
            /*YAML::Record failure;
            failure.add( L"DB", L"Failure" );
            failure.add( L"Place", L"DBConnection::~DBConnection()" );
            failure.add( L"Exception", L"Unknown exception" );
            failure.add( L"Connection", L"Read" );
            failure.log();*/
        }
    } catch ( exceptions::exception & ) {
        absorbException();
    }
}


void fostlib::dbconnection::create_database( const fostlib::string &name ) {
    if ( writeDSN().isnull() )
        throw exceptions::transaction_fault( L"Cannot create database without a write connection" );
    m_interface.create_database( *this, name );
}
void fostlib::dbconnection::drop_database( const fostlib::string &name ) {
    if ( writeDSN().isnull() )
        throw exceptions::transaction_fault( L"Cannot drop database without a write connection" );
    m_interface.drop_database( *this, name );
}


const dbinterface &fostlib::dbconnection::interface() const {
    return m_interface;
}


recordset fostlib::dbconnection::query( const string &command ) {
    if ( !m_connection )
        throw exceptions::transaction_fault( L"Database connection has not started a read transaction" );
    return m_connection->query( command );
}


bool fostlib::dbconnection::in_transaction() const {
    return m_transaction != NULL;
}


dbtransaction &fostlib::dbconnection::transaction() {
    return *m_transaction;
}


/*
    Transaction
*/


fostlib::dbtransaction::dbtransaction( dbconnection &dbc )
: m_connection( dbc ) {
    if ( m_connection.in_transaction() )
        throw exceptions::transaction_fault( L"Nested transaction not yet supported" );
    m_transaction = dbc.m_connection->writer();
    m_connection.m_transaction = this;
}


fostlib::dbtransaction::~dbtransaction() {
    if ( m_transaction )
        m_transaction->rollback();
    if ( m_connection.m_transaction != this )
        throw exceptions::transaction_fault( L"The current transaction has changed" );
    m_connection.m_transaction = NULL;
}


void fostlib::dbtransaction::create_table( const string &table, const std::list< std::pair< string, string > > &key, const std::list< std::pair< string, string > > &columns ) {
    if ( !m_transaction )
        throw exceptions::transaction_fault( L"This transaction has already been used" );
    m_transaction->create_table( table, key, columns );
}


void fostlib::dbtransaction::drop_table( const fostlib::string &table ) {
    if ( !m_transaction )
        throw exceptions::transaction_fault( L"This transaction has already been used" );
    m_transaction->drop_table( table );
}


dbtransaction &fostlib::dbtransaction::execute( const fostlib::string &c ) {
    if ( !m_transaction )
        throw exceptions::transaction_fault( L"This transaction has already been used" );
    m_transaction->execute( c );
    return *this;
}


void fostlib::dbtransaction::commit() {
    m_transaction->commit();
    m_transaction = boost::shared_ptr< dbinterface::write >();
}


/*
    Recordset
*/


fostlib::recordset::recordset( boost::shared_ptr< dbinterface::recordset > rs )
: m_interface( rs ) {
}


fostlib::recordset::~recordset()
try {
} catch ( ... ) {
    absorbException();
}


bool fostlib::recordset::eof() const {
    return m_interface->eof();
}


bool fostlib::recordset::isnull( const fostlib::string &i ) const {
    return field( i ).isnull();
}


bool fostlib::recordset::isnull( std::size_t i ) const {
    return field( i ).isnull();
}


void fostlib::recordset::moveNext() {
    m_interface->moveNext();
}


std::size_t fostlib::recordset::fields() const {
    return m_interface->fields();
}


const string &fostlib::recordset::name( std::size_t i ) const {
    return m_interface->name( i );
}


const json &fostlib::recordset::field( std::size_t i ) const {
    if ( m_interface.get() )
        return m_interface->field( i );
    else
        throw exceptions::unexpected_eof( L"The recordset came from a write SQL instruction" );
}


const json &fostlib::recordset::field( const string &name ) const {
    if ( m_interface.get() )
        return m_interface->field( name );
    else
        throw exceptions::unexpected_eof( L"The recordset came from a write SQL instruction" );
}
