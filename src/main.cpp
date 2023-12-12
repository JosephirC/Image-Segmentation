#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"
#include "CreatRegions.hpp"
#include "Region.hpp"
#include "Seed.hpp"

int main() {
    // We load the image
    cv::Mat image = cv::imread("Images/lena_color.png");
    // We display the image
    cv::imshow("Image", image);
    // We creat the regions

    std::cout << "We creat the regions" << std::endl;
    CreatRegions regions(image);
    // We put the seeds in the image
    regions.putSeeds(1);

    // We calculate the regions
    std::cout << "We calculate the regions" << std::endl;
    for (int i = 0; i < 5; i++) {
        regions.calculateRegions();
    }
    // We display the regions
    std::cout << "We display the regions" << std::endl;
    // regions.display();
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
    
