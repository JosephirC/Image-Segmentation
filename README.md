# README

Ce code en C++ utilise la bibliothèque OpenCV pour effectuer diverses opérations sur une image en niveaux de gris. Voici une brève explication des principales fonctions implémentées dans le code :

## Fonctions principales

1. **minMaxHist** : Calcule les valeurs minimales et maximales d'un histogramme.

2. **minMaxIm** : Calcule les valeurs minimales et maximales d'une image.

3. **calculerHistogrammeCumule** : Calcule l'histogramme cumulé d'un histogramme donné.

4. **monCalcHist** : Calcule l'histogramme d'une image.

5. **imgToHistoCumul** : Calcule l'histogramme cumulé d'une image en utilisant la fonction `monCalcHist`.

6. **egalizeHistOpenCV** : Égalise l'histogramme d'une image à l'aide de la fonction équivalente d'OpenCV.

7. **egaliseHist** : Égalise l'histogramme d'une image en utilisant une approche personnalisée.

8. **egalizeHistFormule** : Égalise l'histogramme d'une image en utilisant une formule spécifique.

9. **etirerHistogramme** : Étire l'histogramme d'une image pour améliorer le contraste.

10. **normalizeHistGris** : Normalise l'histogramme d'une image en niveaux de gris.

11. **afficherHistogramme** : Affiche un histogramme à partir d'une matrice d'histogramme.

12. **HistogrammeGris** : Calcule et affiche l'histogramme d'une image en niveaux de gris.

13. **appliquerFiltre** : Applique un filtre à une image en utilisant une opération de convolution.

## Utilisation dans le programme principal

Le programme principal commence par charger une image en niveaux de gris depuis le chemin spécifié. Ensuite, il effectue plusieurs opérations telles que le calcul et l'affichage de l'histogramme, l'égalisation d'histogramme, l'étirement d'histogramme, l'application de filtres, etc.

Chaque opération est affichée dans une fenêtre séparée, permettant une visualisation interactive des résultats. Vous pouvez ajuster le chemin de l'image à traiter en modifiant la variable `image_path` dans la fonction `main`.

## Prérequis

Assurez-vous d'avoir installé la bibliothèque OpenCV sur votre système avant de compiler et exécuter ce code. Vous pouvez utiliser la commande suivante pour compiler le programme :

```bash
g++ -o mon_programme mon_code.cpp `pkg-config --cflags --libs opencv`
```

Un makefile est mise a votre disposition pour une meillieur compilation. (Faire make dans votre terminal).