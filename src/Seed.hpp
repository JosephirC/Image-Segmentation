#ifndef SEED_HPP
#define SEED_HPP

#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

/**
 * Class createing a seed
*/
class Seed {
public:
    /**
     * Default constructor
    */
    Seed();

    /**
     * Constructor
     * @param x : x coordinate
     * @param y : y coordinate
    */
    Seed(const int x, const int y);

    /**
    * Constructor
    * @param point : point
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
     * @param point : point
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
     * @param seed : seed
    */
    bool operator==(const Seed& seed) const;

private:
    cv::Point point;
};

#endif // SEED_HPP