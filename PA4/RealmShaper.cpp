#include "RealmShaper.h"
#include <fstream>
#include <sstream>
#include <iostream>

#define NECESSARY_ENERGY 2000 // Necessary energy to craft an Isle

RealmShaper::RealmShaper(std::string name, int honour) : name(name), honour(honour), collectedEnergyPoints(0)
{
    // Constructor initializes name, honour, and energy points to zero
}

const std::string &RealmShaper::getName() const
{
    return this->name;
}

int RealmShaper::getEnergyLevel()
{
    return this->collectedEnergyPoints;
}

int RealmShaper::getHonour()
{
    return this->honour;
}

void RealmShaper::gainHonour()
{
    this->honour += 150;
}

void RealmShaper::loseHonour()
{
    this->honour -= 150;
}

void RealmShaper::collectItem(Item item)
{
    this->collectedEnergyPoints += item;  // Assuming Item has a method getEnergy()
}

void RealmShaper::loseEnergy()
{
    this->collectedEnergyPoints -= NECESSARY_ENERGY;
}

bool RealmShaper::hasEnoughEnergy()
{
    return this->collectedEnergyPoints >= NECESSARY_ENERGY;
}

// Static method to read players from a file and return a vector of pointers to RealmShapers
std::vector<RealmShaper *> RealmShaper::readFromFile(const std::string &filename) {
    std::vector<RealmShaper *> players;

    std::ifstream inputFile(filename);  // Open the input file
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return players;
    }

    std::string line;
    while (std::getline(inputFile, line)) {  // Read the file line by line
        std::istringstream stream(line);
        std::string playerName;
        int honourPoints;

        // Read the player name and honour points, accepting any white space as a separator
        stream >> playerName >> honourPoints;

        if (!playerName.empty() && stream) {
            // Create a new RealmShaper object and add it to the vector
            RealmShaper *player = new RealmShaper(playerName, honourPoints);
            players.push_back(player);
        }
    }

    inputFile.close();  // Close the file after reading
    return players;
}


bool RealmShaper::operator==(const RealmShaper &other) const
{
    // Compare RealmShapers by their names
    return this->name == other.name;
}

std::ostream &operator<<(std::ostream &os, const RealmShaper &p)
{
    // Outputs the player name and honour for displaying
    os << p.name << " (Honour: " << p.honour << ")";
    return os;
}
