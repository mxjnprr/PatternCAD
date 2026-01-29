# PatternCAD - Raccourcis Clavier

Ce document liste tous les raccourcis clavier de PatternCAD, basés sur les standards professionnels de l'industrie du patronage.

## Légende
- **Implémenté** ✅ : Fonctionnalité actuellement disponible
- **Planifié** 🔜 : À implémenter prochainement
- **Futur** 📋 : Fonctionnalité avancée pour versions ultérieures

## Outils de Base

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Z` | Select tool | Sélection et manipulation d'objets | ✅ |
| `Shift+Z` | Trace Piece Zones | Use the Trace tool to create a Zone | 📋 |
| `Ctrl+Z` | Undo | Annuler la commande précédente | ✅ |
| `Ctrl+Shift+Z` | Redo | Refaire la commande annulée | ✅ |

## Outils de Dessin - Lignes et Points

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `O` | Add Point on Contour | Ajouter un point sur un segment pour modifier sa forme | 🔜 |
| `M` | Move Point | Déplacer un point | 🔜 |
| `Shift+M` | Move Point Along Contour | Déplacer un point le long du contour | 📋 |
| `Ctrl+M` | Move Points Proportionally | Déplacer la sélection proportionnellement | 📋 |
| `Ctrl+Alt+M` | Move Points | Déplacer une chaîne de points | 📋 |
| `Ctrl+Shift+M` | Move Points Parallelly | Déplacer un segment fixe (parallèlement) | 📋 |

## Outils de Dessin - Formes Géométriques

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Ctrl+Alt+C` | Circle | Créer un cercle interne | ✅ (Circle Tool - à réassigner) |
| `Ctrl+Shift+Alt+C` | Two Circle Tangent | Créer une ligne tangente à deux cercles | 📋 |
| `L` | Pleat | Créer un pli boîte ou pli creux | 📋 |
| `Shift+L` | Pleat lines | Créer des lignes de pli dans une pièce | 📋 |
| `P` | Create parallel | Créer un contour interne parallèle au segment sélectionné | 📋 |
| `Shift+P` | Extend in parallel | Étendre la partie sélectionnée de la pièce parallèlement | 📋 |
| `D` | Draft/Polyline | Dessiner des polylignes avec courbes | ✅ |
| `Shift+Y` | Paste Y Grading | Coller les valeurs de gradation Y | 📋 |

## Construction de Pièces

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `B` | Build Piece | Former une nouvelle pièce à partir de zones de pièces existantes | 📋 |
| `Ctrl+B` | Trace Segments | Tracer des segments pour créer une nouvelle pièce | 📋 |
| `Ctrl+Alt+B` | Add Button | Ajouter des boutons (percer des trous) | 📋 |
| `Ctrl+Shift+B` | Trace Piece | Créer une nouvelle pièce à partir de pièces qui se croisent | 📋 |
| `J` | Join Pieces | Joindre et combiner deux pièces | 📋 |
| `Shift+J` | Join Contours | Joindre deux contours internes non fermés | 📋 |

## Découpe et Manipulation

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `C` | Cut a Piece | Couper une pièce | 📋 |
| `Ctrl+Shift+C` | Cut a Piece Along Internal | Couper le long d'une ligne interne | 📋 |
| `D` | Draft | Drafter une pièce ou un contour interne | 📋 |
| `E` | Extend Internal | Étendre un contour interne, cercle ou pince | 📋 |
| `Shift+E` | Create Point Connection | Créer une connexion de point pour le point sélectionné | 📋 |
| `Ctrl+E` | Add Points and Create Connection | Ajouter des points sur les contours proches et connecter | 📋 |
| `R` | Rotate Piece | Faire pivoter une pièce (ou toutes les pièces sélectionnées avec Shift) | 📋 |
| `Alt+R` | Rotate Contour or Text | Faire pivoter un contour interne/externe ou du texte séparément | 📋 |
| `Ctrl+R` | Round Corner | Arrondir un coin de pièce | 📋 |

## Pliage (Folding)

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Shift+F` | Fold In | Créer un contour de pli vers l'intérieur et couper | 📋 |
| `Ctrl+Shift+F` | Fold Out | Plier vers l'extérieur selon un contour interne sélectionné | 📋 |
| `H` | Set Half Piece Line | Définir la ligne de demi-pièce | 📋 |
| `Shift+H` | Open Half | Ouvrir une demi-pièce | 📋 |
| `Ctrl+H` | Close Half | Fermer une demi-pièce | 📋 |
| `Ctrl+Alt+H` | Set Mirror Line | Définir la ligne de miroir | 📋 |

## Pinces (Darts)

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Ctrl+Alt+D` | Add or Rotate Dart | Créer ou faire pivoter une pince | 📋 |

## Couture (Seam)

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `S` | Add Seam | Ajouter une marge de couture à un segment | 📋 |
| `Shift+S` | Remove Seam | Retirer la couture des pièces ou trous sélectionnés | 📋 |
| `Ctrl+Shift+Alt+S` | Remove Seam on Segment | Retirer la couture sur un segment | 📋 |

## Crans (Notches)

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `N` | Add Notch | Ajouter un cran | 📋 |
| `Shift+N` | Add Notch on Point | Ajouter un cran sur un point | 📋 |
| `Ctrl+Shift+N` | Add Points to all the Notches | Ajouter des points aux crans existants sur le contour | 📋 |

## Objets Internes

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `I` | Move Internal | Déplacer ou copier un objet interne | 📋 |
| `Shift+I` | Select Internals | Sélectionner des objets internes par rectangle | 📋 |
| `Ctrl+I` | Piece Info | Voir les informations de la pièce courante | 📋 |
| `Ctrl+Alt+I` | Copy Internal objects | Copier les objets internes sélectionnés | 📋 |
| `Ctrl+Shift+I` | Change Internal attributes | Modifier les paramètres internes globalement | 📋 |
| `Ctrl+Alt+P` | Paste Internal objects | Coller des objets internes sur la pièce courante | 📋 |

## Texte et Annotations

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `T` | Text | Ajouter ou éditer du texte dans une pièce | 📋 |
| `Shift+T` | Trim | Rogner les lignes internes | 📋 |
| `Ctrl+Shift+T` | Trace and Trim | Tracer et rogner les lignes internes | 📋 |

## Alignement et Lignes Guides

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `G` | Align Points | Aligner plusieurs points | 📋 |
| `Shift+G` | Cut by Guide Line | Couper la pièce courante par la ligne guide active | 📋 |
| `Ctrl+G` | Copy | Copier le segment sélectionné dans le presse-papiers | 📋 |
| `Ctrl+Alt+G` | Delete All Guide Lines | Supprimer toutes les lignes guides | 📋 |
| `Ctrl+Shift+G` | Grid | Afficher les points de grille | 📋 |
| `Ctrl+Shift+Alt+G` | Guide Lines | Afficher ou masquer les lignes guides | 📋 |

## Mesure et Analyse

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Ctrl+D` | Measure | Mesurer des distances | 📋 |

## Sélection Multiple et Déplacement

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Q` | Multi Move | Déplacer points et objets internes par rectangle | 📋 |
| `Shift+Q` | Add Spline Support Points | Ajouter des points de support pour les splines | 📋 |
| `Ctrl+Q` | Points Cleanup | Supprimer les points superflus | 📋 |
| `Ctrl+Alt+Q` | Definition | Segments égaux: créer ou supprimer groupes et segments | 📋 |

## Gradation

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Shift+C` | Copy Grading | Copier les valeurs de gradation du point sélectionné | 📋 |
| `Shift+V` | Paste Grading | Coller les valeurs de gradation X & Y | 📋 |
| `Shift+X` | Paste X Grading | Coller les valeurs de gradation X | 📋 |
| `Shift+Y` | Paste Y Grading | Coller les valeurs de gradation Y | 📋 |
| `Ctrl+Shift+Alt+R` | Rules Library | Afficher ou masquer la bibliothèque de règles de gradation | 📋 |

## Placement et Arrangement

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Ctrl+K` | Arrange on Working Area | Arranger toutes les pièces sur la zone de travail | 📋 |
| `Ctrl+Shift+K` | Arrange with Large Gap | Arranger avec un grand écart | 📋 |
| `Ctrl+L` | Plot | Tracer les pièces sur la zone de travail | 📋 |
| `Ctrl+Alt+R` | Restore Placement | Restaurer le placement précédemment sauvegardé | 📋 |
| `Ctrl+Alt+S` | Save Placement | Sauvegarder le placement actuel des pièces | 📋 |

## Lignes de Traçage (Track Lines)

| Raccourci | Outil | Description | Status |
|-----------|-------|-------------|--------|
| `Ctrl+T` | Clear Track Lines | Supprimer toutes les lignes de traçage | 📋 |
| `Ctrl+Alt+T` | Track Lines | Créer des lignes de traçage pour chaque déplacement | 📋 |
| `Ctrl+Shift+Alt+T` | Show Track Lines | Afficher les lignes de traçage | 📋 |

## Fichier

| Raccourci | Action | Description | Status |
|-----------|--------|-------------|--------|
| `Ctrl+N` | New | Créer un nouveau fichier de style | ✅ |
| `Ctrl+O` | Open | Ouvrir un fichier de style existant | ✅ |
| `Ctrl+S` | Save | Sauvegarder le fichier actuel | ✅ |
| `Ctrl+Shift+S` | Save As | Sauvegarder sous un nouveau nom | ✅ |
| `Ctrl+Q` | Quit | Quitter l'application | ✅ |

## Édition

| Raccourci | Action | Description | Status |
|-----------|--------|-------------|--------|
| `Ctrl+Z` | Undo | Annuler la commande précédente | ✅ |
| `Ctrl+Shift+Z` | Redo | Refaire la commande annulée | ✅ |
| `Ctrl+C` | Copy | Copier la/les pièce(s) sélectionnée(s) | 🔜 |
| `Ctrl+X` | Cut | Couper la pièce courante | 🔜 |
| `Ctrl+V` | Paste | Coller la/les pièce(s) du presse-papiers | 🔜 |

## Impression

| Raccourci | Action | Description | Status |
|-----------|--------|-------------|--------|
| `Ctrl+P` | Print | Imprimer les pièces sur la zone de travail | 📋 |

## Vue et Navigation

| Raccourci | Action | Description | Status |
|-----------|--------|-------------|--------|
| `Ctrl++` | Zoom In | Zoom avant | ✅ |
| `Ctrl+-` | Zoom Out | Zoom arrière | ✅ |
| `F` | Zoom to Fit | Zoom pour tout afficher | ✅ |
| `Molette` | Zoom | Zoom (si configuré) | ✅ |
| `Clic milieu + glisser` | Pan | Déplacer la vue | ✅ |

## Outil Select (Z)

| Raccourci | Action | Description | Status |
|-----------|--------|-------------|--------|
| `Clic` | Sélectionner | Sélectionner un objet | ✅ |
| `Clic-glissé` | Bounding Box | Sélection par rectangle | ✅ |
| `Ctrl+Clic` | Multi-select | Ajouter/retirer de la sélection | ✅ |
| `Espace` | Pick and Place | Ramasser/poser les objets sélectionnés | ✅ |
| `Clic droit` | Context Menu | Menu contextuel (changer layer, supprimer) | ✅ |
| `Suppr` | Delete | Supprimer les objets sélectionnés | ✅ |
| `Échap` | Cancel | Annuler l'action / Désélectionner | ✅ |

## Outil Draft/Polyline (D)

| Raccourci | Action | Description | Status |
|-----------|--------|-------------|--------|
| `Clic` | Add Point | Ajouter un point pointu (angle) | ✅ |
| `Clic-glissé` | Add Curve | Ajouter un point courbe (distance = tension) | ✅ |
| `Double-clic` | Close | Fermer la polyligne | ✅ |
| `Entrée` | Close | Fermer la polyligne | ✅ |
| `Échap` | Cancel | Annuler la polyligne en cours | ✅ |

## Notes d'Implémentation

### Raccourcis Actuellement Assignés (à réassigner selon la liste standard)
- **Z** : Select tool ✅
- **D** : Draft/Polyline tool ✅
- **O** : Add Point on Contour - à implémenter (permet de modifier la forme d'une ligne existante)
- **Ctrl+Alt+C** : Circle tool - à réassigner depuis C simple

### Raccourcis Temporaires des Outils Actuels (en attendant réassignation)
Ces raccourcis sont temporaires et seront remplacés lors de l'implémentation complète:
- **L** : Line tool (temporaire - sera remplacé par "Pleat")
- **C** : Circle tool (temporaire - sera remplacé par "Cut a Piece")
- **R** : Rectangle tool (temporaire - sera remplacé par "Rotate Piece")
- **P** : Point tool (temporaire - sera remplacé par "Create parallel")

### Raccourcis Réservés pour Futurs Outils
Les raccourcis suivants sont réservés pour des outils avancés:
- **A** : Arc
- **B** : Build Piece
- **E** : Extend Internal
- **G** : Align Points
- **M** : Move Point
- **T** : Text

## Aide

| Raccourci | Action | Description | Status |
|-----------|--------|-------------|--------|
| `F1` | Keyboard Shortcuts | Afficher ce guide des raccourcis | ✅ |
