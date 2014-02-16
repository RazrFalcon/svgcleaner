<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="fr_FR">
<defaultcodec>UTF-8</defaultcodec>
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="14"/>
        <source>Information</source>
        <translation>Infos</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="45"/>
        <source>About</source>
        <translation>A Propos</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="59"/>
        <source>Authors</source>
        <translation>Auteurs</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="73"/>
        <source>License</source>
        <translation>Licence</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="36"/>
        <source>Generally, SVG files produced by vector editors contain a lot of unused elements and attributes that just blow up their size without providing better visible quality.</source>
        <translation>Le plus souvent, les fichiers SVG produits par les logiciels de dessin vectoriel, contiennent foule d&apos;éléments et d&apos;attributs non utilisés, qui augmentent leur taille sans pour autant en améliorer la qualité.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="40"/>
        <source>SVG Cleaner could help you to clean up your SVG files from unnecessary data. It has a lot of options for cleanup and optimization, works in batch mode, provides threaded processing on the multicore processors and basically does two things:&lt;br /&gt;- removing elements and attributes that don&apos;t contribute to the final rendering;&lt;br /&gt;- making those elements and attributes in use more compact.</source>
        <translation>C&apos;est là qu&apos;intervient SVG CLeaner. Son rôle: vous aider à débarasser vos fichiers .svg(z) , des données inutiles. Il dispose d&apos;un arsenal d&apos;options, aussi bien pour le nettoyage que l&apos;optimisation. En outre, il permet le traitement en lot de données et supporte le mutitâche sur les machines pourvues de plusieurs processeurs. De façon basique il joue deux rôles principaux:&lt;br /&gt;- la suppression des éléments et attributs qui ne contribuent pas au rendu final;&lt;br /&gt;- une utilisation optimale et plus judicieuse des éléménets et attributs contribuant effectivement à la réalisation du rendu final.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="48"/>
        <source>Images cleaned by SVG Cleaner are typically 40-60 percent smaller than the original ones.</source>
        <translation>Les images nettoyées à l&apos;aide de SVG CLeaner sont approximativement 40-60 pour cent plus petites que les originales.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="51"/>
        <source>Important! The internal image viewer in SVG Cleaner uses the QtSvg module for rendering SVG images. Qt supports only the static features of SVG 1.2 Tiny, and that imposes a number of restrictions on rendering of advanced features. For instance, elements such as clipPath, mask, filters etc. will not be rendered at all.</source>
        <translation>Attention! La visionneuse interne incorporée à SVG CLeaner utilise le module QtSvg pour le rendu des images SVG. Qt supporte uniquement les fonctionnalités statiques de SVG 1.2 Tiny, ce qui implique un certain nombre de restrictions quand il s&apos;agit de faire des rendus avancés. Par exemple, les éléments tels que les  chemins de découpe, les masques, les filtres etc. , ne pourront pas être rendus.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="57"/>
        <source>Please send bug reports to </source>
        <translation>Merci de signaler les bugs à</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="63"/>
        <source>Developers:</source>
        <translation>Développeurs:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="66"/>
        <source>Previous developers:</source>
        <translation>Equipe Précédente:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="69"/>
        <source>Special thanks:</source>
        <translation>Spécials Remerciements:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="72"/>
        <source>Logo design:</source>
        <translation>Logo design:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="75"/>
        <source>Translators:</source>
        <translation>Traducteurs:</translation>
    </message>
</context>
<context>
    <name>FilesView</name>
    <message>
        <location filename="../src/gui/filesview.cpp" line="431"/>
        <location filename="../src/gui/filesview.cpp" line="439"/>
        <location filename="../src/gui/filesview.cpp" line="447"/>
        <location filename="../src/gui/filesview.cpp" line="607"/>
        <source>Warning</source>
        <translation>Avertissement</translation>
    </message>
    <message>
        <location filename="../src/gui/filesview.cpp" line="432"/>
        <source>Selected folder is already exist in folder tree.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/filesview.cpp" line="440"/>
        <source>Selected folder does not contains any svg(z) files.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/filesview.cpp" line="448"/>
        <source>Selected file is already exist in files tree.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/filesview.cpp" line="608"/>
        <source>You can drop only svg(z) files or folders.</source>
        <translation>Vous pouvez déposer uniquement des fichiers ou dossiers svg(z).</translation>
    </message>
</context>
<context>
    <name>Keys</name>
    <message>
        <location filename="../src/cli/keys.cpp" line="134"/>
        <source>Remove XML prolog</source>
        <translation>Supprimer le prologue XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="136"/>
        <source>Remove XML comments</source>
        <translation>Supprimer les commentaires XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="138"/>
        <source>Remove XML processing instruction</source>
        <translation>Supprimer les directives de traitement XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="140"/>
        <source>Remove unused elements in &apos;defs&apos; element</source>
        <translation>Supprimer les élémentens unitilisés des éléments &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="142"/>
        <source>Remove non SVG elements</source>
        <translation>Supprimer les éléments non SVG</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="144"/>
        <source>Remove metadata elements</source>
        <translation>Supprimer les métadonnées</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="146"/>
        <source>Remove Inkscape namespaced elements</source>
        <translatorcomment>J&apos;ignore comment traduire namespaced dans ce contexte-ci</translatorcomment>
        <translation>Supprimer les éléments namespaced de Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="148"/>
        <source>Remove SodiPodi namespaced elements</source>
        <translation>Supprimer les éléments namespaced de SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="150"/>
        <source>Remove Adobe Illustrator namespaced elements</source>
        <translation>Supprimer les éléments namespaced de Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="152"/>
        <source>Remove CorelDRAW namespaced elements</source>
        <translation>Supprimer les éléments namespaced de CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="154"/>
        <source>Remove MS Visio namespaced elements</source>
        <translation>Supprimer les éléments namespaced de MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="156"/>
        <source>Remove Sketch namespaced elements</source>
        <translation>Supprimer les éléments namespaced de Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="158"/>
        <source>Remove invisible elements</source>
        <translation>Supprimer les éléments invisibles</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="160"/>
        <source>Remove empty containers elements</source>
        <translation>Supprimer les éléments des conteneurs vides</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="162"/>
        <source>Remove elements which is outside the viewbox</source>
        <translation>Supprimer les éléments situés hors de la zone d&apos;affichage</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="164"/>
        <source>Replace equal elements by the &apos;use&apos;</source>
        <translation>Remplacer les éléments équivalents par le &apos;use&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="166"/>
        <source>Ungroup container elements, when possible</source>
        <translation>Dégrouper si possible, les éléments du conteneur</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="168"/>
        <source>Remove duplicate elements in the &apos;defs&apos; element</source>
        <translation>Supprimer les éléments dupliqués dans l&apos;élément &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="170"/>
        <source>Merge &apos;linearGradient&apos; into &apos;radialGradient&apos;, when possible</source>
        <translation>Insérer si possible, le &apos;linearGradient&apos;dans le &apos;radialGradient&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="172"/>
        <source>Remove Gaussian blur filters with deviation lower than</source>
        <translation>Supprimer les filtres de flou Gaussien ayant un écart inférieur à</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="175"/>
        <source>Remove SVG version</source>
        <translation>Supprimer la version SVG</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="177"/>
        <source>Remove unreferenced id&apos;s</source>
        <translation>Supprimer les identifiants non référencés</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="179"/>
        <source>Trim &apos;id&apos; attributes</source>
        <translation>Ordonner les attributs des &apos;id&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="181"/>
        <source>Keep unreferenced id&apos;s which contains only letters</source>
        <translation>Garder les identifiants non référencés, contenant uniquement des lettres</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="183"/>
        <source>Remove not applied attributes</source>
        <translation>Supprimer les attributs non appliqués</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="185"/>
        <source>Remove attributes with default values</source>
        <translation>Supprimer les attributs ayant les valeurs par défaut</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="187"/>
        <source>Remove Inkscape namespaced attributes</source>
        <translation>Supprimer les attributs namespaced de Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="189"/>
        <source>Remove SodiPodi namespaced attributes</source>
        <translation>Supprimer les attributs namespaced de SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="191"/>
        <source>Remove Adobe Illustrator namespaced attributes</source>
        <translation>Supprimer les attributs namespaced de Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="193"/>
        <source>Remove CorelDRAW namespaced attributes</source>
        <translation>Supprimer les attributs namespaced de CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="195"/>
        <source>Remove MS Visio namespaced attributes</source>
        <translation>Supprimer les attributs namespaced de MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="197"/>
        <source>Remove Sketch namespaced attributes</source>
        <translation>Supprimer les attributs namespaced de Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="199"/>
        <source>Remove stroke properties when no stroking</source>
        <translation>Supprimer les propriétés des tracés en l&apos;absence de tracés</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="201"/>
        <source>Remove fill properties when no filling</source>
        <translation>Supprimer les propriétés de remplissage en l&apos;absence de remplissage</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="203"/>
        <source>Remove XLinks which pointed to nonexistent elements</source>
        <translation>Suprimer les XLinks qui pointent vers des éléments non existants</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="205"/>
        <source>Group elements by style properties</source>
        <translation>Regrouper les éléments par propriété de style</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="207"/>
        <source>Merge style properties into &apos;style&apos; attribute</source>
        <translation>Insérer les propritétés de style dans l&apos;attribut de &apos;style&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="209"/>
        <source>Simplify transform matrices into short equivalent, when possible</source>
        <translation>Simplifier si possible, les matrices de transformation en équivalents plus petits</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="211"/>
        <source>Apply transform matrices to gradients, when possible</source>
        <translation>Appliquer si possible, les matrices de transformation aux dégradés</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="213"/>
        <source>Apply transform matrices to basic shapes, when possible</source>
        <translation>Appliquer si possible les matrices de transformation aux formes basiques</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="216"/>
        <source>Convert absolute coordinates into relative ones</source>
        <translation>Convertir les coordonnées absolues en coordonnées relatives</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="218"/>
        <source>Remove unneeded symbols in &apos;d&apos; attribute</source>
        <translation>Supprimer les symboles inutiles de l&apos;attribut &apos;d&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="220"/>
        <source>Remove tiny or empty segments</source>
        <translation>Supprimer les segments vides ou minuscules</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="222"/>
        <source>Convert segments into shorter equivalent, when possible</source>
        <translation>Convertir si possible, les segments en équivalents plus petits</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="224"/>
        <source>Apply transform matrices, when possible</source>
        <translation>Appliquer si possible les matrices de transformation</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="227"/>
        <source>Convert &apos;height&apos; and &apos;width&apos; attributes into &apos;viewBox&apos; attribute</source>
        <translation>Convertir les attributs &apos;height&apos; et &apos;view&apos; en &apos;viewbox&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="229"/>
        <source>Convert colors into #RRGGBB format</source>
        <translation>Convertir les couleurs au format #RRGGBB</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="231"/>
        <source>Convert #RRGGBB colors into #RGB format, when possible</source>
        <translation>Convertir si possible, les couleurs #RRGGBB au format #RGB</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="233"/>
        <source>Convert &apos;polygon&apos;, &apos;polyline&apos;, &apos;line&apos;, &apos;rect&apos; into paths</source>
        <translation>Convertir &apos;polygon&apos;, &apos;polylin&apos;, &apos;line&apos;, &apos;rect&apos; en chemins</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="235"/>
        <source>Set rounding precision for transformations</source>
        <translation>Définir la précision de l&apos;arrondi pour les transformations</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="237"/>
        <source>Set rounding precision for coordinates</source>
        <translation>Définir la précision de l&apos;arrondi pour les coordonnées</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="239"/>
        <source>Set rounding precision for attributes</source>
        <translation>Définir la précision de l&apos;arrondi pour les attributs</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="241"/>
        <source>Save file with only required whitespace and newlines</source>
        <translatorcomment>J&apos;ignore comment traduire whitespace et newline dans ce contexte-ci</translatorcomment>
        <translation>Sauvegarder les fichiers avec uniquement les whitespace et newlines nécessaires</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="243"/>
        <source>Sort elements by name in &apos;defs&apos; element</source>
        <translation>Trier les éléments par nom dans l&apos;élément &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="455"/>
        <source>&lt;b&gt;Basic&lt;/b&gt; preset are designed to remove all unnecessary data from SVG file, without changing it structure. Allows you to continue editing of the file.&lt;br&gt;Сan not damage your files. Otherwise, please send this files to our email.</source>
        <translation>&lt;b&gt;Par Défaut&lt;/b&gt; Vous permet de débarasser de toutes les données inutiles votre fichier SVG, sans en changer la structure. Vous pourrez ainsi rééditer le fichier plus tard si besoin est.&lt;br&gt;Vos fichiers sont ainsi préservés (pas endommagés). Sinon, merci de nous signaler le probème.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="460"/>
        <source>&lt;b&gt;Complete&lt;/b&gt; preset are designed to create a file which will be used only for showing. This preset completely change file structure, what in most cases prevents future editing.&lt;br&gt;Should not damage your files. Otherwise, please send this files to our email.</source>
        <translation>&lt;b&gt;Complet&lt;/b&gt; Vous permet de créer un fichier exclusivement destiné à l&apos;affichage. Ce préréglage change complètement la structure de votre fichier, ce qui la plupart du temps empêche la réédition.&lt;br&gt;Toutefois, vos fichiers ne devraient pas être endommagés. Sinon, merci de nous signaler le problème.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="464"/>
        <source>&lt;b&gt;Extreme&lt;/b&gt; preset does the same that &lt;b&gt;Complete&lt;/b&gt; do, but also enables some unstable features.&lt;br&gt;It will definitely change displaying of your file and could even damage it.</source>
        <translation>&lt;b&gt;Extrème&lt;/b&gt; Semblable à l&apos;option &lt;b&gt;Complet&lt;/b&gt; cependant, permet l&apos;utilisation de certaines fonctionnalités expérimentales.&lt;br&gt;L&apos;affichage de votre fichier en sera affecté et il pourrait même être endommagé.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="467"/>
        <source>&lt;b&gt;Custom&lt;/b&gt; preset is used to store your own cleaning options. By default all options are off.</source>
        <translation>&lt;b&gt;Personnalisé&lt;/b&gt; Ici, toutes les options sont désactivées par défaut. A utiliser si vous savez ce que vous faites.</translation>
    </message>
</context>
<context>
    <name>LineEdit</name>
    <message>
        <source>%1 files</source>
        <translation type="obsolete">%1 fichiers</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="131"/>
        <source>Statistics</source>
        <translation>Statistiques</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="147"/>
        <source>Files</source>
        <translation>Fichiers</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="153"/>
        <source>Total count:</source>
        <translation>Dénombrement:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="176"/>
        <source>Cleaned:</source>
        <translation>Nettoyé(s):</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="199"/>
        <source>Crashed:</source>
        <translation>Echoué(s):</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="222"/>
        <source>Size before:</source>
        <translation>Taille Avant:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="232"/>
        <source>Size after:</source>
        <translation>Taille Après:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="277"/>
        <source>Cleaned</source>
        <translation>Nettoyé(s)</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="283"/>
        <location filename="../src/gui/mainwindow.ui" line="377"/>
        <source>Maximum:</source>
        <translation>Maximum:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="306"/>
        <location filename="../src/gui/mainwindow.ui" line="397"/>
        <source>Minimum:</source>
        <translation>Minimum:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="329"/>
        <location filename="../src/gui/mainwindow.ui" line="367"/>
        <source>Total:</source>
        <translation>Total:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="361"/>
        <source>Processing time</source>
        <translation>Temps de traitement</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="387"/>
        <source>Average:</source>
        <translation>Moyenne:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="407"/>
        <location filename="../src/gui/mainwindow.ui" line="420"/>
        <location filename="../src/gui/mainwindow.ui" line="433"/>
        <location filename="../src/gui/mainwindow.ui" line="452"/>
        <source>000ms</source>
        <translation>000ms</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="544"/>
        <source>Information</source>
        <translation>Information</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="568"/>
        <source>Compare view</source>
        <translation>Vue comparative</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="62"/>
        <source>Sort by name</source>
        <translation>Trier par nom</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="63"/>
        <source>Sort by size</source>
        <translation>Trier par taille</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="64"/>
        <source>Sort by cleaning</source>
        <translation>Trier par nettoiement</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="65"/>
        <source>Sort by attributes</source>
        <translation>Trier par attributs</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="66"/>
        <source>Sort by elements</source>
        <translation>Trier par objets</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="67"/>
        <source>Sort by time</source>
        <translation>Trier par durée</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="78"/>
        <source>Open the wizard</source>
        <translation>Préférences</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="79"/>
        <source>Start processing</source>
        <translation>Commencer le traitement</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="80"/>
        <source>Pause processing</source>
        <translation>Mettre en pause</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="81"/>
        <source>Stop cleaning</source>
        <translation>Arrêter le nettoyage</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="281"/>
        <source>Compare view: on</source>
        <translation>Vue comparative: activé</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="284"/>
        <source>Compare view: off</source>
        <translation>Vue comparative: désactivé</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="411"/>
        <source>Warning</source>
        <translation>Avertissement</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="412"/>
        <source>You can drop only svg(z) files or folders.</source>
        <translation>Vous pouvez déposer uniquement des fichiers ou dossiers svg(z).</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="429"/>
        <source>Cleaning is not finished.
Did you really want to exit?</source>
        <translation>Le nettoyage n&apos;est pas terminé.
Voulez vous vraiment quitter?</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/gui/someutils.cpp" line="35"/>
        <source>B</source>
        <translation>B</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="35"/>
        <source>KiB</source>
        <translation>KiB</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="36"/>
        <source>MiB</source>
        <translation>MiB</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="46"/>
        <source>h</source>
        <translation>h</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="48"/>
        <source>m</source>
        <translation>m</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="50"/>
        <source>s</source>
        <translation>s</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="51"/>
        <source>ms</source>
        <translation>ms</translation>
    </message>
</context>
<context>
    <name>ThumbWidget</name>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="55"/>
        <source>&lt;b&gt;Name:&lt;/b&gt;</source>
        <translation>&lt;b&gt;Nome:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="93"/>
        <source>Sizes:</source>
        <translation>Tailles:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="109"/>
        <source>Attributes:</source>
        <translation>Attributs:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="125"/>
        <source>Elements:</source>
        <translation>Objets:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="141"/>
        <source>Processing time:</source>
        <translation>Temps de traitement:</translation>
    </message>
</context>
<context>
    <name>WizardDialog</name>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="20"/>
        <source>Wizard - SVG Cleaner</source>
        <translation>Configuratiion - SVG Cleaner</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="107"/>
        <source>Set an output folder</source>
        <translation>Définir un dossier de sortie</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="130"/>
        <source>Output folder:</source>
        <translation>Dossier de sortie:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="215"/>
        <location filename="../src/gui/wizarddialog.cpp" line="283"/>
        <source>filename</source>
        <translation>Nom du fichier</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="228"/>
        <source>For example: </source>
        <translation>Par exemple:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="235"/>
        <source>Set prefix and/or suffix:</source>
        <translation>Configurer les préfixes et/ou suffixes:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="251"/>
        <source>Saving method:</source>
        <translation>Méthode de sauvegarde:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="273"/>
        <source>save cleaned files into the output folder</source>
        <translation>Sauvegarder les fichiers nettoyés dans le dossier de sortie</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="286"/>
        <source>save cleaned files into the current folder</source>
        <translation>Sauvegarder les fichiers nettoyés dans le dossier courant</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="299"/>
        <source>overwrite original files</source>
        <translation>Ecraser les fichiers source</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="320"/>
        <source>Search for SVG files in subfolders</source>
        <translation>Rechercher des fichiers SVG dans les sous-dossiers</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="323"/>
        <source>Recursive folder scanning</source>
        <translation>Analyse récursive des dosssiers</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="342"/>
        <location filename="../src/gui/wizarddialog.cpp" line="602"/>
        <source>Add files</source>
        <translation>Ajouter des fichiers</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="377"/>
        <location filename="../src/gui/wizarddialog.cpp" line="617"/>
        <source>Add folder</source>
        <translation>Ajouter un dossier</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="430"/>
        <source>Preset:</source>
        <translation>Préréglages:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="438"/>
        <location filename="../src/gui/wizarddialog.cpp" line="304"/>
        <location filename="../src/gui/wizarddialog.cpp" line="444"/>
        <source>Basic</source>
        <translation>Par Défaut</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="443"/>
        <location filename="../src/gui/wizarddialog.cpp" line="145"/>
        <location filename="../src/gui/wizarddialog.cpp" line="306"/>
        <location filename="../src/gui/wizarddialog.cpp" line="446"/>
        <location filename="../src/gui/wizarddialog.cpp" line="483"/>
        <source>Complete</source>
        <translation>Complet</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="448"/>
        <location filename="../src/gui/wizarddialog.cpp" line="308"/>
        <location filename="../src/gui/wizarddialog.cpp" line="448"/>
        <source>Extreme</source>
        <translation>Extrème</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="453"/>
        <location filename="../src/gui/wizarddialog.cpp" line="310"/>
        <location filename="../src/gui/wizarddialog.cpp" line="573"/>
        <source>Custom</source>
        <translation>Personnalisé</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="494"/>
        <source>Compressing</source>
        <translation>Compression</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="509"/>
        <source>Compress level:</source>
        <translation>Niveau de compression:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="526"/>
        <source>fastest</source>
        <translation>ultra-rapide</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="531"/>
        <source>fast</source>
        <translation>rapide</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="536"/>
        <source>normal</source>
        <translation>normal</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="541"/>
        <source>maximum</source>
        <translation>maximum</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="546"/>
        <source>ultra</source>
        <translation>ultra</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="563"/>
        <source>Compress all processed files</source>
        <translation>Compresser tous les fichiers traités</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="582"/>
        <source>Compress processed .svgz files only</source>
        <translation>Compresser uniquement les fichiers .svgz traités</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="604"/>
        <source>Multithreading</source>
        <translatorcomment>Please what does this means? I don&apos;t figure out the most accurate translation for itl. So If you could tell me more about the word, I may be able to find its best match.</translatorcomment>
        <translation>Multithreading</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="628"/>
        <source>The number of threads:</source>
        <translatorcomment>Please what does this option do? I&apos;m not sure that I&apos;ve translated the word &quot;threads&quot; very well. So If you could tell me more about the word, I may be able to find its best match.</translatorcomment>
        <translation>Nombre de processus:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="82"/>
        <source>prefix</source>
        <translation>préfixe</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="83"/>
        <source>suffix</source>
        <translation>suffixe</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="91"/>
        <source>Files</source>
        <translation>Fichiers</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="92"/>
        <source>Preferences</source>
        <translation>Préférences</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="93"/>
        <source>Elements</source>
        <translation>Eléments</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="94"/>
        <source>Attributes</source>
        <translation>Attributs</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="95"/>
        <source>Paths</source>
        <translation>Chemins</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="96"/>
        <source>Optimizations</source>
        <translation>Optimisations</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="199"/>
        <source>Additional:</source>
        <translation>Additionnel:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="200"/>
        <source>Options below do not increase cleaning value, but can be useful.</source>
        <translation>Les options ci-dessous n&apos;améliorent pas la valeur du nettoyage, mais peuvent être utiles.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="282"/>
        <source>For example</source>
        <translation>Par exemple</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="499"/>
        <source>Select an output folder</source>
        <translation>Choisir un dossier de sortie</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="510"/>
        <source>No files are selected.</source>
        <translation>Aucun fichier sélectionné.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="513"/>
        <source>An output folder is not selected.</source>
        <translation>Aucun dossier de sortie n&apos;a été sélectionné.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="516"/>
        <source>You have to set a prefix or a suffix for this save method.</source>
        <translation>Vous devrez spécifier un préfixe et un suffixe pour cette méthode de sauvegarde.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="523"/>
        <source>The &apos;svgcleaner-cli&apos; executable is not found.</source>
        <translation>L&apos;exécutable &apos;svgcleaner-cli&apos; est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="530"/>
        <source>The &apos;7za&apos; executable is not found.

You will not be able to clean the SVGZ files.</source>
        <translation>L&apos;exécutable 7za est introuvable.

Impossible de nettoyer les fichiers SVGZ.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="534"/>
        <source>Selected output folder is not writable.</source>
        <translation>Impossible de sauvegarder dans le dossier choisi.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="542"/>
        <source>Warning</source>
        <translation>Avertissement</translation>
    </message>
</context>
</TS>
