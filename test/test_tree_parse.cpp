#include <string>
#include <cassert>
#include <sstream>
#include "test_tree_parse.hpp"
#include "../tree_parse.hpp"
#include "../tree_adapter.hpp"

void test_tree_parse()
{
    using namespace binary_tree_nm;
    using namespace std::literals;
    std::istringstream stream(R"~( [ (\,, 1), ([, 2), (null,3), null, null, null, (],4), null, null] )~"s);
    tree_parse<left_first_t, detail::stored_t<std::string, int>> parse(stream);
    auto tree = parse.get_binary_tree();
    auto root = tree->root<preorder_t>();
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
