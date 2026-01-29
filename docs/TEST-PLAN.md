# PatternCAD - Plan de Test Systématique

**Objectif:** Valider toutes les fonctionnalités implémentées avant d'avancer.

**Processus:**
1. Je propose une feature à tester
2. Tu testes et réponds OUI/NON
3. Si NON → on corrige jusqu'à OUI
4. Une fois OUI → on valide et passe à la suivante

---

## Phase 1: Fondations (Application & UI)

### TEST-01: Lancement de l'application ✅
**À tester:**
- L'application se lance
- Une fenêtre s'affiche
- Pas de crash au démarrage

**Procédure:**
```bash
cd /path/to/PatternCAD/build
./PatternCAD
```

**Critère de succès:**
- Fenêtre "PatternCAD - Untitled" apparaît
- Interface visible

**Statut:** ⏳ À tester

---

### TEST-02: Structure de la fenêtre principale ✅
**À tester:**
- Barre de menus visible (File, Edit, View, Draw, Modify, Tools, Window, Help)
- Zone de dessin centrale (canvas)
- Barre d'état en bas
- Panel "Tools" à gauche

**Procédure:**
- Observer l'interface

**Critère de succès:**
- Tous les éléments UI visibles
- Pas d'erreur visuelle

**Statut:** ⏳ À tester

---

### TEST-03: Panneaux dockables ✅
**À tester:**
- Menu Window → voir les panneaux disponibles
- Afficher/masquer chaque panneau

**Procédure:**
1. Menu Window
2. Cocher/décocher: Tools, Properties, Layers, Parameters

**Critère de succès:**
- Chaque panneau apparaît/disparaît correctement
- Pas de crash

**Statut:** ⏳ À tester

---

### TEST-04: Canvas - Pan (déplacement vue) ✅
**À tester:**
- Bouton milieu souris pour déplacer la vue
- Ou touche Espace + glisser

**Procédure:**
1. Maintenir bouton milieu souris + glisser
2. OU: Maintenir Espace + glisser gauche

**Critère de succès:**
- La vue se déplace
- Retour fluide

**Statut:** ⏳ À tester

---

### TEST-05: Canvas - Zoom ✅
**À tester:**
- Molette souris pour zoomer/dézoomer
- Touches + et - pour zoomer

**Procédure:**
1. Molette haut = zoom in
2. Molette bas = zoom out
3. Touche + et -

**Critère de succès:**
- Zoom fonctionne
- Pourcentage affiché dans status bar change
- Range 10%-1000%

**Statut:** ⏳ À tester

---

### TEST-06: Grille ✅
**À tester:**
- Touche G pour afficher/masquer grille

**Procédure:**
1. Appuyer sur G

**Critère de succès:**
- Grille apparaît/disparaît
- Points ou lignes visibles

**Statut:** ⏳ À tester

---

## Phase 2: Outils de Dessin Basiques

### TEST-07: Point Tool ❌ SUPPRIMÉ
**Raison:** Fonctionnalité inutile - les points standalone ne servent à rien dans une application de design de patrons.

**Note:** La classe géométrique Point2D est conservée en interne pour d'autres usages.

**Statut:** ❌ Supprimé (pas de test nécessaire)

---

### TEST-08: Line Tool ⏳
**À tester:**
- Tracer une ligne entre deux points

**Procédure:**
1. Touche L
2. Clic point A
3. Clic point B

**Critère de succès:**
- Ligne visible entre A et B
- Longueur/angle affichés pendant traçage

**Statut:** ⏳ À tester

---

### TEST-09: Line Tool - Mode orthogonal ⏳
**À tester:**
- Shift pour forcer horizontal/vertical

**Procédure:**
1. Touche L
2. Clic point A
3. Maintenir Shift
4. Déplacer souris
5. Clic point B

**Critère de succès:**
- Ligne forcée H ou V selon direction
- Angle = 0° ou 90°

**Statut:** ⏳ À tester

---

### TEST-10: Line Tool - Dimension input ⏳
**À tester:**
- Tab pour saisir longueur/angle précis

**Procédure:**
1. Touche L
2. Clic point A
3. Appuyer Tab
4. Saisir longueur (ex: 100)
5. Tab
6. Saisir angle (ex: 45)
7. Entrée

**Critère de succès:**
- Overlay de saisie apparaît
- Ligne créée avec dimensions exactes

**Statut:** ⏳ À tester

---

### TEST-11: Circle Tool ⏳
**À tester:**
- Dessiner un cercle

**Procédure:**
1. Touche C
2. Clic pour centre
3. Déplacer pour rayon
4. Clic pour valider

**Critère de succès:**
- Cercle visible
- Rayon affiché pendant traçage

**Statut:** ⏳ À tester

---

### TEST-12: Rectangle Tool ⏳
**À tester:**
- Dessiner un rectangle

**Procédure:**
1. Touche R
2. Clic coin 1
3. Glisser
4. Clic coin 2

**Critère de succès:**
- Rectangle visible
- Dimensions affichées

**Statut:** ⏳ À tester

---

### TEST-13: Rectangle Tool - Mode carré ⏳
**À tester:**
- Shift pour forcer un carré

**Procédure:**
1. Touche R
2. Clic coin 1
3. Maintenir Shift
4. Glisser
5. Clic

**Critère de succès:**
- Forme carrée (W = H)

**Statut:** ⏳ À tester

---

### TEST-14: Polyline Tool ⏳
**À tester:**
- Ligne polygonale multi-segments

**Procédure:**
1. Sélectionner Polyline tool
2. Clic point 1
3. Clic point 2
4. Clic point 3
5. Double-clic ou Entrée pour terminer

**Critère de succès:**
- Segments connectés
- Polyline visible

**Statut:** ⏳ À tester

---

## Phase 3: Sélection et Manipulation

### TEST-15: Select Tool - Sélection simple ⏳
**À tester:**
- Sélectionner un objet

**Procédure:**
1. Dessiner quelques objets (lignes, cercles)
2. Touche S (Select tool)
3. Cliquer sur un objet

**Critère de succès:**
- Objet sélectionné (surligné ou poignées visibles)
- Properties panel se met à jour

**Statut:** ⏳ À tester

---

### TEST-16: Select Tool - Déplacement ⏳
**À tester:**
- Glisser-déposer un objet

**Procédure:**
1. Sélectionner un objet
2. Glisser avec souris

**Critère de succès:**
- Objet suit la souris
- Position se met à jour

**Statut:** ⏳ À tester

---

### TEST-17: Select Tool - Sélection multiple ⏳
**À tester:**
- Ctrl+clic pour multi-sélection

**Procédure:**
1. Dessiner 3 objets
2. Clic objet 1
3. Ctrl+clic objet 2
4. Ctrl+clic objet 3

**Critère de succès:**
- 3 objets sélectionnés simultanément
- Déplacement groupe possible

**Statut:** ⏳ À tester

---

### TEST-18: Select Tool - Rectangle de sélection ⏳
**À tester:**
- Drag pour sélectionner zone

**Procédure:**
1. Select tool
2. Cliquer zone vide
3. Glisser pour créer rectangle
4. Relâcher

**Critère de succès:**
- Rectangle de sélection visible
- Objets dans zone = sélectionnés

**Statut:** ⏳ À tester

---

### TEST-19: Delete Objects ⏳
**À tester:**
- Supprimer objets sélectionnés

**Procédure:**
1. Sélectionner 1+ objets
2. Appuyer Delete ou Backspace

**Critère de succès:**
- Objets disparaissent
- Canvas mis à jour

**Statut:** ⏳ À tester

---

### TEST-20: Undo/Redo ⏳
**À tester:**
- Annuler et refaire actions

**Procédure:**
1. Dessiner un objet
2. Ctrl+Z (undo)
3. Ctrl+Y (redo)

**Critère de succès:**
- Objet disparaît puis réapparaît
- Edit menu montre actions

**Statut:** ⏳ À tester

---

## Phase 4: Transformations

### TEST-21: Rotate Tool ⏳
**À tester:**
- Rotation interactive

**Procédure:**
1. Dessiner un rectangle
2. Ctrl+R (ou menu Modify → Rotate)
3. Clic sur rectangle pour sélectionner
4. Clic pour définir centre rotation
5. Déplacer souris pour angle
6. Clic pour valider

**Critère de succès:**
- Rectangle tourne autour du centre
- Angle affiché
- Undo possible

**Statut:** ⏳ À tester

---

### TEST-22: Mirror Tool - Horizontal ⏳
**À tester:**
- Symétrie horizontale

**Procédure:**
1. Dessiner un objet non-symétrique
2. Ctrl+M (ou menu Modify → Mirror)
3. Clic sur objet
4. Appuyer H (horizontal)

**Critère de succès:**
- Copie miroir créée
- Original intact
- Position symétrique

**Statut:** ⏳ À tester

---

### TEST-23: Mirror Tool - Vertical ⏳
**À tester:**
- Symétrie verticale

**Procédure:**
1. Dessiner un objet
2. Ctrl+M
3. Clic sur objet
4. Appuyer V (vertical)

**Critère de succès:**
- Copie miroir créée
- Symétrie verticale

**Statut:** ⏳ À tester

---

### TEST-24: Scale Tool ⏳
**À tester:**
- Mise à échelle

**Procédure:**
1. Dessiner un cercle
2. Menu Modify → Scale
3. Clic sur cercle
4. Clic pour définir origine
5. Déplacer pour agrandir/réduire
6. Clic pour valider

**Critère de succès:**
- Cercle change de taille
- Pourcentage affiché
- Undo possible

**Statut:** ⏳ À tester

---

### TEST-25: Align - Left ⏳
**À tester:**
- Aligner bords gauches

**Procédure:**
1. Dessiner 3 rectangles à positions différentes
2. Les sélectionner tous (Ctrl+A ou rectangle sélection)
3. Menu Modify → Align → Align Left
4. OU: Ctrl+Shift+L

**Critère de succès:**
- Bords gauches alignés
- Objets déplacés horizontalement

**Statut:** ⏳ À tester

---

### TEST-26: Distribute - Horizontal ⏳
**À tester:**
- Espacement horizontal égal

**Procédure:**
1. Dessiner 4 objets à espacements irréguliers
2. Les sélectionner
3. Menu Modify → Distribute → Distribute Horizontal
4. OU: Ctrl+Shift+D

**Critère de succès:**
- Premier et dernier objets fixes
- Objets du milieu espacés également
- Espacement uniforme

**Statut:** ⏳ À tester

---

## Phase 5: Layers

### TEST-27: Layers Panel ⏳
**À tester:**
- Créer nouveau layer

**Procédure:**
1. Ouvrir Layers panel (Window → Layers)
2. Clic bouton "+" ou "New Layer"

**Critère de succès:**
- Nouveau layer dans liste
- Nom éditable

**Statut:** ⏳ À tester

---

### TEST-28: Layer - Visibilité ⏳
**À tester:**
- Toggle visibilité layer

**Procédure:**
1. Dessiner objets sur layer
2. Dans Layers panel, clic icône œil

**Critère de succès:**
- Objets du layer disparaissent/réapparaissent
- Canvas mis à jour

**Statut:** ⏳ À tester

---

### TEST-29: Layer - Lock ⏳
**À tester:**
- Verrouiller layer

**Procédure:**
1. Dessiner objets
2. Lock le layer
3. Essayer de sélectionner objets

**Critère de succès:**
- Objets non-sélectionnables
- Pas de modification possible

**Statut:** ⏳ À tester

---

## Phase 6: Autres Features UI

### TEST-30: Properties Panel ⏳
**À tester:**
- Affichage propriétés objet

**Procédure:**
1. Dessiner un rectangle
2. Le sélectionner
3. Observer Properties panel

**Critère de succès:**
- Position X, Y affichée
- Taille W, H affichée
- Valeurs correctes

**Statut:** ⏳ À tester

---

### TEST-31: Parameters Panel ⏳
**À tester:**
- Ajouter paramètre

**Procédure:**
1. Ouvrir Parameters panel (Window → Parameters)
2. Clic bouton "Add"
3. Éditer nom, valeur, unité

**Critère de succès:**
- Paramètre ajouté dans table
- Édition inline fonctionne
- Validation nom

**Statut:** ⏳ À tester

---

### TEST-32: Keyboard Shortcuts Dialog ⏳
**À tester:**
- Afficher aide raccourcis

**Procédure:**
1. Menu Help → Keyboard Shortcuts
2. OU: F1

**Critère de succès:**
- Dialog s'ouvre
- Liste des raccourcis affichée

**Statut:** ⏳ À tester

---

### TEST-33: Preferences Dialog ⏳
**À tester:**
- Ouvrir préférences

**Procédure:**
1. Menu Edit → Preferences

**Critère de succès:**
- Dialog s'ouvre
- Onglets visibles
- Paramètres éditables

**Statut:** ⏳ À tester

---

### TEST-34: Spacebar Grab-and-Move ⏳
**À tester:**
- Pan temporaire avec Espace

**Procédure:**
1. Sélectionner un outil (ex: Line)
2. Maintenir Espace
3. Glisser pour déplacer vue
4. Relâcher Espace

**Critère de succès:**
- Vue se déplace pendant Espace
- Retour à Line tool après
- Outil actif inchangé

**Statut:** ⏳ À tester

---

## Résumé

**Total tests:** 33 (1 supprimé)
**Réussis:** 6 ✅
**Échoués:** 0 ❌
**Supprimés:** 1 ❌
**Non testés:** 27 ⏳

---

## Ordre de Test Recommandé

1. **Fondations (1-6)** - Base application
2. **Dessin basique (7-14)** - Outils création
3. **Sélection (15-20)** - Manipulation objets
4. **Transformations (21-26)** - Outils avancés
5. **Layers (27-29)** - Organisation
6. **UI supplémentaire (30-34)** - Panneaux et dialogs

---

**INSTRUCTION:** On commence par TEST-01. Lance l'application et dis-moi OUI/NON.
