#include "GameController.h"
#include <fstream>
#include <sstream>

// Simply instantiates the game
GameController::GameController(
        const string &space_grid_file_name,
        const string &celestial_objects_file_name,
        const string &leaderboard_file_name,
        const string &player_file_name,
        const string &player_name
) {
    game = new AsteroidDash(space_grid_file_name, celestial_objects_file_name, leaderboard_file_name, player_file_name,
                            player_name);
    // Additional initializations can be done here if needed
}

// Reads commands from the given input file, executes each command in a game tick
void GameController::play(const string &commands_file) {
    ifstream file(commands_file);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "MOVE_LEFT") {
            game->player->move_left();
        } else if (command == "MOVE_RIGHT") {
            game->player->move_right(game->space_grid[0].size());
        } else if (command == "MOVE_UP") {
            game->player->move_up();
        } else if (command == "MOVE_DOWN") {
            game->player->move_down(game->space_grid.size());
        } else if (command == "SHOOT") {
            game->shoot();
        } else if (command == "NOP") {
            continue;
        } else if (command == "PRINT_GRID") {
            // Print the updated state of the grid after processing the game tick
            for (size_t i = 0; i < game->space_grid.size(); ++i) {
                for (size_t j = 0; j < game->space_grid[i].size(); ++j) {
                    // Check for the player or celestial objects
                    if (game->space_grid[i][j] == 1) {  // This is just an example condition
                        // If there is something important like the player or an object, print it
                        cout << 'O';  // O for celestial object or player (or any object)
                    } else {
                        cout << '.';  // Empty space
                    }
                }
                cout << endl;  // Move to the next line after each row
            }
        }

        // Update game state after every command (such as player movement, celestial object movements, etc.)
        game->update_space_grid();

        // More commands can be added here if necessary
    }

    file.close();
}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController() {
    delete game;
}
