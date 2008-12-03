/*
    Copyright 1999-2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_ATTRIBUTES_FIELDS_HPP
#define FOST_ATTRIBUTES_FIELDS_HPP
#pragma once


#include <fost/schema/fields.hpp>


namespace fostlib {


    template< typename value_type >
    class field_wrapper : public field_base {
        class value : public attribute_base {
        };
    public:
        field_wrapper( const string &type_name )
        : field_base( type_name ) {
        }
    };


    extern const field_wrapper< int64_t > integer;
    extern const field_wrapper< string > varchar, text;


}


#endif // FOST_ATTRIBUTES_FIELDS_HPP
