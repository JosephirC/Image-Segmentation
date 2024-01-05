#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"
#include "ComputeRegions.hpp"
#include "Region.hpp"
#include "Seed.hpp"

/**
 * For arguments
*/
std::unordered_map<std::string, bool> getArgs(int argc, char** argv) {
    // Define default values
    bool smoothEnabled = true;
    bool mergeEnabled = false;
    bool recalculEnabled = false;
    bool analyseEnabled = false;
    std::unordered_map<std::string, bool> args = {
        {"smooth", smoothEnabled},
        {"merge", mergeEnabled},
        {"recalcul", recalculEnabled},
        {"analyse", analyseEnabled}
    };

    // Get the arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') { 
            size_t equalPos = arg.find('=');
            if (equalPos != std::string::npos) {
                std::string key = arg.substr(1, equalPos - 1);  // Exclure le tiret initial
                std::string value = arg.substr(equalPos + 1);
                // Vérifie que la clé est valide
                if (key == "smooth" || key == "merge" || key == "recalcul" || key == "analyse") {
                    args[key] = (value == "true");
                } else {
                    std::cerr << "Error: Invalid key '" << key << "'" << std::endl;
                }
            } else {
                std::cerr << "Error: Malformed argument '" << arg << "'" << std::endl;
            }
        }
    }
    return args;
}

int main(int argc, char** argv) {
    // Get the arguments
    std::unordered_map<std::string, bool> params = getArgs(argc, argv);
    // display the arguments
    std::cout << "Smooth Enabled: " << (params["smooth"] ? "true" : "false") << std::endl;
    std::cout << "Merge Enabled: " << (params["merge"] ? "true" : "false") << std::endl;
    std::cout << "Recalcul Enabled: " << (params["recalcul"] ? "true" : "false") << std::endl;
    std::cout << "Analyse Enabled: " << (params["analyse"] ? "true" : "false") << std::endl;


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

    auto start_smooth = std::chrono::high_resolution_clock::now();
    regions.smoothingReg();
    auto end_smooth = std::chrono::high_resolution_clock::now();
    regions.display2("smoothing de base");

    // regions.merge();
    // regions.display2("merge_2");

    regions.displayBorderInner("border_inner_smooth", 2);

    // regions.merge();
    // regions.display2("merge_3");

    regions.reCalculateRegions(20.0);
    regions.display2("region_after_recalcul");

    regions.merge();
    regions.display2("merge_recalcul");

    regions.smoothingReg();
    regions.display2("smoothing_after_merge_after_recalcul");

   

    std::cout << "END" << std::endl; 

    // We display all times
    auto end_total = std::chrono::high_resolution_clock::now();
    std::cout << "For " << nbSeedsStart << " regions put in start" << std::endl;
    std::cout << "For " << nbSeedsEnd << " regions after merge" << std::endl;
    std::cout << "Total time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_total - start_total).count() << " ms" << std::endl;
    std::cout << "Load time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_load - start_load).count() << " ms" << std::endl;
    std::cout << "Equalize time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_equalize - start_equalize).count() << " ms" << std::endl;
    std::cout << "Seeds time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_seeds - start_seeds).count() << " ms" << std::endl;
    std::cout << "Growing time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_calcul - start_calcul).count() << " ms" << std::endl;
    std::cout << "Merge time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count() << " ms" << std::endl;
    std::cout << "Smooth time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_smooth - start_smooth).count() << " ms" << std::endl;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(end_calcul - start_calcul).count() - 1500 < std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count()) {
        std::cout << "Merge time is too long. You can put fewer pixels to make them bigger and shrink more time merge" << std::endl;
    } else if (std::chrono::duration_cast<std::chrono::milliseconds>(end_calcul - start_calcul).count() > std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count()) {
        std::cout << "Calcul time is too long. You can put more pixels to make them smaller and shrink less time merge" << std::endl;
    }
    std::cout << "Pourcent point not in region : " << regions.getPourcentNotInReg() << std::endl;

    // cv::waitKey(0);
    return 0;
}
