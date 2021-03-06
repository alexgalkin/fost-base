/**
    Copyright 2001-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#ifndef FOST_EXCEPTION_NOT_A_NUMBER_HPP
#define FOST_EXCEPTION_NOT_A_NUMBER_HPP
#pragma once


#include <fost/exception.hpp>


namespace fostlib {


    namespace exceptions {


        class FOST_CORE_DECLSPEC not_a_number : public exception {
          public:
            not_a_number(const string &message) noexcept;
            not_a_number(const string &message, const string &value) noexcept;

          protected:
            const wchar_t *const message() const noexcept;
        };


    }


}


#endif // FOST_EXCEPTION_NOT_A_NUMBER_HPP
