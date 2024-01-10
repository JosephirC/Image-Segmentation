#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include "Seed.hpp"

Seed::Seed() {
    point = cv::Point(0, 0);
}

Seed::Seed(const int x, const int y) {
    point.x = x;
    point.y = y;
}

Seed::Seed(const cv::Point& point) {
    this->point = point;
}

Seed::~Seed() {}

cv::Point Seed::getPoint() const {
    // std::cout<< "GET THE POINT" << point.x << "/" << point.y << std::endl;
    return point;
}

void Seed::setPoint(const cv::Point& point) {
    this->point = point;
}

int Seed::getX() {
    return point.x;
}

int Seed::getY() {
    return point.y;
}

bool Seed::operator==(const Seed& seed) const {
    return (point.x == seed.point.x && point.y == seed.point.y);
}
