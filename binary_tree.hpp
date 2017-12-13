#ifndef INC_201703_BINARY_TREE_HPP
#define INC_201703_BINARY_TREE_HPP

#include <cstdlib>
#include <memory>
#include <cassert>

namespace binary_tree
{
    struct left_first;
    struct right_first;
    using left_child = left_first;
    using right_child = right_first;

    template <typename T>
    struct node
    {
        using value_type = T;

        template <typename U>
        explicit node(U &&value, node *parent, std::unique_ptr<node> left,
                      std::unique_ptr<node> right)
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
    struct iterate_direction<left_first>
    {
        using inverse_direction = right_first;
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
    struct iterate_direction<right_first>
    {
        using inverse_direction = left_first;
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
        using inverse_order = inorder<T, typename direction::inverse_direction>;
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
            while (current->parent != nullptr && direction::second_child(current->parent).get() == current)
                current = current->parent;
            if (current->parent == nullptr)
                return nullptr;
            assert(direction::first_child(current->parent).get() == current);
            return current->parent;
        }
    };

    template <typename T, typename dir>
    struct postorder;
    template <typename T, typename dir>
    struct preorder
    {
        using node_type = node<T>;
        using direction = iterate_direction<dir>;
        using inverse_order = postorder<T, typename direction::inverse_direction>;
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
                return backtrack(current);
        }
        static node_type *backtrack(node_type *current)
        {
            while (current->parent != nullptr &&
                   (direction::second_child(current->parent).get() == current ||
                    !direction::second_child(current->parent)))
                current = current->parent;
            if (current->parent == nullptr)
                return nullptr;
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
        using inverse_order = preorder<T, typename direction::inverse_direction>;
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
            if (current->parent == nullptr)
                return nullptr;
            else if (direction::first_child(current->parent).get() == current)
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
        template <typename U, typename dir>
        using default_order = inorder<U, dir>;
        using default_direction = left_first;
    public:
        using node_type = node<T>;
        using value_type = T;
        using handler_type = std::unique_ptr<node_type>;
        using size_type = std::size_t;

    private:
        struct base_iter
        {
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_categary = std::bidirectional_iterator_tag;

        };
        explicit binary_tree(handler_type root)
            : root_(std::move(root))
        {
        }
    public:

        template <template <typename...> class, typename>
        class iterator;

        template <template <typename...> class default_order, typename default_direction>
        class const_iterator : public base_iter
        {
            template <template <typename...> class, typename>
            friend
            class iterator;
            friend class binary_tree;

            const_iterator(const binary_tree *tree, node_type *p)
                : tree(tree), node(p)
            {
            }

            const binary_tree *tree;
            node_type *node;
        public:
            template <template <typename ...> class order = default_order, typename direction = default_direction>
            const_iterator(const_iterator<order, direction> const &src)
                :tree(src.tree), node(src.node)
            {
            }
            value_type const &operator*() const
            {
                return node->value;
            }
            value_type const *operator->() const
            {
                return &node->value;
            }
            auto &operator++()
            {
                return next(), *this;
            }
            auto operator++(int)
            {
                auto iter = *this;
                return iter.next(), iter;
            }
            auto &operator--()
            {
                return previous(), *this;
            }
            auto operator--(int)
            {
                auto iter = *this;
                return iter.previous(), iter;
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void next()
            {
                assert(node);
                node = order<T, direction>::next(node);
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void previous()
            {
                if (node == nullptr)
                    node = order<T, direction>::inverse_order::begin(tree->root_.get());
                else
                {
                    auto pre = order<T, direction>::inverse_order::next(node);
                    if (pre != nullptr)
                        node = pre;
                }
            }
            template <typename direction = default_direction>
            const_iterator first_child() const
            {
                assert(node);
                return const_iterator(tree, iterate_direction<direction>::first_child(node).get());
            }
            template <typename direction = default_direction>
            const_iterator second_child() const
            {
                assert(node);
                return const_iterator(tree, iterate_direction<direction>::second_child(node).get());
            }
            const_iterator parent() const
            {
                assert(node && node->parent);
                return const_iterator(tree, node->parent);
            }
            template <template <typename ...> class order = default_order, typename direction = default_direction>
            auto change() const
            {
                return const_iterator<order, direction>(*this);
            }
            template <typename iter1, typename iter2>
            friend bool operator==(iter1 const&, iter2 const&);
        };

        template <template <typename...> class default_order, typename default_direction>
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
            template <template <typename ...> class order = default_order, typename direction = default_direction>
            iterator(iterator<order, direction> const &src)
                :tree(src.tree), node(src.node)
            {
            }
            template <template <typename ...> class order, typename direction>
            operator const_iterator<order, direction>()
            {
                return const_iterator<order, direction>(tree, node);
            };
            value_type &operator*() const
            {
                return node->value;
            }
            value_type *operator->() const
            {
                return &node->value;
            }
            auto &operator++()
            {
                return next(), *this;
            }
            auto operator++(int)
            {
                auto iter = *this;
                return iter.next(), iter;
            }
            auto &operator--()
            {
                return previous(), *this;
            }
            auto operator--(int)
            {
                auto iter = *this;
                return iter.previous(), iter;
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void next()
            {
                assert(node);
                node = order<T, direction>::next(node);
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void previous()
            {
                if (node == nullptr)
                    node = order<T, direction>::inverse_order::begin(tree->root_.get());
                else
                {
                    auto pre = order<T, direction>::inverse_order::next(node);
                    if (pre != nullptr)
                        node = pre;
                }
            }
            template <typename direction = default_direction>
            iterator first_child() const
            {
                assert(node);
                return iterator(tree, iterate_direction<direction>::first_child(node).get());
            }
            template <typename direction = default_direction>
            iterator second_child() const
            {
                assert(node);
                return iterator(tree, iterate_direction<direction>::second_child(node).get());
            }
            iterator parent() const
            {
                assert(node && node->parent);
                return iterator(tree, node->parent);
            }
            template <template <typename ...> class order = default_order, typename direction = default_direction>
            auto change() const
            {
                return iterator<order, direction>(*this);
            }

            template <typename iter1, typename iter2>
            friend bool operator==(iter1 const&, iter2 const&);
        };

        binary_tree() = default;

        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto begin()
        {
            return get_iter<order, direction>(order<T, direction>::begin(root_.get()));
        }

        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto begin() const
        {
            return get_const_iter<order, direction>(order<T, direction>::begin(root_.get()));
        }
        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto cbegin() const
        {
            return begin<order, direction>();
        }

        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto end()
        {
            return get_iter<order, direction>(nullptr);
        }
        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto end() const
        {
            return get_const_iter<order, direction>(nullptr);
        }
        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto cend() const
        {
            return get_const_iter<order, direction>(nullptr);
        }

        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto root()
        {
            return get_iter<order, direction>(root_.get());
        }
        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto root() const
        {
            return get_const_iter<order, direction>(root_.get());
        }

        template <typename iter>
        binary_tree replace(iter replaced, binary_tree &&new_tree)
        {
            handler_type *handler = nullptr;
            if (replaced == root())
                handler = &root_;
            else
                handler = &get_handler(replaced.node);
            auto returned = std::move(*handler);
            *handler = std::move(new_tree.root_);
            return binary_tree(returned);
        }

        template <typename iter>
        binary_tree remove(iter subtree)
        {
            return replace(subtree, binary_tree{});
        }

        template <typename ...Args>
        void set_root(Args &&...args)
        {
            root_ = make_handler(T(std::forward<Args>(args)...), nullptr);
        }
        template <typename direction = default_direction, typename iter, typename ...Args>
        iter set_child(iter parent, Args &&...args)
        {
            auto &child = iterate_direction<direction>::first_child(parent.node);
            child = make_handler(T(std::forward<Args>(args)...), parent.node);
            return iter(this, child.get());
        }

    private:

        template <template <typename...> class default_order, typename default_direction>
        auto get_iter(node_type *p)
        {
            return iterator<default_order, default_direction>{this, p};
        }
        template <template <typename...> class default_order, typename default_direction>
        auto get_const_iter(node_type *p) const
        {
            return const_iterator<default_order, default_direction>{this, p};
        }
        auto &get_handler(node_type *p)
        {
            if (p->parent->left_child.get() == p)
                return p->parent->left_child;
            else
                return p->parent->right_child;
        }
        auto make_handler(T &&t, node_type *parent = nullptr, handler_type left = nullptr, handler_type right = nullptr)
        {
            return std::make_unique<node_type>(std::move(t), parent, std::move(left), std::move(right));
        }
        handler_type root_;
    };

    template <typename iter1, typename iter2>
    bool operator==(iter1 const &lhs, iter2 const &rhs)
    {
        return lhs.node == rhs.node;
    }
}

#endif //INC_201703_BINARY_TREE_HPP
