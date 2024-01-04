#include "ReSeed.hpp"

#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>


ReSeed::ReSeed() {
    point = cv::Point(0, 0);
}

ReSeed::ReSeed(const int x, const int y) {
    point.x = x;
    point.y = y;
}

ReSeed::ReSeed(const cv::Point& point) {
    this->point = point;
}

ReSeed::~ReSeed() {}

cv::Point ReSeed::getPoint() const {
    // std::cout<< "GET THE POINT" << point.x << "/" << point.y << std::endl;
    return point;
}

void ReSeed::setPoint(const cv::Point& point) {
    this->point = point;
}

int ReSeed::getX() {
    return point.x;
}

int ReSeed::getY() {
    return point.y;
}

bool ReSeed::operator==(const ReSeed& seed) const {
    return (point.x == seed.point.x && point.y == seed.point.y);
}
