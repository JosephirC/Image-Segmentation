#ifndef REGION_HPP
#define REGION_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>


// This function is used to display a color
void displayColor (const cv::Vec3b& couleur) {
    std::cout << "Canal Bleu : " << static_cast<int>(couleur[0]) << std::endl;
    std::cout << "Canal Vert : " << static_cast<int>(couleur[1]) << std::endl;
    std::cout << "Canal Rouge : " << static_cast<int>(couleur[2]) << std::endl;
}

/**
 * Class representing a region
*/
class Region {
public:
    /**
     * Default constructor
    */
    Region() : image(new cv::Mat()),
                points(new std::vector<cv::Point>),
                colors(new std::vector<cv::Vec3b>) {};

    /**
     * Constructor
    */
    Region(cv::Point p, cv::Vec3b col, cv::Mat img): color(col), 
                    image(new cv::Mat(img.size(), img.type())),
                    points(new std::vector<cv::Point>),
                    colors(new std::vector<cv::Vec3b>) {
        image = new cv::Mat(img.size(), img.type());
        // We make the image black
        for (int i = 0; i < image->rows; ++i) {
            for (int j = 0; j < image->cols; ++j) {
                image->at<cv::Vec3b>(i, j) = cv::Vec3b(0,0,0);
            }
        }
        colors->push_back(col);
        averageColorSeuil();
        // We add the first point to the region
        grow(p, col);
    }

    /**
     * Copy constructor
    */
    Region(const Region& r) {
        image = new cv::Mat(r.image->size(), r.image->type());
        points = new std::vector<cv::Point>();
        colors = new std::vector<cv::Vec3b>();
        color = r.color;
        color_seuil_inf = r.color_seuil_inf;
        color_seuil_sup = r.color_seuil_sup;
        for (int i = 0; i < r.points->size(); i++) {
            points->push_back(r.points->at(i));
        }
        for (int i = 0; i < r.colors->size(); i++) {
            colors->push_back(r.colors->at(i));
        }
        *image = r.image->clone();
    }

    /**
     * Destructor
    */
    ~Region(){
        delete image;
        delete points;
        delete colors;
    };

    /**
     * Copy assignment operator
    */
    Region& operator=(const Region& r) {
        if (this != &r) {
            delete image;
            delete points;
            delete colors;
            image = new cv::Mat(r.image->size(), r.image->type());
            points = new std::vector<cv::Point>();
            colors = new std::vector<cv::Vec3b>();
            color = r.color;
            color_seuil_inf = r.color_seuil_inf;
            color_seuil_sup = r.color_seuil_sup;
            for (int i = 0; i < r.points->size(); i++) {
                points->push_back(r.points->at(i));
            }
            for (int i = 0; i < r.colors->size(); i++) {
                colors->push_back(r.colors->at(i));
            }
            *image = r.image->clone();
        }
        return *this;
    }

    /**
     * This function is used to add a point to the region
     * @param p The point to add
     * @param col The color of the point
     * @return true if the point has been added to the region
    */
    bool grow(cv::Point p, cv::Vec3b col) {
        // We verify if the point can be added to the region
        // In a first time we verify if the point is not in the region
        if (image->at<cv::Vec3b>(p) == cv::Vec3b(0,0,0)) {
            // If not we verrify if the color of the point is the same as the region with the seuil
            std::cout << "Verify color " << std::endl;
            displayColor(col);
            std::cout << "  Color averge " << std::endl;
            displayColor(color);
            std::cout << "      Color seuil inf " << std::endl;
            displayColor(color_seuil_inf);
            std::cout << "      Color seuil sup " << std::endl;
            displayColor(color_seuil_sup);
            
            if (verifyColor(col)) {
                std::cout << "Color verified " << std::endl;
                // If yes we add the point to the region
                image->at<cv::Vec3b>(p) = col;
                colors->push_back(col);
                // We update the average color of the region
                averageColor();
                averageColorSeuil();

                // And we update the contour of the region
                updateContour(p);
                std::cout << "Point added " <<p.x<<"/ "<<p.y<<std::endl;
                return true;
            }
        }
        std::cout << "Point not added " <<p.x<<"/ "<<p.y << std::endl;
        return false;
    };

    /**
     * This function verify if two regions can be fused
    */
    bool verifyFusion (const Region& r) {
        // We verify if the two regions in the same image
        if (image->size() != r.image->size()) {
            std::cout << "The two regions are not in the same image" << std::endl;
            return false;
        }
        // We verify if the two regions have the same color with the seuil
        return verifyColor(r.color);
    }

    /**
     * Get the contour of the region
     * @return the contour of the region
    */
    std::vector<cv::Point> getContour() {
        return *points;

        // // We verify if the contour of the region is empty
        // if (points->size() == 0) {
        //     // If yes we calculate the contour of the region
        //     for (int i = 1; i < image->rows - 1; ++i) {
        //         for (int j = 1; j < image->cols - 1; ++j) {
        //             verifyContour(cv::Point(i, j));
        //         }
        //     }
        // }
        // return *points;
    }

    /**
     * This function is used to fuse two regions
    */
    Region& operator + (const Region& r) {
        // We verify if the two regions in the same image
        if (image->size() != r.image->size()) {
            std::cout << "The two regions are not in the same image" << std::endl;
            return *this;
        }
        // We create a new region
        Region * new_region = new Region();
        // We add the points of the two regions
        for (int i = 0; i < points->size(); i++) {
            new_region->points->push_back(points->at(i));
        }
        for (int i = 0; i < r.points->size(); i++) {
            new_region->points->push_back(r.points->at(i));
        }
        // We add the colors of the two regions
        for (int i = 0; i < colors->size(); i++) {
            new_region->colors->push_back(colors->at(i));
        }
        for (int i = 0; i < r.colors->size(); i++) {
            new_region->colors->push_back(r.colors->at(i));
        }
        // We calculate the average color of the new region
        new_region->averageColor();
        new_region->averageColorSeuil();
        return *new_region;
    }

    /**
     * This function display the region.
    */
    void display(const std::string title = "Region", bool average = false) {
        if (average) {
            // In image of the region we put the averge color of the region
            for (int i = 1; i < image->rows - 1; ++i) {
                for (int j = 1; j < image->cols - 1; ++j) {
                    if (image->at<cv::Vec3b>(i, j) != cv::Vec3b(0,0,0)) {
                        image->at<cv::Vec3b>(i, j) = color;
                    }
                }
            }
        }
        cv::imshow(title, *image);
        // cv::waitKey(0);
    }

private:
    cv::Mat * image;
    cv::Vec3b color;
    cv::Vec3b color_seuil_inf;
    cv::Vec3b color_seuil_sup;
    std::vector<cv::Point> * points; // Content the points of contour of the region
    std::vector<cv::Vec3b> * colors; // Content the colors of the region
    
    /**
     * This function is used to calculate the average color of the region
    */
    void averageColor(){
        int r = 0;
        int g = 0;
        int b = 0;
        for (int i = 0; i < colors->size(); i++) {
            r += colors->at(i)[0];
            g += colors->at(i)[1];
            b += colors->at(i)[2];
        }
        r /= colors->size();
        g /= colors->size();
        b /= colors->size();
        color = cv::Vec3b(r, g, b);
    };

    /**
     * This function is used to calculate the seuil color of the region
    */
    void averageColorSeuil() {
        int r = 0;
        int g = 0;
        int b = 0;
        if (colors->size() > 50) {
            // We calculate the standard deviation of the colors
            for (int i = 0; i < colors->size(); i++) {
                r += pow(colors->at(i)[0] - color[0], 2);
                g += pow(colors->at(i)[1] - color[1], 2);
                b += pow(colors->at(i)[2] - color[2], 2);
            }
            r /= colors->size();
            g /= colors->size();
            b /= colors->size();
            r = sqrt(r);
            g = sqrt(g);
            b = sqrt(b);
            color_seuil_inf = cv::Vec3b(color[0] - r, color[1] - g, color[2] - b);
            color_seuil_sup = cv::Vec3b(color[0] + r, color[1] + g, color[2] + b);
        } else {
            std::cout << "Seuil static" << std::endl;
            color_seuil_sup[0] = (static_cast<int>(color[0]) + 20 < 255) ? static_cast<int>(color[0]) + 20 : 255;
            color_seuil_sup[1] = (static_cast<int>(color[1]) + 20 < 255) ? static_cast<int>(color[1]) + 20 : 255;
            color_seuil_sup[2] = (static_cast<int>(color[2]) + 20 < 255) ? static_cast<int>(color[2]) + 20 : 255;
            color_seuil_inf[0] = (static_cast<int>(color[0]) - 20 > 0) ? static_cast<int>(color[0]) - 20 : 0;
            color_seuil_inf[1] = (static_cast<int>(color[1]) - 20 > 0) ? static_cast<int>(color[1]) - 20 : 0;
            color_seuil_inf[2] = (static_cast<int>(color[2]) - 20 > 0) ? static_cast<int>(color[2]) - 20 : 0;
        }
    }

    /**
     * This function is used to verify if the color is in the seuil of the region
     * @param col the color to verify
     * @return true if the color is in the seuil of the region
    */
    bool verifyColor(cv::Vec3b col) {
        return (col[0] >= color_seuil_inf[0] && col[0] <= color_seuil_sup[0])
            && (col[1] >= color_seuil_inf[1] && col[1] <= color_seuil_sup[1]) 
            && (col[2] >= color_seuil_inf[2] && col[2] <= color_seuil_sup[2]);
    }

    /**
     * This function is used to verify a point is in the contour of the region
     * @param p The point to verify
    */
    bool verifyContour(cv::Point p) {
        // We verify if the point is not in the region
        if (image->at<cv::Vec3b>(p) == cv::Vec3b(0,0,0)) {
            // We verify if the point is in the contour of the region
            if (image->at<cv::Vec3b>(p.x + 1, p.y) != cv::Vec3b(0,0,0) 
                || image->at<cv::Vec3b>(p.x - 1, p.y) != cv::Vec3b(0,0,0)
                || image->at<cv::Vec3b>(p.x, p.y + 1) != cv::Vec3b(0,0,0)
                || image->at<cv::Vec3b>(p.x, p.y - 1) != cv::Vec3b(0,0,0)) {
                    points->push_back(p);
            }
        }
    }

    /**
     * To update contour of the region
     * @param p The point add to the region
    */
    void updateContour(cv::Point p) {
        // If the point is in the contour of the region, 
        // We remove it from the contour
        for (int i = 0; i < points->size(); i++) {
            if (points->at(i) == p) {
                points->erase(points->begin() + i);
                break;
            }
        }
        // We parcour the 4 points around the point
        if (image->at<cv::Vec3b>(p.x + 1, p.y) == cv::Vec3b(0,0,0)) {
            points->push_back(cv::Point(p.x + 1, p.y));
        }
        if (image->at<cv::Vec3b>(p.x - 1, p.y) == cv::Vec3b(0,0,0)) {
            points->push_back(cv::Point(p.x - 1, p.y));
        }
        if (image->at<cv::Vec3b>(p.x, p.y + 1) == cv::Vec3b(0,0,0)) {
            points->push_back(cv::Point(p.x, p.y + 1));
        }
        if (image->at<cv::Vec3b>(p.x, p.y - 1) == cv::Vec3b(0,0,0)) {
            points->push_back(cv::Point(p.x, p.y - 1));
        }
    }
};

#endif