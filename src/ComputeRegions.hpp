#ifndef COMPUTREGIONS_HPP
#define COMPUTREGIONS_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Region.hpp"
#include "ComputeSeed.hpp"
#include <set>
#include <unordered_set>

/**
 * Class calculate region in an image
*/
class ComputeRegions {
public:
    /**
     * Default constructor
    */
    ComputeRegions() {
        image = new cv::Mat();
    }

    /**
     * Constructor
    */
    ComputeRegions(cv::Mat img, unsigned int _pourcent, unsigned int _rep = 16) {
        // We check the repartition is correct
        if (_rep % 4 != 0) {
            std::cout << "The repartition is not correct :" << _rep << std::endl;
            exit(1);
        }
        this->pourcent = _pourcent;
        this->rep = _rep;
        size_x_tabInfo = img.cols;
        size_y_tabInfo = img.rows;
        this->nb_pixels = size_x_tabInfo * size_y_tabInfo;
        this->nb_pixels_by_region = nb_pixels / _rep;
        nb_regions = nb_pixels_by_region * _pourcent / 100 * _rep;
        // regions = std::vector<Region *>(nb_regions);
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
    ~ComputeRegions() {
        delete image;

        std::sort(regions.begin(), regions.end());
        regions.erase(std::unique(regions.begin(), regions.end()), regions.end());

        for (unsigned int i = 0; i < regions.size() - 1; i++) {
            if (regions[i] != nullptr) {
                delete regions[i];
            }
            regions[i] = nullptr;
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
        ComputeSeed seedRandImg (image->rows, image->cols, nb_regions / this->rep, this->rep);
        // We put the seeds in the image
        int i = 0;
        for (const auto &seed : seedRandImg.getSeedVector()) {
            i++;
            image_seeds->at<cv::Vec3b>(seed->getPoint()) = cv::Vec3b(0, 0, 255);
            // We creat a new region
            std::cout << "Region " << i << "/" << this->nb_regions<<std::endl;
            Region * r = new Region(i, seed->getPoint(), tabInfo, image);
            // We add the region in the list of regions
            regions.push_back(r);
            // display regions
            std::cout << "regions size" << regions.size() << std::endl;
        }
        // We display the image with the seeds
        cv::resize(*image_seeds, *image_seeds, cv::Size(), 3, 3, cv::INTER_NEAREST);
        cv::imshow("Image with seeds div", *image_seeds);
        cv::waitKey(0);
    }

    /**
     * Calculate the regions
    */
    void calculateRegions() {
        // std::cout << "size Calculate" << image->rows << " / " << image->cols <<std::endl;
        std::cout << "Calculate regions" << std::endl;
        // For each region we calculate grow of this outline
        for (int i = 0; i < nb_regions; i++) {
            std::cout << "Region " << i << std::endl;
            regions[i]->getoutline();
            std::cout << "done" << std::endl;
            if (regions[i]->getoutline().size() > 0) {
                std::cout << "Region " << i << " is not empty" << std::endl;
                regions[i]->grow();
            } else {
                std::cout << "Region " << regions[i]->getId() << " is empty" << std::endl;
                if (regions[i]->getIsIncrease()) {
                    // std::cout << "Region " << regions[i]->getId() << " is increase" << std::endl;
                    regions[i]->increaseThreshold();
                    regions[i]->setoutline(regions[i]->getborder());
                    regions[i]->clearborder();
                    regions[i]->grow();
                }
            }
        }
        std ::cout << "End of calculate regions" << std::endl;
    }

    std::vector<Region *> calculateRegions(std::vector<Region *> allRegions) {
        // std::cout << "size Calculate" << image->rows << " / " << image->cols <<std::endl;
        // std::cout << "Calculate regions" << std::endl;
        // For each region we calculate grow of this outline
        std::vector<Region *> newRegions = std::vector<Region *>();
        for (unsigned int i = 0; i < allRegions.size(); i++) {
            if (allRegions[i]->getoutline().size() > 0) {
                allRegions[i]->grow();
                newRegions.push_back(allRegions[i]);
            } else {
                // std::cout << "Region " << i << " is empty" << std::endl;
                if (allRegions[i]->getIsIncrease()) {
                    // std::cout << "Region " << i << " is increase" << std::endl;
                    allRegions[i]->increaseThreshold();
                    allRegions[i]->setoutline(allRegions[i]->getborder());
                    allRegions[i]->clearborder();
                    allRegions[i]->grow();
                    newRegions.push_back(allRegions[i]);
                }
            }
        }
        return newRegions;
    }

    void calculateAllRegions(int nbr) {
        std::vector<Region *> allRegions(regions);
        for (int i = 0; i < nbr; i++) {
            if(allRegions.size() == 0) {
                std::cout << "All regions are empty at " << i << std::endl;
                break;
            }
            allRegions = calculateRegions(allRegions);
        }
    }


    /**
     * Display list of point
    */
    void displayListPoint(std::vector<cv::Point> & listPoint) {
        for (const auto &element : listPoint) {
            std::cout << " Point " << element.x << " / " << element.y << " | " << tabInfo[element.x][element.y] << " /// ";
        }
        std::cout << std::endl;
    }


    /**
     * Display  std::unordered_map<int, std::unordered_set<int>>
    */
    void displayMap(std::unordered_map<int, std::unordered_set<int>> & map) {
        for (const auto &element : map) {
            std::cout << "Region " << element.first << " : ";
            for (const auto &element2 : element.second) {
                std::cout << element2 << " / ";
            }
            std::cout << std::endl;
        }
    }

    

    /**
     * Merge with vector of point border of region
    */
    Region * mergeBorder(Region * r, std::unordered_set<int> & alereadyMerge, std::unordered_set<int> & mergeInidice) {
        // std::cout << "Merge border" << std::endl;
        std::vector<cv::Point> listeBorder = r->getborder();
        // displayListPoint(listeBorder);
        while (!listeBorder.empty()) {
            cv::Point p = listeBorder.back();
            listeBorder.pop_back();
            // int id = getIdRegion(p);
            int indice = tabInfo[p.x][p.y];
            // std :: cout << "Point " << p.x << " / " << p.y << " indice " << indice << std::endl;
            if (indice > 0 && alereadyMerge.find(indice) == alereadyMerge.end()) {
                // std::cout << "We put this region " << indice << std::endl;
                Region * r2 = regions[indice - 1];
                //alereadyMerge.insert(r2->getId());
                alereadyMerge.insert(indice);
                if (r->verifyFusion(*r2)) {
                    r2 = mergeBorder(r2, alereadyMerge, mergeInidice);
                    *r += *r2;
                    mergeInidice.insert(indice -1);
                } else {
                    std::unordered_set<int> mergeInidice2;
                    mergeBorder(r2, alereadyMerge, mergeInidice2);
                    while (!mergeInidice2.empty()) {
                        int idMerged = *mergeInidice2.begin();
                        // std::cout<<"Element to remove" << idMerged << std::endl;
                        mergeInidice2.erase(idMerged);
                        delete regions[idMerged];
                        regions[idMerged] = r2;
                    }
                }
            }
        }
        // std::cout << "End of merge border for region : " << r->getId() << std::endl;
        return r;
    }

    /**
     * Merge regions
    */
    void merge () {
        std::cout << "Merge" << std::endl;
        std::unordered_set<int> alereadyMerge;
        std::unordered_set<int> notMerge;
        std::unordered_set<int> mergeInidice;
        for (int i = 0; i < nb_regions; i++) {
            notMerge.insert(regions[i]->getId());
        }
        while (!notMerge.empty()) {
            // We get the first element
            int id = *notMerge.begin();
            // std::cout << "Element to merge " << id << std::endl;
            alereadyMerge.insert(id);
            notMerge.erase(id);
            Region * r = mergeBorder(regions[id - 1], alereadyMerge, mergeInidice);
            // std::cout << "Region Finish merge" << r->getId() << " color averge" << r->getColor() <<std::endl;
            while (!mergeInidice.empty()) {
                int idMerged = *mergeInidice.begin();
                // std::cout<<"Element to remove" << idMerged << std::endl;
                notMerge.erase(idMerged);
                mergeInidice.erase(idMerged);
                delete regions[idMerged];
                regions[idMerged] = r;
            }
        }
    }
    
    /**
     * Display the regions
    */
    void display(const std::string title = "Image with regions") {
        std::cout << "size display" << image->rows << " / " << image->cols <<std::endl;
        // We call the display function for each region
        // we creat a new image to see the regions
        cv::Mat * image_regions = new cv::Mat(image->clone());
        for (int i = 0; i < size_x_tabInfo; i++) {
            for (int j = 0; j < size_y_tabInfo; j++) {
                std::cout << "coo " << i << " / " <<  j << std :: endl;
                std::cout << "tabInfo[" << i << "][" << j << "] = " << tabInfo[i][j] << std::endl;
                int id = tabInfo[i][j];
                if (id > 0) {
                    // image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regions[id - 1]->getColor();
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regions[id - 1]->getColor();

                } else if (id < 0) {
                    // image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 255);
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 255);
                } else {
                    // image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 0);
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = /*image->at<cv::Vec3b>(cv::Point(i, j)); */ cv::Vec3b(0, 0, 0);
                }
                std::cout << "color put " << image_regions->at<cv::Vec3b>(cv::Point(i, j)) << std::endl;
            }
        }
        // We show in cout pixel
        for (int i = 0; i < image_regions->cols; i++) {
            for (int j = 0; j < image_regions->rows; j++) {
                std::cout << "color " << i << " / " << j << " : " << image_regions->at<cv::Vec3b>(cv::Point(i, j)) << std::endl;
            }
        }

        std::cout << ".??????????????????" << std::endl;
        cv::imshow("mais", *image);
        cv::waitKey(0);
       
        // We display the image with the regions
        // Increase the size of the image
        std::cout << "end display" << std::endl;
        std::cout << "size display " << image_regions->rows << " / " << image_regions->cols <<std::endl;
        // cv::resize(*image_regions, *image_regions, cv::Size(), 2, 2, cv::INTER_NEAREST);
        // cv::imshow(title, *image_regions);
        // cv::imshow(title, *image);
        if (!image_regions->empty()) {
            cv::imshow(title, *image_regions);
            cv::waitKey(0);
        } else {
            std::cerr << "Error: Empty or invalid image." << std::endl;
        }
        cv::waitKey(0);
        std::cout << "end display" << std::endl;
        // delete image_regions;
    }

    void display2(const std::string & name = "Image with regions") {
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
                int id = getIdRegion(cv::Point(i, j));
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
        cv::imshow(name, *image_regions);
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
    // void display3() {
    //     // Créer une copie de l'image originale
    //     cv::Mat * image_regions = new cv::Mat(image->clone());

    //     for (int i = 0; i < size_x_tabInfo; i++) {
    //         for (int j = 0; j < size_y_tabInfo; j++) {
    //             int id = tabInfo[i][j];

    //             if (id > 0) {
    //                 // Utiliser la fonction membre pour obtenir la couleur de la région
    //                 cv::Vec3b regionColor = regions[id - 1]->getColorById(id);
                    
    //                 // Colorier le pixel avec la couleur de la région
    //                 image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regionColor;
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

    /**
     * Return region in a point
    */
    Region * getRegion(cv::Point p) {
        int id = tabInfo[p.x][p.y];
        if (id > 0) {
            return regions[id - 1];
        } else {
            return nullptr;
        }
    }

    /**
     * Return the id of regions
    */
    int getIdRegion(cv::Point p) {
        if (tabInfo[p.x][p.y] == 0) {
            return 0;
        } else if (tabInfo[p.x][p.y] < 0) {
            return regions[(tabInfo[p.x][p.y] + 1) * -1]->getId() * -1;
        } else {
            // std::cout << "id " << tabInfo[p.x][p.y] << std::endl;
            // std::cout << "id " << regions[tabInfo[p.x][p.y] - 1]->getId() << std::endl;
            return regions[tabInfo[p.x][p.y] - 1]->getId();
        }
    }

private:
    cv::Mat * image;
    std::vector<Region *> regions;
    int nb_regions;
    unsigned int pourcent;
    unsigned int rep;
    int ** tabInfo;
    int size_x_tabInfo;
    int size_y_tabInfo;
    int nb_pixels_by_region;
    int nb_pixels;

};

#endif // COMPUTREGIONS_HPP
