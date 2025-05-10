#include "Map.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <climits> // For INT_MAX



// Constructor to initialize the map
Map::Map() {
    // Initialize the distance matrix
    distanceMatrix = new int*[MAX_SIZE];
    for (int i = 0; i < MAX_SIZE; i++) {
        distanceMatrix[i] = new int[MAX_SIZE];
        for (int j = 0; j < MAX_SIZE; j++) {
            distanceMatrix[i][j] = (i == j) ? 0 : INT_MAX; // 0 for same province, INT_MAX for no connection
        }
        visited[i] = false; // Initially, no provinces are visited
    }
}

// Destructor to free memory
Map::~Map() {
    for (int i = 0; i < MAX_SIZE; i++) {
        delete[] distanceMatrix[i];
    }
    delete[] distanceMatrix;
}

// Loads distance data from a file and fills the distanceMatrix
void Map::loadDistanceData(const std::string& filename)  {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    std::string line;
    int row = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        int col = 0;

        while (std::getline(ss, cell, ',')) {
            try {
                distanceMatrix[row][col] = std::stoi(cell);
            } catch (...) {
                distanceMatrix[row][col] = INT_MAX; // No direct connection
            }
            col++;
        }
        row++;
    }

    file.close();
}

// Checks if the distance between two provinces is within the allowed maxDistance
bool Map::isWithinRange(int provinceA, int provinceB, int maxDistance) const {
    return distanceMatrix[provinceA][provinceB] <= maxDistance;
}

// Marks a province as visited
void Map::markAsVisited(int province) {
    if (province >= 0 && province < MAX_SIZE) {
        visited[province] = true;
    }
}

// Checks if a province has already been visited
bool Map::isVisited(int province) const {
    return province >= 0 && province < MAX_SIZE && visited[province];
}

// Resets all provinces to unvisited
void Map::resetVisited() {
    for (bool & i : visited) {
        i = false;
    }
}

// Function to count the number of visited provinces
int Map::countVisitedProvinces() const {
    int count = 0;
    for (bool i : visited) {
        if (i) {
            count++;
        }
    }
    return count;
}

// Function to get the distance between two provinces
int Map::getDistance(int provinceA, int provinceB) const {
    if (provinceA >= 0 && provinceA < MAX_SIZE && provinceB >= 0 && provinceB < MAX_SIZE) {
        return distanceMatrix[provinceA][provinceB];
    }
    return INT_MAX; // Return no connection if indices are out of bounds
}
