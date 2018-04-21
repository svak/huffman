#include <functional>
#include <queue>

#include "HuffmanTree.hpp"

void Huffman::TreeBuilder::Process(const std::vector<TSymbol>& aData)
{
    for (const auto& symbol : aData)
    {
        mFrequencies[symbol]++;
    }
}

Huffman::Tree Huffman::TreeBuilder::Build() const
{
    Huffman::Tree tree;

    // Initalize Huffman tree.

    std::priority_queue<std::reference_wrapper<Tree::Node>> data;

    for (const auto& item : mFrequencies)
    {
        const auto& symbol = item.first;
        const auto& value = item.second;

        auto id = static_cast<int>(tree.Nodes.size());

        tree.Codes[symbol] = SymbolCode{0, 0};
        tree.Nodes.emplace_back(id, symbol, value);

        data.emplace(tree.Nodes[id]);
    }

    // Build Huffman tree.

    while (!data.empty())
    {
        if (data.size() == 1)
        {
            tree.Root = &data.top().get();
            break;
        }
        auto& left = data.top().get();
        data.pop();

        auto& right = data.top().get();
        data.pop();

        auto id = static_cast<int>(tree.Nodes.size());
        tree.Nodes.emplace_back(id, left.Frequency + right.Frequency, left.Id, right.Id);

        data.emplace(tree.Nodes[id]);
    }

    // Calculate symbol codes.

    if (tree.Root)
    {
        int code = 0;
        int length = tree.Nodes[tree.Root->Id].IsLeaf() ? 1 : 0;
        CalculateSymbolCodes(tree, tree.Root->Id, code, length);
    }

    return tree;
}

void Huffman::TreeBuilder::CalculateSymbolCodes(Tree& aTree, Tree::Node::TNodeIndex aNodeIndex, int aCode, int aLength) const
{
    const auto& node = aTree.Nodes[aNodeIndex];
    if (node.IsLeaf())
    {
        auto& symbolCode = aTree.Codes[node.Symbol];
        symbolCode.Value = aCode;
        symbolCode.Length = aLength;
        return;
    }
    CalculateSymbolCodes(aTree, node.Left, aCode << 1, aLength + 1);
    CalculateSymbolCodes(aTree, node.Right, (aCode << 1) | 1, aLength + 1);
}

