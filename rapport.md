# Rapport sur le Projet d'Analyse d'Image

## Introduction
Le projet d'analyse d'image vise à explorer et mettre en pratique les concepts fondamentaux du traitement d'images à l'aide de la bibliothèque OpenCV en langage C++. Ce rapport détaillera les différentes fonctionnalités implémentées, notamment le calcul d'histogrammes, la normalisation et l'étirement d'histogrammes, l'égalisation d'histogrammes, ainsi que l'application de filtres de convolution sur une image. Voici le lien de notre git : [Lien HTML](https://forge.univ-lyon1.fr/p2024398/m1-analyse-image.git).

## I. Histogrammes
### A. Calcul de l'histogramme
La fonction `monCalcHist` permet de calculer l'histogramme d'une image en niveaux de gris. Elle parcourt chaque pixel de l'image, incrémente le compartiment correspondant à l'intensité du pixel, et stocke les résultats dans une matrice.
|Version openCV| Version faite par nous|
| --- | --- |
|![](image_cree/image_histo_openCV.png) | ![](image_cree/image_histograme.png)|

### B. Normalisation d'histogramme
La fonction `normalizeHist` normalise l'histogramme en le mettant à l'échelle pour qu'il puisse tenir dans une image. Elle utilise la valeur maximale de l'histogramme pour normaliser chaque compartiment. Cette fonction est utilisé pour normaliser notre histogramme.

### C. Affichage d'histogramme
La fonction `afficherHistogramme` dessine et affiche l'histogramme d'une image. Elle utilise la bibliothèque OpenCV pour créer une représentation visuelle de l'histogramme.

### D. Histograme cumulé
La fonction `calculerHistogrammeCumule`  calcule l'histogramme cumulé à partir d'un histogramme donné en utilisant une approche itérative, où chaque élément de l'histogramme cumulé est la somme cumulée des éléments correspondants de l'histogramme initial. La fonction `imgToHistoCumul` calcul d'abord l'histogramme puis appel la fonction `calculerHistogrammeCumule`.
| Histo cumuler| 
| --- |
| <img src="image_cree/histo_cumulé.png" alt="" width="300">|

### E. Étirement d'histogramme
La fonction `etirerHistogramme` étire l'histogramme d'une image entre deux valeurs spécifiées (`newMin` et `newMax`). Elle parcourt l'image, ajuste les valeurs des pixels en fonction de l'intervalle spécifié, et génère une nouvelle image étirée.
|Version claire| Version sombre|
| --- | --- |
|![](image_cree/image_etirer.png) | ![](image_cree/image_etirer_sombre.png)|

### F. Égalisation d'histogramme
La fonction `egalizeHistFormule` effectue l'égalisation d'histogramme en calculant l'histogramme cumulé et en appliquant la formule vue en cour. Avant de réussir a comprendre nous avons fait une autre fonction `egaliseHist`, qui applique aussi une autre manière de faire des histogrammes égalisé.
|Version openCV| Version faite par nous|
| --- | --- |
|![](image_cree/image_egaliser_openCV.png) | ![](image_cree/image_egaliser_formule.png)|

### G. Filtrage par convolution
La fonction `appliquerFiltre` applique un filtre générique 3x3 sur une image en utilisant l'opération de convolution. Elle parcourt l'image, multiplie les valeurs des pixels par les coefficients du filtre, et génère une nouvelle image filtrée.

- Filtre de flou
Un filtre de flou (`filtreBlur`) est appliqué pour créer un effet de flou sur l'image. Ce filtre utilise une moyenne pondérée pour lisser les transitions entre les pixels adjacents. Cette matrice est reprise du cour

|Version openCV| Version faite par nous|
| --- | --- |
|![](image_cree/image_egaliser_openCV.png) | ![](image_cree/image_egaliser_formule.png)|


### H. Difficulté
Difficulté à l'installation d'openCV. Impossible sous windows de réussir à installer cette librairie. Même avec l'aide d'un professeur et en consultant plusieurs forums. Plus de 5 h d'installation, pour au final installer un autre OS, Debian.  
Au niveau du code, les fonctions sont simples à mettre en place une fois que l'on comprend ce qu'il faut faire. Petite difficulté pour l'égalisation, au début, nous avons mal compris ce qu'est la dynamique d'une image. Nous pouvons aussi dire qu'il n'est pas évident de savoir si nous avons réussi correctement une fonction. Pour cela, nous avons utilisé les fonctions openCV pour comparer nos résultats.

|Version openCV| Version faite par nous| Image origine |
| --- | --- | --- |
|![](image_cree/Image_filtre_openCV%20.png) | ![](image_cree/image_filtre.png)| ![](image_cree/Image_originale.png) |


## Conclusion
Ce projet a permis de mettre en pratique divers concepts d'analyse d'image vue en cour, du calcul d'histogrammes à l'application de filtres par convolution. Les différentes fonctions implémentées illustrent les opérations de base utilisées dans le traitement d'images. Chaque résultat de fonction a été comparé à des fonctions d'openCV.
