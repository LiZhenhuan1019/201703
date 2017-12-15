#ifndef INC_201703_TREE_PARSE_HPP
#define INC_201703_TREE_PARSE_HPP

#include <optional>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <functional>
#include "binary_tree.hpp"

namespace binary_tree_nm
{
    using namespace std::literals;
    struct parse_failed : std::domain_error
    {
        parse_failed()
            : domain_error("parse failed in tree_parse.")
        {
        }
    };
    struct expect_failed : std::domain_error
    {
        expect_failed(std::string const &s)
            : domain_error("expect '"s + s + "' but got something else.")
        {
        }
    };
    struct unexpected_end : std::domain_error
    {
        unexpected_end()
            : domain_error("unexpected end of string when parsing.")
        {
        }
    };
    namespace detail
    {
        template <typename U>
        struct final_call
        {
            final_call(U c)
                : call(c)
            {
            }
            ~final_call()
            {
                call();
            }
            U call;
        };
    }

    template <typename value>
    void assign_element(std::string &&str, value &v)
    {
        std::istringstream stream(std::move(str));
        stream >> v;
        if (!stream)
            throw parse_failed();
    }
    template <>
    inline void assign_element(std::string &&str, std::string &v)
    {
        v = std::move(str);
    }

    namespace detail
    {

        template <typename ...Stops>
        std::string read_until(std::string_view source, std::size_t &pos, Stops ...stop)
        {
            std::string str;
            while (pos < source.size() && ((source[pos] != stop) && ...))
            {
                if (source[pos] == '\\')
                    ++pos;
                if (pos == source.size())
                    throw unexpected_end();
                str.push_back(source[pos++]);
            }
            return str;
        }
        inline void eat_blank(std::string_view source, std::size_t &pos)
        {
            while (pos < source.size() && std::isblank(source[pos]))
                ++pos;
        }
        inline bool read_char(std::string_view source, std::size_t &pos, char c)
        {
            eat_blank(source, pos);
            if (pos < source.size() && source[pos] == c)
                return ++pos, true;
            else
                return false;
        }
        inline void force_read_char(std::string_view source, std::size_t &pos, char c)
        {
            if (!read_char(source, pos, c))
                throw expect_failed(std::string() + c);
        }
    }

    template <typename direction, typename T>
    class tree_parse
    {
        using tree_type = binary_tree<T>;
        using value_type = typename tree_type::value_type;
        std::string_view source;
        std::size_t pos = 0;

    public:
        tree_parse(std::string_view str)
            : source(str)
        {
        }
        std::optional<tree_type> get_binary_tree()
        {
            detail::force_read_char(source, pos, '[');
            auto _ = detail::final_call{[=]
                                        { detail::force_read_char(source, pos, ']'); }};
            if (auto element = get_element())
            {
                auto tree = get_subtree(std::move(element.value()));
                return tree;
            } else
                return std::nullopt;
        }

    private:
        tree_type get_subtree(value_type &&parent_element)
        {
            tree_type tree;
            tree.set_root(parent_element);
            fill_child<direction>(tree);
            fill_child<typename direction::inverse>(tree);
            return tree;
        }
        template <typename dir>
        void fill_child(tree_type &tree)
        {
            detail::force_read_char(source, pos, ',');
            if (auto element = get_element())
            {
                tree.template replace_child<dir>(tree.root(), get_subtree(std::move(element.value())));
            }
        }
        std::optional<value_type> get_element()
        {
            if (is_null())
                return std::nullopt;
            else
                return read_element();
        }
        bool is_null()
        {
            using namespace std::literals;
            return read_word("null");
        }
        bool read_word(std::string_view word)
        {
            auto recover = pos;
            detail::eat_blank(source, pos);
            std::size_t begin_pos = pos;
            while (pos < source.size() && std::isalpha(source[pos]))
                ++pos;
            if (std::string_view(source.data() + begin_pos, pos - begin_pos) == word)
                return true;
            pos = recover;
            return false;
        }

        value_type read_element()
        {
            std::string input;
            if (detail::read_char(source, pos, '('))
            {
                input = detail::read_until(source, pos, ')');
                detail::force_read_char(source, pos, ')');
            } else
                input = detail::read_until(source, pos, ',', ']');
            value_type result;
            assign_element(std::move(input), result);
            return result;
        }
    };

}
#endif //INC_201703_TREE_PARSE_HPP
