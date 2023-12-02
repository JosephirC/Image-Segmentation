# Rapport sur le Projet d'Analyse d'Image

## Introduction
Le projet d'analyse d'image vise à explorer et mettre en pratique les concepts fondamentaux du traitement d'images à l'aide de la bibliothèque OpenCV en langage C++. Ce rapport détaillera les différentes fonctionnalités implémentées, notamment le calcul d'histogrammes, la normalisation et l'étirement d'histogrammes, l'égalisation d'histogrammes, ainsi que l'application de filtres de convolution sur une image. Voici le lien de notre git : [via https](https://forge.univ-lyon1.fr/p2024398/m1-analyse-image.git), ou [via ssh](git@forge.univ-lyon1.fr:p2024398/m1-analyse-image.git)

## I. Histogrammes
### A. Calcul de l'histogramme
La fonction `monCalcHist` permet de calculer l'histogramme d'une image en niveaux de gris. Elle parcourt chaque pixel de l'image, incrémente le compartiment correspondant à l'intensité du pixel, et stocke les résultats dans une matrice.

### B. Normalisation d'histogramme
La fonction `normalizeHistGris` normalise l'histogramme en le mettant à l'échelle pour qu'il puisse tenir dans une image. Elle utilise la valeur maximale de l'histogramme pour normaliser chaque compartiment.

### C. Étirement d'histogramme
La fonction `etirerHistogramme` étire l'histogramme d'une image entre deux valeurs spécifiées (`newMin` et `newMax`). Elle parcourt l'image, ajuste les valeurs des pixels en fonction de l'intervalle spécifié, et génère une nouvelle image étirée.

### D. Égalisation d'histogramme
La fonction `egalizeHistFormule` effectue l'égalisation d'histogramme en calculant l'histogramme cumulé et en appliquant vue en cour. Avant de réussir a comprendre nous avons fait une autre fonction `egalizeFormule`, qui applique aussi une autre manière de faire des histogrammes égalisé.

### E. Affichage d'histogramme
La fonction `afficherHistogramme` dessine et affiche l'histogramme d'une image. Elle utilise la bibliothèque OpenCV pour créer une représentation visuelle de l'histogramme.

### F. Filtrage par convolution
La fonction `appliquerFiltre` applique un filtre générique 3x3 sur une image en utilisant l'opération de convolution. Elle parcourt l'image, multiplie les valeurs des pixels par les coefficients du filtre, et génère une nouvelle image filtrée.

- Filtre de flou
Un filtre de flou (`filtreBlur`) est appliqué pour créer un effet de flou sur l'image. Ce filtre utilise une moyenne pondérée pour lisser les transitions entre les pixels adjacents. Cette matrice est reprise du cour

### G. Difficulté
Grosse difficulté à l'installation d'openCV. Impossible sous windows de réussir à installer cette librérie. Même avec l'aide d'un professeur et en cusltant plusieurs forum. Plus de 5h à d'installation, pour au final installer un autre os, debian.  
Au niveau du code, les fonctions sont simple a mettre en place une fois que l'on comprends se qu'il faut faire. Petite difficulté pour l'égalisation, au début nous avons mal compris ce qu'es la dynamique d'une image. Nous pouvons aussi dire qu'il n'est pas évident de savoir si nous avons réussit corectement une fonction. Pour cela nous avons utilisé les fonctions openCV pour comparé nos résulat. Nous avons aussi discuté et comparé nos résultat entre groupe.


## Conclusion
Ce projet a permis de mettre en pratique divers concepts d'analyse d'image vue en cour, du calcul d'histogrammes à l'application de filtres par convolution. Les différentes fonctions implémentées illustrent les opérations de base utilisées dans le traitement d'images. Chaque résultat de fonction a été comparé à des fonctions d'openCV.
