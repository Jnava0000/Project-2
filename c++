#include <iostream>
#include <stdexcept>

void unlockFeature(const std::string& feature) {
    try {
        if (feature.empty()) {
            throw std::invalid_argument("Feature name cannot be empty.");
        }
        std::cout << "Unlocking feature: " << feature << std::endl;
        // Simulate unlocking process
        // Add actual unlocking logic here
    } catch (const std::exception& e) {
        std::cerr << "Error unlocking feature: " << e.what() << std::endl;
    }
}
