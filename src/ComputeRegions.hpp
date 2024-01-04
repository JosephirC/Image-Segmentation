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
        ComputeRegions();

        /**
        * Constructor
        */
        ComputeRegions(cv::Mat img, float _pourcentByRep, unsigned int _rep = 16);

        /**
        * Destructor
        */
        ~ComputeRegions();

        /**
        * Put the seeds in the image
        */
        void putSeeds();

        /**
        * Calculate the regions
        */
        void calculateRegions();

        /**
        * Calculate the region in parameter
        * @param regToCal : list of region to calculate
        * @return the region can be grow
        */
        std::queue<Region *> calculateRegions(std::queue<Region *> & regToCal);
        
        /**
        * We calculate a queue of regions
        * @param reg : regions 
        * @param nbrCallMax : number of call max
        */
        void calculatQueueRegion(std::queue<Region *> reg, int nbrCallMax = 100);

        /**
        * We calculate as long as there are regions to calculate (or X)
        * @param nbrCallMax : number of call max
        */
        void calculateToTheEnd(int nbrCallMax = 100);

        /**
        * Display list of point
        */
        void displayListPoint(std::vector<cv::Point> & listPoint);

        /**
        * Display  std::unordered_map<int, std::unordered_set<int>>
        */
        void displayMap(std::unordered_map<int, std::unordered_set<int>> & map);

        /**
        * Find indice in vector of undored_set<int>, with a int
        */
        int findInt(const int target, const std::vector<std::unordered_set<int>> & listOfSets) const;

        /**
        * update tabInfos and regions
        */
        void updateStorageRegions (std::vector<std::unordered_set<int>> listIR);

        /**
        * Update a border region
        */
        void updateBorder (Region * r);

        /**
        * Merge a Region 
        */
        Region * mergeRegion(const int id, std::unordered_set<int> & alereadyMerge, std::unordered_set<int> & mergeInidice, int iteration, int & regTraited);

        /**
        * Merge regions
        */
        void merge();
        
        /**
        * Display the regions
        * @param title : title of the window
        */
        void display(const std::string title = "Image with regions");

        cv::Mat * display2(const std::string & name = "Image with regions", int resize = 1, const std::string & directory = "image_cree/");
        // void displayWithRegionId();

        //displayWithRegionId aussi
        // void display3();

        /**
        * Display the outline regions
        */
        void displayoutlines();

        /**
        * Return region in a point
        * @param p : point
        */
        Region * getRegion(cv::Point p);

        /**
        * Return the id of regions
        * @param p : point
        */
        int getIdRegion(cv::Point p);

        /**
        * Return number of regions
        */
        int getNbRegions() const;

        /**
        * Return number of pixels
        */
        int getNbPixels() const;
        /**
        * Get point not in regions in vector
        */
        std::vector<cv::Point> getNotInReg();

        /**
        * ReCalculate regions in image
        * @param pourcent : pourcent of point not in region to put in new seeds
        */
        void reCalculateRegions(float pourcent = 30);

        /**
        * We check if neghtbor is in same region, or in boder of this region 
        */
        bool checkNeigthor(int x, int y, int id);

        /**
        * Smoothing regions in image
        */
        void smoothingReg();

        /**
        * Calculate the pourcent of point not in region
        */
        float getPourcentNotInReg();

        /**
        * Display borderInner
        */
        void displayBorderInner (std::string name = "Image with border inner", int resize = 1, const std::string & directory = "image_cree/");

    private:
        cv::Mat * image;
        std::vector<Region *> regions;
        int nb_regions;
        float pourcent;
        unsigned int rep;
        int ** tabInfo;
        int size_x_tabInfo;
        int size_y_tabInfo;
        int nb_pixels;
        /**
        * Calculate neightors same id of a point
        * @return queue of Point
        */
        std::vector<cv::Point> calNeightorsId (cv::Point p, int id);

        /**
        * Calculate border inner each region
        */
        std::vector<cv::Point> calculateBorderInner();
};

#endif // COMPUTREGIONS_HPP
