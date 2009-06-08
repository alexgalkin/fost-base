/*
    Copyright 2009, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-schema-test.hpp"
#include <fost/db>


using namespace fostlib;


FSL_TEST_SUITE( basic_models );


class BasicModel : public model< BasicModel > {
public:
    BasicModel( const factory &f, dbconnection &dbc, const json &j )
    : superclass( f, dbc, j ) {
    }
};
template<>
const BasicModel::factory BasicModel::superclass::s_factory( L"BasicModel" );


class BasicSubModel : public model< BasicSubModel, BasicModel > {
public:
    BasicSubModel( const factory &f, dbconnection &dbc, const json &j )
    : superclass( f, dbc, j ) {
    }
};
template<>
const BasicSubModel::factory BasicSubModel::superclass::s_factory( L"BasicSubModel" );


class HostModel : public model< HostModel > {
public:
    HostModel( const factory &f, dbconnection &dbc, const json &j )
    : superclass( f, dbc, j ) {
    }

    class NestedModel : public model< NestedModel > {
    public:
        NestedModel( const factory &f, dbconnection &dbc, const json &j )
        : superclass( f, dbc, j ) {
        }
    };
};
template<>
const HostModel::factory HostModel::superclass::s_factory( L"HostModel" );
template<>
const HostModel::NestedModel::factory HostModel::NestedModel::superclass::s_factory( HostModel::s_factory, L"NestedModel" );

class SubHostModel : public model< SubHostModel, HostModel > {
public:
    SubHostModel( const factory &f, dbconnection &dbc, const json &j )
    : superclass( f, dbc, j ) {
    }
};
template<>
const SubHostModel::factory SubHostModel::superclass::s_factory( L"SubHostModel" );


#define FSL_TEST_HAS_SUPERCLASS( i, S ) FSL_CHECK( \
        std::find( \
            i->_meta()._meta().superclasses().begin(), i->_meta()._meta().superclasses().end(), \
            S::s_factory._meta() \
        ) != i->_meta()._meta().superclasses().end() \
    )
#define FSL_TEST_HAS_NO_SUPERCLASS( i ) FSL_CHECK_EQ( 0, i->_meta()._meta().superclasses().size() )


FSL_TEST_FUNCTION( constructors_basic ) {
    dbconnection dbc( L"master", L"master" );
    boost::shared_ptr< BasicModel > instance = BasicModel::s_factory( dbc, json() );

    // We should always get the same meta_instance
    FSL_CHECK_EQ( &instance->_meta()._meta(), &instance->_meta()._meta() );
    // The name in the meta_instance must match the one we set
    FSL_CHECK_EQ( instance->_meta()._meta().name(), L"BasicModel" );
    // The instance's type is in the global namespace
    FSL_CHECK( instance->_meta()._meta().in_global() );
    // This type has no superclasses
    FSL_TEST_HAS_NO_SUPERCLASS( instance );
}


FSL_TEST_FUNCTION( constructors_subclass ) {
    dbconnection dbc( L"master", L"master" );

    boost::shared_ptr< BasicSubModel > i1 = BasicSubModel::s_factory( dbc, json() );

    // We should be able to assign to the superclass smart pointer type
    boost::shared_ptr< BasicModel > i2 = BasicSubModel::s_factory( dbc, json() );

    // Check that we have two instances of the same thing
    FSL_CHECK( i1->_meta()._meta().in_global() );
    FSL_CHECK( i2->_meta()._meta().in_global() );

    // Check that the sub model is a sub-class
    FSL_TEST_HAS_SUPERCLASS( i1, BasicModel );
    FSL_TEST_HAS_SUPERCLASS( i2, BasicModel );
}


FSL_TEST_FUNCTION( constructors_nested ) {
    dbconnection dbc( L"master", L"master" );

    { // Create some simple instances and run some basic checks
        boost::shared_ptr< HostModel > i1 = HostModel::s_factory( dbc, json() );
        boost::shared_ptr< HostModel::NestedModel > i2 = HostModel::NestedModel::s_factory( dbc, json() );

        FSL_CHECK_EQ( i1->_meta()._meta().name(), L"HostModel" );
        FSL_CHECK( i1->_meta()._meta().in_global() );

        FSL_CHECK_EQ( i2->_meta()._meta().name(), L"NestedModel" );
        FSL_CHECK_EQ( i2->_meta()._meta().parent().name(), L"HostModel" );
        FSL_CHECK( !i2->_meta()._meta().in_global() );
        FSL_TEST_HAS_NO_SUPERCLASS( i1 );
        FSL_TEST_HAS_NO_SUPERCLASS( i2 );

        boost::shared_ptr< SubHostModel > i3 = SubHostModel::s_factory( dbc, json() );

        FSL_CHECK_EQ( i3->_meta()._meta().name(), L"SubHostModel" );
        FSL_CHECK( i3->_meta()._meta().in_global() );
        FSL_TEST_HAS_SUPERCLASS( i3, HostModel );
    }
}
