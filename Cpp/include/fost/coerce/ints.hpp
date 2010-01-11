/*
    Copyright 2008-2010, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef FOST_COERCE_INTS_HPP
#define FOST_COERCE_INTS_HPP
#pragma once


#include <fost/coerce.hpp>
#include <fost/exception/out_of_range.hpp>

#include <boost/type_traits.hpp>
#include <boost/mpl/comparison.hpp>


namespace fostlib {


    /** \brief Coercion between two identically sized integral types is safe.

        If the number of digits each type can hold is the same and they are both
        either unsigned or signed then we shouldn't need to do any range checking.
    */
    template< typename T, typename F >
    struct coercer<
        T, F,
        typename boost::enable_if<
            boost::mpl::and_<
                boost::mpl::bool_< boost::is_integral< T >::value >,
                boost::mpl::bool_< boost::is_integral< F >::value >,
                boost::mpl::equal_to<
                    boost::mpl::int_< sizeof(T) >,
                    boost::mpl::int_< sizeof(F) >
                >,
                boost::mpl::or_<
                    boost::mpl::and_<
                        boost::mpl::bool_< boost::is_signed< T >::value >,
                        boost::mpl::bool_< boost::is_signed< F >::value >
                    >,
                    boost::mpl::and_<
                        boost::mpl::bool_< boost::is_unsigned< T >::value >,
                        boost::mpl::bool_< boost::is_unsigned< F >::value >
                    >
                >
            >
        >::type
    > {
        T coerce( F i ) { return i; }
    };


    /** \brief Coerce from a smaller integral type to a larger one.

        If the two types are integral, and the range of the from type
        is within the range of the to type we are always safe.

        The range is calculated by the number of non-signed digits and we also make
        sure that either both types are signed, or both types are unsigned.
    */
    template< typename T, typename F >
    struct coercer<
        T, F,
        typename boost::enable_if<
            boost::mpl::and_<
                boost::mpl::bool_< boost::is_integral< T >::value >,
                boost::mpl::bool_< boost::is_integral< F >::value >,
                boost::mpl::greater<
                    boost::mpl::int_< sizeof(T) >,
                    boost::mpl::int_< sizeof(F) >
                >,
                boost::mpl::or_<
                    boost::mpl::and_<
                        boost::mpl::bool_< boost::is_signed< T >::value >,
                        boost::mpl::bool_< boost::is_signed< F >::value >
                    >,
                    boost::mpl::and_<
                        boost::mpl::bool_< boost::is_unsigned< T >::value >,
                        boost::mpl::bool_< boost::is_unsigned< F >::value >
                    >
                >
            >
        >::type
    > {
        T coerce( F i ) { return i; }
    };


    /** \brief Coerce from a larger integral type to a smaller one

        The from type is able to hold a larger range than the to type so we need to do
        a run time range check to make sure that the value fits.

        This coercion is only used where both types are either signed or unsigned.
    */
    template< typename T, typename F >
    struct coercer<
        T, F,
        typename boost::enable_if<
            boost::mpl::and_<
                boost::mpl::bool_< boost::is_integral< T >::value >,
                boost::mpl::bool_< boost::is_integral< F >::value >,
                boost::mpl::less<
                    boost::mpl::int_< sizeof(T) >,
                    boost::mpl::int_< sizeof(F) >
                >,
                boost::mpl::or_<
                    boost::mpl::and_<
                        boost::mpl::bool_< boost::is_signed< T >::value >,
                        boost::mpl::bool_< boost::is_signed< F >::value >
                    >,
                    boost::mpl::and_<
                        boost::mpl::bool_< boost::is_unsigned< T >::value >,
                        boost::mpl::bool_< boost::is_unsigned< F >::value >
                    >
                >
            >
        >::type
    > {
        T coerce( F i ) {
            if (
                i > std::numeric_limits< T >::max()
                || i < std::numeric_limits< T >::min()
            )
                throw fostlib::exceptions::out_of_range< T, F >(
                    std::numeric_limits< T >::min(),
                    std::numeric_limits< T >::max(),
                    i
                );
            return T( i );
        }
    };


    /** \brief Coerce from an unsigned integral type to a smaller signed one.

        Because we are going from unsigned to signed we can't be out of limits at
        the bottom of the range. We do need to check the upper limit at run time as the
        value may be larger than the one we can store in the signed type.
    */
    template< typename T, typename F >
    struct coercer<
        T, F,
        typename boost::enable_if<
            boost::mpl::and_<
                boost::mpl::bool_< boost::is_integral< T >::value >,
                boost::mpl::bool_< boost::is_integral< F >::value >,
                boost::mpl::less<
                    boost::mpl::int_< sizeof(T) >,
                    boost::mpl::int_< sizeof(F) >
                >,
                boost::mpl::bool_< boost::is_signed< T >::value >,
                boost::mpl::bool_< boost::is_unsigned< F >::value >
            >
        >::type
    > {
        T coerce( F i ) {
            if ( i > F(std::numeric_limits< T >::max()) )
                throw fostlib::exceptions::out_of_range< T, F >(
                    std::numeric_limits< T >::min(),
                    std::numeric_limits< T >::max(),
                    i
                );
            return T( i );
        }
    };


    /** \brief Coerce from an unsigned integral type to a larger signed one.

        Because the signed range must exceed the range on the unsigned one we
        don't need to do any range checks.
    */
    template< typename T, typename F >
    struct coercer<
        T, F,
        typename boost::enable_if<
            boost::mpl::and_<
                boost::mpl::bool_< boost::is_integral< T >::value >,
                boost::mpl::bool_< boost::is_integral< F >::value >,
                boost::mpl::greater<
                    boost::mpl::int_< sizeof(T) >,
                    boost::mpl::int_< sizeof(F) >
                >,
                boost::mpl::bool_< boost::is_signed< T >::value >,
                boost::mpl::bool_< boost::is_unsigned< F >::value >
            >
        >::type
    > {
        T coerce( F i ) { return i; }
    };


    /** \brief Coerce from a signed integral type to a larger unsigned type.

        Because we know the range of the unsigned type we are going to exceeds the
        smaller signed limit we don't need to check that. We only need to make sure that
        the signed value is not negative.

        If the two are the same number of bytes in size we know that the range
        of the unsigned type is still larger at the top end.
    */
    template< typename T, typename F >
    struct coercer<
        T, F,
        typename boost::enable_if<
            boost::mpl::and_<
                boost::mpl::bool_< boost::is_integral< T >::value >,
                boost::mpl::bool_< boost::is_integral< F >::value >,
                boost::mpl::greater_equal<
                    boost::mpl::int_< sizeof(T) >,
                    boost::mpl::int_< sizeof(F) >
                >,
                boost::mpl::bool_< boost::is_unsigned< T >::value >,
                boost::mpl::bool_< boost::is_signed< F >::value >
            >
        >::type
    > {
        T coerce( F i ) {
            if ( i < 0 )
                throw fostlib::exceptions::out_of_range< T, F >(
                    std::numeric_limits< T >::min(),
                    std::numeric_limits< T >::max(),
                    i
                );
            return T( i );
        }
    };


    /** \brief Coerce from a larger signed integer to a smaller unsigned one.

        We must test both ends of the range as the signed integer may be negative
        or too largely positive.
    */
    template< typename T, typename F >
    struct coercer<
        T, F,
        typename boost::enable_if<
            boost::mpl::and_<
                boost::mpl::bool_< boost::is_integral< T >::value >,
                boost::mpl::bool_< boost::is_integral< F >::value >,
                boost::mpl::less<
                    boost::mpl::int_< sizeof(T) >,
                    boost::mpl::int_< sizeof(F) >
                >,
                boost::mpl::bool_< boost::is_unsigned< T >::value >,
                boost::mpl::bool_< boost::is_signed< F >::value >
            >
        >::type
    > {
        T coerce( F i ) {
            if ( i < 0 || i > std::numeric_limits< T >::max() )
                throw fostlib::exceptions::out_of_range< T, F >(
                    std::numeric_limits< T >::min(),
                    std::numeric_limits< T >::max(),
                    i
                );
            return T( i );
        }
    };
}


#endif // FOST_COERCE_INTS_HPP
