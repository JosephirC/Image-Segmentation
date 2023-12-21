#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"
#include "ComputeRegions.hpp"
#include "Region.hpp"
#include "Seed.hpp"
#include <ctime>

int main() {
    std::cout << "We start the program" << std::endl;

    

    // We load the image
    // cv::Mat image = cv::imread("Images/bigSegmentation.png");
    cv::Mat image = cv::imread("Images/segmentation.png");
    cv::Mat image2;
    etirerHistogrammeCouleur(image, image2, 0, 255);

    cv::Mat imageShow = image2.clone();
    // cv::Mat image = cv::imread("Images/baboon_color.png");

    // cv::Mat image = cv::imread("Images/lena_color.png");
    std::cout << "size" << image.rows << " / " << image.cols <<std::endl;

    // We display the image
    cv::resize(imageShow, imageShow, cv::Size(), 4, 4, cv::INTER_NEAREST);
    cv::imshow("Image", imageShow);
    // We creat the regions
    

    std::cout << "We creat the regions" << std::endl;
    ComputeRegions regions(image2, 300);
    // We put the seeds in the image
    regions.putSeeds();
    // We calculate the regions
    std::cout << "We calculate the regions" << std::endl;
    for (int i = 0; i < 300; i++) {
        regions.calculateRegions();
    }

    regions.display();
    regions.display2("1fst step");
    // regions.calculateAllRegions(500);
    std::cout << "We merge the regions" << std::endl;
    regions.merge();
    // We display the regions
    std::cout << "We display the regions" << std::endl;
    regions.display();
    regions.display2("merge");
    std::cout << "END" << std::endl; 
    // regions.displayoutlines();

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
