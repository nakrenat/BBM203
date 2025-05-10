#include "RoutePlanner.h"
#include <iostream>
#include <fstream>
using namespace std;
// Array to help you out with name of the cities in order
const std::string cities[81] = {
    "Adana", "Adiyaman", "Afyon", "Agri", "Amasya", "Ankara", "Antalya", "Artvin", "Aydin", "Balikesir", "Bilecik",
    "Bingol", "Bitlis", "Bolu", "Burdur", "Bursa", "Canakkale", "Cankiri", "Corum", "Denizli", "Diyarbakir", "Edirne",
    "Elazig", "Erzincan", "Erzurum", "Eskisehir", "Gaziantep", "Giresun", "Gumushane", "Hakkari", "Hatay", "Isparta",
    "Mersin", "Istanbul", "Izmir", "Kars", "Kastamonu", "Kayseri", "Kirklareli", "Kirsehir", "Kocaeli", "Konya", "Kutahya",
    "Malatya", "Manisa", "Kaharamanmaras", "Mardin", "Mugla", "Mus", "Nevsehir", "Nigde", "Ordu", "Rize", "Sakarya",
    "Samsun", "Siirt", "Sinop", "Sivas", "Tekirdag", "Tokat", "Trabzon", "Tunceli", "Urfa", "Usak", "Van", "Yozgat",
    "Zonguldak", "Aksaray", "Bayburt", "Karaman", "Kirikkale", "Batman", "Sirnak", "Bartin", "Ardahan", "Igdir",
    "Yalova", "Karabuk", "Kilis", "Osmaniye", "Duzce"
};


RoutePlanner::RoutePlanner(const std::string& distance_data, const std::string& priority_data, const std::string& restricted_data, int maxDistance)
    : maxDistance(maxDistance), totalDistanceCovered(0), numPriorityProvinces(0), numWeatherRestrictedProvinces(0) {

    map.loadDistanceData(distance_data);
    map.resetVisited();

    loadPriorityProvinces(priority_data);
    loadWeatherRestrictedProvinces(restricted_data);
}

void RoutePlanner::loadPriorityProvinces(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    numPriorityProvinces = 0;

    while (std::getline(file, line) && numPriorityProvinces < MAX_PRIORITY_PROVINCES) {
        try {
            size_t openParen = line.find('(');
            size_t closeParen = line.find(')');

            if (openParen != std::string::npos && closeParen != std::string::npos) {

                std::string indexStr = line.substr(openParen + 1, closeParen - openParen - 1);
                int provinceIndex = std::stoi(indexStr);

                if (provinceIndex >= 0 && provinceIndex < MAX_SIZE) {
                    priorityProvinces[numPriorityProvinces++] = provinceIndex;
                }
            }
        } catch (...) {
            std::cerr << "Invalid data in priority provinces file: " << line << std::endl;
        }
    }
}


void RoutePlanner::loadWeatherRestrictedProvinces(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    numWeatherRestrictedProvinces = 0;

    while (std::getline(file, line) && numWeatherRestrictedProvinces < MAX_WEATHER_RESTRICTED_PROVINCES) {
        try {
            size_t openParen = line.find('(');
            size_t closeParen = line.find(')');

            if (openParen != std::string::npos && closeParen != std::string::npos) {
                std::string indexStr = line.substr(openParen + 1, closeParen - openParen - 1);
                int provinceIndex = std::stoi(indexStr);

                if (provinceIndex >= 0 && provinceIndex < MAX_SIZE) {
                    weatherRestrictedProvinces[numWeatherRestrictedProvinces++] = provinceIndex;
                }
            }
        } catch (...) {
            std::cerr << "Invalid data in weather-restricted provinces file: " << line << std::endl;
        }
    }
}


bool RoutePlanner::isPriorityProvince(int province) const {
    for (int i = 0; i < numPriorityProvinces; i++) {
        if (priorityProvinces[i] == province) return true;
    }
    return false;
}

bool RoutePlanner::isWeatherRestricted(int province) const {
    for (int i = 0; i < numWeatherRestrictedProvinces; i++) {
        if (weatherRestrictedProvinces[i] == province) return true;
    }
    return false;
}

void RoutePlanner::exploreRoute(int startingCity) {
    stack.push(startingCity);
    map.markAsVisited(startingCity);
    route.push_back(startingCity);

    while (!isExplorationComplete()) {
        exploreFromProvince(stack.peek());
    }

    displayResults();
}

void RoutePlanner::exploreFromProvince(int province) {
    if (isExplorationComplete()) {  // Check if exploration is complete before proceeding
        return;  // If exploration is complete, exit the function
    }

    enqueueNeighbors(province);
    bool foundNextProvince = false; // Flag to track whether a valid province is found

    while (!queue.isEmpty()) {
        int nextProvince = queue.dequeue();

        // Skip weather-restricted provinces directly in this loop as well
        if (isWeatherRestricted(nextProvince)) {
            cout << "Province " << cities[nextProvince] << " is weather-restricted. Skipping." << endl;
            continue;
        }

        int distance = map.getDistance(province, nextProvince);
        if (!map.isVisited(nextProvince) && distance <= maxDistance) {
            stack.push(nextProvince);
            map.markAsVisited(nextProvince);
            route.push_back(nextProvince);
            totalDistanceCovered += distance;
            foundNextProvince = true; // Mark that a valid path was found
            break;
        }
    }

    if (!foundNextProvince) {
        // Only backtrack if no valid neighbors were found and the queue is empty
        if (queue.isEmpty()) {
            backtrack();
        }
    }
}


void RoutePlanner::enqueueNeighbors(int province) {
    for (int neighbor = 0; neighbor < 81; neighbor++) {
        int distance = map.getDistance(province, neighbor);
        if (distance <= maxDistance) {
            if ( map.isVisited(neighbor)) continue; // Skip if it's the same province or already visited




            // Skip weather-restricted provinces completely
            if (isWeatherRestricted(neighbor)) {
                cout << "Province " << cities[neighbor] << " is weather-restricted. Skipping." << endl;
                continue;
            }

            // Enqueue valid provinces (priority or not)
            if (isPriorityProvince(neighbor)) {
                queue.enqueuePriority(neighbor); // Enqueue priority province at the front
            } else {
                queue.enqueue(neighbor); // Enqueue non-priority province normally
            }
        }
    }
}



void RoutePlanner::backtrack() {
    stack.pop(); // Remove the dead-end province

    // Check if the queue is empty before trying to explore further
    if (!queue.isEmpty() && !stack.isEmpty()) {
        // If the queue is not empty, we can continue exploring from the next province
        exploreFromProvince(stack.peek());
    }
}

bool RoutePlanner::isExplorationComplete() const {
    return stack.isEmpty() && queue.isEmpty();
}

void RoutePlanner::displayResults() const {
    cout << "----------------------------" << endl;
    cout << "Journey Completed!" << endl;
    cout << "----------------------------" << endl;

    cout << "Total Number of Provinces Visited: " << route.size() << endl;
    cout << "Total Distance Covered: " << totalDistanceCovered << " km" << endl;

    cout << "Route Taken: "<< endl;
    for (size_t i = 0; i < route.size(); i++) {
        cout << cities[route[i]];
        if (i < route.size() - 1) cout << " -> ";
    }
    cout << endl;

    cout << "Priority Provinces Status:" << endl;
    int visitedCount = 0;
    for (int i = 0; i < numPriorityProvinces; i++) {
        int province = priorityProvinces[i];
        cout << "- " << cities[province] << " (" << (map.isVisited(province) ? "Visited" : "Not Visited") << ")" << endl;
        if (map.isVisited(province)) visitedCount++;
    }

    cout << "Total Priority Provinces Visited: " << visitedCount << " out of " << numPriorityProvinces << endl;
    if (visitedCount == numPriorityProvinces) {
        cout << "Success: All priority provinces were visited." << endl;
    } else {
        cout << "Warning: Not all priority provinces were visited." << endl;
    }
}
