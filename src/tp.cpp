#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

void minMaxIm(const cv::Mat& image, double& minVal, double& maxVal) {
    // On s'assure que l'image est en niveaux de gris
    if (image.channels() != 1) {
        std::cerr << "L'image doit être en niveaux de gris." << std::endl;
        return;
    }

    // On initialise les valeurs min et max
    minVal = std::numeric_limits<double>::max();
    maxVal = std::numeric_limits<double>::lowest();

    // On parcour l'image
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            // On trouve l'intensité du pixel (i, j)
            double intensite = static_cast<double>(image.at<uchar>(i, j));

            // On met à jour les valeurs min et max
            if (intensite < minVal) {
                minVal = intensite;
            }

            if (intensite > maxVal) {
                maxVal = intensite;
            }
        }
    }
}

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

void egalizeHistOpenCV(const cv::Mat& image, cv::Mat& newImage) {
    // Appliquer la transformation
    cv::equalizeHist(image, newImage);
}

void egaliseHist(const cv::Mat& image, cv::Mat& newImage) {
    // Calculer l'histogramme cumulé
    cv::Mat hist;
    monCalcHist(image, hist); // Utilisez votre fonction monCalcHist() ici

    cv::Mat histCumule;
    calculerHistogrammeCumule(hist, histCumule); // Utilisez votre fonction calculerHistogrammeCumule() ici

    // Nombre total de pixels dans l'image
    int totalPixels = image.rows * image.cols;

    // Calculer la transformation d'égalisation
    cv::Mat transform(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        transform.at<uchar>(0, i) = static_cast<uchar>((histCumule.at<float>(0, i) * 255.0) / totalPixels);
    }

    // Appliquer la transformation aux pixels de l'image
    newImage = image.clone(); // Copiez l'image originale pour y appliquer la transformation

    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            int pixelValue = static_cast<int>(image.at<uchar>(i, j));
            newImage.at<uchar>(i, j) = transform.at<uchar>(0, pixelValue);
        }
    }
}

void egalizeHistFormule(const cv::Mat& image, cv::Mat& resultat) {
    // On calcule l'histogramme de l'image
    cv::Mat hist;
    monCalcHist(image, hist);

    // On calcule l'histogramme cumulé
    cv::Mat histCumule;
    calculerHistogrammeCumule(hist, histCumule);

    // On trouve la valeur maximale de l'histogramme cumulé
    double maxHistCumule;
    double minHistCumule;
    minMaxIm(histCumule, minHistCumule, maxHistCumule);

    // cv::minMaxLoc(histCumule, &minHistCumule, &maxHistCumule, nullptr, nullptr);

    // Avec le min et le max on calcul la dynamqiue de l'image
    double dynamique = maxHistCumule - minHistCumule;

    // On applique la transformation d'égalisation
    resultat = cv::Mat(image.size(), image.type());
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            // On recupère l'intensité du pixel (i, j)
            int intensite = static_cast<int>(image.at<uchar>(i, j));

            // On appplique la formule d'égalisation mise à jour
            int nouvelleIntensite = static_cast<int>((pow(2, dynamique) - 1) * histCumule.at<float>(0, intensite) / (image.rows * image.cols));
            std::cout << "la fonction de cumule" << histCumule.at<float>(0, intensite) << std::endl;

            std::cout << "nouvelle instensité" << nouvelleIntensite << std::endl;

            // On met à jour la valeur du pixel dans l'image résultante
            resultat.at<uchar>(i, j) = static_cast<uchar>(nouvelleIntensite);
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
    minMaxIm(image, minVal, maxVal);
    // cv::minMaxLoc(image, &minVal, &maxVal);

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
    minMaxIm(hist, maxVal, maxVal);
    // cv::minMaxLoc(hist, nullptr, &maxVal, nullptr, nullptr);

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

// Fonction pour appliquer un filtre à une image
cv::Mat appliquerFiltre(const cv::Mat& image, const cv::Mat& filtre) {
    // On verifie si le filtre est de taille 3x3
    if (filtre.rows != 3 || filtre.cols != 3) {
        std::cerr << "Le filtre doit être de taille 3x3." << std::endl;
        return cv::Mat();
    }

    // On crée une image résultante
    cv::Mat resultat = cv::Mat::zeros(image.size(), image.type());

    // On applique le filtre par convolution
    for (int i = 1; i < image.rows - 1; ++i) {
        for (int j = 1; j < image.cols - 1; ++j) {
            double valeur = 0.0;

            // On applique la convolution avec le filtre 3x3
            for (int m = -1; m <= 1; ++m) {
                for (int n = -1; n <= 1; ++n) {
                    valeur += image.at<uchar>(i + m, j + n) * filtre.at<double>(m + 1, n + 1);
                }
            }

            // On met à jour la valeur dans l'image résultante
            resultat.at<uchar>(i, j) = static_cast<uchar>(valeur);
        }
    }

    return resultat;
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


        cv::Mat imageEqualiseeOpenCV;
        // On égalise l'histogramme avec openCV
        egalizeHistOpenCV(image, imageEqualiseeOpenCV);
        // On met en gris l'image égalisée
        cv::cvtColor(imageEqualiseeOpenCV, imageEqualiseeOpenCV, cv::COLOR_GRAY2BGR);
        // On affiche l'image égalisée
        cv::imshow("Image Equalisee OpenCV", imageEqualiseeOpenCV);
        
        // On applique notre fonction d'égalisation
        cv::Mat imageEgalisee;
        egaliseHist(image, imageEgalisee);
        // On met en gris l'image égalisée
        cv::cvtColor(imageEgalisee, imageEgalisee, cv::COLOR_GRAY2BGR);
        // On affiche l'image égalisée
        cv::imshow("Image Egalisee", imageEgalisee);


        // On applique notre fonction d'égalisation avec la formule
        cv::Mat imageEgaliseeFormule;
        egalizeHistFormule(image, imageEgaliseeFormule);
        // On met en gris l'image égalisée
        cv::cvtColor(imageEgaliseeFormule, imageEgaliseeFormule, cv::COLOR_GRAY2BGR);
        // On affiche l'image égalisée
        cv::imshow("Image Egalisee Formule", imageEgaliseeFormule);

        // On applique un filtre de détection de contours
        cv::Mat filtreContours = (cv::Mat_<double>(3, 3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
        // On applique le filtre
        cv::Mat imageContours = appliquerFiltre(image, filtreContours);
        // On met en "couleur" l'image des contours
        cv::cvtColor(imageContours, imageContours, cv::COLOR_GRAY2BGR);
        // On affiche l'image des contours
        cv::imshow("Image Contours", imageContours);

        // On applique un filtre masques (noyeux) a taille reduite
        cv::Mat filtreMasque = (cv::Mat_<double>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
        filtreMasque *= 9;
        // On applique le filtre
        cv::Mat imageMasque = appliquerFiltre(image, filtreMasque);
        // On met en "couleur" l'image des contours
        cv::cvtColor(imageMasque, imageMasque, cv::COLOR_GRAY2BGR);
        // On affiche l'image des contours
        cv::imshow("Image Masque", imageMasque);

        // On attend que l'utilisateur appuie sur une touche pour quitter
        cv::waitKey(0);
        // On ferme toutes les fenêtres
        cv::destroyAllWindows();
    } else {
        std::cout << "Erreur de chargement de l'image." << std::endl;
    }

    return 0;
}
