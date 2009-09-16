/*
    Copyright 1997-2008, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_THREAD_HPP
#define FOST_THREAD_HPP
#pragma once


#include <fost/string.hpp>
#include <fost/nullable.hpp>

#include <fost/exception/null.hpp>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

#ifdef WIN32
#    pragma warning ( push )
#    pragma warning ( disable : 4244 ) // Boost 1.35 pre release - warning C4244: '=' : conversion from '__w64 int' to 'unsigned int', possible loss of data
#    pragma warning ( disable : 4267 ) // Boost 1.35 pre release - warning C4267: 'return' : conversion from 'size_t' to 'int', possible loss of data
#    pragma warning ( disable : 4311 ) // Boost 1.35 pre release - warning C4311: 'type cast' : pointer truncation from 'void *const ' to 'long'
#    pragma warning ( disable : 4312 ) // Boost 1.35 pre release - warning C4312: 'type cast' : conversion from 'long' to 'void *' of greater size
#endif
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#ifdef WIN32
#    pragma warning ( pop )
#endif


namespace fostlib {


    template< typename F, typename K = fostlib::string, typename S = std::multimap< K, F > >
    class library {
    public:
        typedef F item_t;
        typedef K key_t;
        typedef S store_t;
        typedef std::vector< key_t > keys_t;
        typedef std::vector< item_t > found_t;

        void add( const key_t &k, const item_t &f ) {
            //ExclusiveWrite::WriteLock lock( m_mutex );
            boost::mutex::scoped_lock lock( m_mutex );
            m_store.insert( std::make_pair( k, f ) );
        }
        bool remove( const key_t &k ) {
            boost::mutex::scoped_lock lock( m_mutex );
            return m_store.erase( k );
        }
        bool remove( const key_t &k, const item_t &f ) {
            boost::mutex::scoped_lock lock( m_mutex );
            for ( typename S::iterator i( m_store.lower_bound( k ) ); i != m_store.upper_bound( k ); ++i )
                if ( i->second == f ) {
                    m_store.erase( i );
                    return true;
                }
            return false;
        }
        found_t find( const key_t &k ) {
            //ExclusiveWrite::ReadLock lock( m_mutex );
            boost::mutex::scoped_lock lock( m_mutex );
            found_t found;
            std::transform(
                    m_store.lower_bound( k ), m_store.upper_bound( k ),
                    std::back_inserter( found ),
                    boost::lambda::bind( &store_t::value_type::second, boost::lambda::_1 ) );
            return found;
        }
        keys_t keys() {
            //ExclusiveWrite::ReadLock lock( m_mutex );
            boost::mutex::scoped_lock lock( m_mutex );
            keys_t all;
            all.reserve( m_store.size() );
            for ( typename store_t::const_iterator i( m_store.begin() ); i != m_store.end(); ++i )
                all.push_back( i->first );
            return all;
        }
    private:
        store_t m_store;
        boost::mutex m_mutex;
        //ExclusiveWrite m_mutex;
    };


    /*
        General thread handling.
        Runs functions in a worker thread
    */
    class worker;

    template< typename R > class future_result;

    class FOST_CORE_DECLSPEC worker : boost::noncopyable {
    private:
        void execute();
    public:
        worker();
        worker( boost::function0< void > init );
        ~worker();

        boost::shared_ptr< future_result< void > > operator()( boost::function0< void > f );

        template< typename R >
        boost::shared_ptr< future_result< R > > operator()( boost::function0< R > f ) {
            return run< R >( f );
        }

        template< typename R >
        boost::shared_ptr< future_result< R > > run( boost::function0< R > f ) {
            boost::shared_ptr< future_result< R > > future( new future_result< R > );
            queue( future, typename future_result< R >::function( future, f ) );
            return future;
        }

        void terminate();

    private:
        void queue( boost::shared_ptr< future_result< void > > j, boost::function0< void > f );

    private:
        typedef std::list< std::pair< boost::shared_ptr< future_result< void > >, boost::function0< void > > > t_queue;
        t_queue m_queue;
        bool m_terminate;

        boost::mutex m_mutex;
        boost::condition m_control;
        boost::thread m_thread;

        friend class future_result< void >;
    };

    template<>
    class FOST_CORE_DECLSPEC future_result< void > {
    protected:
        future_result();
    public:
        virtual ~future_result();

        void wait();
        fostlib::nullable< fostlib::string > exception();
    private:
        volatile bool m_completed;
        fostlib::nullable< fostlib::string > m_exception;

        boost::mutex m_mutex;
        boost::condition m_has_result;

        friend void worker::execute();
        friend boost::shared_ptr< future_result< void > > worker::operator()( boost::function0< void > f );
    };

    template< typename R >
    class future_result : public future_result< void > {
    private:
        struct function {
            function( boost::shared_ptr< future_result< R > > j, boost::function0< R > f )
            : m_future( j ), m_f( f ) {
            }
            void operator() () {
                m_future->m_result = m_f();
            }
        private:
            boost::shared_ptr< future_result< R > > m_future;
            boost::function0< R > m_f;
        };

        future_result()
        : m_result() {
        }
    public:
        R result() {
            wait();
            return m_result;
        }
    private:
        R m_result;
        // friend boost::shared_ptr< future< R > > worker::operator()( boost::function0< R > f );
        friend class worker;
    };


    /*
        in_process wraps worker to give a simpler synchronous and asynchronous processing model
    */
    template< typename O > class in_process;
    class workerpool;
    template< typename R >
    class result {
    public:
        result() {}

        R operator() () const {
            if ( ! m_result.get() )
                throw exceptions::null("The result/future has not been initialised");
            return m_result->result();
        }

    private:
        result( boost::shared_ptr< future_result< R > > r )
        : m_result( r ) {
        }

        boost::shared_ptr< future_result< R > > m_result;

        template< typename O > friend class in_process;
        friend class workerpool;
    };
    template< typename O >
    class in_process : private worker {
    private:
        template< typename B >
        struct functor {
            functor( O &o, boost::function< B ( O & ) > b ) : m_o( o ), m_f( b ) {}
            B operator() () { return m_f( m_o ); }
            O &m_o; boost::function< B ( O & ) > m_f;
        };
    public:
        in_process( boost::function0< O * > c )
        : object( (worker::operator() ( c ))->result() ) {
        }
        explicit in_process( O *o )
        : object( o ) {
        }

        template< typename B >
        B synchronous( boost::function< B ( O & ) > b ) {
            return asynchronous< B >( b )();
        }

        template< typename B >
        result< B > asynchronous( boost::function< B ( O & ) > b ) {
            return result< B >( worker::operator ()< B >( functor< B >( *object, b ) ) );
        }

    private:
        boost::scoped_ptr< O > object;
    };


    /*
        A thread safe counter
    */
    class FOST_CORE_DECLSPEC counter : boost::noncopyable {
        struct counter_impl;
    public:
        counter();
        virtual ~counter();

        int operator ++();
        int value() const;

    private:
        counter_impl *m_impl;
    };


    namespace exceptions {


        class FOST_CORE_DECLSPEC forwarded_exception : public fostlib::exceptions::exception {
        public:
            forwarded_exception( const fostlib::string &message ) throw ();

        protected:
            const wchar_t * const message() const throw ();
        };


    }


}


#endif // FOST_THREAD_HPP

