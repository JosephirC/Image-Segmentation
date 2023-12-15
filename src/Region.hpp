#ifndef REGION_HPP
#define REGION_HPP

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <queue>

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
        border = new std::vector<cv::Point>();
        colors = new std::vector<cv::Vec3b>();

        allRegionColors = new std::unordered_map<int, cv::Vec3b>();
    };

    /**
     * Constructor
     * @param _id The id of the region
     * @param p The first point of the region
     * @param vecShare The table of information on all region
     * @param imageOriginal The original image
     * 
    */
    Region(int _id ,cv::Point p, int ** tabShare, cv::Mat * imageOriginal, int _threshold = 50, float _coefSD = 1.3):
                    id(_id),
                    size_x(imageOriginal->cols),
                    size_y(imageOriginal->rows),
                    tabInfo(tabShare),
                    color(imageOriginal->at<cv::Vec3b>(p)), 
                    image(imageOriginal),
                    threshold(_threshold),
                    coefSD(_coefSD),
                    isIncrease(true),
                    border(new std::vector<cv::Point>),
                    outline(new std::queue<cv::Point>),
                    colors(new std::vector<cv::Vec3b>),
                    allRegionColors(new std::unordered_map<int, cv::Vec3b>) {
        // std::cout << "Region constructor" << std::endl;
        colors->push_back(color);
        averageColorSeuil();
        // We add the first point to the region
        outline->push(p);
        // std::cout << "We add the first point to the region" << std::endl;
        grow();
    };

    /**
     * TODO : copier TOUS les elements de la region
     * 
     * Copy constructor
    */
    Region(const Region& r) {
        tabInfo = r.tabInfo;
        outline = new std::queue<cv::Point>();
        colors = new std::vector<cv::Vec3b>();
        
        // pas sur si le constructeur par copie d'un unordered_map est bon comme ca
        allRegionColors = new std::unordered_map<int, cv::Vec3b>(*r.allRegionColors);

        color = r.color;
        color_seuil_inf = r.color_seuil_inf;
        color_seuil_sup = r.color_seuil_sup;
        // We copy the outline and the colors
        // std::queue<cv::Point> _outlinesCopie = r.getoutline();
        for (int i = 0; i < r.colors->size(); i++) {
            colors->push_back(r.colors->at(i));
        }
    };

    /**
     * Destructor
    */
    ~Region(){
        // std::cout << "Region destructor" << std::endl;
        delete outline;
        delete colors;

        // delete allRegionColors;

        // std::cout << "Region destructor" << std::endl;
    };

    /**
     * Copy assignment operator
     * TODO : youssef : je ne pense pas qu'il est bon de faire l'operator = de cette maniere
     *      surtout pour le tabInfo
    */
    Region& operator=(const Region& r) {
        if (this != &r) {
            //delete vecInfo;
            delete outline;
            delete colors;

            delete allRegionColors;
            allRegionColors = new std::unordered_map<int, cv::Vec3b>(*r.allRegionColors);

            tabInfo = r.tabInfo;
            outline = new std::queue<cv::Point>();
            colors = new std::vector<cv::Vec3b>();
            color = r.color;
            color_seuil_inf = r.color_seuil_inf;
            color_seuil_sup = r.color_seuil_sup;
            // We copy the outline and the colors
            // std::queue<cv::Point> _outlinesCopie = r.getoutline();

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
        // std::cout << "In grow" << std::endl;
        // We get all Point in queue
        while (!_outlines.empty()) {
            cv::Point p = _outlines.front();

            _outlines.pop();
            cv::Vec3b col = image->at<cv::Vec3b>(p);
            // We verify if the point is in the image
            if (p.x < 0 || p.x >= image->cols || p.y < 0 || p.y >= image->rows) {
                std::cout << "Point not in the image " << image->rows << " / " << image->cols << std::endl;
                std::cout <<p.x<<"/ "<<p.y << std::endl;
            }
            // We verify if the point is not in an region
            if (tabInfo [p.x] [p.y] == 0) {
                if (verifyColor(col)) {
                    // If yes we add the point to the region
                    // std::cout << "Point add to the tab INFO " <<p.x<<"/ "<<p.y << std::endl;
                    tabInfo [p.x] [p.y] = id;
                    // std::cout << "Point add to the tab INFO " <<p.x<<"/ "<<p.y << std::endl;
                    colors->push_back(col);
                    // We update the average color of the region
                    averageColor();

                    (*allRegionColors)[id] = color; // je stocke la couleur moyenen de la region dans la map
                    
                    averageColorSeuil();
                    // And we update the outline of the region
                    updateoutline(p);
                    // We verify updateConture
                    // std::queue<cv::Point> _outlines = *outline;
                    // while (!_outlines.empty()) {
                    //     cv::Point p = _outlines.front();
                    //     std::cout<<"size " << _outlines.size();
                    //     _outlines.pop();
                    //     std::cout << "Point in outline " << p.x << "/" << p.y << std::endl;
                    // }
                    // std::cout << "Point added " <<p.x<<"/ "<<p.y<<std::endl;
                } else {
                    tabInfo [p.x] [p.y] = -1 * id;
                    border->push_back(p);
                    // std::cout << "Point not added " <<p.x<<"/ "<<p.y << std::endl;
                    // We remove the point from the outline of the region
                }
            } else {
                // if (tabInfo [p.x] [p.y] != id && tabInfo [p.x] [p.y] != -1 * id) {
                //     // std::cout << "Point already in the region or traited" << std::endl;
                // } else {
                //     // std::cout << "Point already in an other region" << std::endl;
                // }
                regionFusion(p);
            }
        }
        // std::cout << "END grow" << std::endl;
    };

    void regionFusion(cv::Point point) {
        // id will never be equal to 0
        if (tabInfo[point.x][point.y] != id && tabInfo[point.x][point.y] != -1 * id) {
            // On est tombe sur un point qui etait deja traite dans une autre region
            
            int neighborId = tabInfo[point.x][point.y];
            std::unordered_map<int, cv::Vec3b>::iterator it = allRegionColors->find(neighborId);
            cv::Vec3b neighborColor;
            // Peut etre ca sera mieux de faire un boucle for sur l'iterator comme vu en prog avance ? 
            if (it != allRegionColors->end()) {
                neighborColor = it->second;
            } else {
                std::cout << "ERROR : neighborId not found in allRegionColors" << std::endl;
            }

            if (neighborColor[0] >= color_seuil_inf[0] && neighborColor[1] >= color_seuil_inf[1] && neighborColor[2] >= color_seuil_inf[2] &&
                neighborColor[0] <= color_seuil_sup[0] && neighborColor[1] <= color_seuil_sup[1] && neighborColor[2] <= color_seuil_sup[2]) {
                // Fusion logic
                (*allRegionColors)[id] = neighborColor;
            } else {
                // Regions cannot be fused
                std::cout << "ERROR: regions can't be fused" << std::endl;
            }
        } else { // On est dans le cas ou le point est dans la region courante
            if (tabInfo[point.x][point.y] == id) {
                std::cout << "il n y a rien a faire le point est deja dans la region" << std::endl;
            } 
            if (tabInfo[point.x][point.y] == -1 * id) {
                std::cout << "il n y a rien a faire le point est deja traite et il ne doit pas etre dans la region" << std::endl;
            }
        }
    }

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
        return verifyColor(r.color); // A REDEFINIR
    };

    /**
     * Get the outline of the region
     * @return the outline of the region
    */
    std::queue<cv::Point> * getoutline() {
        // Display the outline
        std::queue<cv::Point> _outlines = *outline;
        // std::cout << "It's queue of outline =============" << _outlines.size() <<std::endl;
        // while (!_outlines.empty()) {
        //     cv::Point p = _outlines.front();
        //     std::cout<<"size " << _outlines.size();
        //     _outlines.pop();
        //     std::cout << "Point in outline " << p.x << "/" << p.y << std::endl;
        // }
        // std::cout << "End of queue of outline" << std::endl;
        return new std::queue<cv::Point>(*outline);
    };

    /**
     * Get the border of the region
     * @return the border of the region
    */
    std::vector<cv::Point> * getborder() const {
        return new std::vector<cv::Point>(*border);
    };

    /**
     * Set the outline of the region
    */
    void setoutline(std::queue<cv::Point> * _outline) {
        outline = _outline;
    };

    /**
     * Set the outline of the region
    */
    void setoutline(std::vector<cv::Point> * _outline) {
        outline = new std::queue<cv::Point>();
        for (int i = 0; i < _outline->size(); i++) {
            outline->push(_outline->at(i));
        }
    };

    /**
     * Set the border of the region
    */
    void setborder(std::vector<cv::Point> * _border) {
        delete border;
        border = _border;
    };

    /**
     * Clear the border of the region
    */
    void clearborder() {
        delete border;
        border = new std::vector<cv::Point>;
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
    cv::Vec3b getColor() const {
        return cv::Vec3b(color);
    };

    /**
     * Display the outline of image
     * @param title The title of the image
     * @param img to display the outline in an image
    */
    // void displayoutline(const std::string title, const cv::Mat & img) {
    //     cv::Mat * image_outline = new cv::Mat(img.size(), img.type());
    //     *image_outline = img.clone();
    //     // We put the outline in the image
    //     for (int i = 0; i < points->size(); i++) {
    //         image_outline->at<cv::Vec3b>(points->at(i)) = cv::Vec3b(255,1,1);
    //     }
    //     cv::imshow(title, *image_outline);
    //     cv::waitKey(0);
    //     delete image_outline;
    // }

    /**
     * For increase the seuil
    */
    void increaseThreshold() {
        std::cout << "Increase threshold" << std::endl;
        if (colors->size() < 50) {
            if (threshold + 10 < 255) {
                threshold += 10;
            } else {
                std::cout<< "FAUX" << std::endl;
                isIncrease = false;
            }
        } else {
            if (coefSD * 1.5 < 4) {
                coefSD *= 1.5;
            } else {
                std::cout<< "FAUX" << std::endl;
                isIncrease = false;
            }
        }
        averageColorSeuil();
    }

    /**
     * Get the isIncrease
    */
    bool getIsIncrease() {
        return isIncrease;
    }

    cv::Vec3b getColorById(int id) const {
        auto it = allRegionColors->find(id);
        if (it != allRegionColors->end()) {
            return it->second;
        } else {
            // Gérer le cas où l'ID de la région n'est pas dans allRegionColors
            // Vous pouvez retourner une couleur par défaut ou lever une exception, selon votre choix
            std::cerr << "Erreur : ID de région introuvable dans allRegionColors" << std::endl;
            return cv::Vec3b(0, 0, 0); // Couleur par défaut
        }
    }

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
    std::vector<cv::Point> * border; // Content the points border of the region, this points are traited
    std::vector<cv::Vec3b> * colors; // Content the colors of the region
    int threshold;
    float coefSD;
    bool isIncrease;

    std::unordered_map<int, cv::Vec3b> * allRegionColors; // cela permet a toutes les regions de savoir les couleurs des autres regions
    
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
        // std::cout << "Colors size " << colors->size() << std::endl;
        if (colors->size() > 50) {
            // std::cout << "Seuil dynamic" << std::endl;
            // We calculate the standard deviation of the colors
            for (int i = 0; i < colors->size(); i++) {
                r += pow(colors->at(i)[0] - color[0], 2);
                g += pow(colors->at(i)[1] - color[1], 2);
                b += pow(colors->at(i)[2] - color[2], 2);
            }
            r /= colors->size();
            g /= colors->size();
            b /= colors->size();
            r = sqrt(r) * coefSD;
            g = sqrt(g) * coefSD;
            b = sqrt(b) * coefSD;
            // Display the standard deviation
            // std::cout << "Dynamic seuil ecart" << std::endl;
            // std::cout << "R : " << r << std::endl;
            // std::cout << "G : " << g << std::endl;
            // std::cout << "B : " << b << std::endl;
            color_seuil_inf = cv::Vec3b(
                (color[0] - r > 0)? color[0] - r:0,
                (color[1] - g > 0)? color[1] - g:0,
                (color[2] - b > 0)? color[2] - b:0);
            color_seuil_sup = cv::Vec3b(
                (color[0] + r > 255)? 255:color[0] + r,
                (color[1] + g > 255)? 255:color[1] + g,
                (color[2] + b > 255)? 255:color[2] + b);
            // std::cout << "Dynamic static" << std::endl;
            // std::cout << "R : " <<(int)color_seuil_inf[0] << "/" <<(int) color_seuil_sup[0] << std::endl;
            // std::cout << "G : " <<(int)color_seuil_inf[1] << "/" <<(int) color_seuil_sup[1] << std::endl;
            // std::cout << "B : " <<(int)color_seuil_inf[2] << "/" <<(int) color_seuil_sup[2] << std::endl;
            // Display new seuil
            // std::cout << "Seuil inf" << std::endl;
            // displayColor(color_seuil_inf);
            // std::cout << "Seuil sup" << std::endl;
            // displayColor(color_seuil_sup);
        } else {
            // std::cout << "Seuil static" << std::endl;
            color_seuil_sup[0] = (static_cast<int>(color[0]) + threshold < 255) ? static_cast<int>(color[0]) + threshold : 255;
            color_seuil_sup[1] = (static_cast<int>(color[1]) + threshold < 255) ? static_cast<int>(color[1]) + threshold : 255;
            color_seuil_sup[2] = (static_cast<int>(color[2]) + threshold < 255) ? static_cast<int>(color[2]) + threshold : 255;
            color_seuil_inf[0] = (static_cast<int>(color[0]) - threshold > 0) ? static_cast<int>(color[0]) - threshold : 0;
            color_seuil_inf[1] = (static_cast<int>(color[1]) - threshold > 0) ? static_cast<int>(color[1]) - threshold : 0;
            color_seuil_inf[2] = (static_cast<int>(color[2]) - threshold > 0) ? static_cast<int>(color[2]) - threshold : 0;
            // Display new seuil
            // std::cout << "Seuil static" << std::endl;
            // std::cout << "R : " <<(int)color_seuil_inf[0] << "/" <<(int) color_seuil_sup[0] << std::endl;
            // std::cout << "G : " <<(int)color_seuil_inf[1] << "/" <<(int) color_seuil_sup[1] << std::endl;
            // std::cout << "B : " <<(int)color_seuil_inf[2] << "/" <<(int) color_seuil_sup[2] << std::endl;
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
     * This function is used to verify a point is in the outline of the region
     * @param p The point to verify
    */
    bool verifyoutline(cv::Point p) {
        // We verify if the point is not in the region
        if (tabInfo[p.x] [p.y] == id) {
            // We verify if the point is in the outline of the region
            
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
        return (p.x >= 0 && p.x < size_x && p.y >= 0 && p.y < size_y && tabInfo[p.x][p.y] != id * -1 && tabInfo[p.x][p.y] != id);
    }

    /**
     * To update outline of the region
     * @param p The point add to the region
    */
    void updateoutline(cv::Point p) {
        // We parcour the 4 points around the point,
        // if a point is a new outline, we add it to the outline
        if (verifyPoint(cv::Point(p.x + 1, p.y))) {
            // std::cout << "Point add to outline " << p.x + 1 << "/" << p.y << std::endl;
            outline->push(cv::Point(p.x + 1, p.y));
        }
        if (verifyPoint(cv::Point(p.x - 1, p.y))) {
            // std::cout << "Point add to outline " << p.x - 1 << "/" << p.y << std::endl;
            outline->push(cv::Point(p.x - 1, p.y));
        }
        if (verifyPoint(cv::Point(p.x, p.y + 1))) {
            // std::cout << "Point add to outline " << p.x << "/" << p.y + 1 << std::endl;
            outline->push(cv::Point(p.x, p.y + 1));
        }
        if (verifyPoint(cv::Point(p.x, p.y - 1))) {
            // std::cout << "Point add to outline " << p.x << "/" << p.y - 1 << std::endl;
            outline->push(cv::Point(p.x, p.y - 1));
        }
        // std::cout<<"We are passed here"<<std::endl;
        // Display the outline
        std::queue<cv::Point> _outlines = *outline;
        // std::cout << "It's queue of outline " << _outlines.size() <<std::endl;
        while (!_outlines.empty()) {
            cv::Point p = _outlines.front();
            // std::cout<<"size " << _outlines.size();
            _outlines.pop();
            // std::cout << "Point in outline " << p.x << "/" << p.y << std::endl;
        }
        // std::cout << "End of queue of outline" << std::endl;
    }
};

#endif // REGION_HPP