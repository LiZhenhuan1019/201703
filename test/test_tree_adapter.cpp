#include "test_tree_adapter.hpp"
#include "../tree_adapter.hpp"

void test_tree_adapter()
{
    using namespace binary_tree_nm;
    tree_adapter<std::string> adapter;
    adapter.InitBiTree();
    adapter.DestroyBiTree();
    adapter.CreateBiTree("[root, left, left left, null, null, null , right, null, right right, null, null ]");
}
