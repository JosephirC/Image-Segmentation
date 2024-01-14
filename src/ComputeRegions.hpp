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
        * @param img : image 
        * @param _pourcentByRep : percent of point by repartition
        * @param _rep : number of repartition
        * @param _seuilMin : min of threshold
        * @param _seuilMax : max of threshold
        * @param _coefSDMin : min of coef of standard deviation
        * @param _coefSD : max of coef of standard deviation
        */
        ComputeRegions(cv::Mat img, float _pourcentByRep, unsigned int _rep = 16, int _seuilMin = 10, int _seuilMax = 30, float _coefSDMin = 1.5, float _coefSD = 1.);

        /**
        * Destructor
        */
        ~ComputeRegions();

        /**
        * Put the seeds in the image
        */
        void putSeeds();

        /**
         * Calculate all Regions
         * @param nbrCallMax : number of max calls
        */
        void calculateAllRegions(int nbrCallMax);

        /**
        * Calculate the regions
        */
        void calculateRegions();

        /**
        * Calculate the region in parameter
        * @param regToCal : list of region to calculate
        * @return the region can be grow
        */
        std::queue<Region *> calculatQueueRegion(std::queue<Region *> & regToCal);
        
        /**
        * We calculate a queue of regions
        * @param reg : regions 
        * @param nbrCallMax : number of max calls
        */
        void calculateRegions(std::queue<Region *> reg, int nbrCallMax = 100);

        /**
        * We calculate as long as there are regions to calculate (or X)
        * @param nbrCallMax : number of call max
        */
        void calculateToTheEnd(int nbrCallMax = 100);

        /**
        * Display list of points
        * @param listPoint : list of points
        */
        void displayListPoint(std::vector<cv::Point> & listPoint);

        /**
        * Display  std::unordered_map<int, std::unordered_set<int>>
        * @param map : map
        */
        void displayMap(std::unordered_map<int, std::unordered_set<int>> & map);

        /**
        * Find index in vector of undored_set<int>, with a int
        * @param target : int to find
        * @param listOfSets : vector of unordered_set<int>
        * @return index of target in vector
        */
        int findInt(const int target, const std::vector<std::unordered_set<int>> & listOfSets) const;

        /**
        * update tabInfos and regions
        * @param listIR : list of unordered_set<int>
        */
        void updateStorageRegions (std::vector<std::unordered_set<int>> listIR);

        /** 
        * update tabInfos and regions
        * @param listIR : list of unordered_set<int>
        */ 
        void updateStorageRegions (std::unordered_map<int, std::unordered_set<int>> listIR);

        /**
        * Update the border of a region
        * @param r : region
        */
        void updateBorder (Region * r);

        /**
        * Merge a Region 
        * @param id : id of region
        * @param alereadyMerge : set of id already merge
        * @param mergeIndex : set of indexes of region to merge
        * @param iteration : number of iteration
        * @param regTraited : number of regions treated
        * @return the region merged
        */
        Region * mergeRegion(const int id, std::unordered_set<int> & alereadyMerge, std::unordered_set<int> & mergeIndex, int & iteration, int & regTraited);

        /**
        * Merge regions
        */
        bool merge();
        
        /**
        * Display the regions
        * @param title : title of the window
        */
        void display(const std::string title = "Image with regions");

        /**
         * Display the regions
         * @param name : name of the window
         * @param resize : resize of the image
        */
        cv::Mat * display2(const std::string & name = "Image with regions", int resize = 1);

        /**
        * Display the outline regions
        */
        void displayoutlines();

        /*
         * Make image with region
        */
        cv::Mat * makeImageWithRegion();

        /**
         * make image with border inner
        */
        cv::Mat * makeImageWithBorderInner();

        /**
        * Return the region of a given point
        * @param p : point
        */
        Region * getRegion(cv::Point p);

        /**
        * Return the id of a region by a point
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
        * @param pourcent : percent of point not in region to put in new seeds
        */
        void reCalculateRegions(float pourcent = 30);

        /**
        * We check if neighbour is in same region, or in boder of this region
        */
        bool checkNeigthor(int x, int y, int id);

        /**
        * Smoothing regions in image
        */
        void smoothingReg();

        /**
        * Calculate the percent of points not in region
        */
        float getPourcentNotInReg();

        /**
         * Check all neightbour regions
         * @param idReg : id of region
         * @return map of neightbor region with their percent of neightbours
        */
        std::unordered_map<int, float> checkNeigthorRegion(int idReg);

        /**
         * Encompassment of regions
         * @param pourcent : pourcent of neightbour regions to encompass
        */
        void encompassmentRegion(float pourcent = 60.0);

        /**
        * Display borderInner
        * @param name : name of the image
        * @param resize : resize of the image
        * @param directory : directory of the image
        */
        void displayBorderInner (std::string name = "Image with border inner", int resize = 1, const std::string & directory = "image_cree/");

        /**
         * Save the image
         * @param name : name of the image
         * @param directory : directory of the image
        */
        void saveImage(const std::string & name, const std::string & directory = "image_cree/");

        /**
         * Save the image with border inner
         * @param name : name of the image
         * @param directory : directory of the image
        */
        void saveImageWithBorderInner(const std::string & name, const std::string & directory = "image_cree/");

        /**
         * For using cv::Point in std::unordered_map
        */
        friend std::hash<cv::Point>;

        /**
         * Define the operator == for the class cv::Point
         * @param a : point a
         * @param b : point b
        */
        friend bool operator==(const cv::Point& a, const cv::Point& b);

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
        int seuilMin;
        int seuilMax;
        float coefSDMin;
        float coefSDMax;

        /**
        * Calculate neightours same id of a point
        * @param p : point
        * @param id : id of region
        * @return queue of Point
        */
        std::vector<cv::Point> calNeightorsId (cv::Point p, int id);

        /**
        * Calculate border inner each region
        * @return vector of border inner
        */
        std::vector<cv::Point> calculateBorderInner();

        /**
         * Find all id of neightbour point
         * @param p : point
         * @return vector of id
        */
        std::vector<int> findNeightbourPoint(const cv::Point & p);
};

#endif // COMPUTREGIONS_HPP
