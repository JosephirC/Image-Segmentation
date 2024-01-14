#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include "ComputeSeed.hpp"
#include "Seed.hpp"

ComputeSeed::ComputeSeed() {
    
}

ComputeSeed::ComputeSeed(const int rows, const int cols, unsigned int nbSeed, const int rep) {
    if (nbSeed == 0) {
        nbSeed = rows * cols * 0.001;
    }

    std::cout << "cols and rows" << cols << "/" << rows<< std::endl;
    std::cout << "nbSeed" << nbSeed<< std::endl;
    // We initialize the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    // We divide the image in rep parts
    int nb_part_col_rows = sqrt(rep);
    int rowsPart = rows / nb_part_col_rows;
    int colsPart = cols / nb_part_col_rows; 
    int rowsIndex = 0;
    int colsIndex = 0;
    // We create the seeds
    for (int i = 0; i < nb_part_col_rows; i++) {
        for (int j = 0; j < nb_part_col_rows; j++) {
            for (int unsigned x = 0; x < nbSeed; x++) {
                // We create a point in the part of the image
                int xPoint = std::rand() % (rowsPart);
                int yPoint = std::rand() % (colsPart);
                // std::cout << "xPoint " << xPoint << std::endl;
                // std::cout << "yPoint " << yPoint << std::endl;
                Seed seed (xPoint, yPoint);
                // std::cout << "seed " << seed.getX() << "/" << seed.getY() << std::endl;
                Seed * seedPointeur = new Seed(colsIndex * colsPart + yPoint, rowsIndex * rowsPart + xPoint);
                // std::cout << "seed " << seed.getX() << "/" << seed.getY() << std::endl;
                seedVector.push_back(seedPointeur);
            }
            colsIndex ++;
        }
        rowsIndex ++;
        colsIndex = 0;
    }
}

ComputeSeed::~ComputeSeed() {
    for (auto& seed : seedVector) {
        delete seed;
        seed = nullptr;
    }
    seedVector.clear();
}

std::vector<Seed *> ComputeSeed::getSeedVector() const {
        return seedVector;
    }
