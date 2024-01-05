#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"
#include "ComputeRegions.hpp"
#include "Region.hpp"
#include "Seed.hpp"

/**
 * For arguments
*/
void getArgs(int argc, char** argv, 
            std::unordered_map<std::string, bool> & functToCall, 
            std::unordered_map<std::string, float> & params,
            std::string & pathImage) {

    functToCall.clear();
    functToCall = {
        {"smooth", true},
        {"merge", true},
        {"recalcul", false},
        {"analyse", true},
        {"equalize", true},
        {"toTheEnd", true},
        {"deleteImg", false}
    };
    params.clear();
    params = {
        {"pourcentSeed", 0.8},
        {"pourcentReCal", 0.8},
        {"nbRepart", 16},
        {"nbIteration", 100}
    };

    pathImage = "Images/lena_color.png";
    // Get the arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') { 
            size_t equalPos = arg.find('=');
            if (equalPos != std::string::npos) {
                std::string key = arg.substr(1, equalPos - 1);  // Exclure le tiret initial
                std::string value = arg.substr(equalPos + 1);
                // Vérifie que la clé est valide
                if (key == "smooth" || key == "merge" || key == "recalcul" || key == "analyse" || key == "equalize" || key == "toTheEnd") {
                    functToCall[key] = (value == "true");
                } else if (key == "pourcentSeed" || key == "pourcentReCal" || key == "nbRepart") {
                    params[key] = (float) std::stof(value);
                } else if (key == "pathImage") {
                    pathImage = value;
                } else {
                    std::cerr << "Error: Invalid key '" << key << "'" << std::endl;
                }
            } else {
                std::cerr << "Error: Malformed argument '" << arg << "'" << std::endl;
            }
        }
    }
}

int merge(ComputeRegions& regions, const int iteration = 0) {
    auto start_merge = std::chrono::high_resolution_clock::now();
    regions.merge();
    auto end_merge = std::chrono::high_resolution_clock::now();
    regions.display2("merge_n°"+ std::to_string(iteration));
    std::cout << "Merge n°" << iteration << " time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count() << " ms" << std::endl;
    return iteration + 1;
}

int smooth(ComputeRegions& regions, const int iteration = 0) {
    auto start_smooth = std::chrono::high_resolution_clock::now();
    regions.smoothingReg();
    auto end_smooth = std::chrono::high_resolution_clock::now();
    regions.display2("smoothing_n°"+ std::to_string(iteration));
    std::cout << "Smooth n°" << iteration << " time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_smooth - start_smooth).count() << " ms" << std::endl;
    return iteration + 1;
}

int reCalcul(ComputeRegions& regions, const float pourcentSeed, const int iteration = 0) {
    auto start_recalcul = std::chrono::high_resolution_clock::now();
    regions.reCalculateRegions(pourcentSeed);
    auto end_recalcul = std::chrono::high_resolution_clock::now();
    regions.display2("region_after_recalcul_n°"+ std::to_string(iteration));
    std::cout << "ReCalcul n°" << iteration << " time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_recalcul - start_recalcul).count() << " ms" << std::endl;
    return iteration + 1;
}

void analyse(ComputeRegions& regions,
    int nbSeedsStart,
    std::unordered_map<std::string, float> & params) {
    int itMerge = 1;
    int itReCalcul = 1;
    int itSmooth = 1;
    int iteration = 1;
    while (nbSeedsStart - regions.getNbRegions() > 20) {
        std::cout << "Iteration n°" << iteration << std::endl;
        iteration++;
        itReCalcul = reCalcul(regions, params["pourcentReCal"], itReCalcul);
        itMerge = merge(regions, itMerge);
        itSmooth = smooth(regions, itSmooth);  
    } 
    regions.displayBorderInner("border_inner_n°"+ std::to_string(iteration), 1);
    std::cout << "Pourcent point not in region : " << regions.getPourcentNotInReg() << std::endl;
}

int main(int argc, char** argv) {
    // Get the arguments
    std::unordered_map<std::string, bool> functToCall;
    std::unordered_map<std::string, float> params;
    std::string pathImage;
    getArgs(argc, argv, functToCall, params, pathImage);
    // display the arguments
    std::cout << "Smooth Enabled: " << (params["smooth"] ? "true" : "false") << std::endl;
    std::cout << "Merge Enabled: " << (params["merge"] ? "true" : "false") << std::endl;
    std::cout << "Recalcul Enabled: " << (params["recalcul"] ? "true" : "false") << std::endl;
    std::cout << "Analyse Enabled: " << (params["analyse"] ? "true" : "false") << std::endl;
    std::cout << "Equalize Enabled: " << (params["equalize"] ? "true" : "false") << std::endl;
    std::cout << "Pourcent Seed: " << params["pourcentSeed"] << std::endl;
    std::cout << "Pourcent ReCal: " << params["pourcentReCal"] << std::endl;
    std::cout << "Nb Repart: " << params["nbRepart"] << std::endl;
    std::cout << "Nb Iteration: " << params["nbIteration"] << std::endl;

    // We load the image
    cv::Mat image = cv::imread(pathImage);
    if (functToCall["equalize"] == true) {
        std::cout << "We equalize the image" << std::endl;
        cv::Mat image2;
        etirerHistogrammeCouleur(image, image2, 0, 255);
        // delete &image;
        image = image2.clone();
        // delete &image2;
    }

    ComputeRegions regions(image, params["pourcentSeed"], params["nbRepart"]);
    auto start_seeds = std::chrono::high_resolution_clock::now();
    regions.putSeeds();
    auto end_seeds = std::chrono::high_resolution_clock::now();
    int nbSeedsStart = regions.getNbRegions();
    std::cout << "For " << nbSeedsStart << " regions put in start" << std::endl;
    std::cout << "Seeds time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_seeds - start_seeds).count() << " ms" << std::endl;
    if (functToCall["toTheEnd"]) {
        regions.calculateToTheEnd(params["nbIteration"]);
    } else {
        regions.calculateAllRegions(params["nbIteration"]);
    }
    
    // Peut être obligatoire tout le temps idk
    if (functToCall["merge"] == true) {
        merge(regions);
    }

    if (functToCall["recalcul"] == true) {
        reCalcul(regions, params["pourcentReCal"]);
    }

    if (functToCall["smooth"] == true) {
        smooth(regions);
    }

    if (functToCall["analyse"] == true) {
        analyse(regions, nbSeedsStart, params);
    }
    return 0;
}
