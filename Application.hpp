#include "Interfaces.hpp"

namespace Application
{
    using TRoutine = void(*)(IStorage::Input&, IStorage::Output&);

    int Run(TRoutine aRoutine, int aArgc, char** aArgv);
}
