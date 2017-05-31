/*
    Copyright 2007-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-core.hpp"
#include <fost/json.hpp>
#include <fost/insert.hpp>
#include <fost/unicode.hpp>
#include <fost/detail/coerce.hpp>
#include <fost/detail/utility.hpp>

#include <fost/exception/json_error.hpp>
#include <fost/exception/not_implemented.hpp>
#include <fost/exception/not_null.hpp>
#include <fost/exception/null.hpp>
#include <fost/exception/out_of_range.hpp>
#include <fost/exception/parse_error.hpp>


/*
 * fostlib
 */


bool fostlib::operator == (const fostlib::jcursor::value_type &lhs, nliteral rhs) {
    auto lhs_sp = boost::get<fostlib::string>(&lhs);
    if ( lhs_sp ) {
        return *lhs_sp == rhs;
    } else {
        return false;
    }
}


/*
 * fostlib::jcursor
 */


fostlib::jcursor::jcursor() {
}
fostlib::jcursor::jcursor( int i ) {
    m_position.push_back( i );
}
fostlib::jcursor::jcursor( json::array_t::size_type i ) {
    m_position.push_back( i );
}
fostlib::jcursor::jcursor( nliteral n ) {
    m_position.push_back( fostlib::string(n) );
}
fostlib::jcursor::jcursor( wliteral n ) {
    m_position.push_back( fostlib::string(n) );
}
fostlib::jcursor::jcursor( const string &i ) {
    m_position.push_back( i );
}
fostlib::jcursor::jcursor( const json &j ) {
    nullable<int64_t> i = j.get<int64_t>();
    if ( i ) {
        m_position.push_back( coerce< json::array_t::size_type >( i.value() ) );
    } else {
        auto s = j.get<utf::u8_view>();
        if ( s ) {
            m_position.push_back(s.value());
        } else {
            throw exceptions::json_error(
                "The jcursor location must be a string or integer", j);
        }
    }
}

fostlib::jcursor::jcursor( stack_t::const_iterator b, stack_t::const_iterator e )
: m_position( b, e ) {
}


fostlib::jcursor fostlib::jcursor::split(const string &s, const string &c) {
    fostlib::split_type path = fostlib::split(s, c);
    fostlib::jcursor position;
    for ( fostlib::split_type::const_iterator part(path.begin());
            part != path.end(); ++part ) {
        try {
            int index = fostlib::coerce<int>(*part);
            position /= index;
        } catch ( fostlib::exceptions::parse_error& ) {
            position /= *part;
        }
    }
    return position;
}


fostlib::jcursor &fostlib::jcursor::operator /= ( json::array_t::size_type i ) {
    m_position.push_back( i );
    return *this;
}
fostlib::jcursor &fostlib::jcursor::operator /= ( const string &i ) {
    m_position.push_back( i );
    return *this;
}
fostlib::jcursor &fostlib::jcursor::operator /= ( const json &j ) {
    return operator /= ( jcursor( j ) );
}
fostlib::jcursor &fostlib::jcursor::operator /= ( const jcursor &r ) {
    m_position.insert( m_position.end(), r.m_position.begin(), r.m_position.end() );
    return *this;
}

fostlib::jcursor &fostlib::jcursor::enter() {
    m_position.push_back( 0 );
    return *this;
}
fostlib::jcursor &fostlib::jcursor::enter( const string &i ) {
    m_position.push_back( i );
    return *this;
}
fostlib::jcursor &fostlib::jcursor::pop() {
    m_position.pop_back();
    return *this;
}

fostlib::jcursor &fostlib::jcursor::operator ++() {
    if ( m_position.empty() )
        throw fostlib::exceptions::null( L"cannot increment an empty jcursor" );
    else if ( boost::get< json::array_t::size_type >( &*m_position.rbegin() ) == NULL )
        throw fostlib::exceptions::null( L"the current jcursor isn't into an array position" );
    else
        ++boost::get< json::array_t::size_type >( *m_position.rbegin() );
    return *this;
}


namespace {
    struct take_step : public boost::static_visitor<fostlib::json*> {
        const fostlib::json &orig;
        fostlib::json::element_t &element;
        bool isnull;

        take_step(const fostlib::json &o, fostlib::json::element_t &j, bool n)
        : orig(o), element(j), isnull(n) {
        }

        fostlib::json *operator () (fostlib::json::array_t::size_type k) const {
            if ( isnull ) element = std::make_shared<fostlib::json::array_t>();
            else if ( !boost::get<fostlib::json::array_p>(&element) ) {
                throw fostlib::exceptions::json_error("Cannot walk through a JSON "
                    "object/value which is not an array using an integer key", orig);
            }
            // Copy the array
            fostlib::json::array_t &array = *boost::get<fostlib::json::array_p>(element);
            auto copy(std::make_shared<fostlib::json::array_t>(array.begin(), array.end()));
            while ( copy->size() <= k )
                copy->push_back(fostlib::json{});
            element = copy;
            return &(*copy)[k];
        }
        fostlib::json *operator () (const fostlib::string &k) const {
            if ( isnull ) element = std::make_shared<fostlib::json::object_t>();
            else if ( !boost::get<fostlib::json::object_p>(&element) ) {
                throw fostlib::exceptions::json_error("Cannot walk through a JSON "
                    "array/value which is not an object using a string key", orig);
            }
            /// Copy the object and return the address of the value at the requested key
            fostlib::json::object_t &object = *boost::get<fostlib::json::object_p>(element);
            auto copy(std::make_shared<fostlib::json::object_t>(object.begin(), object.end()));
            element = copy;
            return &(*copy)[k];
        }
    };
}
fostlib::json &fostlib::jcursor::operator() (json &j) const {
    try {
        json *loc = &j;
        for ( stack_t::const_iterator p(m_position.begin()); p != m_position.end(); ++p ) {
            loc = boost::apply_visitor(take_step(j, loc->m_element, loc->isnull()), *p);
        }
        return *loc;
    } catch ( exceptions::exception &e ) {
        fostlib::insert(e.data(), "jcursor", *this);
        fostlib::insert(e.data(), "traversing", j);
        throw;
    }
}


fostlib::json &fostlib::jcursor::push_back(json &j, const json &v) const {
    json &array = (*this)(j);
    if ( array.isnull() ) {
        json::array_t na;
        na.push_back(v);
        array = na;
    } else if ( array.isarray() ) {
        boost::get<json::array_p>(array.m_element)->push_back(v);
    } else throw exceptions::json_error( L"Can only push onto the back of a JSON array" );
    return j;
}


fostlib::json &fostlib::jcursor::insert( json &j, const json &v ) const {
    if ( !j.has_key(*this) ) {
        (*this)( j ) = v;
    } else {
        exceptions::not_null error( L"There is already some JSON at this key position");
        fostlib::insert(error.data(), "json", j);
        fostlib::insert(error.data(), "value", v);
        fostlib::insert(error.data(), "key", *this);
        throw error;
    }
    return j;
}


fostlib::json &fostlib::jcursor::replace( json &j, const json &v ) const {
    if ( j.has_key(*this) )
        (*this)(j) = v;
    else
        throw exceptions::null( L"There is nothing to replace at this key position",
            json::unparse( j, true ) + L"\n" + json::unparse( v, true ));
    return j;
}


fostlib::json &fostlib::jcursor::set(json &j, const json &v) const {
    (*this)(j) = v;
    return j;
}


namespace {
    struct del_key : public boost::static_visitor< void > {
        fostlib::json::element_t &element;
        del_key(fostlib::json::element_t &j)
        : element( j ) {
        }

        void operator () (fostlib::json::array_t::size_type k) const {
            fostlib::json::array_p *arr(boost::get<fostlib::json::array_p>(&element));
            if ( !arr )
                throw fostlib::exceptions::json_error(
                    "A numeric key can only be used to delete from a JSON array");
            if ( k >= (*arr)->size() )
                throw fostlib::exceptions::out_of_range< std::size_t >(
                    "Trying to erase beyond the end of the array",
                    0, (*arr)->size(), k);
            (*arr)->erase((*arr)->begin() + k);
        }
        void operator () (const fostlib::string &k) const {
            fostlib::json::object_p *obj(boost::get<fostlib::json::object_p>(&element) );
            if ( not obj )
                throw fostlib::exceptions::json_error(
                    "A string key can only be deleted from JSON objects");
            fostlib::json::object_t::iterator p((*obj)->find(k));
            if ( p == (*obj)->end() )
                throw fostlib::exceptions::json_error(
                    "Key can't be removed from object as it doesn't exist in the object", **obj);
            (*obj)->erase(p);
        }
    };
}
fostlib::json &fostlib::jcursor::del_key(json &j) const {
    try {
        if ( m_position.size() == 0 ) {
            throw exceptions::out_of_range< std::size_t >(
                "The jcursor must have at least one level of items in it",
                1, std::numeric_limits< std::size_t >::max(), m_position.size());
        } else if ( j.has_key(*this) ) {
            (*this)(j);
            jcursor head(begin(), --end());
            auto &from = const_cast<json::element_t&>(j[head].m_element);
            boost::apply_visitor(::del_key(from), *m_position.rbegin());
        } else {
            throw exceptions::json_error("The key cannot be deleted because it doesn't exist");
        }
        return j;
    } catch ( exceptions::exception &e ) {
        fostlib::insert(e.data(), "json", j);
        fostlib::insert(e.data(), "path", *this);
        throw;
    }
}


bool fostlib::jcursor::operator == ( const jcursor &j ) const {
    return m_position == j.m_position;
}

