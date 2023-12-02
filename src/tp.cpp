#include <opencv2/opencv.hpp>
#include <iostream>
#include "fonctions.hpp"

int main() {
    std::string image_path = "Images/lena.png";
    cv::Mat image = cv::imread(image_path);

    // Vérifier si l'image a été chargée avec succès
    if (!image.empty()) {
        // Créer une fenêtre pour afficher l'image
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

        // On agrandit la fenêtre pour voir l'histogramme
        // cv::namedWindow("Image Originale", cv::WINDOW_NORMAL);
        
        cv::imshow("Image Originale", image);
        cv::Mat hist;

        comparaisonHist(image, hist);
        
        comparasonEtirement(image, hist);
        
        comparaisonEgalisation(image);
        
        comparaisonConvolution(image);

        // On attend que l'utilisateur appuie sur une touche pour quitter
        cv::waitKey(0);
        // On ferme toutes les fenêtres
        cv::destroyAllWindows();
    } else {
        std::cout << "Erreur de chargement de l'image." << std::endl;
    }

    return 0;
}
