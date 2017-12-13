#ifndef INC_201703_BINARY_TREE_HPP
#define INC_201703_BINARY_TREE_HPP

#include <cstdlib>
#include <memory>
#include <istream>
#include <cassert>

namespace binary_tree
{
    struct left_to_right;
    struct right_to_left;

    template <typename T>
    struct node
    {
        using value_type = T;

        template <typename U>
        explicit node(U &&value, node *parent = nullptr, std::unique_ptr<node> left = nullptr,
                      std::unique_ptr<node> right = nullptr)
            :value(std::forward<U>(value)), left_child(std::move(left)), right_child(std::move(right)), parent(parent)
        {
        }

        value_type value;
        std::unique_ptr<node> left_child;
        std::unique_ptr<node> right_child;
        node *parent = nullptr;
    };

    template <typename direction_tag>
    struct iterate_direction;

    template <>
    struct iterate_direction<left_to_right>
    {
        ~iterate_direction() = default;
        template <typename T>
        static auto &first_child(node<T> *p)
        {
            return p->left_child;
        }
        template <typename T>
        static auto &second_child(node<T> *p)
        {
            return p->right_child;
        }
    };

    template <>
    struct iterate_direction<right_to_left>
    {
        ~iterate_direction() = delete;
        template <typename T>
        static auto &first_child(node<T> *p)
        {
            return p->right_child;
        }
        template <typename T>
        static auto &second_child(node<T> *p)
        {
            return p->left_child;
        }
    };

    template <typename T, typename dir>
    struct inorder
    {
        using node_type = node<T>;
        using direction = iterate_direction<dir>;
        static node_type *begin(node_type *root)
        {
            assert(root);
            auto current = root;
            while (direction::first_child(current))
                current = direction::first_child(current).get();
            return current;
        }
        static node_type *next(node_type *current)
        {
            assert(current);
            if (direction::second_child(current))
                return begin(direction::second_child(current).get());
            else
                return backtrack(current);
        }
        static node_type *backtrack(node_type *current)
        {
            while (direction::second_child(current->parent).get() == current)
                current = current->parent;
            assert(direction::first_child(current->parent).get() == current);
            return current->parent;
        }
    };

    template <typename T, typename dir>
    struct preorder
    {
        using node_type = node<T>;
        using direction = iterate_direction<dir>;
        static node_type *begin(node_type *root)
        {
            assert(root);
            return root;
        }
        static node_type *next(node_type *current)
        {
            assert(current);
            if (direction::first_child(current))
                return direction::first_child(current).get();
            else if (direction::second_child(current))
                return direction::second_child(current).get();
            else
                return direction::second_child(backtrack(current)).get();
        }
        static node_type *backtrack(node_type *current)
        {
            while (direction::second_child(current->parent).get() == current ||
                   !direction::second_child(current->parent))
                current = current->parent;
            assert(direction::first_child(current->parent).get() == current &&
                   direction::second_child(current->parent) != nullptr);
            return direction::second_child(current->parent).get();
        }
    };

    template <typename T, typename dir>
    struct postorder
    {
        using node_type = node<T>;
        using direction = iterate_direction<dir>;
        static node_type *begin(node_type *root)
        {
            assert(root);
            auto current = root;
            while (direction::first_child(current))
                current = direction::first_child(current).get();
            while (direction::second_child(current))
            {
                current = direction::second_child(current).get();
                while (direction::first_child(current))
                    current = direction::first_child(current).get();
            }
            return current;
        }
        static node_type *next(node_type *current)
        {
            assert(current);
            if (direction::first_child(current->parent).get() == current)
                return begin(direction::second_child(current->parent).get());
            else
            {
                assert(direction::second_child(current->parent).get() == current);
                return current->parent;
            }
        }
    };

    template <typename T>
    class binary_tree
    {

    public:
        using node_type = node<T>;
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

        class const_iterator : public base_iter
        {
            friend class iterator;

            friend class binary_tree;

            const_iterator(const binary_tree *tree, node_type *p)
                : tree(tree), node(p)
            {
            }

            const binary_tree *tree;
            node_type *node;
        public:
        };

        class iterator : public base_iter
        {
            friend class binary_tree;

            iterator(binary_tree *tree, node_type *p)
                : tree(tree), node(p)
            {
            }

            binary_tree *tree;
            node_type *node;
        public:
            operator const_iterator() const
            {
                return const_iterator(tree, node);
            }

            template <template <typename, typename> class order = inorder, typename direction = left_to_right>
            void next()
            {
                if (node != nullptr)
                    node = order<T, direction>::next(node);
            }
        };

        binary_tree() = default;

        explicit binary_tree(std::istream &in)
        {
        }

        template <template <typename, typename> class order = inorder, typename direction = left_to_right>
        iterator begin()
        {
            return get_iter(order<T, direction>::begin(root.get()));
        }


    private:

        iterator get_iter(node_type *p)
        {
            return iterator{this, p};
        }
        handler_type root;
        size_type size_ = 0;
    };


}

#endif //INC_201703_BINARY_TREE_HPP
