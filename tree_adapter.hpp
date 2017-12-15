#ifndef INC_201703_TREE_ADAPTER_HPP
#define INC_201703_TREE_ADAPTER_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <queue>
#include "binary_tree.hpp"
#include "tree_parse.hpp"

namespace binary_tree_nm
{
    class null_value_tag;
    namespace detail
    {
        template <typename Key, typename Value>
        struct stored_t
        {
            Key key;
            Value value;
            friend std::ostream &operator<<(std::ostream &out, stored_t const &s)
            {
                out << s.key << " " << s.value;
                return out;
            }
        };
        template <typename T>
        auto const &get_key(T const &t)
        {
            return t;
        }
        template <typename Key, typename Value>
        auto const &get_key(stored_t<Key, Value> const &s)
        {
            return get_key(s.key);
        }
        template <typename t1, typename t2>
        bool operator<(t1 const &lhs, t2 const &rhs)
        {
            return get_key(lhs) < get_key(rhs);
        }
        template <typename t1, typename t2>
        struct support_equality
        {
            constexpr static bool value = std::is_convertible_v<decltype(get_key(std::declval<t1>()) == get_key(std::declval<t2>())), bool>;
        };
        template <typename t1, typename t2, std::enable_if_t<support_equality<t1, t2>::value, int> = 0>
        bool operator==(t1 const &lhs, t2 const &rhs)
        {
            return get_key(lhs) == get_key(rhs);
        }
        template <typename t1, typename t2, std::enable_if_t<!support_equality<t1, t2>::value, int> = 0>
        bool operator==(t1 const &lhs, t2 const &rhs)
        {
            return !(get_key(lhs) < get_key(rhs)) && !(get_key(rhs) < get_key(rhs));
        }
        template <typename t1, typename t2>
        bool operator!=(t1 const&lhs, t2 const&rhs)
        {
            return !(lhs == rhs);
        };
        template <typename T>
        auto &get_value(T &t)
        {
            return t;
        }
        template <typename T>
        auto &get_value(T const &t)
        {
            return t;
        }
        template <typename Key, typename Value>
        auto &get_value(stored_t<Key, Value> const &s)
        {
            return get_value(s.value);
        }
        template <typename Key, typename Value>
        auto &get_value(stored_t<Key, Value> &s)
        {
            return get_value(s.value);
        }

        template <typename Key, typename Value>
        struct value_traits
        {
            using type = stored_t<Key, Value>;
            using key_type = Key;
            using value_type = Value;
        };
        template <typename Key>
        struct value_traits<Key, null_value_tag>
        {
            using type = Key;
            using key_type = Key;
            using value_type = Key;
        };
        template <typename Key, typename Value>
        void assign_element(std::string &&str, detail::stored_t<Key, Value> &v)
        {
            using binary_tree_nm::assign_element;
            std::string_view source = str;
            std::size_t pos = 0;
            auto key_input = read_until(source, pos, ',');
            detail::force_read_char(source, pos, ',');
            auto value_input = read_until(source, pos);
            assign_element(std::move(key_input), v.key);
            assign_element(std::move(value_input), v.value);
        }
    }
    using detail::get_key;
    using detail::get_value;
    using detail::value_traits;

    using namespace std::literals;

    template <typename Key_t, typename Value_t = null_value_tag>
    class tree_adapter
    {
        using element_type = typename value_traits<Key_t, Value_t>::type;
        using tree_type = binary_tree<element_type>;
        std::optional<tree_type> tree;
        using key_type = typename value_traits<Key_t, Value_t>::key_type;
        using value_type = typename value_traits<Key_t, Value_t>::value_type;

    public:
        struct tree_exists : std::logic_error
        {
            explicit tree_exists(std::string const &function)
                : logic_error("The tree already exists in function"s + function + ".")
            {
            }
        };
        struct tree_not_exist : std::logic_error
        {
            explicit tree_not_exist(std::string const &function)
                : logic_error("The tree doesn't exist in function"s + function + ".")
            {
            }
        };
        struct parse_failed : std::domain_error
        {
            parse_failed(std::string const &function)
                : domain_error("Parse tree failed in function"s + function + ".")
            {
            }
        };
        struct precondition_failed_to_satisfy : std::logic_error
        {
            precondition_failed_to_satisfy(std::string const &function)
                : logic_error("The precondition of funciton "s + function + "failed to satisfy.")
            {
            }
        };

        void InitBiTree()
        {
            if (tree)
                throw tree_exists(__func__);
            tree = binary_tree<element_type>();
        }
        void DestroyBiTree()
        {
            if (!tree)
                throw tree_not_exist(__func__);
            tree.reset();
        }
        void CreateBiTree(std::string_view definition)
        {
            auto generated_tree = tree_parse<left_first_t, element_type>(definition).get_binary_tree();
            if (!generated_tree)
                throw parse_failed(__func__);
            tree = generated_tree;
        }
        void ClearBiTree()
        {
            if (!tree)
                throw tree_not_exist(__func__);
            tree->clear();
        }
        auto BiTreeEmpty()
        {
            if (!tree)
                throw tree_not_exist(__func__);
            return tree->empty();
        }
        auto BiTreeDepth()
        {
            if (!tree)
                throw tree_not_exist(__func__);
            return tree->depth();
        }
        auto Root()
        {
            if (!tree)
                throw tree_not_exist(__func__);
            return tree->root();
        }
        template <typename order_t = preorder_t, typename dir_t = left_first_t>
        auto &Value(key_type const &key, order_t = order_t{}, dir_t = dir_t{}) const
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->template end<order_t, dir_t>(), key))
                return get_value(*iter);
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename U, typename order_t = preorder_t, typename dir_t = left_first_t>
        void Assign(key_type const &key, U &&value, order_t = order_t{}, dir_t = dir_t{})
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->template end<order_t, dir_t>(), key))
                get_value(*iter) = std::forward<U>(value);
            else
                throw precondition_failed_to_satisfy(__func__);
        }

        template <typename order_t = preorder_t, typename dir_t = left_first_t>
        auto Parent(key_type const &key, order_t = order_t{}, dir_t = dir_t{}) const
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->template end<order_t, dir_t>(), key))
                return iter.parent();
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename child_t, typename order_t = preorder_t, typename dir_t = left_first_t>
        auto Child(key_type const &key, child_t = child_t{}, order_t = order_t{}, dir_t = dir_t{}) const
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->template end<order_t, dir_t>(), key))
                return iter.template first_child<child_t>();
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename child_t, typename order_t = preorder_t, typename dir_t = left_first_t>
        auto Sibling(key_type const &key, child_t = child_t{}, order_t = order_t{}, dir_t = dir_t{}) const
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->template end<order_t, dir_t>(), key))
            {
                auto desired_child = iter.parent().template first_child<child_t>();
                if (desired_child == iter)
                    return tree->template end<order_t, dir_t>();
                return desired_child;
            }
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename child_t, typename iter, typename dir_t = right_t>
        void InsertChild(iter pos, tree_type inserted, dir_t = dir_t{})
        {
            auto replaced = tree->template replace_child<child_t>(pos, std::move(inserted));
            auto farest = tree->template begin<inorder_t, dir_t>();
            auto empty = tree->template replace_child<dir_t>(farest, std::move(replaced));
            assert(empty.empty());
        }
        template <typename child_t, typename iter>
        auto DeleteChild(iter pos)
        {
            return tree->template replace_child<child_t>(pos, tree_type{});
        }
        template <typename Callable, typename order_t, typename dir_t = left_first_t>
        void Traverse(Callable callable, order_t order, dir_t = dir_t{})
        {
            for(auto iter = tree->template begin<order_t, dir_t>(); iter != tree->template end<order_t, dir_t>(); ++iter)
                callable(*iter);
        }
        template <typename Callable, typename dir_t = left_first_t>
        void LevelOrderTraverse(Callable callable, dir_t = dir_t{})
        {
            auto iter = tree->template root<preorder_t, dir_t>();
            std::queue<decltype(iter)> queue{iter};
            while(!queue.empty())
            {
                iter = queue.back(), queue.pop();
                if(iter.template first_child<dir_t>())
                    queue.push(iter.template first_child<dir_t>());
                if(iter.template second_child<dir_t>())
                    queue.push(iter.template second_child<dir_t>());
                callable(*iter);
            }
        }

        friend bool operator==(tree_adapter const&lhs, tree_adapter const&rhs)
        {
            return *lhs.tree == *rhs.tree;
        }
    };
}

#endif //INC_201703_TREE_ADAPTER_HPP
