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
        {"deleteImg", true}
    };
    params.clear();
    params = {
        {"pourcentSeed", 2.},
        {"pourcentReCal", 5.},
        {"nbRepart", 16},
        {"nbIteration", 100},
        {"nbIterationReCal", 3},
        {"blur", 3},
        {"seuilMax", 30.},
        {"seuilMin", 10.},
        {"coefSDMax", 1.5}
    };

    pathImage = "Images/lena_color.png";
    // pathImage = "Images/4couleurs.png";
    // pathImage = "Images/bigSegmentation.png";
    // pathImage = "Images/big4couleurs.png";
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
                } else if (key == "pourcentSeed" || key == "pourcentReCal" || key == "nbRepart" || key == "nbIteration" || key == "nbIterationReCal" || key == "blur" || key == "seuilMax" || key == "seuilMin" || key == "coefSDMax") {
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

// void deleteImgInDir(const std::string& directoryPath = "image_cree") {
//     for (const auto& entry : fs::directory_iterator(directoryPath)) {
//         if (entry.is_regular_file() && entry.path().extension() == ".png") {
//             fs::remove(entry.path());
//             std::cout << "Deleted: " << entry.path() << std::endl;
//         }
//     }
// }

int merge(ComputeRegions& regions, const int iteration = 0) {
    auto start_merge = std::chrono::high_resolution_clock::now();
    regions.merge();
    auto end_merge = std::chrono::high_resolution_clock::now();
    // regions.saveImage("merge_n°"+ std::to_string(iteration));
    std::cout << "Merge n°" << iteration << " time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_merge - start_merge).count() << " ms" << std::endl;
    return iteration + 1;
}

int smooth(ComputeRegions& regions, const int iteration = 0) {
    auto start_smooth = std::chrono::high_resolution_clock::now();
    regions.smoothingReg();
    auto end_smooth = std::chrono::high_resolution_clock::now();
    // regions.saveImage("smoothing_n°"+ std::to_string(iteration));
    std::cout << "Smooth n°" << iteration << " time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_smooth - start_smooth).count() << " ms" << std::endl;
    return iteration + 1;
}

int reCalcul(ComputeRegions& regions, const float pourcentSeed, const int iteration = 0) {
    auto start_recalcul = std::chrono::high_resolution_clock::now();
    regions.reCalculateRegions(pourcentSeed);
    auto end_recalcul = std::chrono::high_resolution_clock::now();
    // regions.saveImage("region_after_recalcul_n°"+ std::to_string(iteration));
    std::cout << "ReCalcul n°" << iteration << " time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_recalcul - start_recalcul).count() << " ms" << std::endl;
    return iteration + 1;
}

void analyse(ComputeRegions& regions,
    // int nbSeedsStart,
    std::unordered_map<std::string, float> & params) {
        
    int itMerge = 1;
    int itReCalcul = 1;
    int itSmooth = 1;
    int iteration = 1;
    regions.saveImage("it_n°"+ std::to_string(iteration));
    while (regions.getPourcentNotInReg() > 5. && iteration < params["nbIteration"]) {
        iteration++;
        std::cout << "Iteration n°" << iteration << std::endl;
        itReCalcul = reCalcul(regions, params["pourcentReCal"], itReCalcul);
        itMerge = merge(regions, itMerge);
        itSmooth = smooth(regions, itSmooth); 
        regions.saveImage("it_n°"+ std::to_string(iteration)); 
    } 
    regions.encompassmentRegion();
    // regions.saveImage("final encompassement");
    regions.saveImageWithBorderInner("border_inner_n°"+ std::to_string(iteration));
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
    std::cout << "Nb Iteration ReCal: " << params["nbIterationReCal"] << std::endl;
    std::cout << "Path Image: " << pathImage << std::endl;
    
    // if (functToCall["deleteImg"] == true) {
    //     deleteImgInDir();
    // }

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
    if (params["blur"] > 0) {
        std::cout << "We blur the image" << std::endl;
        cv::Mat image2;
        cv::blur(image, image2, cv::Size(params["blur"], params["blur"]));
        image = image2.clone();
    }

    ComputeRegions regions(image, params["pourcentSeed"], params["nbRepart"], params["seuilMax"], params["seuilMin"], params["coefSDMax"]);
    auto start_seeds = std::chrono::high_resolution_clock::now();
    regions.putSeeds();
    auto end_seeds = std::chrono::high_resolution_clock::now();
    int nbSeedsStart = regions.getNbRegions();
    std::cout << "For " << nbSeedsStart << " regions put in start" << std::endl;
    std::cout << "Seeds time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_seeds - start_seeds).count() << " ms" << std::endl;
    auto start_calcul = std::chrono::high_resolution_clock::now();
    if (functToCall["toTheEnd"]) {
        regions.calculateToTheEnd(params["nbIteration"]);
    } else {
        regions.calculateAllRegions(params["nbIteration"]);
    }
    auto end_calcul = std::chrono::high_resolution_clock::now();
    std::cout << "Calcul time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end_calcul - start_calcul).count() << " ms" << std::endl;
    
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
        analyse(regions, params);
    }
    
    regions.saveImage("final");
    
}
