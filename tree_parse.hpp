#ifndef INC_201703_TREE_PARSE_HPP
#define INC_201703_TREE_PARSE_HPP

#include <optional>
#include <string_view>
#include <sstream>
#include "binary_tree.hpp"
namespace binary_tree
{
    template <typename T, template <typename...> class order_t, typename direction>
    class tree_parse
    {
        using order = order_t<T, direction>;
        std::string_view source;
        std::size_t pos = 0;

    public:
        tree_parse(std::string_view str)
            :source(str)
        {}
        std::optional<binary_tree<T>> get_binary_tree()
        {
            if(auto element = get_element())
                return get_subtree(std::move(element.value()));
            else
                return std::nullopt;
        }
        binary_tree<T> get_subtree(T &&parent_element)
        {
            binary_tree<T> tree;
            tree.set_root(parent_element);
            fill_child<direction>(tree);
            fill_child<typename direction::inverse>(tree);
            return tree;
        }
        std::optional<T> get_element()
        {
            if(is_null())
                return std::nullopt;
            else
                return read_element();
        }
    private:
        template <typename dir>
        void fill_child(binary_tree<T> &tree)
        {
            if(auto element = get_element())
                tree.template replace_child<dir>(tree.root(), get_subtree(std::move(element.value())));
        }
        bool is_null()
        {
            using namespace std::literals;
            auto recover = pos;
            if(read_word() == "null"sv)
                return true;
            else
            {
                pos = recover;
                return false;
            }
        }
        std::string_view read_word()
        {
            eat_blank();
            std::size_t begin_pos = pos;
            while(std::isalpha(source[pos]))
                ++pos;
            return std::string_view(source.data() + begin_pos, pos - begin_pos);
        }
        void eat_blank()
        {
            while(source[pos] == ' ')
                ++pos;
        }
        T read_element()
        {
            std::istringstream stream(std::string(source.data() + pos, source.size() - pos));
            T result;
            stream >> result;
            return result;
        }
    };
}
#endif //INC_201703_TREE_PARSE_HPP
