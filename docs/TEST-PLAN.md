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

### TEST-08: Line Tool ❌ SUPPRIMÉ
**Raison:** Les lignes standalone ne servent pas dans une application de design de patrons. Les pièces nécessitent des **contours fermés** (rectangles, polylignes fermées).

**Note:** La classe géométrique `Line` est conservée en interne. Elle sera réintégrée plus tard comme "internal" pour les lignes de pli, marquages, etc. à l'intérieur des pièces.

**Statut:** ❌ Supprimé (pas de test nécessaire)

---

### TEST-09: Line Tool - Mode orthogonal ❌ SUPPRIMÉ
**Raison:** Dépendance de TEST-08 (Line Tool supprimé)

**Statut:** ❌ Supprimé

---

### TEST-10: Line Tool - Dimension input ❌ SUPPRIMÉ
**Raison:** Dépendance de TEST-08 (Line Tool supprimé)

**Statut:** ❌ Supprimé

---

### TEST-11: Circle Tool ❌ SUPPRIMÉ
**Raison:** Cercles standalone inutiles dans une application de design de patrons. Les pièces nécessitent des **contours fermés modifiables** (rectangles, polylignes fermées).

**Note:** La classe géométrique `Circle` est conservée en interne. Elle sera réintégrée plus tard comme "internal" pour boutons, marquages circulaires, etc. à l'intérieur des pièces.

**Statut:** ❌ Supprimé (pas de test nécessaire)

---

### TEST-12: Rectangle Tool ❌ SUPPRIMÉ
**Raison:** Rectangles standalone inutiles dans une application de design de patrons. Les pièces nécessitent des **contours fermés modifiables** (polylignes fermées).

**Note:** La classe géométrique `Rectangle` est conservée en interne. Elle sera réintégrée plus tard comme "internal" pour marquages rectangulaires, etc. à l'intérieur des pièces.

**Statut:** ❌ Supprimé (pas de test nécessaire)

---

### TEST-13: Rectangle Tool - Mode carré ❌ SUPPRIMÉ
**Raison:** Dépendance de TEST-12 (Rectangle Tool supprimé)

**Statut:** ❌ Supprimé

---

### TEST-14: Polyline Tool ✅
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

**Statut:** ✅ VALIDÉ

---

## Phase 3: Sélection et Manipulation

### TEST-15: Select Tool - Sélection simple ✅
**À tester:**
- Sélectionner un objet

**Procédure:**
1. Dessiner quelques polylignes
2. Touche Z (Select tool)
3. Cliquer sur une polyligne

**Critère de succès:**
- Objet sélectionné (surligné directement, pas de bounding box)
- Properties panel se met à jour
- Vertex handles visibles

**Statut:** ✅ VALIDÉ

---

### TEST-16: Select Tool - Déplacement ❌ SUPPRIMÉ
**Raison:** Fonctionnalité non souhaitée. Le déplacement de la vue se fait uniquement avec la barre Espace (TEST-34).

**Statut:** ❌ Supprimé

---

### TEST-17: Select Tool - Sélection multiple ❌ SUPPRIMÉ
**Raison:** Dépendance de TEST-16 (déplacement d'objets non souhaité).

**Statut:** ❌ Supprimé

---

### TEST-18: Select Tool - Rectangle de sélection ✅
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

**Statut:** ✅ VALIDÉ

---

### TEST-19: Delete Objects ✅
**À tester:**
- Supprimer objets sélectionnés

**Procédure:**
1. Sélectionner 1+ objets
2. Appuyer Delete ou Backspace

**Critère de succès:**
- Objets disparaissent
- Canvas mis à jour

**Statut:** ✅ VALIDÉ

---

### TEST-20: Undo/Redo ✅
**À tester:**
- Annuler et refaire actions

**Procédure:**
1. Dessiner un objet
2. Ctrl+Z (undo)
3. Ctrl+Y (redo)

**Critère de succès:**
- Objet disparaît puis réapparaît
- Edit menu montre actions

**Statut:** ✅ VALIDÉ

---

## Phase 4: Transformations

### TEST-21: Rotate Tool ✅
**À tester:**
- Rotation interactive

**Procédure:**
1. Sélectionner une polyligne (Z + clic)
2. Ctrl+R (ou menu Modify → Rotate)
3. Clic + relâche pour définir centre rotation
4. Déplacer souris pour voir preview
5. Tab pour input numérique OU Enter/Clic pour valider
6. (Shift pour snap 15°)

**Critère de succès:**
- Centre de rotation définissable
- Preview en temps réel
- Tab ouvre input numérique fonctionnel
- Enter valide l'angle
- Undo possible

**Statut:** ✅ VALIDÉ

---

### TEST-22: Mirror Tool - Horizontal ✅
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

**Statut:** ✅ VALIDÉ (+ aide contextuelle H/V/C, menu clic-droit, auto-retour Select)

---

### TEST-23: Mirror Tool - Vertical ✅
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

**Statut:** ✅ VALIDÉ (+ aide contextuelle dynamique H/V/C avec actions explicites)

---

### TEST-24: Scale Tool ✅
**À tester:**
- Mise à échelle

**Procédure:**
1. Dessiner un cercle
2. Menu Modify → Scale ou raccourci S
3. Clic-glissé pour agrandir/réduire
4. Ou Tab pour entrer valeur numérique
5. Mode uniforme/non-uniforme avec combo

**Critère de succès:**
- Cercle change de taille ✅
- Pourcentage affiché ✅
- Tab ouvre dialogue avec X/Y séparés ✅
- Mode uniforme/non-uniforme fonctionne ✅
- Undo possible ✅

**Statut:** ✅ VALIDÉ

**Bugs corrigés:**
- Flag uniform passé dans signal avant hide()
- Dialog mode uniform par défaut
- Raccourci S global ajouté

---

### TEST-25: Align - Left ❌
**Statut:** ❌ SUPPRIMÉ - Fonctionnalité non prioritaire

---

### TEST-26: Distribute - Horizontal ❌
**Statut:** ❌ SUPPRIMÉ - Fonctionnalité non prioritaire

---

## Phase 5: Layers

### TEST-27: Layers Panel ✅
**À tester:**
- Créer nouveau layer

**Procédure:**
1. Ouvrir Layers panel (Window → Layers)
2. Clic bouton "+" ou "New Layer"

**Critère de succès:**
- Nouveau layer dans liste ✅
- Nom éditable ✅

**Statut:** ✅ VALIDÉ

---

### TEST-28: Layer - Visibilité ✅
**À tester:**
- Toggle visibilité layer

**Procédure:**
1. Dessiner objets sur layer
2. Dans Layers panel, clic checkbox du layer

**Critère de succès:**
- Objets du layer disparaissent/réapparaissent ✅
- Sélections (highlights verts) cachées avec le layer ✅
- Canvas mis à jour ✅

**Statut:** ✅ VALIDÉ

**Bugs corrigés:**
- LayersPanel utilisait le texte affiché avec compteur au lieu du vrai nom
- Stockage du vrai nom dans Qt::UserRole
- Ajout de vérifications de visibilité dans SelectTool::drawOverlay

---

### TEST-29: Layer - Lock ✅
**À tester:**
- Verrouiller layer

**Procédure:**
1. Dessiner objets
2. Lock le layer
3. Essayer de sélectionner objets
4. Essayer de modifier vertices
5. Essayer de supprimer objets

**Critère de succès:**
- Objets non-sélectionnables ✅
- Vertices non-modifiables ✅
- Aucune modification possible (move, delete, transform) ✅
- Message "Cannot modify object on locked layer" affiché ✅

**Statut:** ✅ VALIDÉ

**Implémentation:**
- Checks ajoutés dans SelectTool pour tous les types d'interactions:
  - Sélection d'objets (clic, rectangle, Space)
  - Sélection de vertices/segments/handles
  - Modifications (G, T, Delete keys)
  - Transformations (rotation, mirror, scale héritent de la sélection bloquée)

---

## Phase 6: Autres Features UI

### TEST-30: Properties Panel ✅
**À tester:**
- Affichage propriétés objet

**Procédure:**
1. Dessiner une polyligne
2. La sélectionner (Z + clic)
3. Observer Properties panel
4. Créer un nouveau layer
5. Changer le layer via Properties panel

**Critère de succès:**
- Panel affiche "Properties (1 object)" ✅
- Name éditable ✅
- Layer dropdown avec tous les layers ✅
- Changement de layer fonctionnel ✅
- Width/Height affichent dimensions bounding box (read-only) ✅

**Statut:** ✅ VALIDÉ

**Note:** PropertiesPanel simplifié - gardé uniquement les champs utiles (Name, Layer, Width, Height). Position X/Y, Line Weight, Line Color et Line Style supprimés.

---

### TEST-31: Parameters Panel ✅
**À tester:**
- Ajouter paramètre

**Procédure:**
1. Ouvrir Parameters panel (Window → Parameters)
2. Vérifier 3 paramètres par défaut (width, height, margin)
3. Clic bouton "Add"
4. Éditer nom, valeur, unité (double-clic cellule)
5. Tester Delete, Duplicate

**Critère de succès:**
- Panel s'ouvre avec 3 paramètres par défaut ✅
- Table avec colonnes : Name, Value, Unit, Expression, Group ✅
- Boutons Add, Delete, Duplicate, Clear All ✅
- Édition inline fonctionne ✅
- Validation nom (pas de doublons) ✅

**Statut:** ✅ VALIDÉ

**Note:** Panel fonctionnel mais pas encore connecté aux pièces (prévu Epic-002 futur). Pour l'instant, sert uniquement à stocker des variables.

---

### TEST-32: Keyboard Shortcuts Dialog ✅
**À tester:**
- Afficher aide raccourcis

**Procédure:**
1. Menu Help → Keyboard Shortcuts
2. OU: F1

**Critère de succès:**
- Dialog s'ouvre ✅
- Liste complète des raccourcis avec catégories ✅
- Formatage HTML avec statuts colorés (✅ Implémenté, 🔜 Planifié, 📋 Futur) ✅
- Raccourci D pour Draw/Polyline ✅
- P réservé pour Create Parallel (futur) ✅
- Bouton Close fonctionne ✅

**Statut:** ✅ VALIDÉ

**Bugs corrigés:** Raccourci P changé en D pour Polyline Tool (Draw)

---

### TEST-33: Preferences Dialog ✅
**À tester:**
- Ouvrir préférences

**Procédure:**
1. Menu Edit → Preferences

**Critère de succès:**
- Dialog s'ouvre ✅
- Onglets visibles (General, Editor, File I/O, Advanced) ✅
- Paramètres éditables ✅
- Boutons OK/Cancel/Apply/Restore Defaults fonctionnels ✅

**Statut:** ✅ VALIDÉ

**Note:** Dialog fonctionnel mais préférences pas encore connectées à l'application (squelette UI pour futur). Les valeurs ne sont pas sauvegardées et n'ont pas d'effet pour l'instant.

---

### TEST-34: Spacebar Grab-and-Move ❌ SUPPRIMÉ
**Raison:** Fonctionnalité non nécessaire. Le pan de la vue se fait déjà avec **clic molette maintenu + glisser** (TEST-04 validé). La barre Espace est utilisée pour "pick and place" dans SelectTool.

**Statut:** ❌ Supprimé

---

## Résumé

**Total tests:** 27 (10 supprimés)
**Réussis:** 17 ✅
**Échoués:** 0 ❌
**Supprimés:** 10 ❌ (Point + Line + 2 Line deps + Circle + Rectangle + 1 Rectangle dep + Déplacement objet + Sélection multiple + Spacebar Grab-and-Move)
**Non testés:** 0 ⏳

**🎉 TOUS LES TESTS SONT TERMINÉS ! 🎉**

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
