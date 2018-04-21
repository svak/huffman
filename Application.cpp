#include <iostream>

#include "Application.hpp"
#include "Filesystem.hpp"

auto Usage(const char* aToolName)
{
    std::cout << "Usage: " << aToolName << " input output\n";
    return 1;
}

int Application::Run(TRoutine aRoutine, int argc, char** argv)
{
    if (argc != 3)
    {
        return Usage(argv[0]);
    }

    try
    {
        std::ios_base::sync_with_stdio(false);

        Filesystem::Reader reader{argv[1]};
        Filesystem::Writer writer{argv[2]};

        aRoutine(reader, writer);
    }
    catch (std::exception& e)
    {
        std::cout << "Fail: " << e.what() << std::endl;
    }

    return 0;
}
