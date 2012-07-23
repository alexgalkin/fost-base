/*
    Copyright 2008-2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/datetime>
#include <fost/test>


FSL_TEST_SUITE( timestamps );


FSL_TEST_FUNCTION( constructors ) {
    fostlib::test::default_copy_constructable< fostlib::timestamp >();
}


FSL_TEST_FUNCTION( basic ) {
    fostlib::timestamp n(fostlib::timestamp::now());
    fostlib::string iso(fostlib::coerce< fostlib::string >(n));
    FSL_CHECK_EQ(iso[0], '2');
    FSL_CHECK_EQ(iso[1], '0');
    FSL_CHECK_EQ(iso[4], '-');

    FSL_CHECK_NOTHROW(fostlib::rfc1123_timestamp rfc1123(
        fostlib::coerce< fostlib::rfc1123_timestamp >(n)));
}


FSL_TEST_FUNCTION( json ) {
    fostlib::timestamp n(fostlib::timestamp(2012, 10, 4, 13, 45));
    fostlib::json j( fostlib::coerce< fostlib::json >( n ) );
    FSL_CHECK(!j.get< fostlib::string >().isnull());
    FSL_CHECK_EQ( j, fostlib::json("2012-10-04T13:45:00Z") );
    FSL_CHECK_EQ( fostlib::coerce< fostlib::timestamp >( j ), n );
}


FSL_TEST_FUNCTION( compare ) {
    fostlib::timestamp s(2012, 10, 4, 13, 45), l(2012, 11, 4, 13, 45);
    FSL_CHECK(s < l);
    FSL_CHECK(l > s);
    FSL_CHECK(s <= l);
    FSL_CHECK(l >= s);
    FSL_CHECK(s <= s);
    FSL_CHECK(s >= s);
}


FSL_TEST_FUNCTION( addition ) {
    fostlib::timestamp ts(2012, 2, 28, 13, 45);
    ts += fostlib::timediff(3, 4, 34);
    FSL_CHECK_EQ(ts, fostlib::timestamp(2012, 2, 28, 16, 49, 34));

    fostlib::timestamp ts2 = ts + fostlib::timediff(15, 10, 4);
    FSL_CHECK_EQ(ts2, fostlib::timestamp(2012, 2, 29, 7, 59, 38));
}


FSL_TEST_FUNCTION( subtraction ) {
    fostlib::timestamp ts(2012, 2, 29, 7, 59, 38);
    ts -= fostlib::timediff(15, 10, 4);
    FSL_CHECK_EQ(ts, fostlib::timestamp(2012, 2, 28, 16, 49, 34));

    fostlib::timestamp ts2 = ts - fostlib::timediff(3, 4, 34);
    FSL_CHECK_EQ(ts2, fostlib::timestamp(2012, 2, 28, 13, 45));
}
