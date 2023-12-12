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
        image = new cv::Mat(img.size(), img.type());
        *image = img.clone();
        regions = new std::vector<Region>();
        seeds = new std::vector<Seed>();
    }

    /**
     * Destructor
    */
    ~CreatRegions() {
        delete image;
        delete seeds;
        delete regions;
    }

    /**
     * Put the seeds in the image
    */
    void putSeeds(int nb_seeds) {
        seeds = new std::vector<Seed>();
        // We creat a new image to see the seeds
        cv::Mat * image_seeds = new cv::Mat(image->size(), image->type());

        for (int i = 0; i < nb_seeds; i++) {
            Seed s(*image);
            seeds->push_back(s);
            // We put the seed in regions
            Region r(s.getPoint(), image->at<cv::Vec3b>(s.getPoint()), *image);
            r.display();
            std::cout << "Region " << i << std::endl;
            regions->push_back(r);
            // We put the seed in the image
            cv::circle(*image_seeds, s.getPoint(), 5, cv::Scalar(0, 0, 255), -1); // To see the seeds
            std::cout << "Seed " << i << std::endl;
        }
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
        for (int i = 0; i < regions->size(); i++) {
            std::vector<cv::Point> contour = regions->at(i).getContour();
            std::cout << "Contour size" << contour.size() << std::endl;

            std::cout << "Region " << i << std::endl;
            for (int j = 0; j < contour.size(); j++) {
                std::cout << "Point " << j << std::endl;
                // We get the color of the pixel
                cv::Vec3b color = image->at<cv::Vec3b>(contour.at(j));
                // We calculate the grow of the contour
                regions->at(i).grow(contour.at(j), color);
            }
        }
    }

    /**
     * Display the regions
    */
    void display() {
        // We call the display function for each region
        for (int i = 0; i < regions->size(); i++) {
            std::cout << "Region " << i << std::endl;
            regions->at(i).display("Region :" + std::to_string(i));
        }
        cv::waitKey(0);
    }
private:
    cv::Mat * image;
    std::vector<Seed> * seeds;
    std::vector<Region> * regions;

};

#endif // CREATREGIONS_HPP
