#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <chrono>
#include <map>

struct Solution {
    double rank, x, y, z;
    int parent1; // To keep track of the first parent
    int parent2; // To keep track of the second parent

    void fitness() {
        double ans = (6 * x - y + std::pow(z, 200)) - 25;
        rank = (ans == 0) ? 9999 : std::abs(1 / ans);
    }
};

void printTopSolutions(const std::vector<Solution>& solutions, int count) {
    std::cout << "Top " << count << " Solutions:\n";
    for (size_t i = 0; i < std::min(solutions.size(), static_cast<size_t>(count)); ++i) {
        const auto& s = solutions[i];
        std::cout << std::fixed
                  << "Rank: " << static_cast<int>(s.rank)
                  << ", x: " << s.x 
                  << ", y: " << s.y 
                  << ", z: " << s.z 
                  << "\n";
    }
    std::cout << std::string(50, '-') << "\n";
}

void printStatistics(const std::vector<Solution>& solutions) {
    double avgX = 0, avgY = 0, avgZ = 0;
    for (const auto& s : solutions) {
        avgX += s.x;
        avgY += s.y;
        avgZ += s.z;
    }
    avgX /= solutions.size();
    avgY /= solutions.size();
    avgZ /= solutions.size();

    std::cout << "Average Values of Top Solutions:\n";
    std::cout << "Average x: " << avgX 
              << ", Average y: " << avgY 
              << ", Average z: " << avgZ 
              << "\n" << std::string(50, '-') << "\n";
}

void explainParameters() {
    std::cout << "Parameters Explanation:\n";
    std::cout << "- x: Represents a parameter that influences the fitness function.\n";
    std::cout << "- y: Another parameter that affects the overall fitness score.\n";    
    std::cout << "- z: A critical parameter raised to a high power in the fitness calculation.\n";    
    std::cout << "These parameters are randomly initialized and evolved over generations to optimize the fitness score.\n\n";    
}

void displayFamilyTree(const std::map<int, std::pair<int, int>>& familyTree) {
    std::cout << "Family Tree of Surviving Species:\n";    
    for (const auto& entry : familyTree) {
        int child = entry.first;    
        int parent1 = entry.second.first;    
        int parent2 = entry.second.second;    
        std::cout << "Child Rank: " << child 
                  << ", Parents: (" << parent1 
                  << ", " << parent2 << ")\n";    
    }
}

int main() {
    std::random_device device;    
    std::uniform_real_distribution<double> unif(-10000, 10000);    
    const int NUM = 100000;

    // User input for generations and sample size    
    int generations;    
    int sampleSize;

    std::cout << "Enter the number of generations to run: ";
    std::cin >> generations;

    std::cout << "Enter the sample size for selection (e.g., 1000): ";
    std::cin >> sampleSize;

    // Initialize population    
    std::vector<Solution> solutions(NUM);    
    for (auto& s : solutions) {        
        s = Solution{0, unif(device), unif(device), unif(device), -1, -1};        
    }

    explainParameters(); // Explain parameters before starting the simulation

    for (int gen = 0; gen < generations; ++gen) {
        // Evaluate fitness        
        for (auto& s : solutions) {            
            s.fitness();        
        }

        // Sort solutions based on rank        
        std::sort(solutions.begin(), solutions.end(),
            [](const auto& lhs, const auto& rhs) {            
                return lhs.rank > rhs.rank;        
            });        

        // Print generation number        
        std::cout << "\nGeneration " << gen + 1 << "\n";

        // Print top solutions at each generation        
        printTopSolutions(solutions, 10);

        // Print statistics of top solutions        
        printStatistics(solutions);

        // Take top solutions        
        std::vector<Solution> sample(solutions.begin(), solutions.begin() + sampleSize);
        solutions.clear();

        // Map to keep track of family tree        
        std::map<int, std::pair<int, int>> familyTree;

        // Mutate the top solutions by a small percentage        
        std::uniform_real_distribution<double> mutation(0.99, 1.01);
        for (size_t i = 0; i < sample.size(); ++i) {
            sample[i].x *= mutation(device);
            sample[i].y *= mutation(device);
            sample[i].z *= mutation(device);

            // Print which rank survives and is being mutated            
            std::cout << "Mutating Rank: " << static_cast<int>(sample[i].rank)
                      << ", New x: " << sample[i].x 
                      << ", New y: " << sample[i].y 
                      << ", New z: " << sample[i].z 
                      << "\n";

            if (i > 0) { // Ensure there are parents to assign                
                sample[i].parent1 = static_cast<int>(sample[i - 1].rank);                
                sample[i].parent2 = static_cast<int>(sample[i].rank); // Current rank as second parent                
                familyTree[sample[i].rank] = {sample[i].parent1, sample[i].parent2};            
            }            

            solutions.push_back(sample[i]); // Keep mutated solution        
        }

        // Crossover to create new generation        
        std::uniform_int_distribution<int> cross(0, sampleSize - 1);        
        for (int i = 0; i < NUM; ++i) {            
            Solution offspring{                
                0,
                sample[cross(device)].x,
                sample[cross(device)].y,
                sample[cross(device)].z,
                static_cast<int>(sample[cross(device)].rank),   // Assigning a random parent rank as first parent
                static_cast<int>(sample[cross(device)].rank)   // Assigning a random parent rank as second parent
            };
            solutions.push_back(offspring);
            familyTree[offspring.rank] = {offspring.parent1, offspring.parent2}; // Track family tree for offspring
        }

        // Optional: Sleep to slow down output for better visibility
        if ((gen + 1) % 5 == 0) { 
            displayFamilyTree(familyTree); // Display family tree every few generations
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "End of Generation " << gen + 1 << "\n" 
                      << std::string(50, '-') << "\n";
        }
        
        if ((gen + 1) == generations) { 
            displayFamilyTree(familyTree); // Display family tree at end of all generations
         }
    }

    return 0;
}