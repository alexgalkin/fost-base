/*
    Copyright 2010, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/log.hpp>


namespace fostlib {
    namespace logging {
        namespace detail {

            /// The log queue co-ordination class
            class log_queue {
                typedef std::vector< logging::detail::scoped_sink_base* >
                    scoped_sinks_type;
                typedef std::map< boost::thread::id, scoped_sinks_type >
                    scoped_thread_sink_type;
                scoped_thread_sink_type scoped_taps;

                typedef std::vector< logging::global_sink_configuration* >
                    global_sinks_type;
                global_sinks_type global_taps;

                public:
                    std::size_t log(boost::thread::id, const fostlib::logging::message &m);

                    std::size_t tap_scoped(
                        boost::thread::id, logging::detail::scoped_sink_base*);
                    std::size_t untap_scoped(
                        boost::thread::id, logging::detail::scoped_sink_base*);

                    std::size_t tap_global(logging::global_sink_configuration*);
                    std::size_t untap_global(logging::global_sink_configuration*);

                    bool exec(boost::function0<void> fn);
            };

            /// Proxy for the actual logging object which is in another thread
            class log_proxy {
                in_process< log_queue > queue;

                log_proxy()
                : queue( new log_queue ) {
                }

                public:
                    static log_proxy &proxy() {
                        static log_proxy p;
                        return p;
                    }

                    void log(const fostlib::logging::message &m);

                    void exec(boost::function0<void> fn);

                    std::size_t tap(logging::detail::scoped_sink_base*);
                    std::size_t untap(logging::detail::scoped_sink_base*);

                    std::size_t tap(logging::global_sink_configuration*);
                    std::size_t untap(logging::global_sink_configuration*);
            };

        }
    }
}

