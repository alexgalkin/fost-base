/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_DETAIL_WORKERPOOL_HPP
#define FOST_DETAIL_WORKERPOOL_HPP
#pragma once


#include <fost/thread.hpp>


namespace fostlib {


    class workerpool : boost::noncopyable {
        struct implementation;
        implementation *impl;
        struct worker {
            template< typename R >
            R exec( boost::function< R ( void ) > lambda, boost::function< void ( void ) > completed ) {
                R r = lambda();
                completed();
                return r;
            }
        };
        boost::shared_ptr< in_process< workerpool::worker > > assign();
        void replace(boost::shared_ptr< in_process< workerpool::worker > >);
        public:
            workerpool();
            ~workerpool();

            /*
                Execute any arbitrary nullary lambda that returns some value in any
                available worker from the pool.
            */
            template< typename R >
            result< R > f ( boost::function< R ( void ) > lambda ) {
                boost::shared_ptr< in_process< workerpool::worker > > w = assign();
                boost::function< void ( void ) > completion = boost::lambda::bind(
                    &workerpool::replace, this, w
                );
                return w->asynchronous< R >( boost::lambda::bind(
                    &worker::exec< R >, boost::lambda::_1, lambda, completion
                ) );
            }

            /*
                Obtain some metrics about the current state of the pool.
            */
            std::size_t available();
            std::size_t peak_used();
    };


}


#endif // FOST_DETAIL_WORKERPOOL_HPP
