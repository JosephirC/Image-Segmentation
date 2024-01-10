#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"
#include "ComputeRegions.hpp"

int main(int argc, char** argv) {
    // We load the image
    cv::Mat image = cv::imread("Images/lena_color.png");
    cv::imshow("Image orignal", image);
    
    cv::Mat outputImage1;
    filtreBruitBilateral(image, outputImage1, 5);
    cv::imshow("Image filtree 5", outputImage1);

    // We equalize the image    
    cv::Mat image2;
    etirerHistogrammeCouleur(outputImage1, image2, 0, 255);

    cv::Mat imageShow = image2.clone();
    std::cout << "size" << image.rows << " / " << image.cols <<std::endl;
    cv::imshow("Image Egaliser apres Blur de 5", imageShow);

    // We creat the regions
    std::cout << "We creat the regions" << std::endl;
    ComputeRegions regions(image2, 0.8, 16);

    // We put the seeds in the image
    regions.putSeeds();

    // We calculate the regions
    regions.calculateToTheEnd(150);

    regions.display2("BLUR NO SMOOTHING 1fst step", 1);

    // We merge the regions
    regions.merge();
    regions.display2("BLUR NO SMOOTHING merge");

    regions.displayBorderInner(" BLUR NO SMOOTHING border_inner_merge", 2);

    //regions.smoothingReg();
    // regions.display2("BLUR NO SMOOTHING smoothing de base");

    regions.displayBorderInner("BLUR NO SMOOTHING border_inner_smooth", 2);

    regions.reCalculateRegions(20.0);
    regions.display2("BLUR NO SMOOTHING region_after_recalcul");

    regions.merge();
    regions.display2("BLUR NO SMOOTHING merge_recalcul");

    // regions.smoothingReg();
    // regions.display2("BLUR NO SMOOTHING smoothing_after_merge_after_recalcul");

    std::cout << "END" << std::endl; 
    cv::waitKey(0);
    return 0;
}
