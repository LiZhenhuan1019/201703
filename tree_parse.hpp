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
    using namespace std::literals;
    template <typename T, typename direction>
    class tree_parse
    {
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
        std::optional<binary_tree < T>> get_binary_tree()
        {
            force_read_char('[');
            auto _ = final_call{[=]
                                { force_read_char(']'); }};
            if (auto element = get_element())
            {
                auto tree = get_subtree(std::move(element.value()));
                return tree;
            } else
                return std::nullopt;
        }

    private:
        binary_tree <T> get_subtree(T &&parent_element)
        {
            binary_tree <T> tree;
            tree.set_root(parent_element);
            fill_child<direction>(tree);
            fill_child<typename direction::inverse>(tree);
            return tree;
        }
        template <typename dir>
        void fill_child(binary_tree <T> &tree)
        {
            force_read_char(',');
            if (auto element = get_element())
            {
                tree.template replace_child<dir>(tree.root(), get_subtree(std::move(element.value())));
            }
        }
        std::optional<T> get_element()
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
        T read_element()
        {
            std::string input;
            if (read_char('"'))
            {
                input = read_until('"');
                force_read_char('"');
            } else
                input = read_until(',', ']');
            std::istringstream stream(input);
            T result;
            stream >> result;
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
