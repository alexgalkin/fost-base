/*
    Copyright 2007-2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core.hpp"
#include <fost/parse/json.hpp>


#include <fost/exception/parse_error.hpp>


fostlib::json fostlib::json::parse( const string &toparse ) {
    fostlib::parser_lock lock;
    fostlib::json ret;
    fostlib::json_parser parser;
    if ( !fostlib::parse( lock,
                coerce< std::wstring >( toparse ).c_str(), parser[ phoenix::var( ret ) = phoenix::arg1 ]
            ).full )
        throw fostlib::exceptions::parse_error( L"Whilst parsing JSON string", toparse );
    return ret;
}

fostlib::json fostlib::json::parse( const string &toparse, const json &def ) {
    fostlib::parser_lock lock;
    fostlib::json ret;
    fostlib::json_parser parser;
    if ( !fostlib::parse( lock,
                coerce< std::wstring >( toparse ).c_str(), parser[ phoenix::var( ret ) = phoenix::arg1 ]
            ).full )
        return def;
    else
        return ret;
}