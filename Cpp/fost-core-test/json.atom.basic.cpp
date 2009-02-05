/*
    Copyright 2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core-test.hpp"


FSL_TEST_SUITE( json_atom );


FSL_TEST_FUNCTION( constructors ) {
    fostlib::test::default_copy_constructable< fostlib::json >();
    fostlib::json v0, v1( true ), v2( 10 ), v3( L"true" ), v4( L"10" ), v5( L"0" ), v6( L"Hello world!" ), v7( float( 1.0 ) ), v8( double( 3.141 ) );
}


FSL_TEST_FUNCTION( isnull ) {
    FSL_CHECK( fostlib::json().isnull() );
    FSL_CHECK( !fostlib::json( true ).isnull() );
    FSL_CHECK( !fostlib::json( 10 ).isnull() );
    FSL_CHECK( !fostlib::json( L"true" ).isnull() );
    FSL_CHECK( !fostlib::json( L"10" ).isnull() );
    FSL_CHECK( !fostlib::json( L"0" ).isnull() );
    FSL_CHECK( !fostlib::json( L"Hello world!" ).isnull() );
    FSL_CHECK( !fostlib::json( double( 3.14 ) ).isnull() );
}


FSL_TEST_FUNCTION( isatom ) {
    FSL_CHECK( !fostlib::json().isatom() );
    FSL_CHECK( fostlib::json( true ).isatom() );
    FSL_CHECK( fostlib::json( 10 ).isatom() );
    FSL_CHECK( fostlib::json( L"true" ).isatom() );
    FSL_CHECK( fostlib::json( L"10" ).isatom() );
    FSL_CHECK( fostlib::json( L"0" ).isatom() );
    FSL_CHECK( fostlib::json( L"Hello world!" ).isatom() );
    FSL_CHECK( fostlib::json( double( 3.14 ) ).isatom() );
}


FSL_TEST_FUNCTION( isarray ) {
    FSL_CHECK( !fostlib::json().isarray() );
    FSL_CHECK( !fostlib::json( true ).isarray() );
    FSL_CHECK( !fostlib::json( 10 ).isarray() );
    FSL_CHECK( !fostlib::json( L"true" ).isarray() );
    FSL_CHECK( !fostlib::json( L"10" ).isarray() );
    FSL_CHECK( !fostlib::json( L"0" ).isarray() );
    FSL_CHECK( !fostlib::json( L"Hello world!" ).isarray() );
    FSL_CHECK( !fostlib::json( double( 3.14 ) ).isarray() );
}


FSL_TEST_FUNCTION( isobject ) {
    FSL_CHECK( !fostlib::json().isobject() );
    FSL_CHECK( !fostlib::json( true ).isobject() );
    FSL_CHECK( !fostlib::json( 10 ).isobject() );
    FSL_CHECK( !fostlib::json( L"true" ).isobject() );
    FSL_CHECK( !fostlib::json( L"10" ).isobject() );
    FSL_CHECK( !fostlib::json( L"0" ).isobject() );
    FSL_CHECK( !fostlib::json( L"Hello world!" ).isobject() );
    FSL_CHECK( !fostlib::json( double( 3.14 ) ).isobject() );
}


FSL_TEST_FUNCTION( as_bool ) {
 //   FSL_CHECK_EXCEPTION( fostlib::json().as_bool(), FSLib::Exceptions::Null& );
    //FSL_CHECK( fostlib::json( true ).as_bool() );
    //FSL_CHECK( fostlib::json( 10 ).as_bool() );
    //FSL_CHECK( !fostlib::json( L"true" ).as_bool() );
    //FSL_CHECK( fostlib::json( L"10" ).as_bool() );
    //FSL_CHECK( !fostlib::json( L"0" ).as_bool() );
    //FSL_CHECK( !fostlib::json( L"Hello world!" ).as_bool() );
    //FSL_CHECK( !fostlib::json( float( 1.0 ) ).as_bool() );
    //FSL_CHECK( !fostlib::json( double( 3.14 ) ).as_bool() );

 //   FSL_CHECK( ( fostlib::json() = true ).as_bool() );
 //   FSL_CHECK( !( fostlib::json() = false ).as_bool() );
 //   FSL_CHECK( !( fostlib::json() = L"something" ).as_bool() );
}


FSL_TEST_FUNCTION( as_int64_t ) {
    //fostlib::json v0, v1( true ), v2( 10 ), v3( L"true" ), v4( L"10" ), v5( L"0" ), v6( L"Hello world!" ), v7( float( 1.0 ) ), v8( double( 3.141 ) );

    //FSL_CHECK_EXCEPTION( v0.as_int64_t(), FSLib::Exceptions::Null& );
    //FSL_CHECK( v1.as_int64_t() == 1 );
    //FSL_CHECK( v2.as_int64_t() == 10 );
    //FSL_CHECK_EXCEPTION( v3.as_int64_t(), FSLib::Exceptions::NotANumber& );
    //FSL_CHECK( v4.as_int64_t() == 10 );
    //FSL_CHECK( v5.as_int64_t() == 0 );
    //FSL_CHECK_EXCEPTION( v6.as_int64_t(), FSLib::Exceptions::NotANumber& );
    //FSL_CHECK( v7.as_int64_t() == 1 );
    //FSL_CHECK( v8.as_int64_t() == 3 );
}


FSL_TEST_FUNCTION( as_wstring ) {
    //fostlib::json v0, v1( true ), v2( 10 ), v3( L"true" ), v4( L"10" ), v5( L"0" ), v6( L"Hello world!" ), v7( float( 1.0 ) ), v8( double( 3.141 ) ), v9( "[Hello]" );

    //FSL_CHECK_EXCEPTION( v0.as_wstring(), FSLib::Exceptions::Null& );
    //FSL_CHECK( v1.as_wstring() == L"1" );
    //FSL_CHECK( v2.as_wstring() == L"10" );
    //FSL_CHECK( v3.as_wstring() == L"true" );
    //FSL_CHECK( v4.as_wstring() == L"10" );
    //FSL_CHECK( v5.as_wstring() == L"0" );
    //FSL_CHECK( v6.as_wstring() == L"Hello world!" );
    //FSL_CHECK( v7.as_wstring() == towstring( float( 1.0 ) ) );
    //FSL_CHECK( v8.as_wstring() == towstring( double( 3.141 ) ) );
    //FSL_CHECK( v9.as_wstring() == L"[Hello]" );
}


FSL_TEST_FUNCTION( equality ) {
    fostlib::json v0, v1( true ), v2( 10 ), v3( L"true" ), v4( L"10" ), v5( L"0" ), v6( L"Hello world!" ), v7( float( 1.0 ) ), v8( double( 3.141 ) );

    FSL_CHECK_EQ( v0, v0 );
    FSL_CHECK_EQ( v1, v1 );
    FSL_CHECK_EQ( v2, v2 );
    FSL_CHECK_EQ( v3, v3 );
    FSL_CHECK_EQ( v4, v4 );
    FSL_CHECK_EQ( v5, v5 );
    FSL_CHECK_EQ( v6, v6 );
    FSL_CHECK_EQ( v7, v7 );
    FSL_CHECK_EQ( v8, v8 );

    FSL_CHECK_NEQ( v0, v1 );
    FSL_CHECK_NEQ( v0, v2 );
    FSL_CHECK_NEQ( v0, v3 );
    FSL_CHECK_NEQ( v0, v4 );
    FSL_CHECK_NEQ( v0, v5 );
    FSL_CHECK_NEQ( v0, v6 );
    FSL_CHECK_NEQ( v0, v7 );
    FSL_CHECK_NEQ( v0, v8 );

    FSL_CHECK_NEQ( v1, v0 );
    FSL_CHECK_NEQ( v1, v2 );
    FSL_CHECK_NEQ( v1, v3 );
    FSL_CHECK_NEQ( v1, v4 );
    FSL_CHECK_NEQ( v1, v5 );
    FSL_CHECK_NEQ( v1, v6 );
    FSL_CHECK_NEQ( v1, v7 );
    FSL_CHECK_NEQ( v1, v8 );

    FSL_CHECK_NEQ( v2, v0 );
    FSL_CHECK_NEQ( v2, v1 );
    FSL_CHECK_NEQ( v2, v3 );
    FSL_CHECK_NEQ( v2, v4 );
    FSL_CHECK_NEQ( v2, v5 );
    FSL_CHECK_NEQ( v2, v6 );
    FSL_CHECK_NEQ( v2, v7 );
    FSL_CHECK_NEQ( v2, v8 );

    FSL_CHECK_NEQ( v3, v0 );
    FSL_CHECK_NEQ( v3, v1 );
    FSL_CHECK_NEQ( v3, v2 );
    FSL_CHECK_NEQ( v3, v4 );
    FSL_CHECK_NEQ( v3, v5 );
    FSL_CHECK_NEQ( v3, v6 );
    FSL_CHECK_NEQ( v3, v7 );
    FSL_CHECK_NEQ( v3, v8 );

    FSL_CHECK_NEQ( v4, v0 );
    FSL_CHECK_NEQ( v4, v1 );
    FSL_CHECK_NEQ( v4, v2 );
    FSL_CHECK_NEQ( v4, v3 );
    FSL_CHECK_NEQ( v4, v5 );
    FSL_CHECK_NEQ( v4, v6 );
    FSL_CHECK_NEQ( v4, v7 );
    FSL_CHECK_NEQ( v4, v8 );

    FSL_CHECK_NEQ( v5, v0 );
    FSL_CHECK_NEQ( v5, v1 );
    FSL_CHECK_NEQ( v5, v2 );
    FSL_CHECK_NEQ( v5, v3 );
    FSL_CHECK_NEQ( v5, v4 );
    FSL_CHECK_NEQ( v5, v6 );
    FSL_CHECK_NEQ( v5, v7 );
    FSL_CHECK_NEQ( v5, v8 );

    FSL_CHECK_NEQ( v6, v0 );
    FSL_CHECK_NEQ( v6, v1 );
    FSL_CHECK_NEQ( v6, v2 );
    FSL_CHECK_NEQ( v6, v3 );
    FSL_CHECK_NEQ( v6, v4 );
    FSL_CHECK_NEQ( v6, v5 );
    FSL_CHECK_NEQ( v6, v7 );
    FSL_CHECK_NEQ( v6, v8 );

    FSL_CHECK_NEQ( v7, v0 );
    FSL_CHECK_NEQ( v7, v1 );
    FSL_CHECK_NEQ( v7, v2 );
    FSL_CHECK_NEQ( v7, v3 );
    FSL_CHECK_NEQ( v7, v4 );
    FSL_CHECK_NEQ( v7, v5 );
    FSL_CHECK_NEQ( v7, v6 );
    FSL_CHECK_NEQ( v7, v8 );

    FSL_CHECK_NEQ( v8, v0 );
    FSL_CHECK_NEQ( v8, v1 );
    FSL_CHECK_NEQ( v8, v2 );
    FSL_CHECK_NEQ( v8, v3 );
    FSL_CHECK_NEQ( v8, v4 );
    FSL_CHECK_NEQ( v8, v5 );
    FSL_CHECK_NEQ( v8, v6 );
    FSL_CHECK_NEQ( v8, v7 );
}


FSL_TEST_FUNCTION( assignment ) {
    fostlib::json v, v0, v1( true ), v2( 10 ), v3( L"true" ), v4( L"10" ), v5( L"0" ), v6( L"Hello world!" ), v7( float( 1.0 ) ), v8( double( 3.141 ) );

    v = v0; FSL_CHECK_EQ( v, v0 );
    v = v1; FSL_CHECK_EQ( v, v1 );
    v = v2; FSL_CHECK_EQ( v, v2 );
    v = v3; FSL_CHECK_EQ( v, v3 );
    v = v4; FSL_CHECK_EQ( v, v4 );
    v = v5; FSL_CHECK_EQ( v, v5 );
    v = v6; FSL_CHECK_EQ( v, v6 );
    v = v7; FSL_CHECK_EQ( v, v7 );
    v = v8; FSL_CHECK_EQ( v, v8 );
}


FSL_TEST_FUNCTION( size ) {
    fostlib::json v0, v1( true ), v2( 10 ), v3( L"true" ), v4( L"10" ), v5( L"0" ), v6( L"Hello world!" ), v7( float( 1.0 ) ), v8( double( 3.141 ) );

    FSL_CHECK_EQ( v0.size(), 0 );
    FSL_CHECK_EQ( v1.size(), 1 );
    FSL_CHECK_EQ( v2.size(), 1 );
    FSL_CHECK_EQ( v3.size(), 1 );
    FSL_CHECK_EQ( v4.size(), 1 );
    FSL_CHECK_EQ( v5.size(), 1 );
    FSL_CHECK_EQ( v6.size(), 1 );
    FSL_CHECK_EQ( v7.size(), 1 );
    FSL_CHECK_EQ( v8.size(), 1 );
}
