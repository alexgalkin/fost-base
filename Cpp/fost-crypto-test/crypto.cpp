/*
    Copyright 2008-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-crypto-test.hpp"
#include <fost/crypto>


FSL_TEST_SUITE( crypto );


FSL_TEST_FUNCTION( sha1 ) {
    // http://en.wikipedia.org/wiki/Examples_of_SHA_digests
    FSL_CHECK_EQ( fostlib::sha1( fostlib::string() ), L"da39a3ee5e6b4b0d3255bfef95601890afd80709" );
    FSL_CHECK_EQ( fostlib::sha1( L"The quick brown fox jumps over the lazy dog" ), L"2fd4e1c67a2d28fced849ee1bb76e7391b93eb12" );
    FSL_CHECK_EQ( fostlib::sha1( L"The quick brown fox jumps over the lazy cog" ), L"de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3" );
}


FSL_TEST_FUNCTION( sha1_hmac ) {
    // http://docs.amazonwebservices.com/AmazonS3/2006-03-01/RESTAuthentication.html
    FSL_CHECK_EQ( fostlib::sha1_hmac(
        L"uV3F3YluFJax1cknvbcGwgjvx4QpvB+leU8dUj2o",
        L"GET\n\n\nTue, 27 Mar 2007 19:36:42 +0000\n/johnsmith/photos/puppy.jpg"
    ), L"xXjDGYUmKxnwqr5KXNPGldn5LbA=" );
    FSL_CHECK_EQ( fostlib::sha1_hmac(
        L"uV3F3YluFJax1cknvbcGwgjvx4QpvB+leU8dUj2o",
        L"PUT\n\nimage/jpeg\nTue, 27 Mar 2007 21:15:45 +0000\n/johnsmith/photos/puppy.jpg"
    ), L"hcicpDDvL9SsO6AkvxqmIWkmOuQ=" );
    FSL_CHECK_EQ( fostlib::sha1_hmac(
        L"uV3F3YluFJax1cknvbcGwgjvx4QpvB+leU8dUj2o",
        L"GET\n\n\nTue, 27 Mar 2007 19:42:41 +0000\n/johnsmith/"
    ), L"jsRt/rhG+Vtp88HrYL706QhE4w4=" );
    FSL_CHECK_EQ( fostlib::sha1_hmac(
        L"uV3F3YluFJax1cknvbcGwgjvx4QpvB+leU8dUj2o",
        L"PUT\n4gJE4saaMU4BqNR0kLY+lw==\napplication/x-download\nTue, 27 Mar 2007 21:06:08 +0000\nx-amz-acl:public-read\nx-amz-meta-checksumalgorithm:crc32\nx-amz-meta-filechecksum:0x02661779\nx-amz-meta-reviewedby:joe@johnsmith.net,jane@johnsmith.net\n/static.johnsmith.net/db-backup.dat.gz"
    ), L"C0FlOtU8Ylb9KDTpZqYkZPX91iI=" );
}

