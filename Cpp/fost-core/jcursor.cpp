/*
    Copyright 2007-2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core.hpp"
#include <fost/json.hpp>
#include <fost/string/coerce.hpp>

#include <fost/exception/json_error.hpp>
#include <fost/exception/not_implemented.hpp>
#include <fost/exception/null.hpp>


using namespace fostlib;


fostlib::jcursor::jcursor() {
}

fostlib::jcursor::jcursor( stack_t::const_iterator b, stack_t::const_iterator e )
: m_position( b, e ) {
}

jcursor jcursor::operator[]( uint64_t i ) const {
    jcursor p( *this );
    p.m_position.push_back( i );
    return p;
}
jcursor jcursor::operator[]( const string &i ) const {
    jcursor p( *this );
    p.m_position.push_back( i );
    return p;
}
jcursor jcursor::operator[]( const json &j ) const {
    nullable< int64_t > i = j.get< int64_t >();
    if ( !i.isnull() )
        return (*this)[ i.value() ];
    nullable< string > s = j.get< string >();
    if ( !s.isnull() )
        return (*this)[ s.value() ];
    throw exceptions::not_implemented(
        L"jcursor::operator[]( const json &j ) const -- for non string or integer",
        json::unparse( j )
    );
}

jcursor &jcursor::enter() {
    m_position.push_back( 0 );
    return *this;
}
jcursor &jcursor::enter( const string &i ) {
    m_position.push_back( i );
    return *this;
}
jcursor &jcursor::pop() {
    m_position.pop_back();
    return *this;
}

jcursor &jcursor::operator ++() {
    if ( m_position.empty() )
        throw fostlib::exceptions::null( L"cannot increment an empty jcursor" );
    else if ( boost::get< int64_t >( &*m_position.rbegin() ) == NULL )
        throw fostlib::exceptions::null( L"the current jcursor isn't into an array position" );
    else
        ++boost::get< int64_t >( *m_position.rbegin() );
    return *this;
}


namespace {
    struct take_step : public boost::static_visitor< json * > {
        json *j;
        take_step( json *j ) : j( j ) {}

        json *operator()( int64_t k ) const {
            throw fostlib::exceptions::not_implemented( L"jcursor::operator() ( json &j ) const -- dereference of integer" );
        }
        json *operator()( const string &k ) const {
            if ( j->isnull() )
                *j = json::object_t();
            if ( !j->isobject() )
                throw fostlib::exceptions::json_error(
                    L"Cannot walk through a JSON array/value which is not an object using a string key", *j
                );
            if ( !j->has_key( k ) )
                j->insert( k, json() );
            return const_cast< json * >( &(*j)[ k ] );
        }
    };
}
json &jcursor::operator() ( json &j ) const {
    json *loc = &j;
    for ( stack_t::const_iterator p( m_position.begin() ); p != m_position.end(); ++p ) {
        loc = boost::apply_visitor( take_step( loc ), *p );
    }
    return *loc;
}
