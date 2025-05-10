#include "Map.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

Map::Map()   {
  this->root = nullptr;
}

Map::~Map()
{
    destroyTree(root);
}

void Map::destroyTree(MapNode* node)
{
    if (node == nullptr) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node->isle;  // Make sure to delete the Isle object
    delete node;
}

void Map::initializeMap(std::vector<Isle *> isles)
{
    for (auto isle : isles) {
        insert(isle);
    }
    populateWithItems();
}

MapNode *Map::rotateRight(MapNode *current)
{
    if (!current || !current->left) {
        std::cerr << "[Right Rotation] " << "Called on invalid node!" << std::endl;
        return nullptr;
    }

    MapNode *newRoot = current->left;
    current->left = newRoot->right;
    newRoot->right = current;

    // Update heights (calculate after rotation)
    current->height = std::max(height(current->left), height(current->right)) + 1;
    newRoot->height = std::max(height(newRoot->left), height(newRoot->right)) + 1;

    return newRoot;
}

MapNode *Map::rotateLeft(MapNode *current)
{
    if (!current || !current->right) {
        std::cerr << "[Left Rotation] " << "Called on invalid node!" << std::endl;
        return nullptr;
    }

    MapNode *newRoot = current->right;
    current->right = newRoot->left;
    newRoot->left = current;

    // Update heights (calculate after rotation)
    current->height = std::max(height(current->left), height(current->right)) + 1;
    newRoot->height = std::max(height(newRoot->left), height(newRoot->right)) + 1;

    return newRoot;
}

int Map::height(MapNode* node) {
    if (!node) return 0;  // Base case: if node is null, height is 0
    return 1 + std::max(height(node->left), height(node->right));
}
MapNode *Map::insert(MapNode *node, Isle *isle)
{
    if (!node) {
        node = new MapNode(isle);
        return node;
    }

    if (*isle < *node->isle) {
        node->left = insert(node->left, isle);
    } else if (*isle > *node->isle) {
        node->right = insert(node->right, isle);
    } else {
        // Isle already exists, do not insert
        return node;
    }

    node->height = 1 + std::max(height(node->left), height(node->right));

    // Balance the tree
    int balance = height(node->left) - height(node->right);
    if (balance > 1 && *isle < *node->left->isle) {
        return rotateRight(node);
    }
    if (balance < -1 && *isle > *node->right->isle) {
        return rotateLeft(node);
    }
    if (balance > 1 && *isle > *node->left->isle) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && *isle < *node->right->isle) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

void Map::insert(Isle *isle)
{
    root = insert(root, isle);
}

MapNode* Map::remove(MapNode* node, Isle* isle)
{
    if (node == nullptr) {
        std::cout << "[Remove] Tree is Empty" << std::endl;
        return nullptr;
    }

    // Step 1: Perform standard BST deletion
    if (isle->getName() < node->isle->getName()) {
        // Isle is in the left subtree
        node->left = remove(node->left, isle);
    } else if (isle->getName() > node->isle->getName()) {
        // Isle is in the right subtree
        node->right = remove(node->right, isle);
    } else {
        // Node to be deleted is found
        if (node->left == nullptr && node->right == nullptr) {
            // Case 1: No children (leaf node)
            delete node;
            return nullptr;
        } else if (node->left == nullptr) {
            // Case 2: One child (right)
            MapNode* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            // Case 3: One child (left)
            MapNode* temp = node->left;
            delete node;
            return temp;
        } else {
            // Case 4: Two children
            // Find the rightmost child in the left subtree
            MapNode* rightmost = node->left;
            while (rightmost->right != nullptr) {
                rightmost = rightmost->right;
            }
            // Replace current node's isle with rightmost child's isle
            node->isle = rightmost->isle;
            // Remove the rightmost child in the left subtree
            node->left = remove(node->left, rightmost->isle);
        }
    }

    // Step 2: Update the height of the current node
    int leftHeight = (node->left) ? node->left->height : 0;
    int rightHeight = (node->right) ? node->right->height : 0;
    node->height = 1 + std::max(leftHeight, rightHeight);

    // Step 3: Rebalance the node (calculate balance factor directly)
    int balance = leftHeight - rightHeight;

    // Left heavy
    if (balance > 1) {
        int leftChildBalance = (node->left->left ? node->left->left->height : 0) -
                               (node->left->right ? node->left->right->height : 0);
        if (leftChildBalance >= 0) {
            // Left-Left case
            return rotateRight(node);
        } else {
            // Left-Right case
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }

    // Right heavy
    if (balance < -1) {
        int rightChildBalance = (node->right->left ? node->right->left->height : 0) -
                                (node->right->right ? node->right->right->height : 0);
        if (rightChildBalance <= 0) {
            // Right-Right case
            return rotateLeft(node);
        } else {
            // Right-Left case
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }

    return node;
}


void Map::remove(Isle *isle)
{
    root = remove(root, isle);
}

void Map::preOrderItemDrop(MapNode *current, int &count) {
    if (!current) return;
	count++;
    // Pre-Order: Current Node First
    if (count % 5 == 0 ) {
        current->isle->setItem(EINSTEINIUM);
        std::cout << "[Item Drop] EINSTEINIUM dropped on Isle: "
                  << current->isle->getName() << std::endl;
    }


    preOrderItemDrop(current->left, count);
    preOrderItemDrop(current->right, count);
}


void Map::postOrderItemDrop(MapNode *current, int &count) {
    if (!current) return;

    // Post-Order: Left -> Right -> Current Node
    postOrderItemDrop(current->left, count);
    postOrderItemDrop(current->right, count);
	count++;
    // Boş olan Isle'lara sadece GOLDIUM bırakılır
    if (count % 3 == 0 ) {
        current->isle->setItem(GOLDIUM);
        std::cout << "[Item Drop] GOLDIUM dropped on Isle: "
                  << current->isle->getName() << std::endl;
    }


}


MapNode* Map::findFirstEmptyIsle(MapNode* node) {
    if (!node) return nullptr; // If the node is null, return immediately

    std::queue<MapNode*> q; // Queue for BFS traversal
    q.push(node);           // Start BFS from the given node

    while (!q.empty()) {
        MapNode* current = q.front(); // Get the front node in the queue
        q.pop(); // Remove it from the queue

        // Check if the current node's isle is empty
        if (current->isle && current->isle->getItem() == EMPTY) {
            return current; // Return the node if it's an empty Isle
        }

        // Push left and right children to the queue
        if (current->left) q.push(current->left);
        if (current->right) q.push(current->right);
    }

    return nullptr; // Return nullptr if no empty Isle was found
}


void Map::dropItemBFS() {
    MapNode* emptyIsleNode = findFirstEmptyIsle(root); // Find the first empty Isle starting from the root

    if (emptyIsleNode) {
        // Drop AMAZONITE on the found empty Isle
        emptyIsleNode->isle->setItem(AMAZONITE);
        std::cout << "[BFS Drop] AMAZONITE dropped on Isle: "
                  << emptyIsleNode->isle->getName() << std::endl;
    } else {
        // No empty Isle found
        std::cout << "[BFS Drop] No eligible Isle found for AMAZONITE drop." << std::endl;
    }
}

void Map::displayMap()
{
    std::cout << "[World Map]" << std::endl;
    display(root, 0, 0);
}

int Map::getDepth(MapNode *node)
{
    return node ? height(node) : -1;
}

int Map::getIsleDepth(Isle *isle)
{
    return getDepth(findNode(*isle));
}

int Map::getDepth()
{
    return getDepth(root);
}
MapNode* Map::findMin(MapNode* node)
{
    if (node == nullptr) {
        return nullptr; // No node, so return null
    }

    // Keep going left until we reach the leftmost node
    while (node->left != nullptr) {
        node = node->left;
    }

    return node; // Return the leftmost node, which is the minimum
}
void Map::addIsle(Isle *isle)
{
    root = insert(root, isle);  // Assuming insert adds an isle to the tree
}

void Map::removeIsle(Isle *isle)
{
    root = remove(root, isle);  // Assuming remove deletes an isle from the tree
}

void Map::populateWithItems()
{
    int goldiumCount = 0;
    int einsteiniumCount = 0;

    // Post-order traversal for GOLDIUM drop
    postOrderItemDrop(this->root, goldiumCount);

    // Pre-order traversal for EINSTEINIUM drop
    preOrderItemDrop(this->root, einsteiniumCount);
}

int Map::calculateMinMapDepthAccess(int playerDepth, int totalShaperTreeHeight, int totalMapDepth){
  	return (int)totalMapDepth * ((double)playerDepth / totalShaperTreeHeight);
  }

Isle *Map::findIsle(Isle isle)
{
    MapNode *node = findNode(isle);
    return node ? node->isle : nullptr;
}

Isle *Map::findIsle(std::string name)
{
    MapNode *node = findNode(name);
    return node ? node->isle : nullptr;
}

MapNode *Map::findNode(Isle isle)
{
    return findNode(isle.getName());
}

MapNode *Map::findNode(std::string name)
{
    MapNode *current = root;
    while (current) {
        if (current->isle->getName() == name) return current;
        else if (name < current->isle->getName()) current = current->left;
        else current = current->right;
    }
    return nullptr;
}

void Map::display(MapNode *current, int depth, int state)
{
    // SOURCE:

    if (current->left)
        display(current->left, depth + 1, 1);

    for (int i = 0; i < depth; i++)
        printf("     ");

    if (state == 1) // left
        printf("   ┌───");
    else if (state == 2) // right
        printf("   └───");

    std::cout << "[" << *current->isle << "] - (" << current->height << ")\n"
              << std::endl;

    if (current->right)
        display(current->right, depth + 1, 2);
}


// Helper function to write a tree to file level by level (BFS)
void Map::writeToFile(const std::string &filename)
{
    if (!root)
    {
        std::cout << "[Output] The tree is empty. No data to write to file." << std::endl;
        return;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    std::queue<MapNode *> q;
    q.push(root);

    while (!q.empty())
    {
        int levelSize = q.size(); // Number of nodes at the current level

        for (int i = 0; i < levelSize; ++i)
        {
            MapNode *current = q.front();
            q.pop();

            if (current)
            {
                outFile << current->isle->getName() << " "; // Adjust getName() based on your Isle class

                // Enqueue children
                q.push(current->left);
                q.push(current->right);
            }
            else
            {
                outFile << "NULL "; // Indicate null nodes in the tree
            }
        }
        outFile << "\n"; // End of the current level
    }

    outFile.close();
    std::cout << "[Output] Tree has been written to " << filename << " level by level." << std::endl;
}

// Write Isles to file in alphabetical order
void Map::writeIslesToFile(const std::string &filename)
{
    if (!root)
    {
        std::cout << "[Output] The tree is empty. No Isles to write to file." << std::endl;
        return;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    std::vector<Isle *> isles;
    collectIsles(root, isles); // Collect all Isles using in-order traversal

    // Sort Isles alphabetically by name
    std::sort(isles.begin(), isles.end(), [](Isle *a, Isle *b) {
        return a->getName() < b->getName(); // Adjust getName() for Isle class
    });

    // Write sorted Isles to the file
    for (Isle *isle : isles)
    {
        outFile << isle->getName() << std::endl; // Write Isle names line by line
    }

    outFile.close();
    std::cout << "[Output] Isles have been written to " << filename << " in alphabetical order." << std::endl;
}

void Map::collectIsles(MapNode *node, std::vector<Isle *> &isles)
{
    if (!node) return;
    collectIsles(node->left, isles);
    isles.push_back(node->isle);
    collectIsles(node->right, isles);
}
