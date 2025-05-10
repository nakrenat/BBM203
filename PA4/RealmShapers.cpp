#include "RealmShapers.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <iostream>
#include <fstream>

ShaperTree::ShaperTree() {}

ShaperTree::~ShaperTree() {
    // Delete dynamically allocated shapers
    for (RealmShaper* shaper : realmShapers) {
        delete shaper;
    }
    realmShapers.clear();  // Optional: clear the vector for safety
}

void ShaperTree::initializeTree(std::vector<RealmShaper *> shapers) {
    // Iterate over the provided shapers and insert them into the tree
    for (RealmShaper* shaper : shapers) {
        // Insert each shaper using the insert method (which preserves tree order)
        insert(shaper);
    }
}

int ShaperTree::getSize() {
    return realmShapers.size();  // Return the number of shapers in the tree
}

std::vector<RealmShaper *> ShaperTree::getTree() {
    return realmShapers;  // Return the entire tree
}

bool ShaperTree::isValidIndex(int index) {
  	bool isValid = false;
    if( index >= 0 && index < realmShapers.size()){
      isValid = true;
      }
      return isValid;
}

void ShaperTree::insert(RealmShaper* shaper) {
    realmShapers.push_back(shaper);
}


int ShaperTree::remove(RealmShaper *shaper) {
    int index = findIndex(shaper);
    if (index == -1) return -1;

    // Remove the element and shift others
    realmShapers.erase(realmShapers.begin() + index);
    return index;
}


int ShaperTree::findIndex(RealmShaper *shaper) {
    int index = 0; // Start from the root of the binary heap

    while (index < realmShapers.size()) {
        // If the current node matches the shaper, return its index
        if (realmShapers[index] == shaper) {
            return index;
        }

        // Calculate left and right child indices
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;

        // Check left child
        if (leftChild < realmShapers.size() && realmShapers[leftChild] == shaper) {
            return leftChild;
        }

        // Check right child
        if (rightChild < realmShapers.size() && realmShapers[rightChild] == shaper) {
            return rightChild;
        }

        // Move to the next node in the vector
        index++;
    }

    return -1; // If not found, return -1
}

int ShaperTree::getDepth(RealmShaper *shaper) {
    int index = findIndex(shaper);
    if (index == -1) return -1;

    int depth = 0;
    while (index > 0) {  // Traverse upwards to find the depth
        index = (index - 1) / 2;
        depth++;
    }
    return depth;
}

int ShaperTree::getDepth() {
    if (realmShapers.empty()) return 0;  // No nodes, depth is 0
    return std::ceil(std::log2(realmShapers.size() + 1)) - 1;
}

RealmShaper ShaperTree::duel(RealmShaper *challenger, bool result)
{
    if (challenger == nullptr) {
        std::cout << "[Error] Invalid challenger in duel." << std::endl;
        return *challenger;
    }

    // 1. Rakip oyuncuyu belirle (parent node)
    RealmShaper* opponent = getParent(challenger); // Parent node'u bul
    if (opponent == nullptr) {
        std::cout << "[Error] No opponent (parent) found for the challenger." << std::endl;
        return *challenger;
    }

    std::string challengerName = challenger->getName();
    std::string opponentName = opponent->getName();

    // 2. Duel sonucu: kazanan ve kaybedeni belirle
    RealmShaper* winner = result ? challenger : opponent;
    RealmShaper* loser = result ? opponent : challenger;

    // 3. Honour puanlarını güncelle
    if (result) {
        winner->gainHonour(); // Kazanan Honour kazanır
        loser->loseHonour();  // Kaybeden Honour kaybeder
    } else {
        winner->gainHonour();
        loser->loseHonour();
    }

    // 4. Duel sonuçlarını ekrana yazdır
    std::cout << "[Duel] " << winner->getName() << " won the duel" << std::endl;
    std::cout << "[Honour] New honour points: ";
    std::cout << challengerName << "-" << challenger->getHonour() << " ";
    std::cout << opponentName << "-" << opponent->getHonour() << std::endl;

    // 5. Honour sıfıra düşerse, kaybedeni ağaçtan sil
    if (loser->getHonour() <= 0) {
        std::cout << "[Duel] " << loser->getName() << " lost all honour, delete" << std::endl;
        remove(loser); // Ağacın remove fonksiyonunu çağır
    }

    // 6. Eğer challenger kazandıysa, pozisyonlarını değiştir
    if (result) {
        replace(challenger, opponent); // Challenger ile parent node yer değiştirir
    }

    return *winner; // Kazanan oyuncuyu döndür
}


RealmShaper *ShaperTree::getParent(RealmShaper *shaper)
{
    RealmShaper *parent = nullptr;  // Bu satır korunuyor

    int index = findIndex(shaper); // Shaper'ın indeksini bul
    if (index > 0) {
        int parentIndex = (index - 1) / 2;  // Parent'ın indeksini hesapla
        parent = realmShapers[parentIndex]; // Parent'ı ata
    }

    return parent;  // Parent'ı döndür
}

void ShaperTree::replace(RealmShaper *player_low, RealmShaper *player_high) {
    int lowIndex = findIndex(player_low);
    int highIndex = findIndex(player_high);
    if (lowIndex != -1 && highIndex != -1) {
        std::swap(realmShapers[lowIndex], realmShapers[highIndex]);
    }
}

// Find a shaper by object (using equality operator)
RealmShaper* ShaperTree::findPlayer(RealmShaper shaper) {
    RealmShaper* foundShaper = nullptr;

    // Iterate through all existing shapers in the tree
    for (auto &existingShaper : realmShapers) {
        if (*existingShaper == shaper) {  // Assuming operator== is implemented for RealmShaper
            foundShaper = existingShaper;
            break;
        }
    }

    return foundShaper;  // Return the found shaper or nullptr if not found
}

// Find a shaper by name
RealmShaper* ShaperTree::findPlayer(std::string name) {
    RealmShaper* foundShaper = nullptr;

    for (auto &existingShaper : realmShapers) {
        if (existingShaper->getName() == name) {
            foundShaper = existingShaper;
            break;
        }
    }

    return foundShaper;  // Return the found shaper or nullptr if not found
}

std::vector<std::string> ShaperTree::inOrderTraversal(int index) {
    std::vector<std::string> result;
    if (isValidIndex(index)) {
        // Traverse left, visit node, traverse right
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (isValidIndex(left)) {
            std::vector<std::string> leftResults = inOrderTraversal(left);
            result.insert(result.end(), leftResults.begin(), leftResults.end());
        }

        result.push_back(realmShapers[index]->getName());  // Visit current node

        if (isValidIndex(right)) {
            std::vector<std::string> rightResults = inOrderTraversal(right);
            result.insert(result.end(), rightResults.begin(), rightResults.end());
        }
    }
    return result;
}

std::vector<std::string> ShaperTree::preOrderTraversal(int index) {
    std::vector<std::string> result;
    if (isValidIndex(index)) {
        result.push_back(realmShapers[index]->getName());  // Visit current node

        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (isValidIndex(left)) {
            std::vector<std::string> leftResults = preOrderTraversal(left);
            result.insert(result.end(), leftResults.begin(), leftResults.end());
        }

        if (isValidIndex(right)) {
            std::vector<std::string> rightResults = preOrderTraversal(right);
            result.insert(result.end(), rightResults.begin(), rightResults.end());
        }
    }
    return result;
}

std::vector<std::string> ShaperTree::postOrderTraversal(int index) {
    std::vector<std::string> result;
    if (isValidIndex(index)) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (isValidIndex(left)) {
            std::vector<std::string> leftResults = postOrderTraversal(left);
            result.insert(result.end(), leftResults.begin(), leftResults.end());
        }

        if (isValidIndex(right)) {
            std::vector<std::string> rightResults = postOrderTraversal(right);
            result.insert(result.end(), rightResults.begin(), rightResults.end());
        }

        result.push_back(realmShapers[index]->getName());  // Visit current node
    }
    return result;
}

void ShaperTree::preOrderTraversal(int index, std::ofstream &outFile) {
    if (isValidIndex(index)) {
        outFile << realmShapers[index]->getName() << std::endl;  // Visit current node

        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (isValidIndex(left)) {
            preOrderTraversal(left, outFile);
        }

        if (isValidIndex(right)) {
            preOrderTraversal(right, outFile);
        }
    }
}

void ShaperTree::breadthFirstTraversal(std::ofstream &outFile) {
    std::queue<int> q;
    q.push(0);  // Start at the root node
    while (!q.empty()) {
        int index = q.front();
        q.pop();
        if (isValidIndex(index)) {
            outFile << realmShapers[index]->getName() << std::endl;
            q.push(2 * index + 1);  // Left child
            q.push(2 * index + 2);  // Right child
        }
    }
}

void ShaperTree::displayTree() {
    std::cout << "[Shaper Tree]" << std::endl;
    printTree(0, 0, "");
}

// Helper function to print tree with indentation
void ShaperTree::printTree(int index, int level, const std::string &prefix) {
    if (!isValidIndex(index))
        return;

    std::cout << prefix << (level > 0 ? "   └---- " : "") << *realmShapers[index] << std::endl;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (isValidIndex(left) || isValidIndex(right)) {
        printTree(left, level + 1, prefix + (level > 0 ? "   │   " : "")); // ╎
        printTree(right, level + 1, prefix + (level > 0 ? "   │   " : ""));
    }
}

void ShaperTree::writeShapersToFile(const std::string &filename)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Calling breadthFirstTraversal to write Shapers to file
    breadthFirstTraversal(outFile);

    outFile.close();
    std::cout << "[Output] Shapers have been written to " << filename << " according to rankings." << std::endl;
}

void ShaperTree::writeToFile(const std::string &filename)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Start pre-order traversal from index 0 (assuming the vector is the tree's root)
    preOrderTraversal(0, outFile);

    outFile.close();
    std::cout << "[Output] Shapers have been written to " << filename << " in pre-order." << std::endl;
}

