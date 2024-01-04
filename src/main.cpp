#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"
#include "ComputeRegions.hpp"
#include "Region.hpp"
#include "Seed.hpp"

int main() {
    auto start_total = std::chrono::high_resolution_clock::now();
    std::cout << "We start the program" << std::endl;

    // We load the image
    auto start_load = std::chrono::high_resolution_clock::now();
    // cv::Mat image = cv::imread("Images/bigSegmentation.png");
    // cv::Mat image = cv::imread("Images/segmentation.png");
    // cv::Mat image = cv::imread("Images/baboon_color.png");
    cv::Mat image = cv::imread("Images/lena_color.png");
    // cv::Mat image = cv::imread("Images/4couleurs.png");
    auto end_load = std::chrono::high_resolution_clock::now();
    
    // We equalize the image    
    cv::Mat image2;
    auto start_equalize = std::chrono::high_resolution_clock::now();
    etirerHistogrammeCouleur(image, image2, 0, 255);
    auto end_equalize = std::chrono::high_resolution_clock::now();

    cv::Mat imageShow = image2.clone();
    std::cout << "size" << image.rows << " / " << image.cols <<std::endl;
    cv::imshow("Image Egaliser", imageShow);

    // We creat the regions
    std::cout << "We creat the regions" << std::endl;
    ComputeRegions regions(image2, 0.8, 16);

    // We put the seeds in the image
    auto start_seeds = std::chrono::high_resolution_clock::now();
    regions.putSeeds();
    int nbSeedsStart = regions.getNbRegions();
    auto end_seeds = std::chrono::high_resolution_clock::now();

    // We calculate the regions
    auto start_calcul = std::chrono::high_resolution_clock::now();
    regions.calculateToTheEnd(150);
    auto end_calcul = std::chrono::high_resolution_clock::now();

    regions.display2("1fst step", 1);
    // regions.calculateAllRegions(500);

    // We merge the regions
    auto start_merge = std::chrono::high_resolution_clock::now();
    regions.merge();
    int nbSeedsEnd = regions.getNbRegions();
    auto end_merge = std::chrono::high_resolution_clock::now();
    regions.display2("merge");

    regions.displayBorderInner("border_inner_merge", 2);
    cv::waitKey(0);

    std::cout << "We start the smoothing" << std::endl;
    regions.smoothingReg();
    regions.display2("smoothing de base");

    // regions.merge();
    // regions.display2("merge_2");

    regions.displayBorderInner("border_inner_smooth", 2);
    cv::waitKey(0);

    // regions.merge();
    // regions.display2("merge_3");

    // regions.reCalculateRegions(20.0);
    // regions.display2("region_after_recalcul");

    // regions.merge();
    // regions.display2("merge_recalcul");
    // // cv::waitKey(0);

    // regions.smoothingReg();
    // regions.display2("smoothing");

    // regions.reCalculateRegions(80.0);
    // regions.display2("region_after_recalcul_smoothing");

    // regions.merge();
    // regions.display2("merge_smoothing_recalcul");

    // regions.smoothingReg();
    // regions.display2("smoothing_after_merge");

   

    std::cout << "END" << std::endl; 

    // We display all times
    auto end_total = std::chrono::high_resolution_clock::now();
    std::cout << "For " << nbSeedsStart << " regions put in start" << std::endl;
    std::cout << "For " << nbSeedsEnd << " regions after merge" << std::endl;
    std::cout << "Total time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_total - start_total).count() << " ms" << std::endl;
    std::cout << "Load time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_load - start_load).count() << " ms" << std::endl;
    std::cout << "Equalize time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_equalize - start_equalize).count() << " ms" << std::endl;
    std::cout << "Seeds time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_seeds - start_seeds).count() << " ms" << std::endl;
    std::cout << "Calcul time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_calcul - start_calcul).count() << " ms" << std::endl;
    std::cout << "Merge time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count() << " ms" << std::endl;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(end_calcul - start_calcul).count() - 1500 < std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count()) {
        std::cout << "Merge time is too long. You can put fewer pixels to make them bigger and shrink more time merge" << std::endl;
    } else if (std::chrono::duration_cast<std::chrono::milliseconds>(end_calcul - start_calcul).count() > std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count()) {
        std::cout << "Calcul time is too long. You can put more pixels to make them smaller and shrink less time merge" << std::endl;
    }
    std::cout << "Pourcent point not in region : " << regions.getPourcentNotInReg() << std::endl;

    // cv::waitKey(0);
    return 0;
}
