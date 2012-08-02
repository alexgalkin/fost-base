/*
    Copyright 1997-2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_THREAD_HPP
#define FOST_THREAD_HPP
#pragma once


#include <fost/threadsafe-store.hpp>


namespace fostlib {


    namespace detail {
        /// The class that actually stores the future. Always pass to fostlib::future rather than use it directly
        template< typename R > class future_result;
    }

    /// Represents a single a thread which can execute functions and return a fostlib::future.
    class FOST_CORE_DECLSPEC worker : boost::noncopyable {
    private:
        void execute();
    public:
        worker();
        worker( boost::function0< void > init );
        ~worker();

        boost::shared_ptr< detail::future_result< void > > operator()
            ( boost::function0< void > f );

        template< typename R >
        boost::shared_ptr< detail::future_result< R > > operator()
                ( boost::function0< R > f ) {
            return run< R >( f );
        }
        template< typename R >
        boost::shared_ptr< detail::future_result< R > > operator()
                ( boost::function0< R > f ) const {
            return run< R >( f );
        }

        template< typename R >
        boost::shared_ptr< detail::future_result< R > > run( boost::function0< R > f ) {
            boost::shared_ptr< detail::future_result< R > > future(
                new detail::future_result< R > );
            queue( future, typename detail::future_result< R >::function( future, f ) );
            return future;
        }
        template< typename R >
        boost::shared_ptr< detail::future_result< R > > run( boost::function0< R > f ) const {
            boost::shared_ptr< detail::future_result< R > > future(
                new detail::future_result< R > );
            queue( future, typename detail::future_result< R >::function( future, f ) );
            return future;
        }

        void terminate();

    private:
        void queue(
            boost::shared_ptr< detail::future_result< void > > j,
            boost::function0< void > f ) const;

    private:
        typedef std::list< std::pair< boost::shared_ptr< detail::future_result< void > >, boost::function0< void > > > t_queue;
        mutable t_queue m_queue;
        bool m_terminate;

        mutable boost::mutex m_mutex;
        mutable boost::condition m_control;
        boost::thread m_thread;

        friend class detail::future_result< void >;
    };

    namespace detail {
        template<>
        class FOST_CORE_DECLSPEC future_result< void > {
        protected:
            future_result();
        public:
            virtual ~future_result();

            /// Blocks waiting for the result to become available
            void wait();
            /// Blocks waiting to see if there is an exception or not
            fostlib::nullable< fostlib::string > exception();
            /// Returns true if the result is available
            bool completed() const {
                return m_completed;
            }

        private:
            bool m_completed;
            fostlib::nullable< fostlib::string > m_exception;

            boost::mutex m_mutex;
            boost::condition m_has_result;

            friend class fostlib::worker;
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
            friend class fostlib::worker;
        };
    }


    template< typename O > class in_process;

    /// Represents the result of a calculation that will finish in the future
    template< typename R >
    class future {
    public:
        future() {}
        future( boost::shared_ptr< detail::future_result< R > > r )
        : m_result( r ) {
        }

        /// Blocks waiting for the future to become available
        R operator() () const {
            assert_valid();
            return m_result->result();
        }
        /// Blocks waiting to see if the future will result in an exception
        nullable<string> exception() const {
            assert_valid();
            return m_result->exception();
        }
        /// Returns true if the result is available
        bool available() const {
            assert_valid();
            return m_result->completed();
        }
    private:
        void assert_valid() const {
            if ( ! m_result.get() )
                throw exceptions::null("The result/future has not been initialised");
        }
        boost::shared_ptr< detail::future_result< R > > m_result;

        template< typename O > friend class in_process;
    };


    /// Wraps fostlib::worker to give a simpler synchronous and asynchronous processing model
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
        /// Construct the in_process from anything convertible to a function returning O*
        template< typename F >
        in_process( F c )
        : object( (worker::operator() ( boost::function0< O* >(c) ))->result() ) {
        }
        explicit in_process( O *o )
        : object( o ) {
        }

        template< typename B >
        B synchronous( boost::function< B ( O & ) > b ) {
            return asynchronous< B >( b )();
        }
        template< typename B >
        B synchronous( boost::function< B ( O & ) > b ) const {
            return asynchronous< B >( b )();
        }

        template< typename B >
        future< B > asynchronous( boost::function< B ( O & ) > b ) {
            return future< B >( worker::operator ()< B >( functor< B >( *object, b ) ) );
        }
        template< typename B >
        future< B > asynchronous( boost::function< B ( O & ) > b ) const {
            return future< B >( worker::operator ()< B >( functor< B >( *object, b ) ) );
        }

    private:
        boost::scoped_ptr< O > object;
    };


    namespace exceptions {


        /// An exception that has been forwareded from another thread
        class FOST_CORE_DECLSPEC forwarded_exception : public fostlib::exceptions::exception {
        public:
            forwarded_exception( const fostlib::string &message ) throw ();

        protected:
            const wchar_t * const message() const throw ();
        };


    }


}


#endif // FOST_THREAD_HPP

