#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"
#include "CreatRegions.hpp"
#include "Region.hpp"
#include "Seed.hpp"
#include <ctime>

int main() {
    std::cout << "We start the program" << std::endl;

    // We initialize the random number generator
    // std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // We load the image
    cv::Mat image = cv::imread("Images/segmentation.png");
    // cv::Mat image = cv::imread("Images/lena_color.png");
    std::cout << "size" << image.rows << " / " << image.cols <<std::endl;

    // We display the image
    cv::imshow("Image", image);
    // We creat the regions
    

    std::cout << "We creat the regions" << std::endl;
    CreatRegions regions(image, 10);
    // We put the seeds in the image
    regions.putSeeds();

    // We calculate the regions
    std::cout << "We calculate the regions" << std::endl;
    for (int i = 0; i < 1000; i++) {
        regions.calculateRegions();
    }
    // We display the regions
    std::cout << "We display the regions" << std::endl;
    regions.display();
    // regions.displayContours();

    // // We calculate the regions
    // regions.calculateRegions();
    // // We display the regions
    // regions.displayRegions();
    // // We display the image with the regions
    // cv::imshow("Image with regions", regions.getImage());
    // // We wait for a key to be pressed
    // cv::waitKey(0);
    return 0;
}
