#pragma once

#include <unordered_map>

#include "Model.hpp"

namespace Helpers
{
    using Model::TSymbol;
    using Model::SymbolInfo;

    struct SymbolsLookup
    {
        struct FindResult
        {
            bool Success{false};
            TSymbol Symbol{};
        };

    public:
        void Put(const SymbolInfo& aSymbolInfo);
        FindResult Find(int aValue, int aLength) const;
        size_t Size() const;

    private:
        using TSymbolsIndex = std::unordered_map<int, TSymbol>;

        std::unordered_map<int, TSymbolsIndex> mLengthsIndex;
    };
}
