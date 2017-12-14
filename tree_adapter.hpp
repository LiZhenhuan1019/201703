#ifndef INC_201703_TREE_ADAPTER_HPP
#define INC_201703_TREE_ADAPTER_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include "binary_tree.hpp"
#include "tree_parse.hpp"

namespace binary_tree
{
    using namespace std::literals;
    template <typename T>
    class tree_adapter
    {
        std::optional<binary_tree<T>> tree;

    public:
        struct tree_exists : std::logic_error
        {
            explicit tree_exists(std::string function)
                : logic_error("The tree already exists in function"s + std::move(function) + ".")
            {
            }
        };
        struct tree_not_exist : std::logic_error
        {
            explicit tree_not_exist(std::string function)
                : logic_error("The tree doesn't exist in function"s + std::move(function) + ".")
            {
            }
        };

        void InitBiTree()
        {
            if (tree)
                throw tree_exists(__func__);
            tree = binary_tree<T>();
        }
        void DestroyBiTree()
        {
            if(!tree)
                throw tree_not_exist(__func__);
            tree.reset();
        }
        void CreateBiTree(std::string const&str)
        {
            if(!tree)
                throw tree_not_exist(__func__);

        }

    };
}

#endif //INC_201703_TREE_ADAPTER_HPP
