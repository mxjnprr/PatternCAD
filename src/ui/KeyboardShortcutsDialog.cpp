/**
 * KeyboardShortcutsDialog.cpp
 *
 * Implementation of KeyboardShortcutsDialog
 */

#include "KeyboardShortcutsDialog.h"
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QDialogButtonBox>

namespace PatternCAD {
namespace UI {

KeyboardShortcutsDialog::KeyboardShortcutsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Keyboard Shortcuts"));
    resize(700, 600);
    setupUI();
}

KeyboardShortcutsDialog::~KeyboardShortcutsDialog()
{
}

void KeyboardShortcutsDialog::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Text browser to display shortcuts
    QTextBrowser* browser = new QTextBrowser(this);
    browser->setOpenExternalLinks(false);
    browser->setHtml(getShortcutsHTML());
    layout->addWidget(browser);

    // Close button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::accept);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

QString KeyboardShortcutsDialog::getShortcutsHTML() const
{
    return R"(
<!DOCTYPE html>
<html>
<head>
<style>
    body { font-family: sans-serif; margin: 20px; font-size: 13px; }
    h1 { color: #2c3e50; border-bottom: 2px solid #3498db; padding-bottom: 10px; font-size: 20px; }
    h2 { color: #34495e; margin-top: 20px; margin-bottom: 10px; font-size: 16px; }
    table { border-collapse: collapse; width: 100%; margin-bottom: 15px; font-size: 12px; }
    th { background-color: #3498db; color: white; padding: 6px; text-align: left; font-size: 12px; }
    td { padding: 5px; border-bottom: 1px solid #ddd; }
    tr:hover { background-color: #f5f5f5; }
    .shortcut { font-weight: bold; color: #2980b9; font-family: monospace; white-space: nowrap; }
    .status-ok { color: #27ae60; font-weight: bold; }
    .status-planned { color: #f39c12; font-weight: bold; }
    .status-future { color: #95a5a6; font-weight: bold; }
    .note { background-color: #e8f4f8; padding: 8px; border-left: 4px solid #3498db; margin: 10px 0; font-size: 12px; }
    .warning { background-color: #fff3cd; padding: 8px; border-left: 4px solid #ffc107; margin: 10px 0; font-size: 12px; }
</style>
</head>
<body>

<h1>PatternCAD - Raccourcis Clavier</h1>

<div class="note">
    <strong>ℹ️ Statuts:</strong>
    <span class="status-ok">✅ Implémenté</span> •
    <span class="status-planned">🔜 Planifié</span> •
    <span class="status-future">📋 Futur</span>
</div>

<h2>🔧 Outils de Base</h2>
<table>
    <tr><th>Raccourci</th><th>Outil</th><th>Status</th></tr>
    <tr><td class="shortcut">Z</td><td>Select tool - Sélection et manipulation</td><td class="status-ok">✅</td></tr>
    <tr><td class="shortcut">D</td><td>Draw/Polyline - Dessiner polylignes avec courbes</td><td class="status-ok">✅</td></tr>
    <tr><td class="shortcut">L</td><td>Line (temporaire) - Sera "Pleat"</td><td class="status-ok">✅</td></tr>
    <tr><td class="shortcut">O</td><td>Add Point on Contour - Modifier forme de ligne</td><td class="status-planned">🔜</td></tr>
    <tr><td class="shortcut">C</td><td>Circle (temporaire) - Sera "Cut a Piece"</td><td class="status-ok">✅</td></tr>
</table>

<h2>🖱️ Outil Select (Z)</h2>
<table>
    <tr><th>Raccourci</th><th>Action</th></tr>
    <tr><td class="shortcut">Clic</td><td>Sélectionner un objet</td></tr>
    <tr><td class="shortcut">Clic-glissé</td><td>Sélection par rectangle (bounding box)</td></tr>
    <tr><td class="shortcut">Ctrl+Clic</td><td>Ajouter/retirer un objet de la sélection</td></tr>
    <tr><td class="shortcut">Espace</td><td>Ramasser/poser les objets sélectionnés (pick & place)</td></tr>
    <tr><td class="shortcut">G</td><td>Grab - Déplacer un vertex sélectionné</td></tr>
    <tr><td class="shortcut">Clic droit</td><td>Menu contextuel (changer layer, supprimer)</td></tr>
    <tr><td class="shortcut">Suppr</td><td>Supprimer les objets sélectionnés</td></tr>
    <tr><td class="shortcut">Échap</td><td>Annuler l'action en cours / Désélectionner</td></tr>
</table>

<h2>✏️ Outil Draw/Polyline (D)</h2>
<table>
    <tr><th>Raccourci</th><th>Action</th></tr>
    <tr><td class="shortcut">Clic</td><td>Ajouter un point pointu (angle)</td></tr>
    <tr><td class="shortcut">Clic-glissé</td><td>Ajouter un point courbe (distance = tension)</td></tr>
    <tr><td class="shortcut">Double-clic</td><td>Fermer la polyligne</td></tr>
    <tr><td class="shortcut">Entrée</td><td>Fermer la polyligne</td></tr>
    <tr><td class="shortcut">Échap</td><td>Annuler la polyligne en cours</td></tr>
</table>

<h2>🔄 Outils de Transformation</h2>
<table>
    <tr><th>Raccourci</th><th>Outil</th><th>Status</th></tr>
    <tr><td class="shortcut">R</td><td>Rotate - Rotation des objets sélectionnés</td><td class="status-ok">✅</td></tr>
    <tr><td class="shortcut">M</td><td>Mirror - Effet miroir (H/V/Custom)</td><td class="status-ok">✅</td></tr>
    <tr><td class="shortcut">S</td><td>Scale - Mise à échelle (uniforme/non-uniforme)</td><td class="status-ok">✅</td></tr>
</table>

<h2>📋 Outils Planifiés</h2>
<table>
    <tr><th>Raccourci</th><th>Outil</th></tr>
    <tr><td class="shortcut">O</td><td>Add Point on Contour</td></tr>
    <tr><td class="shortcut">B</td><td>Build Piece - Former une nouvelle pièce</td></tr>
    <tr><td class="shortcut">E</td><td>Extend Internal - Étendre contour interne</td></tr>
    <tr><td class="shortcut">H</td><td>Set Half Piece Line</td></tr>
    <tr><td class="shortcut">I</td><td>Move Internal - Déplacer objet interne</td></tr>
    <tr><td class="shortcut">J</td><td>Join Pieces - Joindre deux pièces</td></tr>
    <tr><td class="shortcut">N</td><td>Add Notch - Ajouter un cran</td></tr>
    <tr><td class="shortcut">P</td><td>Create Parallel - Contour parallèle</td></tr>
    <tr><td class="shortcut">Q</td><td>Multi Move - Déplacer par rectangle</td></tr>
    <tr><td class="shortcut">T</td><td>Text - Ajouter/éditer du texte</td></tr>
    <tr><td class="shortcut">Ctrl+Alt+C</td><td>Circle - Créer un cercle interne</td></tr>
    <tr><td class="shortcut">Ctrl+Alt+D</td><td>Add or Rotate Dart - Créer/pivoter une pince</td></tr>
    <tr><td class="shortcut">Ctrl+D</td><td>Measure - Mesurer des distances</td></tr>
</table>

<h2>🔍 Vue et Navigation</h2>
<table>
    <tr><th>Raccourci</th><th>Action</th></tr>
    <tr><td class="shortcut">Ctrl++</td><td>Zoom avant</td></tr>
    <tr><td class="shortcut">Ctrl+-</td><td>Zoom arrière</td></tr>
    <tr><td class="shortcut">F</td><td>Zoom pour tout afficher</td></tr>
    <tr><td class="shortcut">Molette</td><td>Zoom</td></tr>
    <tr><td class="shortcut">Clic milieu+glisser</td><td>Déplacer la vue (pan)</td></tr>
</table>

<h2>📁 Fichier</h2>
<table>
    <tr><th>Raccourci</th><th>Action</th></tr>
    <tr><td class="shortcut">Ctrl+N</td><td>Nouveau document</td></tr>
    <tr><td class="shortcut">Ctrl+O</td><td>Ouvrir un document</td></tr>
    <tr><td class="shortcut">Ctrl+S</td><td>Sauvegarder</td></tr>
    <tr><td class="shortcut">Ctrl+Shift+S</td><td>Sauvegarder sous...</td></tr>
    <tr><td class="shortcut">Ctrl+Q</td><td>Quitter</td></tr>
</table>

<h2>✂️ Édition</h2>
<table>
    <tr><th>Raccourci</th><th>Action</th><th>Status</th></tr>
    <tr><td class="shortcut">Ctrl+Z</td><td>Undo - Annuler</td><td class="status-ok">✅</td></tr>
    <tr><td class="shortcut">Ctrl+Shift+Z</td><td>Redo - Refaire</td><td class="status-ok">✅</td></tr>
    <tr><td class="shortcut">Ctrl+C</td><td>Copy - Copier</td><td class="status-planned">🔜</td></tr>
    <tr><td class="shortcut">Ctrl+X</td><td>Cut - Couper</td><td class="status-planned">🔜</td></tr>
    <tr><td class="shortcut">Ctrl+V</td><td>Paste - Coller</td><td class="status-planned">🔜</td></tr>
</table>

<h2>📐 Raccourcis Avancés</h2>
<table>
    <tr><th>Catégorie</th><th>Exemples de raccourcis</th></tr>
    <tr><td><strong>Pliage</strong></td><td>Shift+F, Ctrl+Shift+F, H, Shift+H, Ctrl+H</td></tr>
    <tr><td><strong>Gradation</strong></td><td>Shift+C, Shift+V, Shift+X, Shift+Y</td></tr>
    <tr><td><strong>Placement</strong></td><td>Ctrl+K, Ctrl+Shift+K, Ctrl+L</td></tr>
    <tr><td><strong>Guides</strong></td><td>Ctrl+G, Ctrl+Alt+G, Ctrl+Shift+G</td></tr>
    <tr><td><strong>Track Lines</strong></td><td>Ctrl+T, Ctrl+Alt+T, Ctrl+Shift+Alt+T</td></tr>
</table>

<div class="note">
    <strong>💡 Astuce:</strong> Après avoir créé un objet avec un outil, PatternCAD revient automatiquement sur l'outil Select (Z).
</div>

<div class="warning">
    <strong>⚠️ Note:</strong> Certains raccourcis sont temporaires pendant le développement. Consulter le fichier KEYBOARD_SHORTCUTS.md pour la liste complète et les statuts.
</div>

</body>
</html>
    )";
}

} // namespace UI
} // namespace PatternCAD
