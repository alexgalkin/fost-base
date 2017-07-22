/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core.hpp"
#include <fost/json-order.hpp>

#include <algorithm>


namespace {
    struct compare_bool : public boost::static_visitor<bool> {
        const bool left;
        compare_bool(bool l)
        : left(l) {
        }
        bool operator () (fostlib::t_null) const {
            return false;
        }
        bool operator () (bool right) const {
            return left < right;
        }
        template <typename O>
        bool operator () (const O &o) const {
            return true;
        }
    };
    struct compare_int : public boost::static_visitor<bool> {
        const int64_t left;
        compare_int(int64_t l)
        : left(l) {
        }
        bool operator () (fostlib::t_null) const {
            return false;
        }
        bool operator () (bool right) const {
            return false;
        }
        bool operator () (int64_t right) const {
            return left < right;
        }
        bool operator () (double right) const {
            return double(left) < right;
        }
        template <typename O>
        bool operator () (const O &o) const {
            return true;
        }
    };
    struct compare_double : public boost::static_visitor<bool> {
        const double left;
        compare_double(double l)
        : left(l) {
        }
        bool operator () (fostlib::t_null) const {
            return false;
        }
        bool operator () (bool right) const {
            return false;
        }
        bool operator () (int64_t right) const {
            return left < double(right);
        }
        bool operator () (double right) const {
            return left < right;
        }
        template <typename O>
        bool operator () (const O &o) const {
            return true;
        }
    };
    struct compare_string : public boost::static_visitor<bool> {
        const fostlib::string &left;
        compare_string(const fostlib::string &l)
        : left(l) {
        }
        template <typename O>
        bool operator () (const O &o) const {
            return false;
        }
        bool operator () (const fostlib::string &right) const {
            return left < right;
        }
        bool operator () (const fostlib::json::array_p &) const {
            return true;
        }
        bool operator () (const fostlib::json::object_p &) const {
            return true;
        }
    };

    struct compare_array : public boost::static_visitor<bool> {
        const fostlib::json::array_t &left;
        compare_array(const fostlib::json::array_t &l)
        : left(l) {
        }
        template<typename T>
        bool operator () (const T &) const {
            return false;
        }
        bool operator () (const fostlib::json::array_p &right) const {
            return lexicographical_compare(left.begin(), left.end(), right->begin(), right->end(),
                [](const auto &left, const auto &right) {
                    return std::less<fostlib::json>()(left, right);
                });
        }
        bool operator () (const fostlib::json::object_p &o) const {
            return true;
        }
    };

    struct compare_object : public boost::static_visitor<bool> {
        const fostlib::json::object_t &left;
        compare_object(const fostlib::json::object_t &l)
        : left(l) {
        }
        template <typename O>
        bool operator () (const O &right) const {
            return false;
        }
        bool operator () (const fostlib::json::object_p &right) const {
            return lexicographical_compare(left.begin(), left.end(), right->begin(), right->end(),
                [](const auto &left, const auto &right) {
                    if ( left.first < right.first ) return true;
                    if ( right.first < left.first ) return false;
                    return std::less<fostlib::json>()(left.second, right.second);
                });
        }
    };

    struct compare_json : public boost::static_visitor<bool> {
        const fostlib::json &right;
        compare_json(const fostlib::json &r)
        : right(r) {
        }

        bool operator () (const fostlib::t_null) const {
            return not right.isnull();
        }
        bool operator () (const bool left) const {
            return boost::apply_visitor(::compare_bool(left), right);
        }
        bool operator () (const int64_t left) const {
            return boost::apply_visitor(::compare_int(left), right);
        }
        bool operator () (const double left) const {
            return boost::apply_visitor(::compare_double(left), right);
        }
        bool operator () (const fostlib::string &left) const {
            return boost::apply_visitor(::compare_string(left), right);
        }
        bool operator () (const fostlib::json::array_p &left) const {
            return boost::apply_visitor(::compare_array(*left), right);
        }
        bool operator () (const fostlib::json::object_p &left) const {
            return boost::apply_visitor(::compare_object(*left), right);
        }
    };
}


bool fostlib::operator < (const fostlib::json& lhs, const fostlib::json& rhs) {
    return boost::apply_visitor(::compare_json(rhs), lhs);
}

