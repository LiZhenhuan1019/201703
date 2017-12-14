#include <string>
#include "test_tree_parse.hpp"
#include "../tree_parse.hpp"

void test_tree_parse()
{
    using namespace binary_tree;
    tree_parse<std::string, preorder, left_first> parse("");
    auto tree = parse.get_binary_tree();
}
