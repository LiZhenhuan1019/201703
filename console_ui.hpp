#ifndef INC_201702_CONSOLE_UI_HPP
#define INC_201702_CONSOLE_UI_HPP

#include <iostream>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <map>
#include "tree_adapter.hpp"
#include "save_load.hpp"

namespace binary_tree_nm
{

    template <typename T>
    class console_ui
    {
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
                print_info();
                print_input(button);
                commands[button].act(*this);
            }
        }

    private:
        void exit()
        {
            quit = true;
        }

        void init()
        {
            print_error_ok(trees[current_list].InitalList());
        }

        void destroy()
        {
            print_error_ok(trees[current_list].DestroyList());
        }

        void clear()
        {
            print_error_ok(trees[current_list].ClearList());
        }

        void empty()
        {
            print_true_false(trees[current_list].ListEmpty());
        }

        void length()
        {
            if (!check_exists())
                return;
            print_value(trees[current_list].ListLength());
        }

        void get()
        {
            if (!check_exists())
                return;
            std::cout << "Please input the index of element to show.\n";
            auto pos = input_index();
            T element;
            auto status = trees[current_list].GetElem(pos, element);
            print_error_ok(status);
            if (status == OK)
            {
                std::cout << "The element ";
                print_value(element);
            }
        }

        void locate()
        {
            if (!check_exists())
                return;
            std::cout << "Please input a element to locate.\n";
            auto element = input_value<T>();
            auto pos = trees[current_list].LocateElem(element, std::equal_to<void>());
            if (pos == 0)
                print_error(ERROR);
            else
            {
                std::cout << "The located ";
                print_value(pos);
            }
        }

        void prev()
        {
            if (!check_exists())
                return;
            std::cout << "Please input the element whose previous element will be located.\n";
            auto current_element = input_value<T>();
            T previous_element;
            auto status = trees[current_list].PriorElem(current_element, previous_element);
            print_error_ok(status);
            if (status == OK)
            {
                std::cout << "The previous element ";
                print_value(previous_element);
            }
        }

        void next()
        {
            if (!check_exists())
                return;
            std::cout << "Please input the element whose next element will be located.\n";
            auto current_element = input_value<T>();
            T next_element;
            auto status = trees[current_list].NextElem(current_element, next_element);
            print_error_ok(status);
            if (status == OK)
            {
                std::cout << "The next element ";
                print_value(next_element);
            }
        }

        void insert()
        {
            if (!check_exists())
                return;
            std::cout << "Please input the index where to insert.\n";
            auto index = input_index(1);
            std::cout << "Please input the element to insert.\n";
            auto element = input_value<T>();
            auto status = trees[current_list].ListInsert(index, element);
            print_error_ok(status);
        }

        void erase()
        {
            if (!check_exists())
                return;
            std::cout << "Please input the index of element delete.\n";
            auto index = input_index();
            T result;
            auto erased_element = trees[current_list].ListDelete(index, result);
            print_error_ok(erased_element);
            if (erased_element == OK)
            {
                std::cout << "The erased ";
                print_value(result);
            }
        }

        void iterate()
        {
            auto status = trees[current_list].ListTraverse([](auto s)
                                                           { std::cout << "visit element " << s << "\n"; });
            print_error_ok(status);
        }

        void save()
        {
            std::ofstream file(save_file_name);
            file << *this;
            if (file.good())
                print_error_ok(OK);
            else
                print_error_ok(ERROR);
        }

        void load()
        {
            std::ifstream file(save_file_name);
            file >> *this;
            if (file.good())
                print_error_ok(OK);
            else
                print_error_ok(ERROR);
        }

        void add_list()
        {
            std::cout << "Please enter the position to insert the list.\n";
            std::cout << "The position should be in the range of [0, " << trees.size() + 1 << ") \n";
            auto index = input_index();
            if ((std::size_t)index > trees.size())
            {
                print_error_ok(ERROR);
                return;
            }
            trees.insert(std::next(trees.begin(), index), list_adapter < T > {});
            print_error_ok(OK);
        }

        void select_list()
        {
            std::cout << "Please enter the index of list to select.\n";
            std::cout << "The index should be in the range of [0, " << trees.size() << ") \n";
            auto index = input_index();
            if ((std::size_t)index >= trees.size())
            {
                print_error_ok(ERROR);
                return;
            }
            current_list = (std::size_t)index;
            print_error_ok(OK);
        }

        void remove_list()
        {
            if (trees.size() == 1)
                print_error_ok(ERROR);
            std::cout << "Please enter the index of list to remove.\n";
            std::cout << "The index should be in the range of [0, " << trees.size() << ") \n";
            auto input = input_index();
            if ((std::size_t)input >= trees.size())
            {
                print_error_ok(ERROR);
                return;
            }
            auto index = (std::size_t)input;
            if (index <= current_list && current_list > 0)
                --current_list;
            trees.erase(std::next(trees.begin(), index));
            print_error_ok(OK);
        }

        void print_menu()
        {
            std::cout << "Menu for linear list sample\n";
            std::cout << "---------------------------\n";
            for (auto i = 0u; i < commands.size(); ++i)
            {
                std::string item = std::to_string(i) + ". " + commands[i].name;
                std::cout << item << "\n";
            }
        }

        void print_info()
        {
            std::cout << "Current selected tree: " << current_list << "\n";
            std::cout << "Number of total trees: " << trees.size() << "\n";
        }

        void print_wait_input()
        {
            std::cout << "press the number to execute the corresponding command.\n";
        }

        static void eat_line(std::istream &i)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //跳过直到换行符, 这样就到了下一行.
        }

        auto input_index(std::size_t start = 0)
        {
            return input_value<std::size_t>(start, std::numeric_limits<int>::max() - 1);
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
            if (std::cin.bad())
                throw bad_input("irrecoverable input stream error.");
            else if (std::cin.fail())
                throw fail_input("input failed.(formatting or extraction error.)");
            else if (std::cin.eof())
                throw eof_input("input stream reached EOF.");
            print_input(input);
            return input;
        }

        void print_true_false(int status)
        {
            if (status == TRUE)
                std::cout << "result: TRUE\n";
            else if (status == FALSE)
                std::cout << "result: FALSE\n";
            else if (status == ERROR)
                std::cout << "result: ERROR\n";
        }

        void print_error_ok(int status)
        {
            if (status == OK)
                std::cout << "result: OK\n";
            else
                print_error(status);
        }

        void print_error(int status)
        {
            if (status == ERROR)
                std::cout << "result: ERROR\n";
        }

        template <typename U>
        void print_value(U const &value)
        {
            std::cout << "value: " << value << "\n";
        }

        bool check_exists()
        {
            if (trees[current_list].ListEmpty() == ERROR)
            {
                print_error_ok(ERROR);
                return false;
            }
            return true;
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
        std::size_t current_list = 0;
        std::map<std::string, binary_tree_nm::tree_adapter<T>> trees;

        inline static auto commands = make_commands(std::pair{&console_ui::exit, "Exit"},
                                                    std::pair{&console_ui::init, "InitList"},
                                                    std::pair{&console_ui::destroy, "DestroyList"},
                                                    std::pair{&console_ui::clear, "ClearList"},
                                                    std::pair{&console_ui::empty, "ListEmpty"},
                                                    std::pair{&console_ui::length, "ListLength"},
                                                    std::pair{&console_ui::get, "GetElem"},
                                                    std::pair{&console_ui::locate, "LocateElem"},
                                                    std::pair{&console_ui::prev, "PriorElem"},
                                                    std::pair{&console_ui::next, "NextElem"},
                                                    std::pair{&console_ui::insert, "ListInsert"},
                                                    std::pair{&console_ui::erase, "ListDelete"},
                                                    std::pair{&console_ui::iterate, "ListTraverse"},
                                                    std::pair{&console_ui::save, "ListSave"},
                                                    std::pair{&console_ui::load, "ListLoad"},
                                                    std::pair{&console_ui::add_list, "AddList"},
                                                    std::pair{&console_ui::select_list, "SelectList"},
                                                    std::pair{&console_ui::remove_list, "RemoveList"}
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
            out << ui.current_list << " " << ui.trees;
            return out;
        }

        friend std::istream &operator>>(std::istream &in, console_ui &ui)
        {
            in >> ui.current_list >> ui.trees;
            return in;
        }
    };
}

#endif //INC_201702_CONSOLE_UI_HPP
