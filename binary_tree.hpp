#ifndef INC_201703_BINARY_TREE_HPP
#define INC_201703_BINARY_TREE_HPP

#include <cstdlib>
#include <memory>
#include <istream>
template <typename T>
class binary_tree
{
    class node
    {
    public:
        using value_type = T;
        template <typename U>
        explicit node(U&& value, node *parent = nullptr, std::unique_ptr<node> left = nullptr, std::unique_ptr<node> right = nullptr)
            :value(std::forward<U>(value)), left_child(std::move(left)), right_child(std::move(right)), parent(parent)
        {}
        value_type value;
        std::unique_ptr<node> left_child;
        std::unique_ptr<node> right_child;
        node *parent = nullptr;
    };

public:
    using node_type = node;
    using value_type = T;
    using handler_type = std::unique_ptr<node_type>;
    using size_type = std::size_t;

    struct base_iter
    {
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_categary = std::bidirectional_iterator_tag;
    };

    class iterator;
    class const_iterator: public base_iter
    {
        friend class iterator;
        friend class binary_tree;
        const_iterator(const binary_tree *tree, node_type *p)
            :tree(tree), node(p)
        {}
        const binary_tree *tree;
        node_type *node;
    public:
    };

    class interator: public base_iter
    {
        friend class binary_tree;
        iterator(binary_tree *tree, node_type *p)
            : tree(tree), node(p)
        {}
        binary_tree *tree;
        node_type *node;
    public:
        operator const_iterator() const
        {
            return const_iterator(tree, node);
        }
    };
    binary_tree() = default;
    binary_tree(std::istream &in)
    {}

private:

    handler_type root;
    size_type size_;
};
#endif //INC_201703_BINARY_TREE_HPP
