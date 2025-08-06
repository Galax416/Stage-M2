# Documentation du Projet Stage-M2

## Présentation Générale

Ce projet est une application de simulation physique et de rendu 3D interactive, développée en C++ avec Qt6 et OpenGL. Elle permet de manipuler, simuler et visualiser des modèles physiques complexes (particules, ressorts, collisions, modèles paramétriques, etc.) dans une interface graphique moderne. Le projet intègre des bibliothèques scientifiques majeures (Eigen, Boost, CGAL) pour la gestion des calculs mathématiques, de la géométrie et des algorithmes avancés.

## Fonctionnalités principales
- **Simulation physique de particules et ressorts** (masses, contraintes, collisions, frottements, etc.)
- **Gestion de modèles 3D** (chargement, sauvegarde, manipulation de fichiers OBJ)
- **Rendu 3D OpenGL** (visualisation filaire, surface, colliders, BVH, etc.)
- **Interface graphique Qt** (widgets de contrôle, sliders, boutons, etc.)
- **Paramétrisation avancée** (modèles paramétriques, courbes, voxels, etc.)
- **Gestion de scènes** (chargement/sauvegarde de scènes, presets)

## Hiérarchie et Structure du Projet

```
Stage-M2/
├── Stage.pro                                   # Fichiers de build Qt
├── src/                                        # Code source principal
│   ├── Main.cpp, MainWindow.h, Mainwindow.cpp  # Point d'entrée et fenêtre principale
│   ├── core/                                   # Structures de base (Rigidbody, Camera, etc.)
│   ├── physics/                                # Système physique (Particle, Spring, PhysicsSystem...)
│   ├── rendering/                              # Rendu 3D (Mesh, Model, Render...)
│   ├── utils/                                  # Utilitaires (chargement OBJ, conversion...)
│   └── widgets/                                # Widgets Qt personnalisés (OpenGL, réglages...)
├── libs/                                       # Dépendances tierces
│   ├── eigen-3.4.0/                            # Algèbre linéaire
│   ├── boost_1_78_0/                           # Algorithmes et utilitaires C++
│   ├── CGAL-5.6.1/                             # Géométrie algorithmique
│   └── stb_image.*                             # Chargement d'images
├── resources/                                  # Ressources (modèles, shaders, etc.)
│   ├── images/                                 # Images/Textures
│   ├── models/                                 # Modèles 3D OBJ
│   ├── shaders/                                # Shaders GLSL
│   └── resources.qrc                           # Fichier Qt Resource
└── saves/                                      # Sauvegardes de scènes
```

## Dépendances principales
- **Qt6** : Interface graphique, gestion des widgets, OpenGL
- **OpenGL** : Rendu 3D
- **Eigen** : Calculs matriciels et vectoriels
- **Boost** : Utilitaires C++ avancés
- **CGAL** : Algorithmes de géométrie
- **stb_image** : Chargement d'images (optionnel)

## Architecture logicielle
- **MainWindow** : Fenêtre principale, gestion des widgets et de l'UI
- **OpenGLWidget** : Widget de rendu 3D, gestion des interactions, affichage de la scène
- **PhysicsSystem** : Moteur physique, gestion des particules, ressorts, collisions, BVH
- **Particle/Spring/Rigidbody** : Structures de base pour la simulation
- **Widgets Qt** : Contrôles pour les paramètres physiques, modèles, etc.

## Utilisation
1. **Compilation** : Utiliser Qt Creator ou `qmake` + `make` (voir `Stage.pro`)
2. **Lancement** : Exécuter le binaire généré (`main.exe` sous Windows)
3. **Chargement de modèles** : Utiliser l'UI pour charger des fichiers OBJ ou des scènes
4. **Simulation** : Manipuler les paramètres physiques, lancer/arrêter la simulation, visualiser les résultats

### Interfaces Utilisateur

Voici un aperçu de l'interface graphique de l'application :

![Interface principale](resources/images/interface1.png)
![Interface principale](resources/images/interface2.png)

_L'interface propose des contrôles pour la simulation, le chargement de modèles, et la visualisation 3D en temps réel._

Contrôles et raccourcis :
- **Clic gauche** : Tourne la scène
- **Clic droit** : Déplace la scène
- **Molette de la souris** : Zoom avant/arrière
- **Espace** : Démarrer/arrêter la simulation
- **R** : Réinitialiser la simulation
- **W** : Wireframe (affichage filaire)
- **S** : Surface (affichage de la surface)
- **B** : BVH (affichage des Bounding Volume Hierarchies)
- **P** : Change la gravité selon la caméra 
- **E** : Ajouter une particule dans le modèle
- **1, 2, 3, 4, 5, 6** : Changer la vue de la caméra (vue de devant, dessus, côté, etc.)
- **Flèches directionnelles** : Déplace le modèle

## Ressources et modèles
- Les modèles 3D sont stockés dans `resources/models/`
- Les shaders OpenGL sont dans `resources/shaders/`

## Contribution
- Le code est organisé par modules (core, physics, rendering, widgets, utils)
- Les dépendances tierces sont incluses dans `libs/`
- Les fichiers de build sont configurés pour Qt5 ou Qt6 et C++11

<!-- ## Auteurs et licences
- Voir les fichiers `README.md` et `LICENSE` dans chaque dossier de librairie tierce pour les conditions d'utilisation.
- Projet principal sous licence propriétaire (à préciser selon votre cas). -->

---
