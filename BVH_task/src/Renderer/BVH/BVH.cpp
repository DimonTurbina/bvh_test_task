#include <BVH/BVH.h>

BvhNodeTree* Tree::createTree(BvhNode item, BvhNodeTree* last, bool isLeft)
{
    {
        if (item.child0 == 4294967295) {
            /*return new BvhNodeTree(item.update, item.aabb0_min_or_v0, item.aabb0_max_or_v1, item.aabb1_min_or_v2, item.aabb1_max_or_v3,
                nullptr, nullptr, getNodeByIndex(item.parent), BvhArray[item.child1].parent);*/
            uint32_t ind = isLeft ? BvhArray[last->index].child0 : BvhArray[last->index].child1;
            BvhNodeTree* buff = new BvhNodeTree(item.update, item.aabb0_min_or_v0, item.aabb0_max_or_v1, item.aabb1_min_or_v2, item.aabb1_max_or_v3,
                nullptr, nullptr, last, ind, true);
            leafArr.push_back(buff);
            return buff;
        }
        BvhNode left = BvhArray[item.child0];
        BvhNode right = BvhArray[item.child1];
        BvhNodeTree* rootNode = new BvhNodeTree(item.update, item.aabb0_min_or_v0, item.aabb0_max_or_v1, item.aabb1_min_or_v2, item.aabb1_max_or_v3,
            nullptr, nullptr, last, BvhArray[item.child1].parent, false);
        rootNode->child0 = createTree(left, rootNode, true);
        rootNode->child1 = createTree(right, rootNode, false);
        if (BvhArray[item.child1].parent == 0)
        {
            root = rootNode;
        }
        return rootNode;
    }
}

void Tree::drawTree(const BvhNodeTree* root)
{
    if (root != nullptr) {

        std::cout << *root << std::endl;
        drawTree(root->child0);
        drawTree(root->child1);
    }
}


void Tree::Destroy_Tree()
{
    destroy_tree(root);
}

BvhNodeTree* Tree::getNodeByIndex(uint32_t item)
{
    {


        BvhNodeTree* node = root;

        std::stack<BvhNodeTree*> s;
        s.push(node);
        while (!s.empty())
        {
            node = s.top();
            s.pop();
            if (node->index == item)
            {
                break;
                return node;
            }
            if (node->child0 != nullptr)
                s.push(node->child0);
            if (node->child1 != nullptr)
                s.push(node->child1);
        }
    }
}

void Tree::destroy_tree(BvhNodeTree* leaf)
{
    if ((leaf != nullptr) && (leaf->child0 != nullptr))
    {
        destroy_tree(leaf->child0);
        destroy_tree(leaf->child1);
        delete leaf;
    }
}

/* Binary file .bin parser function.
 * Builds the tree by bv-hierarchy
 * RETURNS: Tree
 */
Tree BuildTree(const char* file) {
    FILE* Dumbs = fopen(file, "rb");
    if (Dumbs == nullptr)
    {
        fputs("Error", stderr);
        exit(1);
    }
    uint32_t treesizeBytes;
    fread(&treesizeBytes, sizeof(uint32_t), 1, Dumbs);

    /*std::cout << "------" << treesizeBytes << "--------"<< std::endl;*/

    size_t treesize = treesizeBytes / 64;

    BvhNode* NodeArr = new BvhNode[treesize];
    for (size_t i = 0; i < treesize; i++)
    {
        fread(&NodeArr[i], sizeof(BvhNode), treesize, Dumbs);
    }

    /*for (size_t i = 0; i < treesize; i++)
    {
        std::cout<< "-------" << i << "--------" << std::endl << NodeArr[i];
    }*/
    Tree tree = Tree(NodeArr);
    BvhNodeTree* root = tree.createTree(NodeArr[0], nullptr, false);

    //std::cout <<std::endl<< std::endl << *root.child0 << *root.child1 << *root.child0->child0;

    //tree.drawTree(root);
    /*std::cout << std::endl << std::endl << std::endl;
    for (size_t i = 0; i < tree.leafArr.size(); i++)
    {
        std::cout << *tree.leafArr[i] << std::endl;
    }*/
    fclose(Dumbs);
    return tree;
}