#include "SymbolsLookup.hpp"

namespace Helpers
{

    void SymbolsLookup::Put(const SymbolInfo& aSymbolInfo)
    {
        const auto& code = aSymbolInfo.Code;

        if (!code.Length)
        {
            return;
        }

        mLengthsIndex[code.Length][code.Value] = aSymbolInfo.Symbol;
    }

    SymbolsLookup::FindResult SymbolsLookup::Find(int aValue, int aLength) const
    {
        static FindResult notFound{false};

        auto lit = mLengthsIndex.find(aLength);
        if (lit == mLengthsIndex.end())
        {
            return notFound;
        }

        const auto& symbols = lit->second;

        auto sit = symbols.find(aValue);
        if (sit == symbols.end())
        {
            return notFound;
        }

        return FindResult{true, sit->second};
    }

    size_t SymbolsLookup::Size() const
    {
        size_t size = 0;
        for (const auto& index : mLengthsIndex)
        {
            size += index.second.size();
        }
        return size;
    }
}
