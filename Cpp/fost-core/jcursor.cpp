/*
    Copyright 2007-2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core.hpp"
#include <fost/json.hpp>
#include <fost/string/coerce.hpp>

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
    throw exceptions::not_implemented( L"jcursor::operator[]( const json &j ) const" );
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
