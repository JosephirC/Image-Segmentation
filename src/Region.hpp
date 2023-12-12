#ifndef REGION_HPP
#define REGION_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

const int SEUIL = 50;

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
                outline(new std::queue<cv::Point>),
                colors(new std::vector<cv::Vec3b>) {};

    /**
     * Constructor
    */
    Region(cv::Point p, cv::Mat img): color(img.at<cv::Vec3b>(p)), 
                    image(new cv::Mat(img.size(), img.type())),
                    outline(new std::queue<cv::Point>),
                    colors(new std::vector<cv::Vec3b>) {
        image = new cv::Mat(img.size(), img.type());
        // We make the image black
        for (int i = 0; i < image->rows; ++i) {
            for (int j = 0; j < image->cols; ++j) {
                image->at<cv::Vec3b>(i, j) = cv::Vec3b(0,0,0);
            }
        }
        colors->push_back(color);
        averageColorSeuil();
        // We add the first point to the region
        std::queue<cv::Point> _outlines;
        _outlines.push(p);
        grow(_outlines, img);
    };

    /**
     * Copy constructor
    */
    Region(const Region& r) {
        image = new cv::Mat(r.image->size(), r.image->type());
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
        *image = r.image->clone();
    };

    /**
     * Destructor
    */
    ~Region(){
        delete image;
        delete outline;
        delete colors;
    };

    /**
     * Copy assignment operator
    */
    Region& operator=(const Region& r) {
        if (this != &r) {
            delete image;
            delete outline;
            delete colors;
            image = new cv::Mat(r.image->size(), r.image->type());
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
     * @param p The points to add
     * @param col The color of the point
     * @return true if the point has been added to the region
    */
    void grow(const std::queue<cv::Point> & param, cv::Mat & img) {
        std::queue<cv::Point> _outlines = param;
        // We get all Point in queue
        while (!_outlines.empty()) {
            cv::Point p = _outlines.front();
            _outlines.pop();
            cv::Vec3b col = img.at<cv::Vec3b>(p);
            // We verify if the point is in the image
            if (p.x < 0 || p.x >= image->rows || p.y < 0 || p.y >= image->cols) {
                std::cout << "Point not in the image" << std::endl;
            }
            // We verify if the point is not in the region
            if (image->at<cv::Vec3b>(p) == cv::Vec3b(0,0,0) || image->at<cv::Vec3b>(p) == cv::Vec3b(255,1,1)) {
                if (verifyColor(col)) {
                    std::cout << "Point in the region" << std::endl;
                    // If yes we add the point to the region
                    image->at<cv::Vec3b>(p) = col;
                    colors->push_back(col);
                    // We update the average color of the region
                    averageColor();
                    averageColorSeuil();

                    // And we update the contour of the region
                    updateContour(p);
                    std::cout << "Point added " <<p.x<<"/ "<<p.y<<std::endl;
                } else {
                    image->at<cv::Vec3b>(p) = cv::Vec3b(1,1,255);
                    // We remove the point from the contour of the region
                }
           }
        std::cout << "Point not added " <<p.x<<"/ "<<p.y << std::endl;
        }
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
    cv::Mat * image; // Content the region, Point traiter, Point to traiter
    cv::Vec3b color;
    cv::Vec3b color_seuil_inf;
    cv::Vec3b color_seuil_sup;
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
        if (image->at<cv::Vec3b>(p) == cv::Vec3b(0,0,0)) {
            // We verify if the point is in the contour of the region
            return (image->at<cv::Vec3b>(p.x + 1, p.y) != cv::Vec3b(0,0,0) 
                || image->at<cv::Vec3b>(p.x - 1, p.y) != cv::Vec3b(0,0,0)
                || image->at<cv::Vec3b>(p.x, p.y + 1) != cv::Vec3b(0,0,0)
                || image->at<cv::Vec3b>(p.x, p.y - 1) != cv::Vec3b(0,0,0));
        } else {
            return false;
        }
    }

    /**
     * To update contour of the region
     * @param p The point add to the region
    */
    void updateContour(cv::Point p) {
        // We parcour the 4 points around the point,
        // if a point is a new outline, we add it to the outline
        if (image->at<cv::Vec3b>(p.x + 1, p.y) == cv::Vec3b(0,0,0)) {
            image->at<cv::Vec3b>(cv::Point(p.x + 1, p.y)) = cv::Vec3b(255,1,1);
            std::cout << "Point add to contour " << p.x + 1 << "/" << p.y << std::endl;
            outline->push(cv::Point(p.x + 1, p.y));
        }
        if (image->at<cv::Vec3b>(p.x - 1, p.y) == cv::Vec3b(0,0,0)) {
            image->at<cv::Vec3b>(cv::Point(p.x - 1, p.y)) = cv::Vec3b(255,1,1);
            std::cout << "Point add to contour " << p.x - 1 << "/" << p.y << std::endl;
            outline->push(cv::Point(p.x - 1, p.y));
        }
        if (image->at<cv::Vec3b>(p.x, p.y + 1) == cv::Vec3b(0,0,0)) {
            image->at<cv::Vec3b>(cv::Point(p.x, p.y + 1)) = cv::Vec3b(255,1,1);
            std::cout << "Point add to contour " << p.x << "/" << p.y + 1 << std::endl;
            outline->push(cv::Point(p.x, p.y + 1));
        }
        if (image->at<cv::Vec3b>(p.x, p.y - 1) == cv::Vec3b(0,0,0)) {
            image->at<cv::Vec3b>(cv::Point(p.x, p.y - 1)) = cv::Vec3b(255,1,1);
            std::cout << "Point add to contour " << p.x << "/" << p.y - 1 << std::endl;
            outline->push(cv::Point(p.x, p.y - 1));
        }
        // Display the contour
        std::queue<cv::Point> _outlines = *outline;
        std::cout << "It's queue of contour " << _outlines.size() <<std::endl;
        while (!_outlines.empty()) {
            cv::Point p = _outlines.front();
            std::cout<<"size " << _outlines.size();
            _outlines.pop();
            std::cout << "Point in Contour " << p.x << "/" << p.y << std::endl;
        }
        std::cout << "End of queue of contour" << std::endl;
    }
};

#endif // REGION_HPP