/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_ACTIONS_CLASS_HPP)
#define BOOST_SPIRIT_ACTIONS_CLASS_HPP

#include <boost/tuple/tuple.hpp>
#include <boost/filesystem/path.hpp>
#include "fwd.hpp"
#include "actions.hpp"
#include "collector.hpp"
#include "template_stack.hpp"

namespace quickbook
{
    namespace qi = boost::spirit::qi;
    namespace fs = boost::filesystem;

    struct actions
    {
        actions(char const* filein_, fs::path const& outdir, string_stream& out_);

    ///////////////////////////////////////////////////////////////////////////
    // State
    ///////////////////////////////////////////////////////////////////////////

        typedef std::vector<std::string> string_list;
        typedef std::pair<char, int> mark_type;
        static int const max_template_depth = 100;

        std::string             doc_id;

    // main output stream
        collector               phrase;

    // state
        fs::path                filename;
        fs::path                outdir;
        macro_symbols           macro;
        int                     section_level;
        std::string             section_id;
        std::string             qualified_section_id;
        std::string             source_mode;

        typedef boost::tuple<
            fs::path
          , fs::path
          , macro_symbols
          , int
          , std::string
          , std::string
          , std::string>
        state_tuple;

        std::stack<state_tuple> state_stack;

    // temporary or global state
        string_list             template_info;
        int                     template_depth;
        template_stack          templates;
        int                     error_count;

    // push/pop the states and the streams
        void push();
        void pop();

    ///////////////////////////////////////////////////////////////////////////
    // actions
    ///////////////////////////////////////////////////////////////////////////
        process_action          process;
        phrase_push_action      phrase_push;
        phrase_pop_action       phrase_pop;
        error_action            error;

        syntax_highlight        syntax_p;
        plain_char_action       plain_char;

        element_id_warning_action element_id_warning;
    };
}

#endif // BOOST_SPIRIT_ACTIONS_CLASS_HPP
