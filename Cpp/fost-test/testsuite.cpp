/*
    Copyright 2007-2012, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-test.hpp"
#include <fost/test.hpp>

#include <fost/insert>
#include <fost/log>
#include <fost/timer>

#include "boost/date_time/posix_time/posix_time.hpp"


using namespace fostlib;


namespace {


    typedef threadsafe_store< fostlib::reference_ptr< const test::suite > >
        suite_t;
    suite_t &g_suites() {
        static suite_t s;
        return s;
    }


    setting< bool > c_verbose(
        L"fost-test/testsuite.cpp",
        L"Tests", L"Display test names", false,
        true );
    setting< bool > c_continue(
        L"fost-test/testsuite.cpp",
        L"Tests", L"Continue after error", true,
        true );
    setting< double > c_warning_test_duration(
        L"fost-test/testsuite.cpp",
        L"Tests", L"Warning test duration", 10.0,
        true );


}


/*
    fostlib::test::test
*/


fostlib::test::test::test( const suite &s, const fostlib::string &n ) {
    s.add( n, this );
}


void fostlib::test::test::execute() const {
    execute_inner();
}


/*
    fostlib::test::suite
*/


fostlib::test::suite::suite( const fostlib::string &name )
: m_name( name ) {
    g_suites().add( m_name, this );
}


void fostlib::test::suite::add( const fostlib::string &n, const fostlib::test::test *t ) const {
    (*g_suites().find( m_name ).begin())->m_tests.add( n, t );
}


namespace {
    class capture_copy {
        fostlib::json messages;
        public:
            typedef fostlib::json result_type;

            bool operator () (const fostlib::log::message &m) {
                using namespace fostlib;
                push_back(messages, coerce<json>(m));
                return true;
            }
            result_type operator () () const {
                return messages;
            }
    };
    bool loop( ostream *op ) {
        bool exception( false );
        suite_t::keys_t suitenames( g_suites().keys() );
        typedef suite_t::keys_t::const_iterator s_it;
        for (s_it sn( suitenames.begin() ); sn != suitenames.end(); ++sn) {
            try {
                suite_t::found_t suites( g_suites().find( *sn ) );
                typedef suite_t::found_t::const_iterator f_it;
                for (f_it s( suites.begin() ); s != suites.end(); ++s) {
                    fostlib::test::suite::test_keys_type testnames( (*s)->test_keys() );
                    typedef fostlib::test::suite::test_keys_type::const_iterator k_it;
                    for (k_it tn( testnames.begin() ); tn != testnames.end(); ++tn) {
                        if ( op && c_verbose.value() )
                            *op << *sn << L": " << *tn << '\n';
                        fostlib::test::suite::tests_type tests( (*s)->tests( *tn ) );
                        typedef fostlib::test::suite::tests_type::const_iterator
                            t_it;
                        for (t_it test( tests.begin() ); test != tests.end(); ++test) {
                            fostlib::log::scoped_sink< capture_copy > cc;
                            try {
                                fostlib::log::info(L"Starting test " + *sn + L"--" + *tn);
                                const timer started;
                                (*test)->execute();
                                const double elapsed = started.elapsed();
                                if ( elapsed >c_warning_test_duration.value() )
                                    fostlib::log::warning(L"Test " + *sn + L"--" + *tn + L" took "
                                        + coerce<string>(elapsed) + L"s");
                            } catch ( fostlib::exceptions::exception &e ) {
                                exception = true;
                                insert(e.data(), "test", "test", *tn);
                                insert(e.data(), "test", "log", cc());
                                throw;
                            } catch ( ... ) {
                                exception = true;
                                throw;
                            }
                        }
                    }
                }
            } catch ( exceptions::exception &e ) {
                insert(e.data(), "test", "suite", *sn);
                if ( op )
                    *op << e << std::endl;
                else if ( !c_continue.value() )
                    throw;
            }
        }
        return exception;
    }
}


bool fostlib::test::suite::execute() {
    return loop( NULL );
}


bool fostlib::test::suite::execute( ostream &o ) {
    return loop( &o );
}


/*
    fostlib::exceptions::test_failure
*/


fostlib::exceptions::test_failure::test_failure(
    const string &cond, nliteral file, int64_t line
) : exception( cond ) {
    fostlib::insert(m_data, "test", "location", "file", file);
    fostlib::insert(data(), "test", "location", "line", line);
}


const wchar_t *const fostlib::exceptions::test_failure::message() const {
    return L"Test failure";
}
