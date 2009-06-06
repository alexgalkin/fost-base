/*
    Copyright 1999-2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_SCHEMA_STATIC_HPP
#define FOST_SCHEMA_STATIC_HPP
#pragma once


#include <fost/schema/dynamic.hpp>


namespace fostlib {


    class FOST_SCHEMA_DECLSPEC model_base {
        boost::shared_ptr< instance > m_instance;
    public:
        struct FOST_SCHEMA_DECLSPEC factory_base {
            factory_base( const string &name );
            factory_base( const enclosure &enc, const string &name );
            factory_base( const factory_base &enc, const string &name );

            const enclosure &ns() const;
            accessors< string > name;

            virtual boost::shared_ptr< meta_instance > meta() const;
        private:
            typedef boost::variant< const enclosure *, const factory_base * > container_type;
            container_type m_container;
            mutable boost::shared_ptr< meta_instance > m_meta;
        };

        model_base( const factory_base &factory, dbconnection &dbc, const json &j );
        virtual ~model_base();

        instance &meta();
    };


    /*
        This handles the case for model types that inherit from other model type
    */
    template< typename model_type, typename superclass_type = t_null >
    class model : public superclass_type {
    public:
        typedef model< model_type, superclass_type > superclass;

        struct factory : public superclass_type::factory {
            factory( const string &name ) : superclass_type::factory( name ) {}
            template< typename E >
            factory( const E &enc, const string &name ) : superclass_type::factory( enc, name ) {}

            boost::shared_ptr< model_type > operator () ( dbconnection &dbc, const json &j ) const {
                return boost::shared_ptr< model_type >(
                    new model_type( dynamic_cast< const typename model_type::factory& >( *this ), dbc, j )
                );
            }

            boost::shared_ptr< meta_instance > meta() const {
                boost::shared_ptr< meta_instance > m = superclass_type::factory::meta();
                m->superclasses().push_back( superclass_type::s_factory.meta() );
                return m;
            }
        };

        model( const factory &f, dbconnection &dbc, const json &j )
        : superclass_type( f, dbc, j ) {
        }

        static const factory s_factory;
    };


    /*
        A specialisation that handles the case when a model type does not
        inherit from another model.
    */
    template< typename model_type >
    class model< model_type, t_null > : public model_base {
    public:
        typedef model< model_type > superclass;

        struct factory : public model_base::factory_base {
            factory( const string &name ) : factory_base( name ) {}
            template< typename E >
            factory( const E &enc, const string &name ) : factory_base( enc, name ) {}

            boost::shared_ptr< model_type > operator () ( dbconnection &dbc, const json &j ) const {
                return boost::shared_ptr< model_type >(
                    new model_type( dynamic_cast< const typename model_type::factory& >( *this ), dbc, j )
                );
            }
        };

        model( const factory &f, dbconnection &dbc, const json &j )
        : model_base( f, dbc, j ) {
        }

        template< typename field_type >
        class attribute {
        public:
        };

        static const factory s_factory;
    };


}


#endif // FOST_SCHEMA_STATIC_HPP
