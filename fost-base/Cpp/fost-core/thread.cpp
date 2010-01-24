/*
    Copyright 1997-2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core.hpp"
#include <fost/thread.hpp>

#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>


using namespace fostlib;


/*
    fostlib::worker
*/


fostlib::worker::worker()
: m_terminate( false ), m_thread( boost::bind( &worker::execute, this ) ) {
}


fostlib::worker::~worker() {
    {
        boost::mutex::scoped_lock lock( m_mutex );
        m_terminate = true;
        m_control.notify_all();
    }
    m_thread.join();
}


boost::shared_ptr< fostlib::detail::future_result< void > > fostlib::worker::operator()( boost::function0< void > f ) {
    boost::shared_ptr< detail::future_result< void > > future( new detail::future_result< void > );
    queue( future, f );
    return future;
}


 void fostlib::worker::queue( boost::shared_ptr< detail::future_result< void > > future, boost::function0< void > f ) {
    boost::mutex::scoped_lock lock( m_mutex );
    m_queue.push_back( std::make_pair( future, f ) );
    m_control.notify_all();
}


void fostlib::worker::execute() {
    fostlib::exceptions::structured_handler handler;
#ifdef FOST_OS_WINDOWS
    com_hr( ::CoInitializeEx( NULL, COINIT_APARTMENTTHREADED ), L"CoInitializeEx at start of fostlib::worker thread" );
#endif
    bool terminate;
    do {
        t_queue job;
        { // Find a job to perform
            boost::mutex::scoped_lock lock( m_mutex );
            terminate = m_terminate;
            if ( !terminate && m_queue.empty() )
                m_control.wait( lock );
            job.swap( m_queue );
        }
        for ( t_queue::const_iterator j( job.begin() ); j != job.end() && !terminate; ++j ) {
            // Execute job
            try {
                j->second();
            } catch ( fostlib::exceptions::exception &e ) {
                boost::mutex::scoped_lock lock( j->first->m_mutex );
                j->first->m_exception = coerce< fostlib::string >( e );
            } catch ( ... ) {
                boost::mutex::scoped_lock lock( j->first->m_mutex );
                j->first->m_exception = L"An unknown exception was caught";
            }
            {// Notify futures
                boost::mutex::scoped_lock lock( j->first->m_mutex );
                j->first->m_completed = true;
                j->first->m_has_result.notify_all();
            }
            {
                boost::mutex::scoped_lock lock( m_mutex );
                terminate = m_terminate;
            }
        }
    } while ( !terminate );
}


/*
    fostlib::detail::future_result< void >
*/


fostlib::detail::future_result< void >::future_result()
: m_completed( false ) {
}


fostlib::detail::future_result< void >::~future_result() {
}


fostlib::nullable< fostlib::string > fostlib::detail::future_result< void >::exception() {
    boost::mutex::scoped_lock lock( m_mutex );
    if ( !this->m_completed )
        m_has_result.wait( lock, boost::lambda::var( m_completed ) );
    return m_exception;
}


void fostlib::detail::future_result< void >::wait() {
    fostlib::nullable< fostlib::string > e( exception() );
    if ( !e.isnull() )
        throw fostlib::exceptions::forwarded_exception( e.value() );
}


#ifdef FOST_OS_WINDOWS
    #include "thread-win.cpp"
#else
    #include "thread-linux.cpp"
#endif
