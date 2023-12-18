#ifndef CREATREGIONS_HPP
#define CREATREGIONS_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Region.hpp"
#include "Seed.hpp"
#include <set>
#include <unordered_set>

/**
 * Class calculate region in an image
*/
class CreatRegions {
public:
    /**
     * Default constructor
    */
    CreatRegions() {
        image = new cv::Mat();
    }

    /**
     * Constructor
    */
    CreatRegions(cv::Mat img, int nb_seeds) {
        // We creat a tab to keep regions.
        
        size_x_tabInfo = img.cols;
        size_y_tabInfo = img.rows;
        nb_regions = nb_seeds;
        this->nb_seeds = nb_seeds;
        seeds = std::vector<Seed>(nb_seeds);
        regions = std::vector<Region *>(nb_seeds);
        tabInfo = new int*[size_x_tabInfo];
        for (int i = 0; i < size_x_tabInfo; i++) {
            tabInfo[i] = new int[size_y_tabInfo];
            for (int j = 0; j < size_y_tabInfo; j++) {
                tabInfo[i][j] = 0;
            }
        }
        image = new cv::Mat(img.size(), img.type());
        *image = img.clone();
    }

    /**
     * Destructor
     */
    ~CreatRegions() {
        delete image;

        for (int i = 0; i < nb_seeds; ++i) {
            delete regions[i];
        }

        for (int i = 0; i < size_x_tabInfo; ++i) {
            delete[] tabInfo[i];
        }
        delete[] tabInfo;
    }

    /**
     * Put the seeds in the image
    */
    void putSeeds() {        
        // std::cout << "size PutSeeds" << image->rows << " / " << image->cols <<std::endl;
        // We creat a new image to see the seeds
        cv::Mat * image_seeds = new cv::Mat(image->clone());
        // std::cout << "size" << image->rows << image->cols <<std::endl;
        for (int i = 0; i < nb_seeds; i++) {
            // We creat a seed
            Seed s(*image);
            seeds.push_back(s);
            regions [i] = (new Region((i + 1), s.getPoint(), tabInfo, image));
            // We put the seed in the image
            // std::cout << "Seed " << i << " : " << s.getPoint().x << "/" << s.getPoint().y << std::endl;
            cv::circle(*image_seeds, s.getPoint(), 1, cv::Scalar(0, 0, 255), -1); // To see the seeds
            // std::cout << "Seed " << i << " : " << s.getPoint().x << "/" << s.getPoint().y << std::endl;
        }
        // We display the image with the seeds
        // std::cout << "We display the image with the seeds" << std::endl;
        cv::imshow("Image with seeds 1", *image_seeds);
        // std::cout << "End of putSeeds" << std::endl;
        // We wait for a key to be pressed
        cv::waitKey(0);
        // delete image_seeds;
    }

    /**
     * Calculate the regions
    */
    void calculateRegions() {
        // std::cout << "size Calculate" << image->rows << " / " << image->cols <<std::endl;
        // std::cout << "Calculate regions" << std::endl;
        // For each region we calculate grow of this outline
        for (int i = 0; i < nb_regions; i++) {
            if (regions[i]->getoutline()->size() > 0) {
                regions[i]->grow();
            } else {
                std::cout << "Region " << i << " is empty" << std::endl;
                if (regions[i]->getIsIncrease()) {
                    std::cout << "Region " << i << " is increase" << std::endl;
                    regions[i]->increaseThreshold();
                    regions[i]->setoutline(regions[i]->getborder());
                    regions[i]->clearborder();
                    regions[i]->grow();
                }
            }
        }
    }


    /**
     * Fusion of region
    */
    void calculateRegionToFuse() {
        std::unordered_map<int, std::unordered_set<int>> regionCalculate = std::unordered_map<int, std::unordered_set<int>>();
        for (int i = 0; i < nb_regions; i++) {
            std::vector<cv::Point> * listeBorder = regions[i]->getborder();
            for (const auto &points : *listeBorder) {
                Region * regionInBorder = getRegion(points);
                if (regionInBorder != nullptr) {
                    if (regions[i]->verifyFusion(*regionInBorder)) {
                        // save the region to fuse if this region is not already in the map
                        if (regionCalculate.find(regionInBorder->getId()) == regionCalculate.end()) {
                            if (regionCalculate.find(i) == regionCalculate.end()) {
                                // We check if the region is not already to fuse for this region

                                // Put id region to fuse
                                regionCalculate[i] = std::unordered_set<int>();
                                regionCalculate[i].insert(regionInBorder->getId());
                            } else {
                                // Put id region to fuse
                                regionCalculate[i].insert(regionInBorder->getId());
                            }
                        }
                    }
                }
            }            
        }
    }

    /**
     * Fusion of region
    */
    void fuseRegion (std::unordered_map<int, std::vector<int>> regionCalculate) {
        for (const auto &region : regionCalculate) {
            // std::cout << "Region " << region.first << " to fuse with ";
            for (const auto &regionToFuse : region.second) {
                // std::cout << regionToFuse << " ";
                regions[region.first] += (regions[regionToFuse - 1]);
            }
            // std::cout << std::endl;
        }
    }

    /**
     * Display the regions
    */
    void display() {
        // We call the display function for each region
        // we creat a new image to see the regions
        cv::Mat * image_regions = new cv::Mat(image->clone());
        for (int i = 0; i < size_x_tabInfo; i++) {
            for (int j = 0; j < size_y_tabInfo; j++) {
                // std::cout << "coo " << i << " / " <<  j << std :: endl;
                // std::cout << "tabInfo[" << i << "][" << j << "] = " << tabInfo[i][j] << std::endl;
                int id = tabInfo[i][j];
                if (id > 0) {
                    // image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regions[id - 1]->getColor();
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regions[id - 1]->getColor();

                } else if (id < 0) {
                    // image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 255);
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 255);
                } else {
                    // image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 0);
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = /*image->at<cv::Vec3b>(cv::Point(i, j));*/ cv::Vec3b(0, 0, 0);
                }
            }
        }
        // std::cout << "end display" << std::endl;
        // We display the image with the regions
        // Increase the size of the image
        cv::resize(*image_regions, *image_regions, cv::Size(), 2, 2, cv::INTER_NEAREST);
        cv::imshow("Image with regions", *image_regions);
        cv::waitKey(0);
    }

    void display2() {
        // Créer une palette de couleurs
        std::vector<cv::Vec3b> colorPalette;
        for (int i = 0; i < nb_regions; ++i) {
            // Générer des couleurs uniques pour chaque région
            uchar r = static_cast<uchar>(std::rand() % 256);
            uchar g = static_cast<uchar>(std::rand() % 256);
            uchar b = static_cast<uchar>(std::rand() % 256);
            colorPalette.push_back(cv::Vec3b(b, g, r));  // Utilisez BGR au lieu de RGB
        }

        // Créer une nouvelle image pour afficher les régions colorées
        cv::Mat *image_regions = new cv::Mat(image->clone());

        for (int i = 0; i < size_x_tabInfo; i++) {
            for (int j = 0; j < size_y_tabInfo; j++) {
                int id = tabInfo[i][j];
                if (id > 0) {
                    // Utiliser la couleur correspondant à l'identifiant de la région
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = colorPalette[id - 1];
                } else if (id < 0) {
                    // Régions non attribuées, les laisser en noir ou utilisez une autre couleur
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 0);
                }
            }
        }

        // Ajuster la taille de l'image pour l'affichage
        cv::resize(*image_regions, *image_regions, cv::Size(), 2, 2, cv::INTER_NEAREST);

        // Afficher l'image avec les régions colorées
        cv::imshow("Image with colored regions", *image_regions);
        cv::waitKey(0);
    }

    // void displayWithRegionId() {
    //     // Créer une copie de l'image originale
    //     cv::Mat * image_regions = new cv::Mat(image->clone());

    //     for (int i = 0; i < size_x_tabInfo; i++) {
    //         for (int j = 0; j < size_y_tabInfo; j++) {
    //             int id = tabInfo[i][j];

    //             if (id > 0) {
    //                 auto it = allRegionColors->find(id);

    //                 if (it != allRegionColors->end()) {
    //                     // Colorier le pixel avec la couleur de la région
    //                     image_regions->at<cv::Vec3b>(cv::Point(i, j)) = it->second;
    //                 } else {
    //                     // Gérer le cas où l'ID de la région n'est pas dans allRegionColors
    //                     std::cerr << "Erreur : ID de région introuvable dans allRegionColors" << std::endl;
    //                 }
    //             } else if (id < 0) {
    //                 // Régions non attribuées, les laisser en noir ou utilisez une autre couleur
    //                 image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 0);
    //             }
    //         }
    //     }

    //     // Ajuster la taille de l'image pour l'affichage
    //     cv::resize(*image_regions, *image_regions, cv::Size(), 2, 2, cv::INTER_NEAREST);

    //     // Afficher l'image avec les régions colorées
    //     cv::imshow("Image with colored regions", *image_regions);
    //     cv::waitKey(0);
    // }

    //displayWithRegionId aussi
    void display3() {
        // Créer une copie de l'image originale
        cv::Mat * image_regions = new cv::Mat(image->clone());

        for (int i = 0; i < size_x_tabInfo; i++) {
            for (int j = 0; j < size_y_tabInfo; j++) {
                int id = tabInfo[i][j];

                if (id > 0) {
                    // Utiliser la fonction membre pour obtenir la couleur de la région
                    cv::Vec3b regionColor = regions[id - 1]->getColorById(id);
                    
                    // Colorier le pixel avec la couleur de la région
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regionColor;
                } else if (id < 0) {
                    // Régions non attribuées, les laisser en noir ou utilisez une autre couleur
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 0);
                }
            }
        }

        // Ajuster la taille de l'image pour l'affichage
        cv::resize(*image_regions, *image_regions, cv::Size(), 2, 2, cv::INTER_NEAREST);

        // Afficher l'image avec les régions colorées
        cv::imshow("Image with colored regions", *image_regions);
        cv::waitKey(0);
    }

    /**
     * Display the outline regions
    */
    void displayoutlines() {
        // We call the display function for each region
        for (int i = 0; i < nb_regions; i++) {
            // std::cout << "Region " << i << std::endl;
            // regions->at(i).displayoutline("outline Region :" + std::to_string(i), *image);
        }
        cv::waitKey(0);
    }


    Region * getRegion(cv::Point p) {
        int id = tabInfo[p.x][p.y];
        if (id > 0) {
            return regions[id - 1];
        } else {
            return nullptr;
        }
    }

private:
    cv::Mat * image;
    std::vector<Seed> seeds;
    std::vector<Region *> regions;
    int nb_seeds;
    int nb_regions;
    int ** tabInfo;
    int size_x_tabInfo;
    int size_y_tabInfo;

};

#endif // CREATREGIONS_HPP
