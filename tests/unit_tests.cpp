#include <sstream>

#include "midi_loader.hpp"
#include "midi_saver.hpp"
#include "music_model.hpp"
#include "plain_loader.hpp"
#include "plain_saver.hpp"
#include "test_utils.hpp"
#include "types.hpp"

static bool test_midi_round_trip()
{
    MidiSaver saver;
    MidiLoader loader;
    std::stringstream stream;
    saver.save(TEST_NOTES, stream);
    const Notes loaded = loader.load(stream);

    return check(loaded == TEST_NOTES, "MIDI ROUND TRIP");
}

static bool test_plain_round_trip()
{
    PlainSaver saver;
    PlainLoader loader;
    std::stringstream stream;
    saver.save(TEST_NOTES, stream);
    const Notes loaded = loader.load(stream);

    return check(loaded == TEST_NOTES, "PLAIN ROUND TRIP");
}

static bool test_notes_track_round_trip()
{
    const Notes result = track_to_notes(notes_to_track(TEST_NOTES));

    return check(result == TEST_NOTES, "NOTES TRACK ROUND TRIP");
}

static bool test_empty_track()
{
    bool passed = check(notes_to_track({}).empty(), "EMPTY NOTES TO TRACK");
    MusicModel model;
    model.train({});
    passed = check(model.empty(), "EMPTY MODEL AFTER EMPTY TRAIN") && passed;
    return passed;
}

static bool test_model_save_load()
{
    MusicModel original;
    original.train(TEST_TRACK);

    std::stringstream stream;
    original.save(stream);

    MusicModel loaded;
    loaded.load(stream);

    return check(original == loaded, "MODEL SAVE LOAD");
}

int main()
{
    bool all_passed = true;
    all_passed = test_empty_track() && all_passed;
    all_passed = test_midi_round_trip() && all_passed;
    all_passed = test_plain_round_trip() && all_passed;
    all_passed = test_notes_track_round_trip() && all_passed;
    all_passed = test_model_save_load() && all_passed;
    std::cout << (all_passed ? "ALL PASSED" : "SOME FAILED") << std::endl;
    return all_passed ? 0 : 1;
}
