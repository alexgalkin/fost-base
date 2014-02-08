/*
    Copyright 2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core-test.hpp"

#include <fost/dynlib>


FSL_TEST_SUITE( dynlib );


FSL_TEST_FUNCTION(datetime) {
    FSL_CHECK_NOTHROW(fostlib::dynlib("fost-datetime"));
}
