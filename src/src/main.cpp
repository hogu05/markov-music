#include "cli.hpp"
#include "tui.hpp"

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        return cli::run({argv, argv + argc});
    }

    Tui{}.run();

    return 0;
}
