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
    Seed();

    /**
     * Constructor
    */
    Seed(const int x, const int y);

    /**
     * Constructor
    */
    Seed(const cv::Point& point);

    /**
     * Destructor
    */
    ~Seed();

    struct HashFunction {
        size_t operator()(const Seed& seed) const {
            return std::hash<int>()(seed.point.x) ^ (std::hash<int>()(seed.point.y) << 1);
        }
    };

    /**
     * Get the point
    */
    cv::Point getPoint() const;

    /**
     * Set the point
    */
    void setPoint(const cv::Point& point);

    /**
     * get the x coordinate
    */
    int getX();

    /**
     * get the y coordinate
    */
    int getY();

    /**
     * operator ==
    */
    bool operator==(const Seed& seed) const;

private:
    cv::Point point;
};

#endif // SEED_HPP