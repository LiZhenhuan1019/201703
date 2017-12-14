#include "test_tree_adapter.hpp"
#include "../tree_adapter.hpp"

void test_tree_adapter()
{
    using namespace binary_tree_nm;
    tree_adapter<std::string, int> adapter;
    adapter.InitBiTree();
    adapter.DestroyBiTree();
    adapter.CreateBiTree(R"~([(root,1), (left,2), (left left,3), null, null, null , (right,4), null, (right right, 5), null, null ])~");
    adapter.ClearBiTree();
    adapter.BiTreeEmpty();
    adapter.BiTreeDepth();
    adapter.Root();
}
