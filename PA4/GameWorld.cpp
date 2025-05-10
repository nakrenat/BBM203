#include "GameWorld.h"
#include <iostream>
#include <fstream>
#include "RealmShapers.h"
#include "RealmShaper.h"
#include "Isle.h"

GameWorld::GameWorld() : mapTree(), shaperTree() {}


void GameWorld::initializeGame(std::vector<Isle *> places, std::vector<RealmShaper *> players)
{
    shaperTree.initializeTree(players);
    mapTree.initializeMap(places);
}

Map& GameWorld::getMapTree()
{
    return mapTree;
}

ShaperTree& GameWorld::getShaperTree()
{
    return shaperTree;
}

bool GameWorld::hasAccess(RealmShaper *realmShaper, Isle *isle)
{
  	 bool hasAccess = false;
    // Check if realmShaper is valid
    if (!realmShaper) {
        std::cout << "[Access Control] " << "RealmShaper not found!" << std::endl;
        return hasAccess;
    }


    // Get the player's depth in the Shaper Tree
    int playerDepth = shaperTree.getDepth(realmShaper);  // Assuming getDepth() returns the player's depth in the tree

    // Get the total height of the Shaper Tree
    int totalHeight = shaperTree.getDepth();  // Assuming getHeight() gives the total height of the tree

    // Get the total depth of the World Tree (map)
    int totalMapDepth = mapTree.getDepth()-1;  // Assuming getMaxDepth() gives the total depth of the world map

    int isleDepth = mapTree.getIsleDepth(isle);

    if (playerDepth == -1 || isleDepth == -1) {
    return false;
}

    // Rule 1: The root player can access everything
     if (shaperTree.getSize() == 1) {
        hasAccess = true;
        return hasAccess;
    }

    int minMapDepthAccess = mapTree.calculateMinMapDepthAccess( playerDepth,  totalHeight,  totalMapDepth);

    // Rule 2: Players at the bottom of the ShaperTree can only access outer Isles
    if (playerDepth == totalHeight) { // Check if player is at the bottom
        if (isleDepth == totalMapDepth) { // Check if Isle is an outer Isle
            hasAccess = true;
            return hasAccess;
        } else {
            std::cout << "[Access Control] " << realmShaper->getName()
                      << " can only access outer Isles. Access denied to " << isle->getName() << std::endl;
            return hasAccess;
        }
    }

    // Check if the player can access the isle
    if (isleDepth >= minMapDepthAccess) {
        hasAccess = true;
        return hasAccess;  // Player can access the isle
    }
}

void GameWorld::exploreArea(RealmShaper *realmShaper, Isle *isle) {
    // Check if the Realm Shaper has access to the Isle
    if (!hasAccess(realmShaper, isle)) {
        std::cout << "[Explore Area] " << realmShaper->getName() << " does not have access to explore area " << *isle << std::endl;
        return;
    }
    else if (hasAccess(realmShaper, isle)) {
      // Exploration log
    std::cout << "[Explore Area] " << realmShaper->getName() << " visited " << isle->getName() << std::endl;
    std::cout << "[Energy] " << realmShaper->getName() << "'s new energy level is " << realmShaper->getEnergyLevel() << std::endl;

    }

    static int Shapercount = isle->getShaperCount();
    const int capacity = isle->getCapacity();

    if (Shapercount < capacity) {
      isle->increaseShaperCount();
    }else if (Shapercount >= capacity) {
     	// Increment the shaper count and check overcrowding
      	// This function returns 'true' if overcrowded
        std::cout << "[Owercrowding] " << isle->getName() << " self-destructed, it will be removed from the map" << std::endl;

        // Remove the Isle from the map
        mapTree.remove(isle); // Assume `remove(Isle*)` exists in Map



    }

}





void GameWorld::craft(RealmShaper *shaper, const std::string &isleName) {
    if (shaper->hasEnoughEnergy()) {
        // If the realm shaper has enough energy, they can craft a new isle
        Isle *newIsle = new Isle(isleName);  // Create a new isle with the provided name
        mapTree.addIsle(newIsle);  // Add the newly crafted isle to the map
		delete newIsle;
        // Output the success message for crafting
        std::cout << "[Energy] " << shaper->getName() << " has enough energy points: " << shaper->getEnergyLevel() << std::endl;
        std::cout << "[Craft] " << shaper->getName() << " crafted new Isle " << isleName << std::endl;

        // Deduct the energy used for crafting
        shaper->loseEnergy();  // Deduct energy for crafting


    } else {
        // If the realm shaper doesn't have enough energy
        std::cout << "[Energy] " << shaper->getName() << " does not have enough energy points: " << shaper->getEnergyLevel() << std::endl;
    }

}

void GameWorld::displayGameState()
{
    //mapTree.displayMap();
    //shaperTree.displayTree();
}


void GameWorld::processGameEvents(const std::string &accessLogs, const std::string &duelLogs)
{
    std::ifstream accessFile(accessLogs);  // Open access log file
    std::ifstream duelFile(duelLogs);      // Open duel log file

    std::string accessLine;  // Holds each line from the access log
    std::string duelLine;    // Holds each line from the duel log

    int accessCount = 0;  // Tracks how many access events have occurred

    // Process each access log line
    while (std::getline(accessFile, accessLine)) {
        // Parse the access log line to extract RealmShaper and Isle names
        std::string realmShaperName, isleName;
        std::istringstream accessStream(accessLine);
        accessStream >> realmShaperName >> isleName;

        // Find the RealmShaper and Isle objects using their names
        RealmShaper *shaper = shaperTree.findPlayer(realmShaperName);  // Find player by name
        Isle *isle = mapTree.findIsle(isleName);  // Find isle by name

        // If both player and isle are valid, call exploreArea and craft functions
        if (shaper && isle) {
            exploreArea(shaper, isle);  // Explore the area for the given RealmShaper and Isle
            craft(shaper, isleName);  // Craft a new isle with the provided name (if possible)
            accessCount++;  // Increment access counter
        }

        // If 5 accesses have been processed, perform a duel
        if (accessCount == 5) {
            if (std::getline(duelFile, duelLine)) {
                // Parse the duel log line to extract the names of the two players
                std::string shaper1Name, shaper2Name;
                std::istringstream duelStream(duelLine);
                duelStream >> shaper1Name >> shaper2Name;

                // Find the two RealmShapers by their names
                RealmShaper *shaper1 = shaperTree.findPlayer(shaper1Name);
                RealmShaper *shaper2 = shaperTree.findPlayer(shaper2Name);

                // If both players are found, perform the duel logic
                if (shaper1 && shaper2) {
                    // Call duel logic (implement the duel function as needed)
                    std::cout << "[Duel] " << shaper1->getName() << " vs " << shaper2->getName() << std::endl;
                    // Implement duel results (update player stats or winner)
                }
            }

            // After performing the duel, reset access count
            accessCount = 0;
        }
    }

    // Close the log files
    accessFile.close();
    duelFile.close();

    // Display the final game state after processing all events
    displayGameState();  // Implement this function to output the game state
}



void GameWorld::saveGameState(const std::string &currentIsles, const std::string &currentWorld, const std::string &currentShapers, const std::string &currentPlayerTree)
{
    mapTree.writeIslesToFile(currentIsles);
    mapTree.writeToFile(currentWorld);
    shaperTree.writeToFile(currentPlayerTree);
    shaperTree.writeShapersToFile(currentShapers);
}
