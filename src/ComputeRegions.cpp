#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Region.hpp"
#include "ComputeRegions.hpp"
#include "ComputeSeed.hpp"
#include <set>
#include <unordered_set>

/**
 * Function not in class Region
*/
bool isUnique(const std::vector<cv::Point>& points) {
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            if (points[i] == points[j]) {
                return false;
            }
        }
    }
    
    return true;
}

ComputeRegions::ComputeRegions() {
    image = new cv::Mat();
}

ComputeRegions::ComputeRegions(cv::Mat img, float _pourcentByRep, unsigned int _rep) {
    // We check the repartition is correct
    if (_rep % 4 != 0) {
        std::cout << "The repartition is not correct :" << _rep << std::endl;
        exit(1);
    }
    this->pourcent = _pourcentByRep;
    this->rep = _rep;
    size_x_tabInfo = img.cols;
    size_y_tabInfo = img.rows;
    this->nb_pixels = size_x_tabInfo * size_y_tabInfo;
    float nb_pixels_by_region = nb_pixels / _rep;
    this->nb_regions = int (nb_pixels_by_region * pourcent / 100) * _rep;
    std::cout << "nb_regions :" << nb_regions << std::endl;
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


ComputeRegions::~ComputeRegions() {
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


void ComputeRegions::putSeeds() {        
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
    }
    // We save the image with the seeds
    cv::imwrite("image_cree/Image_with_seeds_div.png", *image_seeds);
}


void ComputeRegions::calculateRegions() {
    // For each region we calculate grow of this outline
    for (int i = 0; i < nb_regions; i++) {
        regions[i]->getoutline();
        if (regions[i]->getoutline().size() > 0) {
            // std::cout << "Region " <<  regions[i]->getId() << " is not empty" << std::endl;
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
}

std::queue<Region *> ComputeRegions::calculateRegions(std::queue<Region *> & regToCal) {
    std::queue<Region *> regCanGrow = std::queue<Region *>();
    // For each region we calculate grow with this outline
    // First step we get outline of each region
    while (!regToCal.empty()) {
        Region * reg = regToCal.front();
        regToCal.pop();
        if (reg->getoutline().size() > 0) {
            reg->grow();
            regCanGrow.push(reg);
        } else {
            // Reg can not grow, we try to be more kind
            if (reg->getIsIncrease()) {
                // If we can be kinder, we are kinder
                reg->increaseThreshold();
                // We change outline by old border
                reg->setoutline(reg->getborder());
                reg->clearborder();
                reg->grow();
                regCanGrow.push(reg);
            }
        }
    }
    return regCanGrow;
}

void ComputeRegions::calculatQueueRegion(std::queue<Region *> reg, int nbrCallMax) {
    // We calculate the regions
    while (!reg.empty() && nbrCallMax > 0) {
        std::cout<<"We are to " << nbrCallMax << " of the end" << std::endl;
        nbrCallMax --;
        reg = calculateRegions(reg);
        for (const auto & r : regions) {
            if (! isUnique(r->getborder())) {
                std::cout << "Error border not unique after growing in region " << r->getId() << " iteration :" << nbrCallMax << std::endl;
            }
        }
    }
    
}

void ComputeRegions::calculateToTheEnd(int nbrCallMax) {
    // We get all regions in a queue
    std::queue<Region *> regToCal = std::queue<Region *>();
    for (int i = 0; i < nb_regions; i++) {
        regToCal.push(regions[i]);
    }
    // We calculate the regions
    calculatQueueRegion(regToCal, nbrCallMax);
}

void ComputeRegions::displayListPoint(std::vector<cv::Point> & listPoint) {
    for (const auto &element : listPoint) {
        std::cout << " Point " << element.x << " / " << element.y << " | " << tabInfo[element.x][element.y] << " /// ";
    }
    std::cout << std::endl;
}

void ComputeRegions::displayMap(std::unordered_map<int, std::unordered_set<int>> & map) {
    for (const auto &element : map) {
        std::cout << "Region " << element.first << " : ";
        for (const auto &element2 : element.second) {
            std::cout << element2 << " / ";
        }
        std::cout << std::endl;
    }
}

int ComputeRegions::findInt(const int target, const std::vector<std::unordered_set<int>> & listOfSets) const {
    // We parcour the list of set
    for (std::size_t i = 0; i < listOfSets.size(); ++i) {
        // To find the target in the set
        if (listOfSets[i].find(target) != listOfSets[i].end()) {
            return static_cast<int>(i); // It's the indice of the vector
        }
    }
    std::cout << "Error findInt" << std::endl;
    return -1; // If the int doesn't exist
}

void ComputeRegions::updateStorageRegions (std::vector<std::unordered_set<int>> listIR) {
    // First step we update regions
    std::vector<Region *> newRegions = std::vector<Region *>();
    for (std::size_t i = 0; i < listIR.size(); i++) {
        newRegions.push_back(regions[*listIR[i].begin() - 1]);
        newRegions[i]->setId(i + 1);
    }
    regions = newRegions;
    nb_regions = regions.size();
    // Second step we update tabInfo
    for (int i = 0; i < size_x_tabInfo; i++) {
        for (int j = 0; j < size_y_tabInfo; j++) {
            int id = tabInfo[i][j];
            if (id > 0) {
                // We get the indice of the set in the list of set then we get the id of the region
                tabInfo[i][j] = regions[findInt(id, listIR)]->getId();
            } else if (id < 0) {
                tabInfo[i][j] = regions[(findInt(id * -1, listIR))]->getId() * -1;
            }
        }
    }
} 

void ComputeRegions::updateBorder (Region * r) {
    std::unordered_set<cv::Point> tmp = r->getborder();
    for (const auto & pborder : tmp) {
        int id = getIdRegion(cv::Point(pborder.x, pborder.y));
        if (id == r->getId()) {
            std::cout << "we remove point" << pborder <<std::endl;
            r->removePointInBorder(pborder);
        }
    }
    std::unordered_set<cv::Point> tmp2 = r->getborder();
    // std::cout << "size border " << tmp.size() << " / " << tmp2.size() << std::endl;
}

Region * ComputeRegions::mergeRegion(const int id, std::unordered_set<int> & alereadyMerge, std::unordered_set<int> & mergeInidice, int iteration, int & regTraited) {
    // std::cout << "Merge region " << id << std::endl;
    Region *r = regions [id - 1];
    mergeInidice.insert(r->getId());
    std::unordered_set<cv::Point> listeBorder = r->getborder();
    iteration --;
    // We parcour the list of border
    for (auto it = listeBorder.begin(); it != listeBorder.end(); ++it) {
        cv::Point p = *it;
        // We get the region of the point (point in border)
        int idReg2 = getIdRegion(p);
        // We check if the region is not already merge
        if (idReg2 > 0 && mergeInidice.find(idReg2) == mergeInidice.end()) {
            // We get the region
            Region * r2 = regions[idReg2 - 1];
            if (alereadyMerge.find(r2->getId()) == alereadyMerge.end() && r->verifyFusion(*r2)) {
                r2 = mergeRegion(r2->getId(), alereadyMerge, mergeInidice, iteration, regTraited);
                // If r have change we update r
                // alereadyMerge.insert(r2->getId());
                r = regions[r->getId() - 1];
                // Finaly we merge r2 in r
                *r += *r2;
            }
        }
    }
    if (iteration <= 0) {
        std::cout << "Error iteration, we don't put region in aleredy merge" << std::endl;
        return r;
    } else {
        regTraited ++;
        std::cout<<"Region " << r->getId() << " is merge (" << regTraited << " / " << nb_regions << ")" << std::endl;
        alereadyMerge.insert(r->getId());
        return r;
    }
}

void ComputeRegions::merge() {
    std::cout << "Merge" << std::endl;
    // We creat a list of region already merge for not merge them again
    std::unordered_set<int> alereadyMerge;
    // We creat a list of region to merge
    std::unordered_set<int> notMerge;
    // We creat a list of indice of region to merge
    std::unordered_set<int> mergeInidice;
    // We keep all list of indice of region to merge for update tabInfo and regions 
    std::vector<std::unordered_set<int>> listOfIndicesToRegion;
    for (int i = 0; i < nb_regions; i++) {
        notMerge.insert(regions[i]->getId());
    }
    int nbRegTraited = 0;
    while (!notMerge.empty()) {
        // We get the first element
        int id = *notMerge.begin();
        notMerge.erase(id);
        Region * r = new Region(*mergeRegion(id, alereadyMerge, mergeInidice, 1000, nbRegTraited));
        // We keep in memory the list of indice of region to merge
        listOfIndicesToRegion.push_back(mergeInidice);
        // We update regions for continue merge
        while (!mergeInidice.empty()) {
            int idMerged = *mergeInidice.begin();
            notMerge.erase(idMerged);
            mergeInidice.erase(idMerged);
            delete regions[idMerged - 1];
            regions[idMerged - 1] = r;
        }
        updateBorder(r);
    }
    std::cout << "Merge end" << std::endl;
    // We update tabInfo
    updateStorageRegions(listOfIndicesToRegion);
}

void ComputeRegions::display(const std::string title) {
    std::cout << "size display" << image->rows << " / " << image->cols <<std::endl;
    // we creat a new image to see the regions
    cv::Mat * image_regions = new cv::Mat(image->clone());
    for (int i = 0; i < size_x_tabInfo; i++) {
        for (int j = 0; j < size_y_tabInfo; j++) {
            int id = tabInfo[i][j];
            if (id > 0) {
                image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regions[id - 1]->getColor();
            } else if (id < 0) {
                image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 255);
            } else {
                image_regions->at<cv::Vec3b>(cv::Point(i, j)) = /*image->at<cv::Vec3b>(cv::Point(i, j)); */ cv::Vec3b(0, 0, 0);
            }
        }
    }       
    // We display the image with the regions
    if (!image_regions->empty()) {
        cv::imshow(title, *image_regions);
    } else {
        std::cerr << "Error: Empty or invalid image. (Error in display)" << std::endl;
    }
    std::cout << "(end display)" << std::endl;
    // delete image_regions;
}

cv::Mat * ComputeRegions::display2(const std::string & name, int resize, const std::string & directory) {    // Créer une palette de couleurs
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
                image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(255, 0, 0);
            } else {
                image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 0);
            }
        }
    }

    // Ajuster la taille de l'image pour l'affichage
    cv::resize(*image_regions, *image_regions, cv::Size(), resize, resize, cv::INTER_NEAREST);

    // Afficher l'image avec les régions colorées
    cv::imshow(name, *image_regions);
    cv::imwrite(directory + name + ".png", *image_regions);
    return image_regions;
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

void ComputeRegions::displayoutlines() {
    // We call the display function for each region
    for (int i = 0; i < nb_regions; i++) {
        // std::cout << "Region " << i << std::endl;
        // regions->at(i).displayoutline("outline Region :" + std::to_string(i), *image);
    }
}

Region * ComputeRegions::getRegion(cv::Point p) {
    int id = tabInfo[p.x][p.y];
    if (id > 0) {
        return regions[id - 1];
    } else {
        return nullptr;
    }
}

int ComputeRegions::getIdRegion(cv::Point p) {
    // If in com are usless buuuuuuttttt if we want to use for debug :-)
    int id = tabInfo[p.x][p.y];
    // if (id > nb_regions || id < -1 * nb_regions) {
    //     std::cout << "Error id " << id << std::endl;
    //     std::cout << "Error nb_regions " << nb_regions << std::endl;
    //     std::cout << "Error p " << p << std::endl;
    // } 
    if (id == 0) {
        return 0;
    } else if (id < 0) {
        // if (regions[(id * -1) - 1]->getId() < nb_regions * -1) {
        //     std::cout << "Error id " << regions[(id - 1)]->getId() << std::endl;
        //     std::cout << "Error nb_regions " << nb_regions << std::endl;
        //     std::cout << "Error p " << p << std::endl;
        // }
        return regions[(id + 1) * -1]->getId() * -1;
    } else {
        // if (regions[(id - 1)]->getId() > nb_regions) {
        //     std::cout << "Error id " << regions[(id - 1)]->getId() << std::endl;
        //     std::cout << "Error nb_regions " << nb_regions << std::endl;
        //     std::cout << "Error p " << p << std::endl;
        // }
        return regions[id - 1]->getId();
    }
}

int ComputeRegions::getNbRegions() const {
    return nb_regions;
}

int ComputeRegions::getNbPixels() const {
    return nb_pixels;
}

std::vector<cv::Point> ComputeRegions::getNotInReg() {
    // A vector of point not in region
    std::vector<cv::Point> notInRegion = std::vector<cv::Point>();
    for (int i = 0; i < size_x_tabInfo; i++) {
        for (int j = 0; j < size_y_tabInfo; j++) {
            if (tabInfo[i][j] <= 0) {
                notInRegion.push_back(cv::Point(i, j));
            }
        }
    }
    return notInRegion;
}

void ComputeRegions::reCalculateRegions(float pourcent) {
    std::vector<cv::Point> pointNotInReg = getNotInReg();
    std::queue<Region *> regToCal = std::queue<Region *>();
    // We get X% of the point not in region to put in new seeds
    for (int i = 0; i < pointNotInReg.size() / pourcent; i++) {
        // We get a random point
        int indice = rand() % pointNotInReg.size();
        cv::Point p = pointNotInReg[indice];
        // We put a new seed
        Region * r = new Region(i + nb_regions + 1, p, tabInfo, image);
        // We add the region in the list of regions
        regions.push_back(r);
        // We add the region in the queue of region to calculate
        regToCal.push(r);
        // We remove the point of the list
        pointNotInReg.erase(pointNotInReg.begin() + indice);
    }
    nb_regions = regions.size();
    // We calculate the regions
    calculatQueueRegion(regToCal);
    std::cout<<"End reCalculateRegions" << std::endl;
}

bool ComputeRegions::checkNeigthor(int x, int y, int id) {
    return (abs(tabInfo[x - 1][y]) == id && abs(tabInfo[x + 1][y]) == id && 
            abs(tabInfo[x][y - 1]) == id && abs(tabInfo[x][y + 1]) == id);
}

void ComputeRegions::smoothingReg() {
    std::cout << "Start smoothing function" << std::endl;
    std::cout << "Debug before, size border 1 : " << regions[0]->getborder().size() << std::endl;
    for (int i = 1; i < size_x_tabInfo - 1; i++) {
        for (int j = 1; j < size_y_tabInfo - 1; j++) {
            int id = tabInfo[i][j];
            if (id < 0) {
                if (checkNeigthor(i, j, id * -1)) {
                    // tabInfo[i][j] *= -1;
                    int idReg = getIdRegion(cv::Point(i, j)) * -1;
                    // We cout size of border
                    int size = regions[idReg - 1]->getborder().size();
                    regions[idReg - 1]->addPoint(cv::Point(i, j));
                    std::cout << "size border " << size << " to " << regions[idReg - 1]->getborder().size() << std::endl;
                }
            }
        }
    }
    std::cout << "Debug after, size border 1 : " << regions[0]->getborder().size() << std::endl;
    // for (auto & r : regions) {
    //     this->updateBorder(r);
    // }
}

float ComputeRegions::getPourcentNotInReg () {
    int nbPointNotInReg = 0;
    for (int i = 0; i < size_x_tabInfo; i++) {
        for (int j = 0; j < size_y_tabInfo; j++) {
            if (tabInfo[i][j] <= 0) {
                nbPointNotInReg ++;
            }
        }
    }
    return (nbPointNotInReg * 100) / nb_pixels;
}

/**
* Display borderInner
*/
void ComputeRegions::displayBorderInner (std::string name, int resize, const std::string & directory) {
    // We copy img original
    cv::Mat * image_borderInner = new cv::Mat(image->clone());
    cv::Mat * image_borderInner2 = new cv::Mat(display2(name, 1, directory)->clone());
    // We get border inner
    for (const auto & borderPoint : calculateBorderInner()){
        image_borderInner->at<cv::Vec3b>(borderPoint) = cv::Vec3b(0, 0, 255);
        image_borderInner2->at<cv::Vec3b>(borderPoint) = cv::Vec3b(0, 0, 255);
    }
    // We resize the image
    cv::resize(*image_borderInner, *image_borderInner, cv::Size(), resize, resize, cv::INTER_NEAREST);
    // We display the image with the border inner
    if (!image_borderInner->empty()) {
        cv::imshow(name, *image_borderInner);
        cv::imshow(name + "2", *image_borderInner2);
        cv::imwrite(directory + name + ".png", *image_borderInner);
        cv::imwrite(directory + name + "2.png", *image_borderInner2);
    } else {
        std::cerr << "Error: Empty or invalid image. (Error in display)" << std::endl;
    }
}

/***** Private functions *****/

std::vector<cv::Point> ComputeRegions::calNeightorsId (cv::Point p, int id) {
    std::vector<cv::Point> neightors;
    if (p.x > 0 && tabInfo[p.x - 1][p.y] == id) {
        neightors.push_back(cv::Point(p.x - 1, p.y));
    }
    if (p.x + 1 < size_x_tabInfo && tabInfo[p.x + 1][p.y] == id) {
        neightors.push_back(cv::Point(p.x + 1, p.y));
    }
    if (p.y > 0  && tabInfo[p.x][p.y - 1] == id) {
        neightors.push_back(cv::Point(p.x, p.y - 1));
    }
    if (p.y + 1 < size_y_tabInfo && tabInfo[p.x][p.y + 1] == id) {
        neightors.push_back(cv::Point(p.x, p.y + 1));
    }
    return neightors;
}

std::vector<cv::Point> ComputeRegions::calculateBorderInner () {
    std::vector<cv::Point> borderInner;
    for (int i = 0; i < nb_regions; i++) {
        std::unordered_set<cv::Point> borderOuter = regions[i]->getborder();
        // if (!isUnique(borderOuter)) {
        //     std::cout << "Error borderOuter not unique" << std::endl;
        // }
        for (const auto& p : borderOuter) {
            std::vector<cv::Point> neightors = calNeightorsId(p, regions[i]->getId());
            borderInner.insert(borderInner.end(), neightors.begin(), neightors.end());
        }
    }
    std::cout << "Debug calculateBorderInner , size border 1" << regions[0]->getborder().size() << std::endl;
    return borderInner;
}
