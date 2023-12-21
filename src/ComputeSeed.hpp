#ifndef COMPUTRESEED_HPP
#define COMPUTRESEED_HPP

#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include "Seed.hpp"
#include <set>
#include <unordered_set>

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
            nbSeed = rows * cols * 0.3;
        }
        std::cout << "cols and rows" << cols << "/" << rows<< std::endl;
        // We initialize the random number generator
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        // We divise the image in rep parts
        int rowsPart = rows / rep;
        int colsPart = cols / rep;
        int rowsIndex = 0;
        int colsIndex = 0;
        // We create the seeds
        for (int i = 0; i < rep; i++) {
            for (int j = 0; j < rep; j++) {
                for (int unsigned x = 0; x < nbSeed / rep; x++) {
                    // We create a seed
                    Seed seed (colsPart, rowsPart);
                    std::cout << "seed" << seed.getX() << "/" << seed.getY() << std::endl;
                    seed.setPoint(cv::Point(rowsIndex * rowsPart + seed.getX(), colsIndex * colsPart + seed.getY()));
                    std::cout << "seed" << seed.getX() << "/" << seed.getY() << std::endl;
                    seedVector.push_back(seed);
                }
                colsIndex ++;
            }
            rowsIndex ++;
            colsIndex = 0;
        }
    }

    /**
     * get the vector of seeds
    */
    std::vector<Seed> getSeedVector() const {
        return seedVector;
    }

private:
std::vector<Seed> seedVector;
    
};

#endif // COMPUTRESEED_HPP