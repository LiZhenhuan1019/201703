#include "test_tree_adapter.hpp"
#include "../tree_adapter.hpp"

void test_tree_adapter()
{
    using namespace binary_tree_nm;
    tree_adapter<std::string, int> adapter;
    std::string_view definition= R"~([(root,1), (left,2), (left left,3), null, null, null , (right,4), null, (right right, 5), null, null ])~";
    adapter.InitBiTree();
    adapter.DestroyBiTree();
    adapter.CreateBiTree(definition);
    adapter.ClearBiTree();
    assert(adapter.BiTreeEmpty());
    adapter.CreateBiTree(definition);
    assert(adapter.BiTreeDepth() == 3);
    assert(get_value(*adapter.Root()) == 1);
    auto value = adapter.Value("left left");
    assert(value == 3);
    adapter.Assign("left", 5);
    assert(adapter.Value("left") == 5);
    adapter.Assign("left", 2);
    assert(adapter.Parent("left") == adapter.Root());
    assert(get_value(*adapter.Child<left_t>("left")) == 3);
    assert(get_value(*adapter.Child<right_t>("right")) == 5);
    assert(get_value(*adapter.Sibling<right_t>("left")) == 4);
    assert(get_value(*adapter.Sibling<left_t>("right")) == 2);
    auto right_node = adapter.Child<right_t>("root");
    adapter.InsertChild<right_t>(right_node, tree_parse<left_first_t, detail::stored_t<std::string, int>>(definition).get_binary_tree().value());
    decltype(adapter) equals;
    equals.CreateBiTree(R"~([(root, 1), (left, 2),(left left,3),null,null,null,(right,4),null, (root, 1), (left,2),(left left,3),null,null,null,(right,4),null,(right right, 5),null, (right right,5),null,null])~");
    assert(adapter == equals);
    auto parent_of_replaced = adapter.Child("right", right_child, inorder, right_first);
    assert(get_key(*parent_of_replaced) == "right right");
    auto replaced = adapter.DeleteChild<right_t>(parent_of_replaced);
    adapter.DeleteChild<right_t>(right_node);
    adapter.InsertChild<right_t>(right_node, replaced);
    equals.CreateBiTree(definition);
    assert(adapter == equals);
}
