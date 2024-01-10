#ifndef REGION_HPP
#define REGION_HPP

#include <iostream>
#include <vector>
#include <unordered_set>
#include <opencv2/opencv.hpp>
#include <queue>
#include <algorithm>

/**
 * Class representing a region
*/
class Region {

    public:
        /**
        * Default constructor
        */
        Region();

        /**
        * Constructor
        * @param _id The id of the region
        * @param p The first point of the region
        * @param vecShare The table of information on all region
        * @param imageOriginal The original image
        * 
        */
        Region(int _id ,cv::Point p, int ** tabShare, cv::Mat * imageOriginal, int _threshold = 5, float _coefSD = 1.);

        /**
        * TODO : copier TOUS les elements de la region
        * 
        * Copy constructor
        */
        Region(const Region& r);

        /**
        * Destructor
        */
        ~Region();

        /**
        * Copy assignment operator /!\ This function is not finished
        * TODO : youssef : je ne pense pas qu'il est bon de faire l'operator = de cette maniere
        *      surtout pour le tabInfo
        */
        Region& operator=(const Region& r);

        /**
        * This function is used to add a queue of point to the region
        * 
        */
        void grow();

        /**
         * Verify if a color can be fused with the region 
        */
        bool verifyFusion2 (const cv::Vec3b& col);

        /**
        * /!\ not finished This function verify if two regions can be fused
        *
        */
        bool verifyFusion (Region& r);

        /**
        * Add Point in region
        */
        void addPoint(cv::Point p);

        /**
        * Remove Point in region
        * @return true if the region is empty
        */
        bool removePoint(cv::Point p);

        /**
        * Get the outline of the region
        * @return the outline of the region
        */
        std::queue<cv::Point> & getoutline() const;

        /**
        * Get the border of the region with vector
        * @return the border of the region
        */
        std::vector<cv::Point> & getborderVector() const;


        /**
        * Get the border of the region
        * @return the border of the region
        */
        std::unordered_set<cv::Point> & getborder() const;

        /**
        * Set the outline of the region
        * @param _outline The outline to set
        */
        void setoutline(std::queue<cv::Point>& _outline);

        /**
        * Set the border of the region
        * @param _border The border to set
        */
        void setborder(const std::unordered_set<cv::Point>& _border);
        
        /**
        * Remove a point in the border of the region
        */
        void removePointInBorder (cv::Point p);

        /**
        * Set the outline of the region
        */
        void setoutline(const std::vector<cv::Point> & _outline);

        /**
        * Set the outline of the region with unordered_set
        */
        void setoutline(const std::unordered_set<cv::Point> & _outline);

        /**
        * Clear the border of the region
        */
        void clearborder();

        /**
        * This function display the region.
        */
        void display(const std::string title = "Region", bool average = false);

        /**
        * Get the color of the region
        */
        cv::Vec3b getColor() const;

        /**
        * Get all colors of the region
        */
        std::vector<cv::Vec3b> getColors() const;

        /**
         * Set all colors of the region
        */
        void setColors(const std::vector<cv::Vec3b> & _colors); 

        /**
        * For increase the seuil
        */
        void increaseThreshold();

        /**
        * Get the isIncrease
        */
        bool getIsIncrease();

        /**
        * Get the id of the region
        */
        int getId() const;

        /**
        * Set new id to the region
        */
        void setId(const int _id);

        /**
        * Make fuse between two regions
        */
        void operator+= (const Region & r2);

        /** 
        * Define the operator < for the class Region
        */
        bool operator<(const Region& other) const;

        /**
        * Define the operator == for the class Region
        */
        bool operator==(const Region& other) const;

        /**
         * For using cv::Point in std::unordered_map
        */
        friend std::hash<cv::Point>;

        /**
         * Define the operator == for the class cv::Point
        */
        friend bool operator==(const cv::Point& a, const cv::Point& b);

        /**
        * This function is used to calculate the average color of the region
        */
        void averageColor();

        /**
        * This function is used to calculate the seuil color of the region
        */
        void averageColorSeuil();

        void computeCritMerge();

    private:
        int id; // The id of the region
        int size_x; // The size of the image in x
        int size_y; // The size of the image in y
        int ** tabInfo;
        cv::Mat * image; // The image
        cv::Vec3b color; // Is the average color of the region
        cv::Vec3b color_seuil_inf; // Is the seuil color of the region
        cv::Vec3b color_seuil_sup; // Is the seuil color of the region
        std::queue<cv::Point> * outline; // Content the points of outlin in the region, this points are not traited
        std::unordered_set<cv::Point> * border; // Content the points border of the region, this points are traited
        std::vector<cv::Vec3b> * colors; // Content the colors of the region
        int threshold;
        float coefSD;
        bool isIncrease;

        // std::unordered_map<int, cv::Vec3b> * allRegionColors; // cela permet a toutes les regions de savoir les couleurs des autres regions

        /**
        * This function is used to verify if the color is in the seuil of the region
        * @param col the color to verify
        * @return true if the color is in the seuil of the region
        */
        bool verifyColor(cv::Vec3b col) const;

        /**
        * This function is used to verify a point is in the outline of the region
        * @param p The point to verify
        */
        bool verifyoutline(cv::Point p) const;

        /**
        * This function is used to verify if a point is in the image and is free
        */
        bool verifyPoint(cv::Point p) const;

        /**
        * To update outline of the region
        * @param p The point add to the region
        */
        void updateoutline(cv::Point p);
};

#endif // REGION_HPP