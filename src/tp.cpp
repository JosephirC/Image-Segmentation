#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>


void calculerHistogrammeCumule(const cv::Mat& hist, cv::Mat& histCumule) {
    // Assurez-vous que l'histogramme est en niveaux de gris
    if (hist.channels() != 1) {
        std::cerr << "L'histogramme doit être en niveaux de gris." << std::endl;
        return;
    }

    int histSize = hist.cols;

    // On crée une matrice pour l'histogramme cumulé
    histCumule = cv::Mat::zeros(1, histSize, CV_32F);

    // On Initialiser le premier élément de l'histogramme cumulé
    histCumule.at<float>(0, 0) = hist.at<float>(0, 0);

    // On calcule le reste de l'histogramme cumulé
    for (int i = 1; i < histSize; ++i) {
        histCumule.at<float>(0, i) = histCumule.at<float>(0, i - 1) + hist.at<float>(0, i);
    }
}




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


void imgToHistoCumul(const cv::Mat& image, cv::Mat& hist) {
    monCalcHist(image, hist);
    calculerHistogrammeCumule(hist, hist);
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



void normalizeHistGris(const cv::Mat& hist, cv::Mat& normalizedHist, int targetHeight) {
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
    normalizeHistGris(hist, normalizedHist, hist_h);

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

void egaliserHistogramme(const cv::Mat& image, cv::Mat& imageEgalisee) {
    // On s'assurez que l'image est en niveaux de gris
    if (image.channels() != 1) {
        std::cerr << "L'image doit être en niveaux de gris." << std::endl;
        return;
    }

    // On calculer l'histogramme cumulé
    cv::Mat hist;
    monCalcHist(image, hist);
    cv::Mat histCumule;
    calculerHistogrammeCumule(hist, histCumule);

    // On trouver la valeur minimale et maximale de l'histogramme cumulé
    double minHistCumule, maxHistCumule;
    cv::minMaxLoc(histCumule, &minHistCumule, &maxHistCumule);

    // On calcule la dynamique de l'image
    int dynamique = static_cast<int>(maxHistCumule - minHistCumule);

    // On applique la transformation d'égalisation d'histogramme

    // On crée une image vide pour stocker le résultat
    imageEgalisee = cv::Mat::zeros(image.size(), CV_8U);

    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            // On trouve l'intensité du pixel (i, j)
            int pixelValue = static_cast<int>(image.at<uchar>(i, j));
            // On calculer la nouvelle valeur du pixel après égalisation
            int nouvelleValeur = static_cast<int>((dynamique * (histCumule.at<float>(0, pixelValue) - minHistCumule) / (image.rows * image.cols - 1)));
            // On met à jour la valeur du pixel dans l'image égalisée
            imageEgalisee.at<uchar>(i, j) = static_cast<uchar>(nouvelleValeur);
        }
    }
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

        // On calcule l'histogramme cumulé
        cv::Mat histCumule;
        calculerHistogrammeCumule(hist, histCumule);
        // On affiche l'histogramme cumulé 
        afficherHistogramme("Histogramme cumule", histCumule);

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

        cv::Mat imageEgalisee;
        egaliserHistogramme(image, imageEgalisee);
    
        // On met en "couleur" l'image égalisée
        cv::cvtColor(imageEgalisee, imageEgalisee, cv::COLOR_GRAY2BGR);
        // On affiche l'image égalisée
        cv::imshow("Image Egalisee", imageEgalisee);


        // On attend que l'utilisateur appuie sur une touche pour quitter
        cv::waitKey(0);
        // On ferme toutes les fenêtres
        cv::destroyAllWindows();
    } else {
        std::cout << "Erreur de chargement de l'image." << std::endl;
    }

    return 0;
}