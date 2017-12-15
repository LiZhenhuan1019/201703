#ifndef INC_201703_TREE_ADAPTER_HPP
#define INC_201703_TREE_ADAPTER_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include "binary_tree.hpp"
#include "tree_parse.hpp"

namespace binary_tree_nm
{
    using namespace std::literals;

    template <typename Key_t, typename Value_t = null_value_tag>
    class tree_adapter
    {
        using tree_type = binary_tree<Key_t, Value_t>;
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
            tree = binary_tree<Key_t, Value_t>();
        }
        void DestroyBiTree()
        {
            if (!tree)
                throw tree_not_exist(__func__);
            tree.reset();
        }
        void CreateBiTree(std::string const &definition)
        {
            auto generated_tree = tree_parse<left_first_t, Key_t, Value_t>(definition).get_binary_tree();
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
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->end(), key))
                return get_value(*iter);
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename U, typename order_t = preorder_t, typename dir_t = left_first_t>
        void Assign(key_type const &key, U &&value, order_t = order_t{}, dir_t = dir_t{})
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->end(), key))
                get_value(*iter) = std::forward<U>(value);
            else
                throw precondition_failed_to_satisfy(__func__);
        }

        template <typename order_t = preorder_t, typename dir_t = left_first_t>
        auto Parent(key_type const &key, order_t = order_t{}, dir_t = dir_t{}) const
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->end(), key))
                return iter.parent();
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename child_t, typename order_t = preorder_t, typename dir_t = left_first_t>
        auto Child(key_type const &key, child_t = child_t{}, order_t = order_t{}, dir_t = dir_t{}) const
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->end(), key))
                return iter.template first_child<child_t>();
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename child_t, typename order_t = preorder_t, typename dir_t = left_first_t>
        auto Sibling(key_type const &key, child_t = child_t{}, order_t = order_t{}, dir_t = dir_t{}) const
        {
            if (auto iter = std::find(tree->template begin<order_t, dir_t>(), tree->end(), key))
            {
                auto desired_child = iter.parent().template first_child<child_t>();
                if (desired_child == iter)
                    return tree->template end<order_t, dir_t>();
                return desired_child;
            }
            throw precondition_failed_to_satisfy(__func__);
        }
        template <typename child_t, typename iter, typename dir_t = right_t>
        void InsertChild(iter pos, tree_type tree, dir_t = dir_t{})
        {
            auto replaced = tree->template replace_child<child_t>(pos, std::move(tree));
            auto farest = tree->template begin<inorder_t, dir_t>();
            auto empty = tree->template replace_child<dir_t>(farest, std::move(replaced));
            assert(empty.empty());
        }
    };
}

#endif //INC_201703_TREE_ADAPTER_HPP
