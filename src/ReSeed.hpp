#ifndef RESEED_HPP
#define RESEED_HPP

#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

/**
 * Class creating a seed
*/
class ReSeed {
public:
    /**
     * Default constructor
    */
    ReSeed();

    /**
     * Constructor
    */
    ReSeed(const int x, const int y);

    /**
     * Constructor
    */
    ReSeed(const cv::Point& point);

    /**
     * Destructor
    */
    ~ReSeed();

    struct HashFunction {
        size_t operator()(const ReSeed& seed) const {
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
    bool operator==(const ReSeed& seed) const;

private:
    cv::Point point;
};

#endif // RESEED_HPP