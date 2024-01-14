#ifndef COMPUTSeed_HPP
#define COMPUTSeed_HPP

#include <opencv2/opencv.hpp>
#include "Seed.hpp"
#include <vector>

/**
 * Class createing a seed
*/
class ComputeSeed {
    public:
        /**
        * Default constructor
        */
        ComputeSeed();

        /**
        * Constructor
        * @param rows the number of rows of the image
        * @param cols the number of columns of the image
        * @param nbSeed the number of seeds to create by defaut, 30% of the image
        * @param rep indicator of distribution of seeds by default 16
        */
        ComputeSeed(const int rows, const int cols, unsigned int nbSeed = 0, const int rep = 16);

        /**
        * Destructor
        */
        ~ComputeSeed();

        /**
        * get the vector of seeds
        */
        std::vector<Seed *> getSeedVector() const;

    private:
        std::vector<Seed *> seedVector;
    
};

#endif // ComputeSeed_HPP