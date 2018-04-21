#pragma once

#include "Interfaces.hpp"
#include "Model.hpp"

namespace Helpers
{
    template <size_t CacheSize = 4096>
    struct StreamWriter
    {
        StreamWriter(IStorage::Output& aOutput)
          : mOutput{aOutput}
        {
        }

        void Write(Model::TSymbol aSymbol)
        {
            if (mConsumed > sizeof(mCache) - 1)
            {
                Flush();
            }

            mCache[mConsumed++] = aSymbol;
        }

        void Flush()
        {
            if (mConsumed)
            {
                mOutput.Write(mCache, mConsumed);
                mConsumed = 0;
            }
        }

    private:
        IStorage::Output& mOutput;

        char mCache[CacheSize];
        size_t mConsumed{0};
    };
}
