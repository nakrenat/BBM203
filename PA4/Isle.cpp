#include "Isle.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

Isle::Isle(std::string name) : name(name), shaperCount(0), item(EMPTY) // Default item set to EMPTY
{
}

const std::string& Isle::getName() const
{
    return this->name;
}

Item Isle::getItem()
{
    return item;
}

void Isle::setItem(Item item)
{
    this->item = item;
}

int Isle::getShaperCount()
{
    return shaperCount;
}
int Isle:: getCapacity(){
  return capacity;
  }

bool Isle::increaseShaperCount()
{
    if (shaperCount < capacity)  // Accessing the constant 'capacity'
    {
        shaperCount++;  // Increase the count if under capacity
        return false;   // Not full yet
    }
    else
    {
        return true;    // Capacity is full
    }
}

bool Isle::decreaseShaperCount()
{
    bool isEmpty = false;

    if (shaperCount > 0) {
        shaperCount--;
    } else {
        isEmpty = true; // If there are no shapers left
    }

    return isEmpty;
}

bool Isle::operator==(const Isle &other) const
{
    return this->name == other.name; // Compare by name
}

bool Isle::operator<(const Isle &other) const
{
    return this->name < other.name; // Compare by name (lexicographically)
}

bool Isle::operator>(const Isle &other) const
{
    return this->name > other.name; // Compare by name (lexicographically)
}

// Implementation of readFromFile
std::vector<Isle *> Isle::readFromFile(const std::string &filename)
{
    std::vector<Isle *> isles;
    std::ifstream file(filename);
    std::string isleName;

    if (file.is_open()) {
        while (std::getline(file, isleName)) {
            // Create an Isle object for each name read and add to vector
            isles.push_back(new Isle(isleName));
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
    }

    return isles;
}

std::ostream &operator<<(std::ostream &os, const Isle &p)
{
    // Prints to terminal with color
    // This function might cause some problems in terminals that are not Linux based
    // If so, comment out the colors while testing on local machine
    // But open them back up while submitting or using TurBo

    std::string einsteiniumColor = "\033[38;2;6;134;151m";
    std::string goldiumColor = "\033[38;2;255;198;5m";
    std::string amazoniteColor = "\033[38;2;169;254;255m";
    std::string resetColorTag = "\033[0m";

    if (p.item == EINSTEINIUM)
        return (os << einsteiniumColor << p.name << resetColorTag);
    if (p.item == GOLDIUM)
        return (os << goldiumColor << p.name << resetColorTag);
    if (p.item == AMAZONITE)
        return (os << amazoniteColor << p.name << resetColorTag);
    return (os << p.name);
}
