#ifndef INC_201703_BINARY_TREE_HPP
#define INC_201703_BINARY_TREE_HPP

#include <cstdlib>
#include <memory>
#include <cassert>

namespace binary_tree_nm
{
    struct right_first;
    struct left_first
    {
        using inverse = right_first;
    };
    struct right_first
    {
        using inverse = left_first;
    };
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
        using inverse_order = inorder<T, typename dir::inverse>;
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
        using inverse_order = postorder<T, typename dir::inverse>;
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
        using inverse_order = preorder<T, typename dir::inverse>;
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

    class null_value_tag;
    namespace detail
    {
        template <typename Key, typename Value>
        struct stored_t
        {
            Key key;
            Value value;
            friend bool operator<(stored_t const &lhs, stored_t const &rhs)
            {
                return lhs.key < rhs.key;
            }
            friend std::istream &operator>>(std::istream &in, stored_t &s)
            {
                in >> s.key >> s.value;
                return in;
            }
            friend std::ostream &operator<<(std::ostream &out, stored_t const &s)
            {
                out << s.key <<" " << s.value;
                return out;
            }
        };
        template <typename Key, typename Value>
        struct value_t
        {
            using type = stored_t<Key, Value>;
        };
        template <typename Key>
        struct value_t<Key, null_value_tag>
        {
            using type = Key;
        };
    }
    template <typename Key, typename Value = null_value_tag>
    class binary_tree
    {
        template <typename U, typename dir>
        using default_order = inorder<U, dir>;
        using default_direction = left_first;
    public:
        using value_type = typename detail::value_t<Key, Value>::type;
        using node_type = node<value_type>;
        using handler_type = std::unique_ptr<node_type>;
        using size_type = std::size_t;

    private:
        struct base_iter
        {
            using difference_type = std::ptrdiff_t;
            using value_type = binary_tree::value_type;
            using pointer = value_type *;
            using reference = value_type &;
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
            explicit operator bool() const
            {
                return node != nullptr;
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
                return this->next(), iter;
            }
            auto &operator--()
            {
                return previous(), *this;
            }
            auto operator--(int)
            {
                auto iter = *this;
                return this->previous(), iter;
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void next()
            {
                assert(node);
                node = order<value_type, direction>::next(node);
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void previous()
            {
                if (node == nullptr)
                    node = order<value_type, direction>::inverse_order::begin(tree->root_.get());
                else
                {
                    auto pre = order<value_type, direction>::inverse_order::next(node);
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
            friend bool operator==(iter1 const &, iter2 const &);
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
            }
            explicit operator bool() const
            {
                return node != nullptr;
            }
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
                return this->next(), iter;
            }
            auto &operator--()
            {
                return previous(), *this;
            }
            auto operator--(int)
            {
                auto iter = *this;
                return this->previous(), iter;
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void next()
            {
                assert(node);
                node = order<value_type, direction>::next(node);
            }
            template <template <typename...> class order = default_order, typename direction = default_direction>
            void previous()
            {
                if (node == nullptr)
                    node = order<value_type, direction>::inverse_order::begin(tree->root_.get());
                else
                {
                    auto pre = order<value_type, direction>::inverse_order::next(node);
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
            friend bool operator==(iter1 const &, iter2 const &);
        };

        binary_tree() = default;
        binary_tree(binary_tree &&src) = default;
        binary_tree(binary_tree const &src)
        {
            set_root(*src.root());
            for (auto src_iter = src.begin<preorder>(), dest_iter = cbegin<preorder>(); src_iter != src.end(); ++src_iter, ++dest_iter)
            {
                if (src_iter.first_child())
                    new_child<default_direction>(dest_iter, *src_iter.first_child());
                if (src_iter.second_child())
                    new_child<default_direction::inverse>(dest_iter, *src_iter.second_child());
            }
        }
        binary_tree &operator=(binary_tree &&) = default;
        binary_tree &operator=(binary_tree const &src)
        {
            *this = binary_tree(src);
            return *this;
        }

        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto begin()
        {
            if (!root_)
                return end<order, direction>();
            return get_iter<order, direction>(order<value_type , direction>::begin(root_.get()));
        }

        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto begin() const
        {
            if (!root_)
                return end<order, direction>();
            return get_const_iter<order, direction>(order<value_type , direction>::begin(root_.get()));
        }
        template <template <typename, typename> class order = default_order, typename direction = default_direction>
        auto cbegin() const
        {
            if (!root_)
                return end<order, direction>();
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

        void clear()
        {
            root_.reset();
        }
        bool empty() const
        {
            return root() == end();
        }
        std::size_t depth() const
        {
            return subtree_depth(root());
        }
        template <typename iter>
        std::size_t subtree_depth(iter subtree_root) const
        {
            if (subtree_root == end())
                return 0;
            return std::max(subtree_depth(subtree_root.first_child()), subtree_depth(subtree_root.second_child()));
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
            root_ = make_handler(value_type(std::forward<Args>(args)...), nullptr);
        }
        template <typename direction, typename iter, typename ...Args>
        iter new_child(iter parent, Args &&...args)
        {
            auto &child = iterate_direction<direction>::first_child(parent.node);
            child = make_handler(value_type(std::forward<Args>(args)...), parent.node);
            return iter(this, child.get());
        }
        template <typename direction, typename iter>
        iter replace_child(iter parent, binary_tree &&tree)
        {
            auto &child = iterate_direction<direction>::first_child(parent.node);
            child = std::move(tree.root_);
            child->parent = parent.node;
            return iter(this, child.get());
        }

        friend bool operator==(binary_tree const &lhs, binary_tree const &rhs)
        {
            auto left_iter = lhs.template begin<preorder>(), right_iter = rhs.template begin<preorder>();
            for (; left_iter != lhs.end() && right_iter != rhs.end(); ++left_iter, ++right_iter)
            {
                if (*left_iter != *right_iter)
                    return false;
            }
            return left_iter == right_iter;
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
        auto make_handler(value_type &&t, node_type *parent = nullptr, handler_type left = nullptr, handler_type right = nullptr)
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
    template <typename iter1, typename iter2>
    bool operator!=(iter1 const &lhs, iter2 const &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif //INC_201703_BINARY_TREE_HPP
