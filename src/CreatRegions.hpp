#ifndef CREATREGIONS_HPP
#define CREATREGIONS_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Region.hpp"
#include "Seed.hpp"

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
        
        size_x_tabInfo = img.rows;
        size_y_tabInfo = img.cols;
        nb_regions = nb_seeds;
        this->nb_seeds = nb_seeds;
        seeds = std::vector<Seed>(nb_seeds);
        regions = std::vector<Region *>(nb_seeds);
        tabInfo = new int*[img.cols];
        for (int i = 0; i < img.cols; i++) {
            tabInfo[i] = new int[img.rows];
            for (int j = 0; j < img.rows; j++) {
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
        //delete seeds;
        
        //delete regions;
        
        // for (int i = 0; i < image->rows; ++i) {
        //     delete tabInfo[i];
        // }
        // delete []tabInfo;
        // std::cout << "End of CreatRegions" << std::endl;
    }

    /**
     * Put the seeds in the image
    */
    void putSeeds() {        
        std::cout << "size PutSeeds" << image->rows << " / " << image->cols <<std::endl;
        // We creat a new image to see the seeds
        cv::Mat * image_seeds = new cv::Mat(image->clone());
        std::cout << "size" << image->rows << image->cols <<std::endl;
        for (int i = 0; i < nb_seeds; i++) {
            // We creat a seed
            Seed s(*image);
            seeds.push_back(s);
            std::cout << "Seed " << i << " : " << s.getPoint().x << "/" << s.getPoint().y << std::endl;
            std::cout << tabInfo [173] [291] << std::endl;
            regions [i] = (new Region((i + 1), s.getPoint(), tabInfo, image));
            // We put the seed in the image
            std::cout << "Seed " << i << " : " << s.getPoint().x << "/" << s.getPoint().y << std::endl;
            cv::circle(*image_seeds, s.getPoint(), 1, cv::Scalar(0, 0, 255), -1); // To see the seeds
            std::cout << "Seed " << i << " : " << s.getPoint().x << "/" << s.getPoint().y << std::endl;
        }
        // We display the image with the seeds
        std::cout << "We display the image with the seeds" << std::endl;
        cv::imshow("Image with seeds 1", *image_seeds);
        std::cout << "End of putSeeds" << std::endl;
        // We wait for a key to be pressed
        cv::waitKey(0);
        // delete image_seeds;
    }

    /**
     * Calculate the regions
    */
    void calculateRegions() {
        std::cout << "size Calculate" << image->rows << " / " << image->cols <<std::endl;
        std::cout << "Calculate regions" << std::endl;
        // For each region we calculate grow of this contour
        for (int i = 0; i < nb_regions; i++) {
            // std::queue<cv::Point>* contour = regions [i].getContour();
            // std::cout << "Contour size: " << contour->size() << std::endl;
            // std::cout << "Region " << i << std::endl;

            //Display the contour
            // std::queue<cv::Point> _outlines = *contour;
            // std::cout << "It's queue of contour " << _outlines.size() <<std::endl;
            // while (!_outlines.empty()) {
            //     cv::Point p = _outlines.front();
            //     std::cout<<"size " << _outlines.size();
            //     _outlines.pop();
            //     std::cout << "Point in Contour " << p.x << "/" << p.y << std::endl;
            // }
            // We calculate the grow of the region
            std::cout << "Region " << i << std::endl;
            regions[i]->grow();
        }
    }

    /**
     * Display the regions
    */
    void display() {
        // We call the display function for each region
        // we creat a new image to see the regions
        cv::Mat * image_regions = new cv::Mat(image->size(), image->type());
        for (int i = 0; i < size_x_tabInfo - 1; i++) {
            for (int j = 0; j < size_y_tabInfo - 1; j++) {
                if (tabInfo[i][j] != 0) {
                    std::cout << "tabInfo[" << i << "][" << j << "] = " << tabInfo[i][j] << std::endl;
                }
                int id = tabInfo[i][j];
                if (id > 0) {
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = regions[id - 1]->getColor();
                } else if (id < 0) {
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 255);
                } else {
                    image_regions->at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(0, 0, 0);
                }
            }
        }
        // We display the image with the regions
        cv::imshow("Image with regions", *image_regions);
        cv::waitKey(0);
    }

    /**
     * Display the contour regions
    */
    void displayContours() {
        // We call the display function for each region
        for (int i = 0; i < nb_regions; i++) {
            std::cout << "Region " << i << std::endl;
            // regions->at(i).displayContour("contour Region :" + std::to_string(i), *image);
        }
        cv::waitKey(0);
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
