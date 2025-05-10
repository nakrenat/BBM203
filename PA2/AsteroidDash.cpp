#include "AsteroidDash.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Constructor to initialize AsteroidDash with the given parameters
AsteroidDash::AsteroidDash(const string &space_grid_file_name,
                           const string &celestial_objects_file_name,
                           const string &leaderboard_file_name,
                           const string &player_file_name,
                           const string &player_name)
    : leaderboard_file_name(leaderboard_file_name), leaderboard(Leaderboard()) {

    read_player(player_file_name, player_name);
    read_space_grid(space_grid_file_name);
    read_celestial_objects(celestial_objects_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

// Function to read the space grid from a file
void AsteroidDash::read_space_grid(const string &input_file) {
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Failed to open space grid file." << endl;
        return;
    }

    int value;
    vector<int> row;
    while (file >> value) {
        row.push_back(value);
        if (file.peek() == '\n' || file.eof()) {
            space_grid.push_back(row);
            row.clear();
        }
    }

    int grid_height = space_grid.size();
    int grid_width = space_grid[0].size();
    file.close();
}

// Function to read the player from a file
void AsteroidDash::read_player(const string &player_file_name, const string &player_name) {
    ifstream file(player_file_name);

    // Check if the file opened successfully
    if (!file.is_open()) {
        cerr << "Error: Unable to open player file: " << player_file_name << endl;
        return;
    }

    // Read the initial position (row and column) from the first line
    int row, col;
    file >> row >> col;

    // Read the spacecraft shape from the subsequent lines
    vector<vector<bool>> shape;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;  // Skip any empty lines

        vector<bool> spacecraft_row;
        for (char ch : line) {
            if (ch == '1') {
                spacecraft_row.push_back(true);
            } else if (ch == '0') {
                spacecraft_row.push_back(false);
            }
        }
        shape.push_back(spacecraft_row);
    }

    // Create a new Player instance with the read data
    player = new Player(shape, row, col, player_name);

    file.close();
}


// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const string &input_file) {
    ifstream file(input_file);

    // Check if the file opened successfully
    if (!file.is_open()) {
        cerr << "Error: Unable to open celestial objects file: " << input_file << endl;
        return;
    }

    string line;

    // Check the first line before entering the loop
    if (getline(file, line)) {
        cout << "First line read: " << line << endl;  // Debug print to check the first line
    }

    // Read the file line by line
    while (getline(file, line)) {
        // Skip any empty lines
        if (line.empty()) continue;

        // Trim leading/trailing whitespace (if any)
        line = line.substr(line.find_first_not_of(" \t"), line.find_last_not_of(" \t") + 1);
        cout << "Processing line: " << line << endl;  // Debug print

        // Check if it's an object shape (either an asteroid or a power-up)
        if (line.front() == '[' || line.front() == '{') {
            // Read the shape matrix
            vector<vector<bool>> shape;
            char delimiter = (line.front() == '[') ? ']' : '}';  // Determine if it's asteroid or power-up
            bool is_asteroid = (line.front() == '[');

            // Read the 2D shape matrix until the closing bracket
            do {
                cout << "Reading shape line: " << line << endl;  // Debug print for shape line
                vector<bool> row;
                for (char ch : line) {
                    if (ch == '1') {
                        row.push_back(true);
                    } else if (ch == '0') {
                        row.push_back(false);
                    }
                }
                shape.push_back(row);
            } while (getline(file, line) && line.front() != delimiter);

            cout << "Shape matrix read complete. Rows: " << shape.size() << endl;

            // Read the metadata: starting row (s), tick (t), and effect (e) if applicable
            int starting_row = -1, time_of_appearance = -1;
            string effect;
            while (getline(file, line)) {
                if (line.empty()) continue;

                cout << "Reading metadata line: " << line << endl;  // Debug print for metadata line

                if (line[0] == 's') {  // Starting row
                    stringstream ss(line.substr(2));  // Skip "s: "
                    ss >> starting_row;
                    cout << "Parsed starting row: " << starting_row << endl;
                } else if (line[0] == 't') {  // Time of appearance
                    stringstream ss(line.substr(2));  // Skip "t: "
                    ss >> time_of_appearance;
                    cout << "Parsed time of appearance: " << time_of_appearance << endl;
                } else if (line[0] == 'e') {  // Effect for power-ups (Life-Up or Ammo)
                    if (!is_asteroid) {
                        effect = line.substr(2);  // Skip "e: "
                        cout << "Parsed effect: " << effect << endl;
                    }
                }

                // If all necessary metadata has been read, we can break out of this loop
                if (starting_row != -1 && time_of_appearance != -1 &&
                    (is_asteroid || !effect.empty())) {
                    break;
                }
            }

            // Convert the effect to ObjectType (LIFE_UP or AMMO) if it's a power-up
            ObjectType object_type = ASTEROID;  // Default type is ASTEROID
            if (!is_asteroid) {
                if (effect == "life") {
                    object_type = LIFE_UP;
                } else if (effect == "ammo") {
                    object_type = AMMO;
                }
            }

            // Create a new CelestialObject with the data read
            CelestialObject *new_object = new CelestialObject(shape, object_type, starting_row, time_of_appearance);

            // Add this new object to the linked list
            if (celestial_objects_list_head == nullptr) {
                celestial_objects_list_head = new_object;  // Set the head of the list
            } else {
                CelestialObject *temp = celestial_objects_list_head;
                while (temp->next_celestial_object != nullptr) {
                    temp = temp->next_celestial_object;
                }
                temp->next_celestial_object = new_object;  // Linking the new object to the list
            }

            // Debug print for verifying linked list addition
            cout << "Added new object with starting_row: " << starting_row
                 << " and time_of_appearance: " << time_of_appearance << endl;

            // Manual rotations code remains unchanged...
        }
    }

    file.close();
    cout << "File reading complete." << endl;  // Debug print
}




// Print the entire space grid
void AsteroidDash::print_space_grid() const {
    for (const auto &row : space_grid) {
        for (int cell : row) {
            cout << (cell == 0 ? unoccupiedCellChar : occupiedCellChar);
        }
        cout << endl;
    }
}

// Function to update the space grid with player, celestial objects, and any other changes
void AsteroidDash::update_space_grid() {
    // Clear the grid (set all cells to 0, representing empty spaces)
    for (auto &row : space_grid) {
        fill(row.begin(), row.end(), 0);
    }

    // Update player position on the grid
    if (player) {
        for (int i = 0; i < player->spacecraft_shape.size(); ++i) {
            for (int j = 0; j < player->spacecraft_shape[i].size(); ++j) {
                int row = player->position_row + i;
                int col = player->position_col + j;
                // Ensure player doesn't go out of bounds
                if (player->spacecraft_shape[i][j] && row >= 0 && row < space_grid.size() && col >= 0 && col < space_grid[0].size()) {
                    space_grid[row][col] = 1; // Mark the cell as occupied by the player (assuming '1' represents the player)
                }
            }
        }
    }

    // Update celestial objects on the grid
    CelestialObject *current = celestial_objects_list_head;
    while (current) {
        for (int i = 0; i < current->shape.size(); ++i) {
            for (int j = 0; j < current->shape[i].size(); ++j) {
                int row = current->starting_row + i;
                int col = j;  // Assuming celestial objects enter from the rightmost column
                // Ensure celestial objects don't go out of bounds
                if (current->shape[i][j] && row >= 0 && row < space_grid.size() && col >= 0 && col < space_grid[0].size()) {
                    space_grid[row][col] = 2; // Mark the cell as occupied by a celestial object (assuming '2' represents celestial objects)
                }
            }
        }
        current = current->next_celestial_object;
    }
}

// Corresponds to the SHOOT command.
void AsteroidDash::shoot() {
    if (player && player->current_ammo > 0) {
        player->current_ammo--;

        // Starting position for the shot (e.g., top of the player's spacecraft)
        int shot_row = player->position_row;
        int shot_col = player->position_col + (player->spacecraft_shape[0].size() / 2); // Middle column of the player

        // Traverse the path of the shot upward
        for (int row = shot_row - 1; row >= 0; --row) {
            bool hit_detected = false;
            CelestialObject *current = celestial_objects_list_head;

            while (current) {
                // Check if the shot hits this celestial object
                if (row >= current->starting_row &&
                    row < current->starting_row + current->shape.size() &&
                    shot_col < current->shape[0].size()) {
                    int local_row = row - current->starting_row;
                    if (current->shape[local_row][shot_col]) {
                        // Hit detected - mark as damaged or remove the object if necessary
                        current->shape[local_row][shot_col] = false; // Mark the hit part
                        hit_detected = true;
                        break;
                    }
                    }
                current = current->next_celestial_object;
            }

            // Stop checking further if a hit was detected
            if (hit_detected) {
                break;
            }
        }
    } else {
        std::cout << "No ammo left!" << std::endl;
    }

}

// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash() {
    delete player;

    CelestialObject *current = celestial_objects_list_head;
    while (current) {
        CelestialObject *to_delete = current;
        current = current->next_celestial_object;
        delete to_delete;
    }
}
