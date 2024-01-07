#include "Region.hpp"
#include <opencv2/opencv.hpp>

/****** Function not in class ******/
float computeCoefficient(int regionSize) {
    return 2;

    float a = 3.0;
    float b = 0.0001;
    float c = 1.;

    return a * exp(-b * regionSize) + c;
}

/***** Friend functions *****/ 
namespace std {
    template <>
    struct hash<cv::Point> {
        size_t operator()(const cv::Point& p) const {
            size_t hash_x = hash<int>()(p.x);
            size_t hash_y = hash<int>()(p.y);
            // Combinaison of hash_x and hash_y
            return hash_x ^ (hash_y + 0x9e3779b9 + (hash_x << 6) + (hash_x >> 2));
        }
    };
}


bool operator==(const cv::Point& a, const cv::Point& b) {
    return a.x == b.x && a.y == b.y;
}


/***** Public functions *****/ 

// This function is used to display a color
void displayColor (const cv::Vec3b& couleur) {
    std::cout << "Canal Bleu : " << static_cast<int>(couleur[0]) << std::endl;
    std::cout << "Canal Vert : " << static_cast<int>(couleur[1]) << std::endl;
    std::cout << "Canal Rouge : " << static_cast<int>(couleur[2]) << std::endl;
}

Region::Region() {
    tabInfo = nullptr;
    outline = new std::queue<cv::Point>();
    border = new std::unordered_set<cv::Point>();
    colors = new std::vector<cv::Vec3b>();
    color = cv::Vec3b(0,0,0);

    // allRegionColors = new std::unordered_map<int, cv::Vec3b>();
}

Region::Region(int _id ,cv::Point p, int ** tabShare, cv::Mat * imageOriginal, int _threshold, float _coefSD):
                id(_id),
                size_x(imageOriginal->cols),
                size_y(imageOriginal->rows),
                tabInfo(tabShare),
                image(imageOriginal),
                color(imageOriginal->at<cv::Vec3b>(p)), 
                outline(new std::queue<cv::Point>),
                border(new std::unordered_set<cv::Point>),
                colors(new std::vector<cv::Vec3b>),
                // allRegionColors(new std::unordered_map<int, cv::Vec3b>),
                threshold(_threshold),
                coefSD(_coefSD),
                isIncrease(false) {
    // std::cout << "Region constructor" << std::endl;
    colors->push_back(color);
    averageColorSeuil();
    // We add the first point to the region
    outline->push(p);
    // std::cout << "We add the first point to the region" << std::endl;
    grow();
}

Region::Region(const Region& r) {
    // We copy basic data
    id = r.id;
    size_x = r.size_x;
    size_y = r.size_y;
    threshold = r.threshold;
    coefSD = r.coefSD;
    isIncrease = r.isIncrease;
    color = r.color;
    color_seuil_inf = r.color_seuil_inf;
    color_seuil_sup = r.color_seuil_sup;

    // We keep same tabInfo and image
    this->tabInfo = r.tabInfo;
    image = r.image;

    // We copy the image, the outline, the border and the colors
    outline = new std::queue<cv::Point>(*r.outline);
    border = new std::unordered_set<cv::Point>(*r.border);
    colors = new std::vector<cv::Vec3b>(*r.colors);
}

Region::~Region(){
   if (outline != nullptr) {
        delete outline;
    }
    outline = nullptr;
    if (border != nullptr) {
        delete border;
    }
    border = nullptr;
    if (colors != nullptr) {
        delete colors;
    }
    colors = nullptr;
}

Region & Region::operator=(const Region& r) {
    if (this != &r) {
        //delete vecInfo;
        delete outline;
        delete colors;
        delete border;

        // delete allRegionColors;
        // allRegionColors = new std::unordered_map<int, cv::Vec3b>(*r.allRegionColors);

        tabInfo = r.tabInfo;
        outline = new std::queue<cv::Point>();
        colors = new std::vector<cv::Vec3b>();
        border = new std::unordered_set<cv::Point>();
        color = r.color;
        color_seuil_inf = r.color_seuil_inf;
        color_seuil_sup = r.color_seuil_sup;
    }
    return *this;
}

void Region::grow() {
    std::queue<cv::Point> _outlines = *outline;
    delete outline;
    outline = new std::queue<cv::Point>();
    // We get all Point in queue
    while (!_outlines.empty()) {
        cv::Point p = _outlines.front();

        _outlines.pop();
        cv::Vec3b col = image->at<cv::Vec3b>(p);
        // We verify if the point is in the image
        if (p.x < 0 || p.x >= image->cols || p.y < 0 || p.y >= image->rows) {
            std::cout << "Point not in the image " << image->cols << " / " << image->rows << std::endl;
            std::cout <<p.x<<"/ "<<p.y << std::endl;
        }
        // We verify if the point is not in an region
        if (tabInfo [p.x] [p.y] <= 0) {
            if (verifyColor(col)) {
                // If yes we add the point to the region
                tabInfo [p.x] [p.y] = id;
                colors->push_back(col);
                // We update the average color of the region
                averageColor();
                averageColorSeuil();
                // And we update the outline of the region
                updateoutline(p);
            } else {
                tabInfo [p.x] [p.y] = -1 * id;
                // We check if the point is not already in the border of the region
                // if (std::find(border->begin(), border->end(), p) == border->end()) {
                    border->insert(p);
                // }
            }
        } else {
            if (tabInfo [p.x] [p.y] != id) {
                // We check if the point is alredy in the border of the region
                if (std::find(border->begin(), border->end(), p) == border->end()) {
                    border->insert(p);
                }
            }
        }
    }
    // std::cout << "END grow" << std::endl;
}


bool Region::verifyFusion2 (const cv::Vec3b & col) {
    this->coefSD = computeCoefficient(colors->size());
    this->threshold = 30;
    this->averageColorSeuil();
    // We verify if the two regions have the same color with the seuil
    return verifyColor(col);
}

bool Region::verifyFusion (Region& r) {
    // We verify if the two regions in the same image
    if (size_x != r.size_x || size_y != r.size_y) {
        std::cout << "The two regions are not in the same image" << std::endl;
        return false;
    }
    this->coefSD = computeCoefficient(colors->size());
    this->threshold = 30;
    this->averageColorSeuil();
    // We verify if the two regions have the same color with the seuil
    return verifyColor(r.color) || r.verifyFusion2(color);
}

void Region::addPoint(cv::Point p) {
    // We verify if the point is in the image
    if (p.x < 0 || p.x >= image->cols || p.y < 0 || p.y >= image->rows) {
        std::cout << "Point not in the image " << image->cols << " / " << image->rows << std::endl;
        std::cout <<p.x<<"/ "<<p.y << std::endl;
    }
    // We get color of point
    cv::Vec3b col = image->at<cv::Vec3b>(p);
    tabInfo [p.x] [p.y] = id;
    colors->push_back(col);
    // We update the average color of the region
    averageColor();
    averageColorSeuil();
    // And we update the border of the region
    removePointInBorder(p);
}

bool Region::removePoint(cv::Point p) {
    // We verify if the point is in the image
    if (p.x < 0 || p.x >= image->cols || p.y < 0 || p.y >= image->rows) {
        std::cout << "Point not in the image " << image->cols << " / " << image->rows << std::endl;
        std::cout <<p.x<<"/ "<<p.y << std::endl;
    }
    // We get color of point
    cv::Vec3b col = image->at<cv::Vec3b>(p);
    auto it = std::find(colors->begin(), colors->end(), col);
    if (it != colors->end()) {
        colors->erase(std::find(colors->begin(), colors->end(), col));
    }
    return colors->empty();
}

std::queue<cv::Point> & Region::getoutline() const {
    assert(outline != nullptr);
    return *outline;
}

std::vector<cv::Point> & Region::getborderVector() const {
    std::vector<cv::Point> * vec_border = new std::vector<cv::Point>();
    for (const auto& element : *border) {
        vec_border->push_back(element);
    }
    return *vec_border;
}

std::unordered_set<cv::Point> & Region::getborder() const {
    return *border;
}

void Region::setoutline(std::queue<cv::Point>& _outline) {
    delete outline;
    *outline = _outline;
}

void Region::setborder(const std::unordered_set<cv::Point>& _border) {
    delete border;
    *border = _border;
}

void Region::removePointInBorder (cv::Point p) {
    if (border->find(p) != border->end()) {
        border->erase(p);
        return;
    }
    // std::cout << "The point " << p.x << "/" << p.y << " is not in the border of the region " << id << std::endl;
}

void Region::setoutline(const std::vector<cv::Point> & _outline) {
    delete outline;
    outline = new std::queue<cv::Point>();
    for (unsigned int i = 0; i < _outline.size(); i++) {
        outline->push(_outline.at(i));
    }
}

void Region::setoutline(const std::unordered_set<cv::Point> & _outline) {
    delete outline;
    outline = new std::queue<cv::Point>();
    for (const auto& element : _outline) {
        outline->push(element);
    }
}

void Region::clearborder() {
    delete border;
    border = new std::unordered_set<cv::Point>;
}

void Region::display(const std::string title, bool average) {
    if (average) {
        return;
    }
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
}

cv::Vec3b Region::getColor() const {
    return cv::Vec3b(color);
}

void Region::increaseThreshold() {
    // std::cout << "Increase threshold" << std::endl;
    if (colors->size() < 50) {
        if (threshold + 5 < 50) {
            threshold += 5;
        } else {
            // std::cout<< "FAUX" << std::endl;
            isIncrease = false;
        }
    } else {
        if (coefSD + 0.2 < 2.4) {
            coefSD += 0.2;
        } else {
            // std::cout<< "FAUX" << std::endl;
            isIncrease = false;
        }
    }
    averageColorSeuil();
}

bool Region::getIsIncrease() {
    return isIncrease;
}

int Region::getId() const {
    return id;
}

void Region::setId(const int _id) {
    id = _id;
}

void Region::operator+=(const Region & r2) {
    // We creat a new region
    // Region * new_region = new Region();
    // We remove in border the points in common
    std::unordered_set<cv::Point> * new_border = new std::unordered_set<cv::Point>();
    for (const auto& element : *(border)) {
        if (tabInfo [element.x] [element.y] != r2.getId()){
            new_border->insert(element);
        }
    }
    for (const auto& element : *(r2.border)) {
        if (abs(tabInfo [element.x] [element.y]) != id){
            new_border->insert(element);
        }
    }
    delete border;
    border = new_border;
    
    // We add all point in the outline of the two regions
    while (!r2.outline->empty()) {
        cv::Point p = r2.outline->front();
        r2.outline->pop();
        outline->push(p);
    }

    // We add the colors of the two regions
    colors->insert(colors->end(), r2.colors->begin(), r2.colors->end());
    // We calculate the average color of the new region
    averageColor();
    averageColorSeuil();
}

bool Region::operator<(const Region& other) const {
    return id < other.id;
}

bool Region::operator==(const Region& other) const {
    return id == other.id;
}

/***** Private functions *****/ 

void Region::averageColor(){
    int r = 0;
    int g = 0;
    int b = 0;
    for (unsigned int i = 0; i < colors->size(); i++) {
        r += colors->at(i)[0];
        g += colors->at(i)[1];
        b += colors->at(i)[2];
    }
    r /= colors->size();
    g /= colors->size();
    b /= colors->size();
    color = cv::Vec3b(r, g, b);
}

void Region::averageColorSeuil() {
    int r = 0;
    int g = 0;
    int b = 0;
    // std::cout << "Colors size " << colors->size() << std::endl;
    if (colors->size() > 50) {
        // std::cout << "Seuil dynamic" << std::endl;
        // We calculate the standard deviation of the colors
        for (unsigned int i = 0; i < colors->size(); i++) {
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
        color_seuil_inf = cv::Vec3b(
            (color[0] - r > 0)? color[0] - r:0,
            (color[1] - g > 0)? color[1] - g:0,
            (color[2] - b > 0)? color[2] - b:0);
        color_seuil_sup = cv::Vec3b(
            (color[0] + r > 255)? 255:color[0] + r,
            (color[1] + g > 255)? 255:color[1] + g,
            (color[2] + b > 255)? 255:color[2] + b);
    } else {
        // std::cout << "Seuil static" << std::endl;
        color_seuil_sup[0] = (static_cast<int>(color[0]) + threshold < 255) ? static_cast<int>(color[0]) + threshold : 255;
        color_seuil_sup[1] = (static_cast<int>(color[1]) + threshold < 255) ? static_cast<int>(color[1]) + threshold : 255;
        color_seuil_sup[2] = (static_cast<int>(color[2]) + threshold < 255) ? static_cast<int>(color[2]) + threshold : 255;
        color_seuil_inf[0] = (static_cast<int>(color[0]) - threshold > 0) ? static_cast<int>(color[0]) - threshold : 0;
        color_seuil_inf[1] = (static_cast<int>(color[1]) - threshold > 0) ? static_cast<int>(color[1]) - threshold : 0;
        color_seuil_inf[2] = (static_cast<int>(color[2]) - threshold > 0) ? static_cast<int>(color[2]) - threshold : 0;
    }
}

bool Region::verifyColor(cv::Vec3b col) const {
    return (col[0] >= color_seuil_inf[0] && col[0] <= color_seuil_sup[0])
        && (col[1] >= color_seuil_inf[1] && col[1] <= color_seuil_sup[1]) 
        && (col[2] >= color_seuil_inf[2] && col[2] <= color_seuil_sup[2]);
}

bool Region::verifyoutline(cv::Point p) const {
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

bool Region::verifyPoint(cv::Point p) const {
    if (p.x < 0 || p.x >= size_x || p.y < 0 || p.y >= size_y) {
        return false;
    } else if (tabInfo[p.x][p.y] > 0) {
        // if (tabInfo[p.x][p.y] != id && std::find(border->begin(), border->end(), p) == border->end()) {
            border->insert(p);
        //}
        return false;
    } else {
        return tabInfo[p.x][p.y] != id * -1;
    }
}

void Region::updateoutline(cv::Point p) {
    // std::cout << "In update outline" << std::endl;
    
    // We parcour the 4 points around the point,
    // if a point is a new outline, we add it to the outline
    if (verifyPoint(cv::Point(p.x + 1, p.y))) {
        // std::cout << "Point add to outline " << p.x + 1 << "/" << p.y << std::endl;
        outline->push(cv::Point(p.x + 1, p.y));
        tabInfo [p.x + 1] [p.y] = id * -1;
    }
    if (verifyPoint(cv::Point(p.x - 1, p.y))) {
        // std::cout << "Point add to outline " << p.x - 1 << "/" << p.y << std::endl;
        outline->push(cv::Point(p.x - 1, p.y));
        tabInfo [p.x - 1] [p.y] = id * -1;
    }
    if (verifyPoint(cv::Point(p.x, p.y + 1))) {
        // std::cout << "Point add to outline " << p.x << "/" << p.y + 1 << std::endl;
        outline->push(cv::Point(p.x, p.y + 1));
        tabInfo [p.x] [p.y + 1] = id * -1;
    }
    if (verifyPoint(cv::Point(p.x, p.y - 1))) {
        // std::cout << "Point add to outline " << p.x << "/" << p.y - 1 << std::endl;
        outline->push(cv::Point(p.x, p.y - 1));
        tabInfo [p.x] [p.y - 1] = id * -1;
    }
}
