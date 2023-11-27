#include <opencv2/opencv.hpp>
#include <iostream>

// La fonction cv::calcHist() de 0 en c++
void monCalcHist(const cv::Mat& image, cv::Mat& hist) {
    // Assurez-vous que l'image est en niveaux de gris
    if (image.channels() != 1) {
        std::cerr << "L'image doit être en niveaux de gris." << std::endl;
        return;
    }

    // Nombre de bins dans l'histogramme
    int histSize = 256;

    // Indicateur pour accumuler les valeurs
    bool accumulate = false;

    // Calculer l'histogramme
    hist = cv::Mat::zeros(1, histSize, CV_32F);

    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            int intensity = static_cast<int>(image.at<uchar>(i, j));
            hist.at<float>(0, intensity) += 1.0;
        }
    }

    // Normaliser l'histogramme si nécessaire
    if (!accumulate) {
        hist /= static_cast<float>(image.total());
    }
}

void normalizeHistGris(const cv::Mat& hist, cv::Mat& normalizedHist, int targetHeight) {
    // Trouver la valeur maximale de l'histogramme pour l'échelle
    double maxVal;
    cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, nullptr);

    // Créer une matrice pour l'histogramme normalisé
    normalizedHist = cv::Mat::zeros(1, hist.cols, CV_32F);

    // Normaliser l'histogramme
    for (int i = 0; i < hist.cols; ++i) {
        normalizedHist.at<float>(0, i) = hist.at<float>(0, i) * targetHeight / maxVal;
    }
}

void afficherHistogramme(const cv::Mat& hist) {
    // Dessiner l'histogramme
    int histSize = hist.cols;
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

    // Normaliser l'histogramme avec la fonction personnalisée
    cv::Mat normalizedHist;
    normalizeHistGris(hist, normalizedHist, hist_h);

    // Dessiner les compartiments de l'histogramme normalisé
    for (int i = 1; i < histSize; i++) {
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(normalizedHist.at<float>(0, i - 1))),
                    cv::Point(bin_w * (i), hist_h - cvRound(normalizedHist.at<float>(0, i))),
                    cv::Scalar(0, 0, 0), 2, 8, 0);
    }

    // Afficher l'histogramme
    cv::imshow("Histogramme Moi", histImage);
    cv::waitKey(0);
}

void HistogrammeGris(cv::Mat & image) {
    // Calculer l'histogramme de l'image
    cv::Mat hist;
    int channels[] = {0}; // Utiliser le canal 0 (niveaux de gris) pour l'histogramme
    int bins = 256; // Nombre de compartiments dans l'histogramme
    int histSize[] = {bins};
    float range[] = {0, 256}; // La plage de valeurs pour le niveau de gris
    const float* ranges[] = {range};
    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

    // Dessiner l'histogramme
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / bins);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

    // Normaliser l'histogramme pour qu'il rentre dans l'image
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // Dessiner les compartiments de l'histogramme
    for (int i = 1; i < bins; i++) {
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
                    cv::Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
                    cv::Scalar(0, 0, 0), 2, 8, 0);
    }

    // Afficher l'histogramme
    cv::imshow("Histogramme Gris", histImage);
}

int main() {
    std::string image_path = "Images/lena.png";
    cv::Mat image = cv::imread(image_path);

    // Vérifier si l'image a été chargée avec succès
    if (!image.empty()) {
        // Créer une fenêtre pour afficher l'image
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
        cv::namedWindow("Image", cv::WINDOW_NORMAL);
        cv::imshow("Image", image);

        HistogrammeGris(image);

        cv::Mat hist;
        monCalcHist(image, hist);
        afficherHistogramme(hist);
        
        cv::waitKey(0);
        cv::destroyAllWindows();
    } else {
        std::cout << "Erreur de chargement de l'image." << std::endl;
    }

    return 0;
}