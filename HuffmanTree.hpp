#pragma once

#include <ostream>
#include <unordered_map>
#include <deque>

#include "Interfaces.hpp"
#include "Model.hpp"

namespace Huffman
{
    using Model::TSymbol;
    using Model::SymbolCode;

    static constexpr TSymbol NoSymbol{};
    static constexpr int NullNode = -1;

    struct Tree
    {
        using TSymbolCodes = std::unordered_map<TSymbol, SymbolCode>;

        struct Node
        {
            using TNodeIndex = int;

            const TNodeIndex Id;
            const TSymbol Symbol;
            const size_t Frequency;
            const TNodeIndex Left;
            const TNodeIndex Right;

            explicit Node(const TNodeIndex aId, const TSymbol& aSymbol, const size_t aFrequency, const TNodeIndex aLeft,
                          const TNodeIndex aRight)
              : Id{aId}
              , Symbol{aSymbol}
              , Frequency{aFrequency}
              , Left{aLeft}
              , Right{aRight}
            {
            }

            Node(const TNodeIndex aId, const TSymbol& aSymbol, const size_t aFrequency)
              : Node{aId, aSymbol, aFrequency, NullNode, NullNode}
            {
            }

            Node(const TNodeIndex aId, size_t aFrequency, const TNodeIndex aLeft, const TNodeIndex aRight)
              : Node{aId, NoSymbol, aFrequency, aLeft, aRight}
            {
            }

            bool IsLeaf() const
            {
                return Left == NullNode && Right == NullNode;
            }

            friend bool operator<(const Node& lhs, const Node& rhs)
            {
                return lhs.Frequency > rhs.Frequency;
            }

            friend std::ostream& operator<<(std::ostream& aStream, const Node& aNode)
            {
                aStream << "Node { ";
                aStream << "Id: " << aNode.Id << ", ";
                aStream << "Symbol: " << aNode.Symbol << ", ";
                aStream << "Freq: " << aNode.Frequency << ", ";
                aStream << "Left: " << aNode.Left << ", ";
                aStream << "Right: " << aNode.Right << ", ";
                aStream << " }";
                return aStream;
            }
        };

        TSymbolCodes Codes;
        std::deque<Node> Nodes;
        Node* Root{nullptr};
    };

    class TreeBuilder
    {
    public:
        void Process(const std::vector<TSymbol>& aData);
        Tree Build() const;

    private:
        void CalculateSymbolCodes(Tree& aTree, Tree::Node::TNodeIndex aNodeIndex, int aCode, int aLength) const;

    private:
        std::unordered_map<TSymbol, size_t> mFrequencies;
    };
}
