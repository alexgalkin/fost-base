 /*
    Copyright 2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core-test.hpp"


using namespace fostlib;


FSL_TEST_SUITE( json_coerce );


FSL_TEST_FUNCTION( nullable ) {
    FSL_CHECK_EQ(
        coerce<json>(nullable<string>()), json());
    FSL_CHECK_EQ(
        coerce<json>(nullable<string>("t")), json("t"));
}