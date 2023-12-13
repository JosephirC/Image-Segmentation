#ifndef REGION_HPP
#define REGION_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <queue>

const int SEUIL = 50;
const float COEF = 1;

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
    Region() {
        tabInfo = nullptr;
        outline = new std::queue<cv::Point>();
        colors = new std::vector<cv::Vec3b>();
    };


    /**
     * Constructor
     * @param _id The id of the region
     * @param p The first point of the region
     * @param vecShare The table of information on all region
     * @param imageOriginal The original image
     * 
    */
    Region(int _id ,cv::Point p, int ** tabShare, cv::Mat * imageOriginal):
                    id(_id),
                    size_x(imageOriginal->rows),
                    size_y(imageOriginal->cols),
                    tabInfo(tabShare),
                    color(imageOriginal->at<cv::Vec3b>(p)), 
                    image(imageOriginal),
                    outline(new std::queue<cv::Point>),
                    colors(new std::vector<cv::Vec3b>) {
        
        std::cout << "Region constructor" << std::endl;
        colors->push_back(color);
        averageColorSeuil();
        // We add the first point to the region
        outline->push(p);
        std::cout << "We add the first point to the region" << std::endl;
        grow();
    };

    /**
     * Copy constructor
    */
    Region(const Region& r) {
        tabInfo = r.tabInfo;
        outline = new std::queue<cv::Point>();
        colors = new std::vector<cv::Vec3b>();
        color = r.color;
        color_seuil_inf = r.color_seuil_inf;
        color_seuil_sup = r.color_seuil_sup;
        // We copy the outline and the colors
        // std::queue<cv::Point> _outlinesCopie = r.getContour();
        for (int i = 0; i < r.colors->size(); i++) {
            colors->push_back(r.colors->at(i));
        }
    };

    /**
     * Destructor
    */
    ~Region(){
        std::cout << "Region destructor" << std::endl;
        delete outline;
        delete colors;
        std::cout << "Region destructor" << std::endl;
    };

    /**
     * Copy assignment operator
    */
    Region& operator=(const Region& r) {
        if (this != &r) {
            //delete vecInfo;
            delete outline;
            delete colors;
            tabInfo = r.tabInfo;
            outline = new std::queue<cv::Point>();
            colors = new std::vector<cv::Vec3b>();
            color = r.color;
            color_seuil_inf = r.color_seuil_inf;
            color_seuil_sup = r.color_seuil_sup;
            // We copy the outline and the colors
            // std::queue<cv::Point> _outlinesCopie = r.getContour();

            // while (!_outlinesCopie.empty()) {
            //     cv::Point p = _outlinesCopie.front();
            //     _outlinesCopie.pop();
            //     outline->push(p);
            // }
        return *this;
        }
    };

    /**
     * This function is used to add a point to the region
     * @return a vector of point to add to the region
    */
    void grow() {
        std::queue<cv::Point> _outlines = *outline;
        delete outline;
        outline = new std::queue<cv::Point>();
        std::cout << "In grow" << std::endl;
        // We get all Point in queue
        while (!_outlines.empty()) {
            cv::Point p = _outlines.front();
            _outlines.pop();
            cv::Vec3b col = image->at<cv::Vec3b>(p);
            // We verify if the point is in the image
            if (p.x < 0 || p.x >= image->rows || p.y < 0 || p.y >= image->cols) {
                std::cout << "Point not in the image " << image->rows << " / " << image->cols << std::endl;
                std::cout <<p.x<<"/ "<<p.y << std::endl;
            }
            // We verify if the point is not in an region
            if (tabInfo [p.x] [p.y] <= 0) {
                if (verifyColor(col)) {
                    // If yes we add the point to the region
                    std::cout << "Point add to the tab INFO " <<p.x<<"/ "<<p.y << std::endl;
                    tabInfo [p.x] [p.y] = id;
                    std::cout << "Point add to the tab INFO " <<p.x<<"/ "<<p.y << std::endl;
                    colors->push_back(col);
                    // We update the average color of the region
                    averageColor();
                    averageColorSeuil();
                    // And we update the contour of the region
                    updateContour(p);
                    // We verify updateConture
                    // std::queue<cv::Point> _outlines = *outline;
                    // while (!_outlines.empty()) {
                    //     cv::Point p = _outlines.front();
                    //     std::cout<<"size " << _outlines.size();
                    //     _outlines.pop();
                    //     std::cout << "Point in Contour " << p.x << "/" << p.y << std::endl;
                    // }
                    std::cout << "Point added " <<p.x<<"/ "<<p.y<<std::endl;
                } else {
                    tabInfo [p.x] [p.y] = -1;
                    std::cout << "Point not added " <<p.x<<"/ "<<p.y << std::endl;
                    // We remove the point from the contour of the region
                }
           } else {
                //std::cout << "Point already in the region or traiter" << std::endl;
           }
        
        }
        std::cout << "END grow" << std::endl;
    };

    /**
     * This function verify if two regions can be fused
    */
    bool verifyFusion (const Region& r) {
        // We verify if the two regions in the same image
        if (size_x != r.size_x || size_y != r.size_y) {
            std::cout << "The two regions are not in the same image" << std::endl;
            return false;
        }
        // We verify if the two regions have the same color with the seuil
        return verifyColor(r.color);
    };

    /**
     * Get the contour of the region
     * @return the contour of the region
    */
    std::queue<cv::Point> * getContour() {
        // Display the contour
        std::queue<cv::Point> _outlines = *outline;
        std::cout << "It's queue of contour =============" << _outlines.size() <<std::endl;
        while (!_outlines.empty()) {
            cv::Point p = _outlines.front();
            std::cout<<"size " << _outlines.size();
            _outlines.pop();
            std::cout << "Point in Contour " << p.x << "/" << p.y << std::endl;
        }
        std::cout << "End of queue of contour" << std::endl;
        return new std::queue<cv::Point>(*outline);
    };

    /**
     * This function is used to fuse two regions
    */
    // Region& operator + (const Region& r) {
    //     // We verify if the two regions in the same image
    //     if (image->size() != r.image->size()) {
    //         std::cout << "The two regions are not in the same image" << std::endl;
    //         return *this;
    //     }
    //     // We create a new region
    //     Region * new_region = new Region();
    //     // We add the points of the two regions
    //     while (!this->outline.empty()) {
    //         cv::Point p = outline.front();
    //         outline.pop();
    //         new_region->outline->push(outline));
    //     }
    //     for (int i = 0; i < r.outline->size(); i++) {
    //         new_region->outline->push(r.outline->at(i));
    //     }
    //     // We add the colors of the two regions
    //     for (int i = 0; i < colors->size(); i++) {
    //         new_region->colors->push_back(colors->at(i));
    //     }
    //     for (int i = 0; i < r.colors->size(); i++) {
    //         new_region->colors->push_back(r.colors->at(i));
    //     }
    //     // We calculate the average color of the new region
    //     new_region->averageColor();
    //     new_region->averageColorSeuil();
    //     return *new_region;
    // }

    /**
     * This function display the region.
    */
    void display(const std::string title = "Region", bool average = false) {
        // We create a new image
        cv::Mat * image = new cv::Mat(size_x, size_y, CV_8UC3, cv::Scalar(0,0,0));
        // We put the points of the region in the image
        for (int i = 0; i < size_x; i++) {
            for (int j = 0; j < size_y; j++) {
                if (tabInfo[i][j] == id) {
                    image->at<cv::Vec3b>(i, j) = color;
                }
            }
        }
        cv::imshow(title, *image);
        // cv::waitKey(0);
    };

    /**
     * Get the color of the region
    */
    cv::Vec3b & getColor() {
        return color;
    };

    /**
     * Display the contour of image
     * @param title The title of the image
     * @param img to display the contour in an image
    */
    // void displayContour(const std::string title, const cv::Mat & img) {
    //     cv::Mat * image_contour = new cv::Mat(img.size(), img.type());
    //     *image_contour = img.clone();
    //     // We put the contour in the image
    //     for (int i = 0; i < points->size(); i++) {
    //         image_contour->at<cv::Vec3b>(points->at(i)) = cv::Vec3b(255,1,1);
    //     }
    //     cv::imshow(title, *image_contour);
    //     cv::waitKey(0);
    //     delete image_contour;
    // }

private:
    int id; // The id of the region
    int size_x; // The size of the image in x
    int size_y; // The size of the image in y
    int ** tabInfo;
    cv::Mat * image; // The image
    cv::Vec3b color; // Is the average color of the region
    cv::Vec3b color_seuil_inf; // Is the seuil color of the region
    cv::Vec3b color_seuil_sup; // Is the seuil color of the region
    std::queue<cv::Point> * outline; // Content the points of outlin in the region
    // std::vector<cv::Point> * points; // Content the points of contour of the region
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
        std::cout << "Colors size " << colors->size() << std::endl;
        if (colors->size() > 50) {
            std::cout << "Seuil dynamic" << std::endl;
            // We calculate the standard deviation of the colors
            for (int i = 0; i < colors->size(); i++) {
                r += pow(colors->at(i)[0] - color[0], 2);
                g += pow(colors->at(i)[1] - color[1], 2);
                b += pow(colors->at(i)[2] - color[2], 2);
            }

            r /= colors->size();
            g /= colors->size();
            b /= colors->size();
            // Display the standard deviation
            std::cout << "Standard deviation" << std::endl;
            std::cout << "R : " << sqrt(r) << std::endl;
            std::cout << "G : " << sqrt(g) << std::endl;
            std::cout << "B : " << sqrt(b) << std::endl;
            r = sqrt(r) * COEF;
            g = sqrt(g) * COEF;
            b = sqrt(b) * COEF;
            color_seuil_inf = cv::Vec3b(color[0] - r, color[1] - g, color[2] - b);
            color_seuil_sup = cv::Vec3b(color[0] + r, color[1] + g, color[2] + b);
            // Display new seuil
            std::cout << "Seuil inf" << std::endl;
            displayColor(color_seuil_inf);
            std::cout << "Seuil sup" << std::endl;
            displayColor(color_seuil_sup);
        } else {
            // std::cout << "Seuil static" << std::endl;
            color_seuil_sup[0] = (static_cast<int>(color[0]) + SEUIL < 255) ? static_cast<int>(color[0]) + SEUIL : 255;
            color_seuil_sup[1] = (static_cast<int>(color[1]) + SEUIL < 255) ? static_cast<int>(color[1]) + SEUIL : 255;
            color_seuil_sup[2] = (static_cast<int>(color[2]) + SEUIL < 255) ? static_cast<int>(color[2]) + SEUIL : 255;
            color_seuil_inf[0] = (static_cast<int>(color[0]) - SEUIL > 0) ? static_cast<int>(color[0]) - SEUIL : 0;
            color_seuil_inf[1] = (static_cast<int>(color[1]) - SEUIL > 0) ? static_cast<int>(color[1]) - SEUIL : 0;
            color_seuil_inf[2] = (static_cast<int>(color[2]) - SEUIL > 0) ? static_cast<int>(color[2]) - SEUIL : 0;
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
        if (tabInfo[p.x] [p.y] == id) {
            // We verify if the point is in the contour of the region
            
            return (tabInfo [(p.x + 1 < size_x)? p.x + 1:size_x ] [p.y] != id ||
                tabInfo [p.x - 1 > 0?p.x - 1:0] [p.y] != id ||
                tabInfo [p.x] [(p.y + 1 < size_y)? p.y + 1: size_y] != id ||
                tabInfo [p.x] [p.y - 1 > 0?p.y - 1:0] != id);
   
        } else {
            return false;
        }
    }

    /**
     * This function is used to verify if a point is in the image and is free
    */
    bool verifyPoint(cv::Point p) {
        return (p.x >= 0 && p.x < size_x && p.y >= 0 && p.y < size_y && tabInfo[p.x][p.y] == 0);
    }

    /**
     * To update contour of the region
     * @param p The point add to the region
    */
    void updateContour(cv::Point p) {
        // We parcour the 4 points around the point,
        // if a point is a new outline, we add it to the outline
        if (verifyPoint(cv::Point(p.x + 1, p.y))) {
            // std::cout << "Point add to contour " << p.x + 1 << "/" << p.y << std::endl;
            outline->push(cv::Point(p.x + 1, p.y));
        }
        if (verifyPoint(cv::Point(p.x - 1, p.y))) {
            // std::cout << "Point add to contour " << p.x - 1 << "/" << p.y << std::endl;
            outline->push(cv::Point(p.x - 1, p.y));
        }
        if (verifyPoint(cv::Point(p.x, p.y + 1))) {
            // std::cout << "Point add to contour " << p.x << "/" << p.y + 1 << std::endl;
            outline->push(cv::Point(p.x, p.y + 1));
        }
        if (verifyPoint(cv::Point(p.x, p.y - 1))) {
            // std::cout << "Point add to contour " << p.x << "/" << p.y - 1 << std::endl;
            outline->push(cv::Point(p.x, p.y - 1));
        }
        std::cout<<"We are passed here"<<std::endl;
        // Display the contour
        std::queue<cv::Point> _outlines = *outline;
        std::cout << "It's queue of contour " << _outlines.size() <<std::endl;
        while (!_outlines.empty()) {
            cv::Point p = _outlines.front();
            // std::cout<<"size " << _outlines.size();
            _outlines.pop();
            // std::cout << "Point in Contour " << p.x << "/" << p.y << std::endl;
        }
        // std::cout << "End of queue of contour" << std::endl;
    }
};

#endif // REGION_HPP