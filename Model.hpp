#pragma once

#include <cstdint>
#include <cstddef>

namespace Model
{
    using TSymbol = uint8_t;

    struct SymbolCode
    {
        int Value;
        int Length;
    };

    struct SymbolInfo
    {
        TSymbol Symbol;
        SymbolCode Code;
    };

    struct FileHeader
    {
        struct SymbolsTable
        {
            using Entry = SymbolInfo;

            size_t Length;
            Entry Entries[];
        };

        size_t FileSize;
        SymbolsTable Table;
    };
}
