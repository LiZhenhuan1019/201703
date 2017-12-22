#ifndef INC_201702_CONSOLE_UI_HPP
#define INC_201702_CONSOLE_UI_HPP

#include <iostream>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <map>
#include "tree_adapter.hpp"
#include "save_load.hpp"

namespace ds_exp
{
    inline namespace ui
    {
        template <typename Key, typename Value = null_value_tag>
        class console_ui
        {
            using tree_type = tree_adapter<Key, Value>;
            using map_type = std::map<std::string, tree_type>;
            using key_type = typename tree_type::key_type;
            using value_type = typename tree_type::value_type;
            using iterator_type = decltype(std::declval<tree_type>().Parent(std::declval<key_type>()));
            struct bad_input : std::runtime_error
            {
                using std::runtime_error::runtime_error;
            };
            struct fail_input : std::runtime_error
            {
                using std::runtime_error::runtime_error;
            };
            struct eof_input : std::runtime_error
            {
                using std::runtime_error::runtime_error;
            };
        public:
            console_ui()
            {
                trees[current_tree_name] = tree_type{};
            }

            void execute()
            {
                clear_screen();
                print_menu();
                print_info();
                while (!quit)
                {
                    print_wait_input();
                    auto button = input_value(0, (int)commands.size());
                    clear_screen();
                    print_menu();
                    print_input(button);
                    try
                    {
                        commands[button].act(*this);
                    }
                    catch (std::logic_error const &e)
                    {
                        std::cout << "exception caught: " << e.what() << "\n";
                    }
                    print_info();

                }
            }

        private:
            void exit()
            {
                quit = true;
            }

            void init()
            {
                trees[current_tree_name].InitBiTree();
                print_ok();
            }

            void destroy()
            {
                trees[current_tree_name].DestroyBiTree();
                print_ok();
            }

            void create()
            {
                std::cout << "Please input the definition of the tree to create.\n";
                std::string_view syntax_prompt = "语法：定义以'['开始，以']'结束，二者之间为以','分隔的列表;\n"
                                                 "列表中每一个元素可以是：'null' -- 表示结点不存在;\n"
                                                 "                      以'('开始，以')'结束，中间以','分隔的键值对;\n"
                                                 "键与值均为字符串，但若其中包含会产生歧义的字符则需在之前添加'\\'进行转义。\n"
                                                 "构造出的树进行前序遍历得到的序列和列表中键值对的顺序相同。\n"
                                                 "空格可在任意地方添加,但键值对中的空格将被视为键值对的一部分。\n"
                                                 "例子： [ (root,root value), (left,left value) ,(left left,2), null,null,null,(right,right value),null , null]\n";
                std::cout << syntax_prompt;
                auto definition = input_line<std::string>();
                tree_type new_tree;
                trees[current_tree_name].CreateBiTree(definition);
                print_ok();
            }

            void clear()
            {
                trees[current_tree_name].ClearBiTree();
                print_ok();
            }

            void empty()
            {
                print_value(trees[current_tree_name].BiTreeEmpty());
                print_ok();
            }

            void depth()
            {
                print_value(trees[current_tree_name].BiTreeDepth());
                print_ok();
            }

            void root()
            {
                auto root = trees[current_tree_name].Root();
                std::cout << "The content of root : " << *root << "\n";
                print_ok();
            }

            void get()
            {
                std::cout << "Please input the element to show.\n";
                auto element = input_line<key_type>();
                auto &value = trees[current_tree_name].Value(element);
                std::cout << "The element ";
                print_value(value);
                print_ok();
            }

            void assign()
            {
                std::cout << "Please input the element to change.\n";
                auto element = input_line<key_type>();
                std::cout << "Please input the value to change to.\n";
                auto value = input_line<value_type>();
                trees[current_tree_name].Assign(element, value);
                print_ok();
            }

            void parent()
            {
                std::cout << "Please input the child.\n";
                auto element = input_line<key_type>();
                auto parent = trees[current_tree_name].Parent(element);
                if (!parent)
                    print_null();
                else
                    print_value(*parent);
                print_ok();
            }

            void child()
            {
                std::cout << "Please input the element whose child will be shown.\n";
                auto element = input_line<key_type>();
                std::cout << "Please select left or right child to show.(0 --> left, nonzero --> right)\n";
                auto select_right = input_value<int>();
                iterator_type child = trees[current_tree_name].get_end_iterator();
                if (select_right)
                    child = trees[current_tree_name].Child(element, right_child);
                else
                    child = trees[current_tree_name].Child(element, left_child);
                print_value(*child);
                print_ok();
            }

            void sibling()
            {
                std::cout << "Please input the element whose sibling will be shown.\n";
                auto element = input_line<key_type>();
                std::cout << "Please select left or right sibling to show.(0 -->left, nozero --> right)\n";
                auto select_right = input_value<int>();
                iterator_type sibling = trees[current_tree_name].get_end_iterator();
                if (select_right)
                    sibling = trees[current_tree_name].Sibling(element, right_child);
                else
                    sibling = trees[current_tree_name].Sibling(element, left_child);
                if (!sibling)
                    print_null();
                else
                {
                    print_value(*sibling);
                    print_ok();
                }
            }

            void insert()
            {
                std::cout << "Please input the element.\n";
                auto element = input_line<key_type>();
                auto iter = trees[current_tree_name].get_iterator(element);
                std::cout << "Please select left or right child to replace.(0 -->left, nozero --> right)\n";
                auto select_right = input_value<int>();
                std::cout << "Please input the definition of the tree to insert.\n";
                auto definition = input_line<std::string>();
                tree_type new_tree;
                new_tree.CreateBiTree(definition);
                if (select_right)
                    trees[current_tree_name].InsertChild(iter, std::move(new_tree), right_child);
                else
                    trees[current_tree_name].InsertChild(iter, std::move(new_tree), left_child);
                print_ok();
            }

            void erase()
            {
                std::cout << "Please input the parent element.\n";
                auto element = input_line<key_type>();
                auto iter = trees[current_tree_name].get_iterator(element);
                std::cout << "Please select left or right child to replace.(0 -->left, nozero --> right)\n";
                auto select_right = input_value<int>();
                tree_type deleted_tree;
                if (select_right)
                    deleted_tree = trees[current_tree_name].DeleteChild(iter, right_child);
                else
                    deleted_tree = trees[current_tree_name].DeleteChild(iter, left_child);
                print_ok();
            }

            void preorder_iterate()
            {
                trees[current_tree_name].Traverse([this](auto const &element)
                                                  {
                                                      std::cout << "visit element ";
                                                      print_value(element);
                                                  }, preorder);
                print_ok();
            }
            void inorder_iterate()
            {
                trees[current_tree_name].Traverse([this](auto const &element)
                                                  {
                                                      std::cout << "visit element ";
                                                      print_value(element);
                                                  }, inorder);
                print_ok();
            }
            void postorder_iterate()
            {
                trees[current_tree_name].Traverse([this](auto const &element)
                                                  {
                                                      std::cout << "visit element ";
                                                      print_value(element);
                                                  }, postorder);
                print_ok();
            }
            void levelorder_iterate()
            {
                trees[current_tree_name].LevelOrderTraverse([this](auto const &element)
                                                            {
                                                                std::cout << "visit element ";
                                                                print_value(element);
                                                            });
                print_ok();
            }

            void save()
            {
                std::ofstream file(save_file_name);
                file << *this;
                if (file.good())
                    print_ok();
                else
                    print_error();
            }

            void load()
            {
                std::ifstream file(save_file_name);
                file >> *this;
                if (file.good())
                    print_ok();
                else
                    print_error();
            }

            void add_tree()
            {
                std::cout << "Please enter the name of the tree to add.\n";
                auto name = input_line<std::string>();
                auto iter = trees.find(name);
                if (iter != trees.end())
                {
                    return print_error();
                }
                trees.insert(std::pair{name, tree_type{}});
                print_ok();
            }

            void select_tree()
            {
                std::cout << "Please enter the name of tree to select.\n";
                auto name = input_line<std::string>();
                auto iter = trees.find(name);
                if (iter != trees.end())
                {
                    current_tree_name = name;
                    return print_ok();
                }
                print_error();
            }

            void remove_tree()
            {
                if (trees.size() == 1)
                    print_error();
                std::cout << "Please enter the name of tree to remove.\n";
                auto name = input_line<std::string>();
                auto iter = trees.find(name);
                if (iter != trees.end())
                {
                    trees.erase(iter);
                    if (name == current_tree_name)
                        current_tree_name = trees.begin()->first;
                    return print_ok();
                }
                print_error();
            }

            void print_menu()
            {
                std::cout << "Menu for binary tree sample\n";
                std::cout << "---------------------------\n";
                for (auto i = 0u; i < commands.size(); ++i)
                {
                    std::string item = std::to_string(i) + ". " + commands[i].name;
                    std::cout << item << "\n";
                }
            }

            void print_info()
            {
                std::cout << "Current selected tree: " << current_tree_name << "\n";
                std::cout << "Number of total trees: " << trees.size() << "\n";
            }

            void print_wait_input()
            {
                std::cout << "press the number to execute the corresponding command.\n";
            }

            static void eat_line(std::istream &i)
            {
                i.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //跳过直到换行符, 这样就到了下一行.
            }

            auto input_index(std::size_t start = 0)
            {
                return input_value<std::size_t>(start, std::numeric_limits<int>::max() - 1);
            }

            template <typename U>
            static auto input_line(std::istream &in = std::cin)
            {
                std::string str;
                getline(in, str);
                U u;
                assign_element(str, u);
                return u;
            }
            template <typename U>
            static void input_line(std::istream &in, U &u)
            {
                u = input_line<U>(in);
            }

            template <typename U>
            static auto input_value()
            {
                while (true)
                {
                    try
                    {
                        U input = wait_for_input<U>();
                        return input;
                    }
                    catch (fail_input const &)
                    {
                        std::cout << "Please input a valid value!\n";
                        std::cin.clear();
                        eat_line(std::cin);
                    }
                }
            }

            template <typename U>
            static auto input_value(U const &lower_bound, U const &upper_bound)
            {
                assert(lower_bound < upper_bound);
                std::cout << "The value should be in the range of [" << lower_bound << ", " << upper_bound << ").\n";
                auto input = input_value<U>();
                while (input < lower_bound || input >= upper_bound)
                {
                    std::cout << "The value is out of range, please input again.\n";
                    std::cout << "The value should be in the range of [" << lower_bound << ", " << upper_bound << ").\n";
                    input = input_value<U>();
                }
                return input;
            }

            template <typename U>
            static auto wait_for_input()
            {
                U input;
                std::cin >> input;
                eat_line(std::cin);
                if (std::cin.bad())
                    throw bad_input("irrecoverable input stream error.");
                else if (std::cin.fail())
                    throw fail_input("input failed.(formatting or extraction error.)");
                else if (std::cin.eof())
                    throw eof_input("input stream reached EOF.");
                print_input(input);
                return input;
            }

            void print_ok()
            {
                std::cout << "result :OK\n";
            }

            void print_error()
            {
                std::cout << "result: ERROR\n";
            }

            void print_null()
            {
                std::cout << "result : null\n";
            }

            template <typename U>
            void print_value(U const &value)
            {
                std::cout << "value: " << value << "\n";
            }

            using action = decltype(std::mem_fn(&console_ui::exit));
            struct command
            {
                action act;
                char const *name;
            };

            template <typename ...T1, typename ...T2>
            static std::vector<command> make_commands(std::pair<T1, T2> &&...t)
            {
                return {command{std::mem_fn(t.first), t.second}...};
            }

            bool quit = false;
            std::string current_tree_name = "default";
            map_type trees;

            inline static auto commands = make_commands(std::pair{&console_ui::exit, "Exit"},
                                                        std::pair{&console_ui::init, "InitBiTree"},
                                                        std::pair{&console_ui::destroy, "DestroyBiTree"},
                                                        std::pair{&console_ui::create, "CreateBiTree"},
                                                        std::pair{&console_ui::clear, "ClearBiTree"},
                                                        std::pair{&console_ui::empty, "BiTreeEmpty"},
                                                        std::pair{&console_ui::depth, "BiTreeDepth"},
                                                        std::pair{&console_ui::root, "Root"},
                                                        std::pair{&console_ui::get, "Value"},
                                                        std::pair{&console_ui::assign, "Assign"},
                                                        std::pair{&console_ui::parent, "Parent"},
                                                        std::pair{&console_ui::child, "Child"},
                                                        std::pair{&console_ui::sibling, "Sibling"},
                                                        std::pair{&console_ui::insert, "InsertChild"},
                                                        std::pair{&console_ui::erase, "DeleteChild"},
                                                        std::pair{&console_ui::preorder_iterate, "PreorderIterate"},
                                                        std::pair{&console_ui::inorder_iterate, "InorderIterate"},
                                                        std::pair{&console_ui::postorder_iterate, "PostorderIterate"},
                                                        std::pair{&console_ui::levelorder_iterate, "LevelOrderIterate"},
                                                        std::pair{&console_ui::save, "Save"},
                                                        std::pair{&console_ui::load, "Load"},
                                                        std::pair{&console_ui::add_tree, "AddTree"},
                                                        std::pair{&console_ui::select_tree, "SelectTree"},
                                                        std::pair{&console_ui::remove_tree, "RemoveTree"}
            );
            inline static const std::string save_file_name = "data.save";

            template <typename U>
            static void print_input(U const &value)
            {
                std::cout << "Input : " << value << "\n";
            }

            static void clear_screen()
            {
                std::cout << "\x1B[2J\x1B[H";
            }

            friend std::ostream &operator<<(std::ostream &out, console_ui const &ui)
            {
                out << ui.current_tree_name << "\n";
                out << ui.trees.size() << "\n";
                for (auto iter = ui.trees.begin(); iter != ui.trees.end(); ++iter)
                    out << iter->first << "\n" << iter->second << "\n";
                return out;
            }

            friend std::istream &operator>>(std::istream &in, console_ui &ui)
            {
                ui.trees.clear();
                std::string str;
                ui.input_line(in, ui.current_tree_name);
                std::size_t size = 0;
                ui.input_line(in, size);
                for (std::size_t i = 0; i < size; ++i)
                {
                    auto name = ui.input_line<std::string>(in);
                    auto tree = ui.input_line<console_ui::tree_type>(in);
                    ui.trees[name] = tree;
                }
                return in;
            }
        };
    }

}

#endif //INC_201702_CONSOLE_UI_HPP
