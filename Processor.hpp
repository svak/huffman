#pragma once

#include "Interfaces.hpp"
#include "Model.hpp"

namespace Processor
{
    struct Config
    {
        static constexpr auto DefaultSize = 4096;

        struct
        {
            size_t InputSize{DefaultSize};
            size_t OutputSize{DefaultSize};
        } Buffer;
    };

    void Encode(IStorage::Input&, IStorage::Output&);
    void Encode(IStorage::Input&, IStorage::Output&, const Config&);

    void Decode(IStorage::Input&, IStorage::Output&);
    void Decode(IStorage::Input&, IStorage::Output&, const Config&);
}
