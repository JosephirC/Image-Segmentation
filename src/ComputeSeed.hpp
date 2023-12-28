#ifndef COMPUTRESEED_HPP
#define COMPUTRESEED_HPP

#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include "Seed.hpp"
#include <set>
#include <unordered_set>
#include <math.h>
#include <memory>

/**
 * Class creating a seed
*/
class ComputeSeed {
public:
    /**
     * Default constructor
    */
    ComputeSeed() {
        
    }

    /**
     * Constructor
     * @param rows the number of rows of the image
     * @param cols the number of columns of the image
     * @param nbSeed the number of seeds to create by defaut, 30% of the image
     * @param rep indicator of distribution of seeds by default 16
    */
    ComputeSeed(const int rows, const int cols, unsigned int nbSeed = 0, const int rep = 16) {
        if (nbSeed == 0) {
            nbSeed = rows * cols * 0.001;
        }
        std::cout << "cols and rows" << cols << "/" << rows<< std::endl;
        std::cout << "nbSeed" << nbSeed<< std::endl;
        // We initialize the random number generator
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        // We divise the image in rep parts
        int nb_part_col_rows = sqrt(rep);
        int rowsPart = rows / nb_part_col_rows;
        int colsPart = cols / nb_part_col_rows + 1; 
        int rowsIndex = 0;
        int colsIndex = 0;
        // We create the seeds
        for (int i = 0; i < nb_part_col_rows; i++) {
            for (int j = 0; j < nb_part_col_rows; j++) {
                for (int unsigned x = 0; x < nbSeed; x++) {
                    // We creat a point in the part of the image
                    int xPoint = std::rand() % (rowsPart);
                    int yPoint = std::rand() % (colsPart);
                    Seed seed (colsPart, rowsPart);
                    std::cout << "seed" << seed.getX() << "/" << seed.getY() << std::endl;
                    Seed * seedPointeur = new Seed(rowsIndex * rowsPart + xPoint, colsIndex * colsPart + yPoint);
                    std::cout << "seed" << seed.getX() << "/" << seed.getY() << std::endl;
                    seedVector.push_back(seedPointeur);
                }
                colsIndex ++;
            }
            rowsIndex ++;
            colsIndex = 0;
        }
    }

    /**
     * Destructor
    */
    ~ComputeSeed() {
        for (const auto& seed : seedVector) {
            delete seed;
        }
        seedVector.clear();
    };

    /**
     * get the vector of seeds
    */
    std::vector<Seed *> getSeedVector() const {
        return seedVector;
    }

private:
std::vector<Seed *> seedVector;
    
};

#endif // COMPUTRESEED_HPP