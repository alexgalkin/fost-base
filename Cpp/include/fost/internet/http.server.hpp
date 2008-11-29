/*
    Copyright 2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_INTERNET_HTTP_SERVER_HPP
#define FOST_INTERNET_HTTP_SERVER_HPP
#pragma once


#include <fost/internet/url.hpp>
#include <fost/internet/mime.hpp>


namespace fostlib {


    class http_server {
    public:
        explicit http_server( const host &h, uint16_t port = 80 );

        accessors< const host > binding;
        accessors< const uint16_t > port;

    private:
        boost::asio::io_service m_service;
        boost::asio::ip::tcp::acceptor m_server;
    };


}


#endif // FOST_INTERNET_HTTP_SERVER_HPP
