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
    Seed(const int rows, const int cols) {
        // We initialize the random number generator
        point.x = rand() % (cols - 1);
        point.y = rand() % (rows - 1);
    }

    /**
     * Destructor
    */
    ~Seed() {};

    struct HashFunction {
        size_t operator()(const Seed& seed) const {
            return std::hash<int>()(seed.point.x) ^ (std::hash<int>()(seed.point.y) << 1);
        }
    };

    /**
     * Get the point
    */
    cv::Point getPoint() const {
        // std::cout<< "GET THE POINT" << point.x << "/" << point.y << std::endl;
        return point;
    }

    /**
     * Set the point
    */
    void setPoint(const cv::Point& point) {
        this->point = point;
    }

    /**
     * get the x coordinate
    */
    int getX() {
        return point.x;
    }

    /**
     * get the y coordinate
    */
    int getY() {
        return point.y;
    }

    /**
     * operator ==
    */
    bool operator==(const Seed& seed) const {
        return (point.x == seed.point.x && point.y == seed.point.y);
    }

private:
    cv::Point point;
};

#endif // SEED_HPP