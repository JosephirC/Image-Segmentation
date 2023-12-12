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
    CreatRegions(cv::Mat img) {
        // We creat a tab to keep regions.
        tabInfo = new int*[img.rows];
        for (int i = 0; i < img.rows; ++i) {
            tabInfo[i] = new int[img.cols];
            for (int j = 0; j < img.cols; ++j) {
                tabInfo[i][j] = 0;
            }
        }
        image = new cv::Mat(img.size(), img.type());
        *image = img.clone();
        regions = nullptr;
        seeds = nullptr;
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
        std::cout << "End of CreatRegions" << std::endl;
    }

    /**
     * Put the seeds in the image
    */
    void putSeeds(int nb_seeds) {
        this->nb_seeds = nb_seeds;
        this->nb_regions = nb_seeds;
        seeds = new Seed[nb_seeds];
        regions = new Region[nb_seeds];
        // We creat a new image to see the seeds
        cv::Mat * image_seeds = new cv::Mat(image->size(), image->type());
        *image_seeds = image->clone();
        std::cout << "Put the seeds in the image" << std::endl;
        for (int i = 0; i < nb_seeds; i++) {
            Seed s(*image);
            std::cout << "Seed " << i << std::endl;
            seeds [i] = s;
            // We put the seed in regions
            std::cout << "Put the seed in regions" << std::endl;
            Region r((i + 1), s.getPoint(), tabInfo, image);
            std::cout << "Region " << i << std::endl;
            regions [i] = r;
            // We put the seed in the image
            std::cout << "Put the seed in the image" << std::endl;
            cv::circle(*image_seeds, s.getPoint(), 1, cv::Scalar(0, 0, 255), -1); // To see the seeds
            std::cout << "Seed " << i << std::endl;
        }
        std::cout << "End of put the seeds in the image" << std::endl;
        // We display the image with the seeds
        cv::imshow("Image with seeds", *image_seeds);
        // We wait for a key to be pressed
        cv::waitKey(0);
        delete image_seeds;
    }

    /**
     * Calculate the regions
    */
    void calculateRegions() {
        std::cout << "Calculate regions" << std::endl;
        // For each region we calculate grow of this contour
        for (int i = 0; i < nb_regions; i++) {
            std::queue<cv::Point>* contour = regions [i].getContour();
            std::cout << "Contour size: " << contour->size() << std::endl;
            std::cout << "Region " << i << std::endl;

            //Display the contour
            std::queue<cv::Point> _outlines = *contour;
            std::cout << "It's queue of contour " << _outlines.size() <<std::endl;
            while (!_outlines.empty()) {
                cv::Point p = _outlines.front();
                std::cout<<"size " << _outlines.size();
                _outlines.pop();
                std::cout << "Point in Contour " << p.x << "/" << p.y << std::endl;
            }
            std::cout << "End of queue of contour" << std::endl;
            // We calculate the grow of the region
            regions [i].grow();
        }
    }

    /**
     * Display the regions
    */
    void display() {
        // We call the display function for each region
        for (int i = 0; i < nb_regions; i++) {
            std::cout << "Region " << i << std::endl;
            regions[i].display("Region :" + std::to_string(i));
        }
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
    Seed * seeds;
    Region * regions;
    int nb_seeds;
    int nb_regions;
    int ** tabInfo;
};

#endif // CREATREGIONS_HPP
