#ifndef TUI_HPP
#define TUI_HPP

#include "music_model.hpp"

class Tui
{
  public:
    void run();

  private:
    MusicModel model;
    std::string info_line;

    static void clear_screen();
    static void print_menu();
    std::string ask_path(const std::string& prompt);
    void update_screen();
    void train();
    void generate_track();
    void score();
    void clear();
};

#endif
