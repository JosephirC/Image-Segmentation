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
    CreatRegions regions(image);
    // We put the seeds in the image
    regions.putSeeds(1);

    // We calculate the regions
    for (int i = 0; i < 100; i++) {
        regions.calculateRegions();
    }
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
    
