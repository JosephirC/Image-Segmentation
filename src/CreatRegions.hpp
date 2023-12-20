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
            if (regions[i]->getoutline().size() > 0) {
                regions[i]->grow();
            } else {
                // std::cout << "Region " << regions[i]->getId() << " is empty" << std::endl;
                if (regions[i]->getIsIncrease()) {
                    // std::cout << "Region " << regions[i]->getId() << " is increase" << std::endl;
                    regions[i]->increaseThreshold();
                    regions[i]->setoutline(regions[i]->getborder());
                    regions[i]->clearborder();
                    regions[i]->grow();
                }
            }
        }
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
     * vector Region refactor (for unicity of regions and put good id in tabInfos)
    */
    void refactRegions () {
        for (int x = 0; x < size_x_tabInfo; x++) {
            for (int y = 0; y < size_y_tabInfo; y++) {
                int id = tabInfo[x][y];
                if (id > 0) {
                    tabInfo[x][y] = regions[id - 1]->getId();
                }
            }
        }
        std::set<int> map;
        std::vector<Region *> newRegions = std::vector<Region *>();
        nb_regions = 0;
        for (auto& region : regions) {
            if (map.find(region->getId()) == map.end()) {
                nb_regions++;
                map.insert(region->getId());
                region->setId(nb_regions);
                newRegions.push_back(region);
            }
        }
        regions = newRegions;
    }

    /**
     * Merge with vector of point border of region
    */
    Region * mergeBorder(Region * r, const int indexRegion, std::unordered_set<int> & alereadyMerge, std::unordered_set<int> & mergeInidice) {
        std::cout << "Merge border" << std::endl;
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
                if (r->verifyFusion(*r2) || true) {
                    std::cout << "R2 BEFORE " << r2->getId() << std::endl;
                    r2 = mergeBorder(r2, indice - 1, alereadyMerge, mergeInidice);
                    std::cout << "Fusion " << r2->getId() << " / " << r->getId() << std::endl;
                    std::cout << "before " << regions[indexRegion]->getId() << std::endl;
                    *r += *r2;
                    mergeInidice.insert(indice -1);
                } else {
                    mergeBorder(r2, indice - 1, alereadyMerge, mergeInidice);
                }
            }
        }
        std::cout << "End of merge border for region : " << r->getId() << std::endl;
        return r;
    }

    /**
     * Merge regions
    */
    void merge () {
        std::unordered_set<int> alereadyMerge;
        std::unordered_set<int> notMerge;
        std::unordered_set<int> mergeInidice;
        for (int i = 0; i < nb_regions; i++) {
            notMerge.insert(regions[i]->getId());
        }
        while (!notMerge.empty()) {
            // We get the first element
            int id = *notMerge.begin();
            std::cout << "Element to merge " << id << std::endl;
            alereadyMerge.insert(id);
            notMerge.erase(id);
            Region * r = mergeBorder(regions[id - 1], id - 1, alereadyMerge, mergeInidice);
            std::cout << "Region Finish merge" << r->getId() << " color averge" << r->getColor() <<std::endl;
            while (!mergeInidice.empty()) {
                int idMerged = *mergeInidice.begin();
                std::cout<<"Element to remove" << idMerged << std::endl;
                notMerge.erase(idMerged);
                mergeInidice.erase(idMerged);
                delete regions[idMerged];
                regions[idMerged] = r;
            }
        }
        for (auto& region : regions) {
            std::cout << "Region " << region->getId() << " color averge" << region->getColor() <<std::endl;
        }
        std::cout << "End of merge" << std::endl;
        refactRegions();
        for (auto& region : regions) {
            std::cout << "Region " << region->getId() << " color averge" << region->getColor() <<std::endl;
        }

        std::cout << " Vraiment End of merge" << std::endl;
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
    std::vector<Seed> seeds;
    std::vector<Region *> regions;
    int nb_seeds;
    int nb_regions;
    int ** tabInfo;
    int size_x_tabInfo;
    int size_y_tabInfo;

};

#endif // CREATREGIONS_HPP
