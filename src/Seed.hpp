#ifndef SEED_HPP
#define SEED_HPP

#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

/**
 * Class creating a seed
*/
class Seed {
public:
    /**
     * Default constructor
    */
    Seed() {
        point = cv::Point(0, 0);
    }

    /**
     * Constructor
    */
    Seed(cv::Mat img) {
        // We choose a random point in the image
        point = cv::Point(rand() % img.cols, rand() % img.rows);
    }

    /**
     * Destructor
    */
    ~Seed() {};

    /**
     * Get the point
    */
    cv::Point getPoint() {
        return point;
    }

private:
    cv::Point point;
};

#endif // SEED_HPP