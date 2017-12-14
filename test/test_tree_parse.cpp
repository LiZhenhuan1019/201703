#include <string>
#include <cassert>
#include "test_tree_parse.hpp"
#include "../tree_parse.hpp"

void test_tree_parse()
{
    using namespace binary_tree_nm;
    tree_parse<std::string, left_first> parse(R"~( [ \, , \[ , "null", null, null, null, \] , null, null] )~");
    auto tree = parse.get_binary_tree();
    auto root = tree->root<preorder>();
    auto iter = root;
    assert(*root == ",");
    assert(*++iter == "[");
    assert(*++iter == "null");
    assert(*++iter == "]");
}
