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
    struct parse_failed : std::domain_error
    {
        parse_failed()
            : domain_error("parse failed in tree_parse.")
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

    }

    using namespace std::literals;
    template <typename direction, typename Key, typename Value = null_value_tag>
    class tree_parse
    {
        using tree_type = binary_tree<Key, Value>;
        using value_type = typename tree_type::value_type;
        std::string_view source;
        std::size_t pos = 0;

    public:
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
        tree_parse(std::string_view str)
            : source(str)
        {
        }
        std::optional<tree_type> get_binary_tree()
        {
            force_read_char('[');
            auto _ = detail::final_call{[=]
                                        { force_read_char(']'); }};
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
            force_read_char(',');
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
            eat_blank();
            std::size_t begin_pos = pos;
            while (pos < source.size() && std::isalpha(source[pos]))
                ++pos;
            if (std::string_view(source.data() + begin_pos, pos - begin_pos) == word)
                return true;
            pos = recover;
            return false;
        }
        void eat_blank()
        {
            while (pos < source.size() && std::isblank(source[pos]))
                ++pos;
        }

        template <typename Value_t = value_type, typename Key_t = Key, typename std::enable_if<std::is_same_v<Value_t, key_t>, int>::type = 0>
        value_type read_element()
        {
            std::string input;
            if (read_char('('))
            {
                input = read_until(')');
                force_read_char(')');
            } else
                input = read_until(',', ']');
            value_type result;
            detail::assign_element(std::move(input), result);
            return result;
        }
        template <std::enable_if_t<std::is_same_v<value_type, detail::stored_t<Key, Value>>, int> = 1>
        value_type read_element()
        {
            force_read_char('(');
            std::string input_key = read_until(',');
            force_read_char(',');
            std::string input_value = read_until(')');
            force_read_char(')');
            value_type result;
            detail::assign_element(std::move(input_key), result.key);
            detail::assign_element(std::move(input_value), result.value);
            return result;
        }
        bool read_char(char c)
        {
            eat_blank();
            if (pos < source.size() && source[pos] == c)
                return ++pos, true;
            else
                return false;
        }
        void force_read_char(char c)
        {
            if (!read_char(c))
                throw expect_failed(std::string() + c);
        }
        template <typename ...Stops>
        std::string read_until(Stops ...stop)
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
            if (pos == source.size())
                throw unexpected_end();
            return str;
        }
    };

}
#endif //INC_201703_TREE_PARSE_HPP
