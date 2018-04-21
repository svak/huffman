#include "Application.hpp"
#include "Processor.hpp"

int main(int argc, char** argv)
{
    return Application::Run(Processor::Encode, argc, argv);
}
