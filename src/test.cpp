#include <opencv2/opencv.hpp>
#include <iostream>

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
    cv::namedWindow("Histogramme", cv::WINDOW_NORMAL);
    cv::imshow("Histogramme", histImage);

}

void RefactorHistogramme(cv::Mat & image) {
    // Calculer l'histogramme de l'image
    cv::Mat hist;
    int channels[] = {0}; // Utiliser le canal 0 (niveaux de gris) pour l'histogramme
    int bins = 256; // Nombre de compartiments dans l'histogramme
    int histSize[] = {bins};
    float range[] = {0, 256}; // La plage de valeurs pour le niveau de gris
    const float* ranges[] = {range};
}

void calculateHistogram(const cv::Mat& image) {
    // Initialiser l'histogramme avec des zéros
    int hist[256] = {0};

    // Parcourir l'image et incrémenter les valeurs de l'histogramme
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            int pixel_value = static_cast<int>(image.at<uchar>(y, x));
            hist[pixel_value]++;
        }
    }

    // Trouver la valeur maximale dans l'histogramme pour la normalisation
    int max_value = *std::max_element(hist, hist + 256);

    // Créer une image pour afficher l'histogramme
    int hist_w = 512;
    int hist_h = 400;
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    // Dessiner les compartiments de l'histogramme
    for (int i = 0; i < 256; i++) {
        int bin_height = cvRound(static_cast<double>(hist[i]) / max_value * hist_h);
        cv::line(histImage, cv::Point(i, hist_h - bin_height), cv::Point(i, hist_h),
                 cv::Scalar(255, 255, 255), 1, 8, 0);
    }

    // Afficher l'histogramme
    cv::namedWindow("Histogramme2", cv::WINDOW_NORMAL);
    cv::imshow("Histogramme2", histImage);
    cv::waitKey(0);
}

void calculateNormalizedColorHistogram(const cv::Mat& image) {
    // Diviser l'image en canaux de couleur
    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    // Nombre de compartiments dans l'histogramme
    int bins = 256;

    // Créer une image pour afficher l'histogramme
    int hist_w = 512;
    int hist_h = 400;
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    // Parcourir chaque canal de couleur
    for (int channel = 0; channel < 3; ++channel) {
        // Initialiser l'histogramme avec des zéros
        int hist[256] = {0};

        // Parcourir l'image et incrémenter les valeurs de l'histogramme pour le canal actuel
        for (int y = 0; y < image.rows; ++y) {
            for (int x = 0; x < image.cols; ++x) {
                int pixel_value = static_cast<int>(channels[channel].at<uchar>(y, x));
                hist[pixel_value]++;
            }
        }

        // Trouver la valeur maximale dans l'histogramme pour normaliser
        int max_value = *std::max_element(hist, hist + 256);

        // Dessiner les compartiments de l'histogramme pour le canal actuel
        for (int i = 0; i < 256; i++) {
            int bin_height = cvRound(static_cast<double>(hist[i]) / max_value * hist_h);
            // Choisir une couleur différente pour chaque canal
            cv::Scalar color;
            if (channel == 0) color = cv::Scalar(255, 0, 0); // Blue
            else if (channel == 1) color = cv::Scalar(0, 255, 0); // Green
            else if (channel == 2) color = cv::Scalar(0, 0, 255); // Red

            // Dessiner une barre verticale pour chaque canal
            cv::line(histImage, cv::Point(i, hist_h - bin_height), cv::Point(i, hist_h),
                     color, 1, 8, 0);
        }
    }

    // Afficher l'histogramme
    cv::namedWindow("Histogramme Couleur Normalisé", cv::WINDOW_NORMAL);
    cv::imshow("Histogramme Couleur Normalisé", histImage);
    cv::waitKey(0);
}

void calculateUnnormalizedColorHistogram(const cv::Mat& image) {
    // Diviser l'image en canaux de couleur
    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    // Créer une image pour afficher l'histogramme
    int hist_w = 512;
    int hist_h = 400;
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    // Parcourir chaque canal de couleur
    for (int channel = 0; channel < 3; ++channel) {
        // Initialiser l'histogramme avec des zéros
        int hist[256] = {0};

        // Parcourir l'image et incrémenter les valeurs de l'histogramme pour le canal actuel
        for (int y = 0; y < image.rows; ++y) {
            for (int x = 0; x < image.cols; ++x) {
                int pixel_value = static_cast<int>(channels[channel].at<uchar>(y, x));
                hist[pixel_value]++;
            }
        }

        // Dessiner les compartiments de l'histogramme pour le canal actuel
        for (int i = 0; i < 256; i++) {
            int bin_height = hist[i];

            // Choisir une couleur différente pour chaque canal
            cv::Scalar color;
            if (channel == 0) {
                color = cv::Scalar(255, 0, 0); // Blue
            } 
            else if (channel == 1) {
                color = cv::Scalar(0, 255, 0); // Green
            } 
            else if (channel == 2) {
                color = cv::Scalar(0, 0, 255); // Red
            } 

            // Dessiner une barre verticale pour chaque canal
            cv::line(histImage, cv::Point(i, hist_h - bin_height), cv::Point(i, hist_h),
                     color, 1, 8, 0);
        }
    }

    // Afficher l'histogramme non normalisé en couleur
    cv::namedWindow("Histogramme Couleur Non Normalisé", cv::WINDOW_NORMAL);
    cv::imshow("Histogramme Couleur Non Normalisé", histImage);
    cv::waitKey(0);
}

int main() {
    std::string image_path = "Images/lena.png";
    cv::Mat image = cv::imread(image_path);

    // Vérifier si l'image a été chargée avec succès
    if (!image.empty()) {
        // Créer une fenêtre pour afficher l'image
        cv::namedWindow("Image", cv::WINDOW_NORMAL);
        cv::imshow("Image", image);

        HistogrammeGris(image);
        //calculateHistogram(image);
        //calculateNormalizedColorHistogram(image);
        // calculateUnnormalizedColorHistogram(image);
        
        cv::waitKey(0);
        cv::destroyAllWindows();
    } else {
        std::cout << "Erreur de chargement de l'image." << std::endl;
    }

    return 0;
}

/*
int main() {
    // Charger une image à partir du fichier
    cv::Mat image = cv::imread("../Images/lena.png");

    // Vérifier si l'image a été correctement chargée
    if (image.empty()) {
        std::cerr << "Impossible de charger l'image." << std::endl;
        return -1;
    }

    // Convertir l'image en niveaux de gris (si elle ne l'est pas déjà)
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

    // Calculer l'histogramme
    int histSize = 256; // Nombre de bins
    float range[] = {0, 256}; // Range de valeurs
    const float *histRange = {range};
    cv::Mat hist;

    cv::calcHist(&image, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

    // Créer une image pour afficher l'histogramme
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

    // Normaliser l'histogramme
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // Dessiner l'histogramme
    for (int i = 1; i < histSize; i++) {
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
                 cv::Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
                 cv::Scalar(0, 0, 255), 2, 8, 0);
    }

    // Afficher l'image et l'histogramme
    cv::imshow("Image chargée", image);
    cv::imshow("Histogramme", histImage);
    cv::waitKey(0);

    return 0;
}
*/