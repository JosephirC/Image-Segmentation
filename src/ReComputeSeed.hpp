#ifndef RECOMPUTRESEED_HPP
#define RECOMPUTRESEED_HPP

#include <opencv2/opencv.hpp>
#include "ReSeed.hpp"
#include <vector>

/**
 * Class creating a seed
*/
class ReComputeSeed {
    public:
        /**
        * Default constructor
        */
        ReComputeSeed();

        /**
        * Constructor
        * @param rows the number of rows of the image
        * @param cols the number of columns of the image
        * @param nbSeed the number of seeds to create by defaut, 30% of the image
        * @param rep indicator of distribution of seeds by default 16
        */
        ReComputeSeed(const int rows, const int cols, unsigned int nbSeed = 0, const int rep = 16);

        /**
        * Destructor
        */
        ~ReComputeSeed();

        /**
        * get the vector of seeds
        */
        std::vector<ReSeed *> getSeedVector() const;

    private:
        std::vector<ReSeed *> seedVector;
    
};

#endif // RECOMPUTESEED_HPP