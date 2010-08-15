/*
    Copyright 2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_PUSH_BACK_HPP
#define FOST_PUSH_BACK_HPP
#pragma once


namespace fostlib {


    /// Allow us to push a value to the back of a container in a chainable manner
    template< typename C, typename V >
    C &push_back( C &c, const V &v ) {
        c.push_back(v);
        return c;
    }


}


#endif // FOST_PUSH_BACK_HPP
