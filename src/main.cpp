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
    // cv::Mat image = cv::imread("Images/segmentation.png");
    // cv::Mat image = cv::imread("Images/baboon_color.png");
    // cv::Mat image = cv::imread("Images/lena_color.png");
    cv::Mat image = cv::imread("Images/4couleurs.png");
    cv::Mat image2;
    etirerHistogrammeCouleur(image, image2, 0, 255);
    cv::Mat imageShow = image2.clone();
    std::cout << "size" << image.rows << " / " << image.cols <<std::endl;

    // We display the image
    cv::imshow("Image Egaliser", imageShow);
    // We creat the regions
    
    std::cout << "We creat the regions" << std::endl;
    ComputeRegions regions(image2, 0.1, 16);
    // We put the seeds in the image
    regions.putSeeds();
    // We calculate the regions
    std::cout << "We calculate the regions" << std::endl;
    for (int i = 0; i < 50; i++) {
        regions.calculateRegions();
    }
    regions.display();
    regions.display2("1fst step", 1);
    // regions.calculateAllRegions(500);
    std::cout << "We merge the regions" << std::endl;
    regions.merge();
    regions.display2("merge");
    std::cout << "END" << std::endl; 
    cv::waitKey(0);
    // regions.displayoutlines();
    return 0;
}
