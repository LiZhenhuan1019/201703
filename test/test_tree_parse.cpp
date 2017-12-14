#include <string>
#include <cassert>
#include "test_tree_parse.hpp"
#include "../tree_parse.hpp"

void test_tree_parse()
{
    using namespace binary_tree_nm;
    tree_parse<left_first, std::string, int> parse(R"~( [ (\,, 1), (\[, 2), (null,3), null, null, null, (\],4), null, null] )~");
    auto tree = parse.get_binary_tree();
    auto root = tree->root<preorder>();
    auto iter = root;
    assert(root->key == ",");
    assert(root->value == 1);
    ++iter;
    assert(iter->key == "[");
    assert(iter->value== 2);
    ++iter;
    assert(iter->key == "null");
    assert(iter->value== 3);
    ++iter;
    assert(iter->key == "]");
    assert(iter->value== 4);
}
