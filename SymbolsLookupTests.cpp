#include "SymbolsLookup.hpp"
#include "TestsBase.hpp"

using Helpers::SymbolsLookup;
using Model::SymbolInfo;

TEST(SymbolsLookup, ShouldIgnoreZeroLength)
{
    SymbolsLookup lookup;

    lookup.Put(SymbolInfo{'A', {0b01, 0}});

    ASSERT_EQ(0, lookup.Size());
}

TEST(SymbolsLookup, ShouldFoundNothingIfHasNoData)
{
    SymbolsLookup lookup;
    ASSERT_EQ(0, lookup.Size());
    EXPECT_FALSE(lookup.Find(0b01, 1).Success);
}

TEST(SymbolsLookup, ShouldFindSymbolsByLength)
{
    SymbolsLookup lookup;

    lookup.Put(SymbolInfo{'A', {0b01, 2}});
    lookup.Put(SymbolInfo{'A', {0b01, 3}});
    lookup.Put(SymbolInfo{'B', {0b11, 4}});

    ASSERT_EQ(3, lookup.Size());

    {
        auto find = lookup.Find(0b01, 2);
        EXPECT_EQ('A', find.Symbol);
        EXPECT_TRUE(find.Success);
    }
    {
        auto find = lookup.Find(0b01, 3);
        EXPECT_EQ('A', find.Symbol);
        EXPECT_TRUE(find.Success);
    }
    {
        auto find = lookup.Find(0b11, 4);
        EXPECT_EQ('B', find.Symbol);
        EXPECT_TRUE(find.Success);
    }
}
