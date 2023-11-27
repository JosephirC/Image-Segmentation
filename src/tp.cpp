#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

// La fonction cv::calcHist() de 0 en c++
void monCalcHist(const cv::Mat& image, cv::Mat& hist) {
    // Assurez-vous que l'image est en niveaux de gris
    if (image.channels() != 1) {
        std::cerr << "L'image doit être en niveaux de gris." << std::endl;
        return;
    }

    // Nombre de bins dans l'histogramme
    int histSize = 256;

    // Calculer l'histogramme
    hist = cv::Mat::zeros(1, histSize, CV_32F);

    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            // Trouver l'intensité du pixel (i, j) et incrémenter le compartiment correspondant
            int intensity = static_cast<int>(image.at<uchar>(i, j));
            hist.at<float>(0, intensity) += 1.0;
        }
    }
}

void equalizeHist(const cv::Mat& image, cv::Mat& resultat) {
    // Calculer l'histogramme de l'image
    cv::Mat hist;
    int channels[] = {0}; // Utiliser le canal 0 (niveaux de gris) pour l'histogramme
    int bins = 256; // Nombre de compartiments dans l'histogramme
    int histSize[] = {bins};
    float range[] = {0, 256}; // La plage de valeurs pour le niveau de gris
    const float* ranges[] = {range};
    
    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

    // Calculer l'histogramme cumulé
    cv::Mat histCumule;
    calculerHistogrammeCumule(hist, histCumule);

    // Calculer la dynamique
    int dynamique = 256; // Vous pouvez ajuster cela en fonction de vos besoins

    // Appliquer la transformation
    resultat = cv::Mat(image.size(), image.type());
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            int intensite = static_cast<int>(image.at<uchar>(i, j));
            resultat.at<uchar>(i, j) = static_cast<uchar>((pow(2, dynamique) - 1) * histCumule.at<float>(intensite) / (image.rows * image.cols));
        }
    }
}
void etirerHistogramme(const cv::Mat& image, cv::Mat& imageEtiree, int newMin, int newMax) {

    // Assurez-vous que l'image est en niveaux de gris
    if (image.channels() != 1) {
        std::cerr << "L'image doit être en niveaux de gris." << std::endl;
        return;
    }

    // On crée une image vide pour stocker le résultat
    imageEtiree = cv::Mat::zeros(image.size(), CV_8U);

    // On trouver les valeurs minimales et maximales de l'image d'entrée
    double minVal, maxVal;
    cv::minMaxLoc(image, &minVal, &maxVal);

    // On calculer l'écart entre les valeurs minimales et maximales dans l'image de sortie
    double newRange = newMax - newMin;

    // On parcourir l'image et appliquer la transformation d'étirement
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            int pixelValue = static_cast<int>(image.at<uchar>(i, j));

            // On applique la transformation d'étirement avec la formule vue en classe
            int newPixelValue = static_cast<int>((newRange * (pixelValue - minVal) / (maxVal - minVal)) + newMin);

            // On mettre à jour la valeur du pixel dans l'image de sortie
            imageEtiree.at<uchar>(i, j) = static_cast<uchar>(newPixelValue);
        }
    }
}

void normalizeHist(const cv::Mat& hist, cv::Mat& normalizedHist, int targetHeight) {
    // Trouver la valeur maximale de l'histogramme pour l'échelle
    double maxVal;
    // On ne garde que la valeur maximal.
    cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, nullptr);

    // On crait une matrice pour l'histogramme normalisé
    normalizedHist = cv::Mat::zeros(1, hist.cols, CV_32F);

    // On parcour l'histogramme
    for (int i = 0; i < hist.cols; ++i) {
        // Et on normalise chaque compartiment
        normalizedHist.at<float>(0, i) = hist.at<float>(0, i) * targetHeight / maxVal;
    }
}

void afficherHistogramme(const std::string titre, const cv::Mat& hist) {
    // Dessiner l'histogramme
    int histSize = hist.cols;
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

    // Normaliser l'histogramme avec la fonction personnalisée
    cv::Mat normalizedHist;
    normalizeHist(hist, normalizedHist, hist_h);

    // Dessiner les compartiments de l'histogramme normalisé
    for (int i = 1; i < histSize; i++) {
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(normalizedHist.at<float>(0, i - 1))),
                    cv::Point(bin_w * (i), hist_h - cvRound(normalizedHist.at<float>(0, i))),
                    cv::Scalar(0, 0, 0), 2, 8, 0);
    }

    // Afficher l'histogramme
    cv::imshow(titre, histImage);
    // cv::waitKey(0);
}

// Fonction pour dessiner un histogramme avec les fonctions de openCV
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
        // On agrandit la fenêtre pour voir l'histogramme
        cv::namedWindow("Image", cv::WINDOW_NORMAL);
        cv::imshow("Image", image);

        // On calcule l'histogramme de l'image avec openCV
        HistogrammeGris(image);

        // On cré nous même l'histogramme
        cv::Mat hist;
        monCalcHist(image, hist);
        // Et on l'affiche pour comparer avec open cv
        afficherHistogramme("Histogramme fait nous meme", hist);

        // On étire l'histogramme
        cv::Mat imageEtiree;
        etirerHistogramme(image, imageEtiree, 200, 255);

        // On met en gris l'image étirée
        cv::cvtColor(imageEtiree, imageEtiree, cv::COLOR_GRAY2BGR);
        // On affiche l'image étirée
        cv::imshow("Image Etiree", imageEtiree);


        cv::Mat histEtiree;

        // On met en gris l'image étirée
        cv::cvtColor(imageEtiree, imageEtiree, cv::COLOR_BGR2GRAY);
        // On calcule l'histogramme de l'image étirée
        monCalcHist(imageEtiree, histEtiree);
        // Et on l'affiche 
        afficherHistogramme("Histogramme etire", histEtiree);


        // On attend que l'utilisateur appuie sur une touche pour quitter
        cv::waitKey(0);
        // On ferme toutes les fenêtres
        cv::destroyAllWindows();
    } else {
        std::cout << "Erreur de chargement de l'image." << std::endl;
    }

    return 0;
}