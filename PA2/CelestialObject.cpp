#include "CelestialObject.h"

// Constructor to initialize CelestialObject with essential properties
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row,
                                 int time_of_appearance)
        : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance) {
    
    // Initial setup can be done here if needed
}

// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
        : shape(other->shape),  // Copy the 2D vector shape
          object_type(other->object_type),  // Copy the object type
          starting_row(other->starting_row),  // Copy the starting row
          time_of_appearance(other->time_of_appearance)  // Copy the time of appearance
{
    // Additional copying logic can be added here if needed
}

// Function to delete rotations of a given celestial object. It should free the dynamically allocated
// memory for each rotation.
void CelestialObject::delete_rotations(CelestialObject *target) {
    // Check if the target has rotations
    if (target->right_rotation) {
        // Break the circular links first before deleting
        target->right_rotation->left_rotation = target->left_rotation; // The left rotation of right_rotation should point to the left rotation of target
        target->left_rotation->right_rotation = target->right_rotation; // The right rotation of left_rotation should point to the right rotation of target

        // Now we can safely delete the right rotation
        delete target->right_rotation;
        target->right_rotation = nullptr; // Set to nullptr after deletion to avoid dangling pointer
    }

    if (target->left_rotation) {
        // Break the circular links first before deleting
        target->right_rotation->left_rotation = target->left_rotation; // The left rotation of right_rotation should point to the left rotation of target
        target->left_rotation->right_rotation = target->right_rotation; // The right rotation of left_rotation should point to the right rotation of target

        // Now we can safely delete the left rotation
        delete target->left_rotation;
        target->left_rotation = nullptr; // Set to nullptr after deletion to avoid dangling pointer
    }
}
