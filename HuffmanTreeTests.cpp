#include "HuffmanTree.hpp"
#include "Interfaces.hpp"
#include "TestsBase.hpp"

std::vector<Huffman::TSymbol> MakeBuffer(const std::string& aSource)
{
    std::vector<Huffman::TSymbol> result;

    result.assign(aSource.begin(), aSource.end());
    return result;
}

TEST(HuffmanTree, ShouldBuildNothingIfNoDataWasGiven)
{
    Huffman::TreeBuilder builder;
    builder.Process(MakeBuffer(""));

    auto tree = builder.Build();

    EXPECT_FALSE(tree.Root);
    EXPECT_TRUE(tree.Codes.empty());
    EXPECT_TRUE(tree.Nodes.empty());
}

TEST(HuffmanTree, ShouldBuildDictAndNodes)
{
    Huffman::TreeBuilder builder;
    builder.Process(MakeBuffer("AAB"));

    auto tree = builder.Build();

    EXPECT_TRUE(tree.Root);
    EXPECT_EQ(2u, tree.Codes.size());
    EXPECT_EQ(3u, tree.Nodes.size());
}

TEST(HuffmanTree, ShouldCalculateCodeForOneSymbol)
{
    Huffman::TreeBuilder builder;
    builder.Process(MakeBuffer("A"));

    auto tree = builder.Build();

    ASSERT_EQ(1u, tree.Codes.size());
    EXPECT_EQ(0b0, tree.Codes['A'].Value);
    EXPECT_EQ(1, tree.Codes['A'].Length);
}

TEST(HuffmanTree, ShouldCalculateCodesWithRespectToSymbolFrequency)
{
    Huffman::TreeBuilder builder;
    builder.Process(MakeBuffer("AAAABBC"));

    auto tree = builder.Build();

    ASSERT_EQ(3u, tree.Codes.size());
    EXPECT_EQ(0b1, tree.Codes['A'].Value);
    EXPECT_EQ(1, tree.Codes['A'].Length);
    EXPECT_EQ(0b01, tree.Codes['B'].Value);
    EXPECT_EQ(2, tree.Codes['B'].Length);
    EXPECT_EQ(0b00, tree.Codes['C'].Value);
    EXPECT_EQ(2, tree.Codes['C'].Length);
}

