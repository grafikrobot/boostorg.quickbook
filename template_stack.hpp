/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_QUICKBOOK_TEMPLATE_STACK_HPP)
#define BOOST_SPIRIT_QUICKBOOK_TEMPLATE_STACK_HPP

#include <string>
#include <deque>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/next_prior.hpp>
#include "fwd.hpp"

namespace quickbook
{
    struct template_scope;

    //  template symbols with N arguments are stored as follows:
    //
    //  vector<std::string>
    //        0: template name
    //        1: template param name[0]
    //        2: template param name[1]
    //      ...
    //        N: template param name[N-1]
    //      N+1: template body
    //  file position
    //  template scope (only used for 1.5+, 1.4- uses the dynamic scope)

    typedef boost::tuple<
            std::vector<std::string>
          , file_position
          , template_scope const*>
    template_symbol;

    typedef boost::spirit::qi::symbols<char, template_symbol> template_symbols;
    
    // template scope
    //
    // 1.4-: parent_scope is the previous scope on the stack
    //       (the template's dynamic parent).
    // 1.5+: parent_scope is the template's lexical parent.
    //
    // This means that a search along the parent_scope chain will follow the
    // correct lookup chain for that version of quickboook.
    //
    // symbols contains the templates defined in this scope.
    
    struct template_scope
    {
        template_scope() : parent_scope() {}
        template_scope const* parent_scope;
        template_symbols symbols;
    };

    struct template_stack
    {
        typedef std::deque<template_scope> deque;

        struct parser : boost::spirit::qi::primitive_parser<parser>
        {
            template <typename Ctx, typename Itr>
            struct attribute {
                typedef template_symbol type;
            };
        
            parser(template_stack& ts)
                : ts(ts) {}

            template <typename Iterator, typename Context, typename Skipper, typename Attribute>
            bool parse(Iterator& first, Iterator const& last
              , Context& context, Skipper const& skipper, Attribute& attr) const
            {
                boost::spirit::qi::skip_over(first, last, skipper);
            
                // TODO: Is this right?
                template_symbol result_attr;

                // search all scopes for the longest matching symbol.
                Iterator f = first;
                std::ptrdiff_t len = -1;
                for (template_scope const* i = &*ts.scopes.begin(); i; i = i->parent_scope)
                {                    
                    // TODO: Implement this without calling 'base'.
                    if(i->symbols.parse(first, last, context, skipper, result_attr) && first.base() - f.base() > len)
                    {
                        boost::spirit::traits::assign_to(result_attr, attr);
                        len = first.base() - f.base();
                    }
                    first = f;
                }
                if (len >= 0)
                    first = boost::next(f, len);
                return len > 0;
            }

            template_stack& ts;
        };

        template_stack();
        template_symbol const* find(std::string const& symbol) const;
        template_symbol const* find_top_scope(std::string const& symbol) const;
        template_symbols const& top() const;
        template_scope const& top_scope() const;
        // Add the given template symbol to the current scope.
        // If it doesn't have a scope, sets the symbol's scope to the current scope.
        void add(std::string const& symbol, template_symbol const& ts);
        void push();
        void pop();

        // Set the current scope's parent.
        void set_parent_scope(template_scope const&);

        parser scope;

    private:

        friend struct parser;
        deque scopes;
    };
}

#endif // BOOST_SPIRIT_QUICKBOOK_TEMPLATE_STACK_HPP
