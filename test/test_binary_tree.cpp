#include <string>
#include "test_binary_tree.hpp"
#include "../binary_tree.hpp"
void test_binary_tree()
{
    binary_tree::binary_tree<std::string> tree;
    auto iter = tree.begin();
    iter.next();
}
