#ifndef TUI_HPP
#define TUI_HPP

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "music_model.hpp"

class Tui
{
    using Action = std::pair<std::string, std::function<void()>>;

  public:
    void run();

  private:
    MusicModel model;
    std::string info_line;
    std::vector<Action> actions;

    static void clear_screen();
    void print_menu() const;
    std::string ask_path(const std::string& prompt, bool check = false);
    std::optional<int> ask_int(const std::string& prompt);
    int ask_option(const std::string& prompt, const std::vector<std::string>& options);
    void update_screen();
    void train();
    void generate_track();
    void score();
    void clear();
    void save_model();
    void load_model();
    void merge_model();
    void prune_model();
};

#endif
