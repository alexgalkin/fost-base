/**
    Copyright 2001-2018, Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#ifndef FOST_STRING_HPP
#define FOST_STRING_HPP
#pragma once


#include <fost/string-fwd.hpp>
#include <f5/cord/iostream.hpp>


namespace fostlib {


    /// ## String
    /**
        Interim wrapper around u8string. The intention is that we merge the
        types together so that we can switch over to u8string later on.
    */
    class string : private f5::u8string {
        mutable u8string c_string = {};

      public:
        using size_type = std::size_t;
        /// Marker for end of string
        constexpr static const size_type npos =
                std::numeric_limits<size_type>::max();

        /// ### Constructors
        using u8string::u8string;
        string(u8string);
        string(std::string);
        string(nliteral);
        string(wliteral);
        string(wchar_t);

        string(size_type, char32_t);

        string(nliteral, nliteral);
        string(wliteral, wliteral);
        string(const string &, size_type, size_type = npos);
        string(std::string::const_iterator, std::string::const_iterator);

        /// ### Conversions
        operator f5::u8view() const { return f5::u8view{*this}; }
        operator std::string() const {
            return u8string::operator std::string();
        }
        std::string std_str() const { return static_cast<std::string>(*this); }
        /**
            `c_str()` is particularly problematic to bridge because we don't
            have storage for it. For the `const` version we just have to have
            storage for it, and we need to deal with it properly :-(
         */
        char const *c_str() { return shrink_to_fit(); }
        char const *c_str() const;

        /// ### Simple query APIs
        using u8string::bytes;
        using u8string::empty;
        auto data() const { return memory(); }
        size_type length() const { return code_points(); }
        size_type native_length() const { return bytes(); }

        /// ### String operations
        string operator+(f5::u8view) const;
        string operator+(char32_t) const;
        string operator+(nliteral) const;
        string operator+(wliteral) const;
        /// Because our underlying string type is immutable it makes no sense
        /// to do any reservation.
        void reserve(size_type) {}

        /// ### Iteration based APIs
        using u8string::begin;
        using u8string::const_iterator;
        using u8string::end;

        /// ### Comparison operators
        using u8string::operator==;
        bool operator==(const string &) const;
        bool operator==(nliteral) const;
        bool operator==(wliteral) const;
        using u8string::operator!=;
        bool operator!=(wliteral) const;
        using u8string::operator<;
        bool operator<(wliteral) const;
        using u8string::operator<=;
        bool operator<=(wliteral) const;
        using u8string::operator>;
        bool operator>(wliteral) const;
        using u8string::operator>=;
        bool operator>=(wliteral) const;

        /// ### Algorithmic APIs
        size_type find(const string &, size_type = 0u) const;
        size_type find_first_of(const string &) const;
        size_type find_first_not_of(const string &) const;
        size_type find_last_not_of(const string &) const;
        using u8string::starts_with;
        bool startswith(const string &) const;
        bool endswith(const string &) const;

        /// ### Mutation APIs (to be deprecated)
        string &operator+=(const string &);
        string &clear();
        string &erase(size_type = 0u, size_type = npos);
        string &insert(size_type, const string &);
        string &
                replace(size_type,
                        size_type,
                        const string &,
                        size_type = 0u,
                        size_type = npos);

        /// ### Substrings and slicing
        string substr(size_type = 0u, size_type = npos) const;
        char32_t at(size_type) const;
        char32_t operator[](size_type p) const { return at(p); }
    };


    /// ### Binary operators needed outside of the class
    bool operator==(f5::lstring, const string &);
    bool operator==(nliteral, const string &);
    bool operator==(wliteral, const string &);
    bool operator==(f5::u8string, wliteral);
    bool operator!=(wliteral, const string &);
    bool operator<(wliteral, const string &);
    bool operator<=(nliteral, const string &);
    bool operator<=(wliteral, const string &);
    bool operator>(nliteral, const string &);
    bool operator>(wliteral, const string &);
    bool operator>=(nliteral, const string &);
    bool operator>=(wliteral, const string &);
    string operator+(nliteral, const string &);
    string operator+(wliteral, const string &);


}


namespace f5::cord {
    bool operator==(const std::string &, u8string);
    u8string operator+(u8string, const std::string &);
}


#ifdef FOST_COERCE_HPP
#include <fost/string/coerce.hpp>
#endif
#ifdef FOST_ACCESSORS_HPP
#include <fost/string/accessors.hpp>
#endif
#ifdef FOST_NULLABLE_HPP
#include <fost/string/nullable.hpp>
#endif


#endif // FOST_STRING_HPP
