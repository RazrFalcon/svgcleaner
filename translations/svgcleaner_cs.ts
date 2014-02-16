<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="cs_CZ">
<defaultcodec>UTF-8</defaultcodec>
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="14"/>
        <source>Information</source>
        <translation>Informace</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="45"/>
        <source>About</source>
        <translation>O</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="59"/>
        <source>Authors</source>
        <translation>Autoři</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="73"/>
        <source>License</source>
        <translation>Povolení</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="36"/>
        <source>Generally, SVG files produced by vector editors contain a lot of unused elements and attributes that just blow up their size without providing better visible quality.</source>
        <translation>Obecně obsahují soubory SVG vytvářené vektorovými editory mnoho nepoužívaných prvků a vlastností, které jen nafukují jejich velikost, ale nezvětšují kvalitu.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="40"/>
        <source>SVG Cleaner could help you to clean up your SVG files from unnecessary data. It has a lot of options for cleanup and optimization, works in batch mode, provides threaded processing on the multicore processors and basically does two things:&lt;br /&gt;- removing elements and attributes that don&apos;t contribute to the final rendering;&lt;br /&gt;- making those elements and attributes in use more compact.</source>
        <translation>Obecně obsahují soubory SVG vytvářené vektorovými editory mnoho nepoužívaných prvků a vlastností, které jen nafukují jejich velikost, ale nezvětšují kvalitu.&lt;br /&gt;SVG Cleaner vám může pomoci soubory SVG očistit od nepotřebných dat. Pracuje v dávkovém režimu a v základě dělá dvě věci:&lt;br /&gt;- odstranění prvků a vlastností, které nepřispívají ke konečnému provedení; &lt;br /&gt;- udělání těchto prvků a vlastností, které jsou doopravdy použity kompaktněji.&lt;br /&gt;Program má hodně možností pro úklid a vyladění a poskytuje vícevláknové zpracování souboru na procesorech s více jádry.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="51"/>
        <source>Important! The internal image viewer in SVG Cleaner uses the QtSvg module for rendering SVG images. Qt supports only the static features of SVG 1.2 Tiny, and that imposes a number of restrictions on rendering of advanced features. For instance, elements such as clipPath, mask, filters etc. will not be rendered at all.</source>
        <translation>Důležité! Vnitřní prohlížeč obrázků v SVG Cleaner používá pro zpracování obrázků SVG QtSvg, protože Qt podporuje jen statické vlastnosti SVG 1.2 Tiny, které zavádí na zpracování pokročilých vlastností mnohá omezení. Například prvky jako clipPath, maska, filtry atd. nebudou zpracovány vůbec.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="48"/>
        <source>Images cleaned by SVG Cleaner are typically 40-60 percent smaller than the original ones.</source>
        <translation>Obvykle jsou obrázky uklizené programem SVG Cleaner menší než 40-60% původní velikosti.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="57"/>
        <source>Please send bug reports to </source>
        <translation>Hlášení o chybách posílejte, prosím, </translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="63"/>
        <source>Developers:</source>
        <translation>Vývojáři:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="66"/>
        <source>Previous developers:</source>
        <translation>Předchozí vývojáři:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="69"/>
        <source>Special thanks:</source>
        <translation>Poděkování:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="72"/>
        <source>Logo design:</source>
        <translation>Návrh symbolu:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="75"/>
        <source>Translators:</source>
        <translation>Překladatelé:</translation>
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
        <translation>Varování</translation>
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
        <translation>Upustit můžete pouze soubory svg(z) nebo složky.</translation>
    </message>
</context>
<context>
    <name>Keys</name>
    <message>
        <location filename="../src/cli/keys.cpp" line="140"/>
        <source>Remove unused elements in &apos;defs&apos; element</source>
        <translation>Odstranit nepoužívané prvky v prvku &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="134"/>
        <source>Remove XML prolog</source>
        <translation>Odstranit prolog XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="136"/>
        <source>Remove XML comments</source>
        <translation>Odstranit poznámky XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="138"/>
        <source>Remove XML processing instruction</source>
        <translation>Odstranit příkaz ke zpracování XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="142"/>
        <source>Remove non SVG elements</source>
        <translation>Odstranit prvky, které nejsou SVG</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="144"/>
        <source>Remove metadata elements</source>
        <translation>Odstranit prvky popisných dat</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="146"/>
        <source>Remove Inkscape namespaced elements</source>
        <translation>Odstranit prvky se jmenným prostorem Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="148"/>
        <source>Remove SodiPodi namespaced elements</source>
        <translation>Odstranit prvky se jmenným prostorem SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="150"/>
        <source>Remove Adobe Illustrator namespaced elements</source>
        <translation>Odstranit prvky se jmenným prostorem Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="152"/>
        <source>Remove CorelDRAW namespaced elements</source>
        <translation>Odstranit prvky se jmenným prostorem CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="154"/>
        <source>Remove MS Visio namespaced elements</source>
        <translation>Odstranit prvky se jmenným prostorem MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="156"/>
        <source>Remove Sketch namespaced elements</source>
        <translation>Odstranit prvky se jmenným prostorem Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="158"/>
        <source>Remove invisible elements</source>
        <translation>Odstranit neviditelné prvky</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="160"/>
        <source>Remove empty containers elements</source>
        <translation>Odstranit prázdné kontejnerové prvky</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="162"/>
        <source>Remove elements which is outside the viewbox</source>
        <translation>Odstranit prvky, jež jsou vně zobrazeného rámečku</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="164"/>
        <source>Replace equal elements by the &apos;use&apos;</source>
        <translation>Nahradit stejné prvky pomocí &apos;use&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="166"/>
        <source>Ungroup container elements, when possible</source>
        <translation>Zrušit seskupení kontejnerových prvků, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="168"/>
        <source>Remove duplicate elements in the &apos;defs&apos; element</source>
        <translation>Odstranit zdvojené prvky v prvku &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="170"/>
        <source>Merge &apos;linearGradient&apos; into &apos;radialGradient&apos;, when possible</source>
        <translation>Sloučit &apos;linearGradient&apos; do &apos;radialGradient&apos;, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="172"/>
        <source>Remove Gaussian blur filters with deviation lower than</source>
        <translation>Odstranit filtry Gaussovského rozmazání s odchylkou menší než</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="175"/>
        <source>Remove SVG version</source>
        <translation>Odstranit verzi SVG</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="177"/>
        <source>Remove unreferenced id&apos;s</source>
        <translation>Odstranit nereferencované ID</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="179"/>
        <source>Trim &apos;id&apos; attributes</source>
        <translation>Oříznout vlastnosti ID</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="181"/>
        <source>Keep unreferenced id&apos;s which contains only letters</source>
        <translation>Zachovat nereferencované ID, které obsahuje pouze písmena</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="183"/>
        <source>Remove not applied attributes</source>
        <translation>Odstranit nepoužité vlastnosti</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="185"/>
        <source>Remove attributes with default values</source>
        <translation>Odstranit vlastnosti s výchozími hodnotami</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="187"/>
        <source>Remove Inkscape namespaced attributes</source>
        <translation>Odstranit vlastnosti se jmenným prostorem Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="189"/>
        <source>Remove SodiPodi namespaced attributes</source>
        <translation>Odstranit vlastnosti se jmenným prostorem SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="191"/>
        <source>Remove Adobe Illustrator namespaced attributes</source>
        <translation>Odstranit vlastnosti  se jmenným prostorem Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="193"/>
        <source>Remove CorelDRAW namespaced attributes</source>
        <translation>Odstranit vlastnosti  se jmenným prostorem CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="195"/>
        <source>Remove MS Visio namespaced attributes</source>
        <translation>Odstranit vlastnosti se jmenným prostorem MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="197"/>
        <source>Remove Sketch namespaced attributes</source>
        <translation>Odstranit vlastnosti se jmenným prostorem Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="199"/>
        <source>Remove stroke properties when no stroking</source>
        <translation>Odstranit vlastnosti tahu, když není žádný tah</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="201"/>
        <source>Remove fill properties when no filling</source>
        <translation>Odstranit vlastnosti výplně, když není žádná výplň</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="203"/>
        <source>Remove XLinks which pointed to nonexistent elements</source>
        <translation>Odstranit XLinky, které ukazovaly na neexistující prvky</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="205"/>
        <source>Group elements by style properties</source>
        <translation>Seskupit prvky podle vlastností stylů</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="207"/>
        <source>Merge style properties into &apos;style&apos; attribute</source>
        <translation>Sloučit vlastností stylů do vlastnosti &apos;style&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="209"/>
        <source>Simplify transform matrices into short equivalent, when possible</source>
        <translation>Zjednodušit transformační matice do krátkého ekvivalentu, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="211"/>
        <source>Apply transform matrices to gradients, when possible</source>
        <translation>Použít transformační matice na přechody, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="213"/>
        <source>Apply transform matrices to basic shapes, when possible</source>
        <translation>Použít transformační matice na základní tvary, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="216"/>
        <source>Convert absolute coordinates into relative ones</source>
        <translation>Převést absolutní souřadnice na relativní</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="218"/>
        <source>Remove unneeded symbols in &apos;d&apos; attribute</source>
        <translation>Odstranit nepotřebné symboly ve vlastnosti &apos;d&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="220"/>
        <source>Remove tiny or empty segments</source>
        <translation>Odstranit nepatrné nebo prázdné segmenty</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="222"/>
        <source>Convert segments into shorter equivalent, when possible</source>
        <translation>Převést segmenty na kratší ekvivalent, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="224"/>
        <source>Apply transform matrices, when possible</source>
        <translation>Použít transformační matice, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="227"/>
        <source>Convert &apos;height&apos; and &apos;width&apos; attributes into &apos;viewBox&apos; attribute</source>
        <translation>Převést vlastnosti &apos;height&apos; (výška) a &apos;width&apos; (šířka) na vlastnost &apos;viewBox&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="229"/>
        <source>Convert colors into #RRGGBB format</source>
        <translation>Převést barvy na formát #RRGGBB</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="231"/>
        <source>Convert #RRGGBB colors into #RGB format, when possible</source>
        <translation>Převést barvy #RRGGBB na formát #RGB, když je to možné</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="233"/>
        <source>Convert &apos;polygon&apos;, &apos;polyline&apos;, &apos;line&apos;, &apos;rect&apos; into paths</source>
        <translation>Převést &apos;polygon&apos; (mnohoúhelník), &apos;polyline&apos; (lomená čára), &apos;line&apos; (čára), &apos;rect&apos; (obdélník) na cesty</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="235"/>
        <source>Set rounding precision for transformations</source>
        <translation>Nastavit přesnost zaokruhlení pro transformace</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="237"/>
        <source>Set rounding precision for coordinates</source>
        <translation>Nastavit přesnost zaokruhlení pro souřadnice</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="239"/>
        <source>Set rounding precision for attributes</source>
        <translation>Nastavit přesnost zaokruhlení pro vlastnosti</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="241"/>
        <source>Save file with only required whitespace and newlines</source>
        <translation>Uložit soubor jen s požadovanými mezerami a konci řádků</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="243"/>
        <source>Sort elements by name in &apos;defs&apos; element</source>
        <translation>Třídit prvky podle názvu v prvku &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="455"/>
        <source>&lt;b&gt;Basic&lt;/b&gt; preset are designed to remove all unnecessary data from SVG file, without changing it structure. Allows you to continue editing of the file.&lt;br&gt;Сan not damage your files. Otherwise, please send this files to our email.</source>
        <translation>&lt;b&gt;Základní&lt;/b&gt; přednastavení je navrženo k odstranění všech nikoli nezbytných dat ze souboru SVG, aniž by došlo ke změně v jeho stavbě. Umožní vám pokračovat v úpravách souboru.&lt;br&gt;Nemůže poškodit vaše soubory. V opačném případě nám, prosím, pošlete takové soubory na naši adresu elektronické pošty.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="460"/>
        <source>&lt;b&gt;Complete&lt;/b&gt; preset are designed to create a file which will be used only for showing. This preset completely change file structure, what in most cases prevents future editing.&lt;br&gt;Should not damage your files. Otherwise, please send this files to our email.</source>
        <translation>&lt;b&gt;Úplné&lt;/b&gt; přednastavení je navrženo k vytvoření souboru, který se bude dát použít jen k zobrazení. Toto přednastavení úplně změní stavbu souboru, což ve většině případů zabrání možnosti jeho úprav v budoucnosti.&lt;br&gt;Nemělo by poškodit vaše soubory. V opačném případě nám, prosím, pošlete takové soubory na naši adresu elektronické pošty.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="464"/>
        <source>&lt;b&gt;Extreme&lt;/b&gt; preset does the same that &lt;b&gt;Complete&lt;/b&gt; do, but also enables some unstable features.&lt;br&gt;It will definitely change displaying of your file and could even damage it.</source>
        <translation>&lt;b&gt;Dokonalé&lt;/b&gt; přednastavení dělá to samé co &lt;b&gt;Úplné&lt;/b&gt;, ale zároveˇn povoluje některé nestabilní funkce&lt;br&gt;Rozhodně dojde ke změně v zobrazení vašeho souboru a dokonce by mohlo dojít k jeho poškození.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="467"/>
        <source>&lt;b&gt;Custom&lt;/b&gt; preset is used to store your own cleaning options. By default all options are off.</source>
        <translation>&lt;b&gt;Vlastní&lt;/b&gt; přednastavení se používá k ukládání vašich vlastních voleb pro úklid. Ve výchozím nastavení jsou všechny volby vypnuty.</translation>
    </message>
</context>
<context>
    <name>LineEdit</name>
    <message>
        <source>%1 files</source>
        <translation type="obsolete">%1 souborů</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="131"/>
        <source>Statistics</source>
        <translation>Statistika</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="277"/>
        <source>Cleaned</source>
        <translation>Vyčištěno</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="329"/>
        <location filename="../src/gui/mainwindow.ui" line="367"/>
        <source>Total:</source>
        <translation>Celkem:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="283"/>
        <location filename="../src/gui/mainwindow.ui" line="377"/>
        <source>Maximum:</source>
        <translation>Nejvíce:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="387"/>
        <source>Average:</source>
        <translation>Průměrně:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="306"/>
        <location filename="../src/gui/mainwindow.ui" line="397"/>
        <source>Minimum:</source>
        <translation>Nejméně:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="147"/>
        <source>Files</source>
        <translation>Soubory</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="153"/>
        <source>Total count:</source>
        <translation>Celkový počet:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="176"/>
        <source>Cleaned:</source>
        <translation>Vyčištěno:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="199"/>
        <source>Crashed:</source>
        <translation>Spadlo:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="222"/>
        <source>Size before:</source>
        <translation>Velikost před:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="232"/>
        <source>Size after:</source>
        <translation>Velikost po:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="361"/>
        <source>Processing time</source>
        <translation>Čas zpracování</translation>
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
        <translation>Informace</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="568"/>
        <source>Compare view</source>
        <translation>Srovnávací pohled</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="62"/>
        <source>Sort by name</source>
        <translation>Třídit podle názvu</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="63"/>
        <source>Sort by size</source>
        <translation>Třídit podle velikosti</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="64"/>
        <source>Sort by cleaning</source>
        <translation>Třídit podle úklidu</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="65"/>
        <source>Sort by attributes</source>
        <translation>Třídit podle vlastností</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="66"/>
        <source>Sort by elements</source>
        <translation>Třídit podle prvků</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="67"/>
        <source>Sort by time</source>
        <translation>Třídit podle času</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="78"/>
        <source>Open the wizard</source>
        <translation>Otevřít průvodce</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="79"/>
        <source>Start processing</source>
        <translation>Spustit zpracování</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="80"/>
        <source>Pause processing</source>
        <translation>Pozastavit zpracování</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="81"/>
        <source>Stop cleaning</source>
        <translation>Zastavit úklid</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="281"/>
        <source>Compare view: on</source>
        <translation>Srovnávací pohled: zapnuto</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="284"/>
        <source>Compare view: off</source>
        <translation>Srovnávací pohled: vypnuto</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="411"/>
        <source>Warning</source>
        <translation>Varování</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="412"/>
        <source>You can drop only svg(z) files or folders.</source>
        <translation>Upustit můžete pouze soubory svg(z) nebo složky.</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="429"/>
        <source>Cleaning is not finished.
Did you really want to exit?</source>
        <translation>Úklid není dokončen.
Skutečně jste chtěl skončit?</translation>
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
        <location filename="../src/gui/thumbwidget.ui" line="125"/>
        <source>Elements:</source>
        <translation>Prvky:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="109"/>
        <source>Attributes:</source>
        <translation>Vlastnosti:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="55"/>
        <source>&lt;b&gt;Name:&lt;/b&gt;</source>
        <translation>&lt;b&gt;Název:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="93"/>
        <source>Sizes:</source>
        <translation>Velikosti:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="141"/>
        <source>Processing time:</source>
        <translation>Čas zpracování:</translation>
    </message>
</context>
<context>
    <name>WizardDialog</name>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="20"/>
        <source>Wizard - SVG Cleaner</source>
        <translation>Průvodce - Uklízeč SVG</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="107"/>
        <source>Set an output folder</source>
        <translation>Nastavit výstupní složku</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="251"/>
        <source>Saving method:</source>
        <translation>Metoda ukládání:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="342"/>
        <location filename="../src/gui/wizarddialog.cpp" line="602"/>
        <source>Add files</source>
        <translation>Přidat soubory</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="377"/>
        <location filename="../src/gui/wizarddialog.cpp" line="617"/>
        <source>Add folder</source>
        <translation>Přidat složku</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="430"/>
        <source>Preset:</source>
        <translation>Přednastavení:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="438"/>
        <location filename="../src/gui/wizarddialog.cpp" line="304"/>
        <location filename="../src/gui/wizarddialog.cpp" line="444"/>
        <source>Basic</source>
        <translation>Základní</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="443"/>
        <location filename="../src/gui/wizarddialog.cpp" line="145"/>
        <location filename="../src/gui/wizarddialog.cpp" line="306"/>
        <location filename="../src/gui/wizarddialog.cpp" line="446"/>
        <location filename="../src/gui/wizarddialog.cpp" line="483"/>
        <source>Complete</source>
        <translation>Úplné</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="448"/>
        <location filename="../src/gui/wizarddialog.cpp" line="308"/>
        <location filename="../src/gui/wizarddialog.cpp" line="448"/>
        <source>Extreme</source>
        <translation>Dokonalé</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="453"/>
        <location filename="../src/gui/wizarddialog.cpp" line="310"/>
        <location filename="../src/gui/wizarddialog.cpp" line="573"/>
        <source>Custom</source>
        <translation>Vlastní</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="509"/>
        <source>Compress level:</source>
        <translation>Úroveň komprimace:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="526"/>
        <source>fastest</source>
        <translation>Nejrychlejší</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="531"/>
        <source>fast</source>
        <translation>Rychle</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="536"/>
        <source>normal</source>
        <translation>Obvyklý</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="541"/>
        <source>maximum</source>
        <translation>Nejvyšší</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="546"/>
        <source>ultra</source>
        <translation>Extrémní</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="563"/>
        <source>Compress all processed files</source>
        <translation>Zabalit všechny zpracované soubory</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="604"/>
        <source>Multithreading</source>
        <translation>Více vláken</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="628"/>
        <source>The number of threads:</source>
        <translation>Počet vláken:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="228"/>
        <source>For example: </source>
        <translation>Například: </translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="235"/>
        <source>Set prefix and/or suffix:</source>
        <translation>Nastavit předponu a/nebo příponu:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="320"/>
        <source>Search for SVG files in subfolders</source>
        <translation>Hledat soubory SVG v podsložkách</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="582"/>
        <source>Compress processed .svgz files only</source>
        <translation>Zabalit pouze zpracované soubory .svgz</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="130"/>
        <source>Output folder:</source>
        <translation>Výstupní složka:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="215"/>
        <location filename="../src/gui/wizarddialog.cpp" line="283"/>
        <source>filename</source>
        <translation>Název souboru</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="323"/>
        <source>Recursive folder scanning</source>
        <translation>Prohledávání složek včetně podsložek</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="273"/>
        <source>save cleaned files into the output folder</source>
        <translation>Uložit uklizené soubory do výstupní složky</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="494"/>
        <source>Compressing</source>
        <translation>Zabalení</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="286"/>
        <source>save cleaned files into the current folder</source>
        <translation>Uložit uklizené soubory do nynější složky</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="299"/>
        <source>overwrite original files</source>
        <translation>Přepsat původní soubory</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="82"/>
        <source>prefix</source>
        <translation>Předpona</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="83"/>
        <source>suffix</source>
        <translation>Přípona</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="91"/>
        <source>Files</source>
        <translation>Soubory</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="92"/>
        <source>Preferences</source>
        <translation>Nastavení</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="96"/>
        <source>Optimizations</source>
        <translation>Optimalizace</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="199"/>
        <source>Additional:</source>
        <translation>Dodatečné:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="200"/>
        <source>Options below do not increase cleaning value, but can be useful.</source>
        <translation>Volby níže nezvyšují hodnotu úklidu, ale mohou být užitečné.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="510"/>
        <source>No files are selected.</source>
        <translation>Nejsou vybrány žádné soubory.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="523"/>
        <source>The &apos;svgcleaner-cli&apos; executable is not found.</source>
        <translation>Spustitelný soubor &apos;svgcleaner-cli&apos; nenalezen.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="530"/>
        <source>The &apos;7za&apos; executable is not found.

You will not be able to clean the SVGZ files.</source>
        <translation>Spustitelný soubor &apos;7za&apos; nenalezen.

Nebudete moci uklízet soubory SVGZ.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="93"/>
        <source>Elements</source>
        <translation>Prvky</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="94"/>
        <source>Attributes</source>
        <translation>Vlastnosti</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="516"/>
        <source>You have to set a prefix or a suffix for this save method.</source>
        <translation>Pro tento způsob ukládání musíte nastavit předponu nebo příponu.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="95"/>
        <source>Paths</source>
        <translation>Cesty</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="282"/>
        <source>For example</source>
        <translation>Například</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="499"/>
        <source>Select an output folder</source>
        <translation>Vybrat výstupní složku</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="513"/>
        <source>An output folder is not selected.</source>
        <translation>Výstupní složka není vybrána.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="534"/>
        <source>Selected output folder is not writable.</source>
        <translation>Vybraná výstupní složka není zapisovatelná.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="542"/>
        <source>Warning</source>
        <translation>Varování</translation>
    </message>
</context>
</TS>
