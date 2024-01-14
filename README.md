# README

## TP1
Dans ce TP, nous avons implémenté un programme qui permet de faire la segmentation d'une image. Le programme est écrit en C++ et utilise la bibliothèque OpenCV pour certainnes de ces fonctionnamlité comme les structures de données Point, Vec3b, Mat, etc. Et aussi ces fonctionnalité de lecture et d'écriture d'image.  
Vous pouver utiliser simplement le programme après la commande make, puis ./tp1.

### Segmentation par seuillage
Nous segmentons nos images en utilisant la méthode de seuillage. D'abords avec un seuil fixe, puis avec un seuil suivant l'écart type de la région.

### Merge des régions
Une fois que nos régions ont grandis nous appliquons un merge qui va fusionner les régions qui sont proches.

### Critères
Afin de rendre notre programme un peu plus intéligents en fonction de l'image que nous lui donnons, nous avons implémenté des critères qui permettent de déterminer si une région est trop petite ou trop grande. Si une région est trop petite, nous la fusionnons avec une région voisine. Si une région n'arrive pas a grandire alors nous agrandisons son seuil de tolérence. Toutes ces particularité sont modulable avec les options d'éxécutions.  

### Option
Chaque image présentent des particularités qui complique la segmentation. Nous appliquons des pré-traitements pour améillorer la segmentation. Malgrès tout si les résultats que vous obtenez pour votre image ne sont pas satisfaisant, vous pouvez utiliser les options suivantes :
#### True ou False
- smooth : Applique un filtre gaussien pour lisser l'image.
- merge : Applique le merge des régions. (Utils si vous voulez voir l'éfficasité du growing)
- recalcul : Recalcul les régions en fonction de la nouvelle image. (Utils si vous voulez voir l'éfficasité du merge)
- analyse : Permet de relancer une croissance et une fusion des régions si le nombre de pixel noir est trop grand.
- equalize : Permet d'égaliser l'histogramme de l'image. PRETRAITEMENT
- toTheEnd : Méthode de croissance, les régions grandirons jusqu'à ce qu'elles en puissent plus.
#### float
- pourcentSeed : Pourcentage de pixel qui deviendrons des régions initiales.
- pourcentReCal : Pourcentage de pixel qui deviendrons des régions initiales après un merge.
- tauxPixelNoir : Taux de pixel noir accepté par l'analyse.
- nbRepart : Nombre de répartition de l'histogramme pour l'analyse.
- nbIteration : Nombre d'itération de la croissance. (Sois une limite au toTheEnd soit un nombre fixe d'itération si vous désactiver toTheEnd)
- blur : Taille du filtre gaussien pour le lissage.
- seuilMax : Seuil maximum pour la croissance.
- seuilMin : Seuil minimum pour la croissance.
- coedSDMax : Coefficient de l'écart type pour le seuil maximum.
- coedSDMin : Coefficient de l'écart type pour le seuil minimum.

### Résultats
Les images de résultats sont enregistrées dans le dossier "images_cree". Vous y trouverez toutes les images intermédiaires ainsi que l'image final sous le nom de final.png

##  TP0

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

10. **normalizeHist** : Normalise l'histogramme d'une image en niveaux de gris.

11. **afficherHistogramme** : Affiche un histogramme à partir d'une matrice d'histogramme.

12. **HistogrammeGrisOpenCV** : Calcule et affiche l'histogramme d'une image en niveaux de gris.

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


