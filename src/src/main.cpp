#include <iostream>

#include "tui.hpp"

int main()
{
    std::cout << "\033[?1049h";
    Tui tui;
    tui.run();
    std::cout << "\033[?1049l";

    return 0;
}
