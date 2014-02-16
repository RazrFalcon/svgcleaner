<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="uk_UA">
<defaultcodec>UTF-8</defaultcodec>
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="14"/>
        <source>Information</source>
        <translation>Інформація</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="45"/>
        <source>About</source>
        <translation>Про програму</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="59"/>
        <source>Authors</source>
        <translation>Автори</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="73"/>
        <source>License</source>
        <translation>Ліцензія</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="36"/>
        <source>Generally, SVG files produced by vector editors contain a lot of unused elements and attributes that just blow up their size without providing better visible quality.</source>
        <translation>Зазвичай, створені редакторами векторної графіки SVG-файли містять досить велику кількість елементів і атрибутів, які не мають практичного значення і лише надмірно збільшують розмір файлів.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="40"/>
        <source>SVG Cleaner could help you to clean up your SVG files from unnecessary data. It has a lot of options for cleanup and optimization, works in batch mode, provides threaded processing on the multicore processors and basically does two things:&lt;br /&gt;- removing elements and attributes that don&apos;t contribute to the final rendering;&lt;br /&gt;- making those elements and attributes in use more compact.</source>
        <translation>SVG Cleaner може допомогти очистити ваші SVG-файли від зазначених зайвих даних. Він працює у пакетному режимі і, власне кажучи, робить дві речі:&lt;br /&gt;- видаляє елементи й атрибути, які не приймають участь у формуванні кінцевого зображення; &lt;br /&gt;- приводить задіяні елементи й атрибути до більш компактного вигляду.
Програма містить велику кількість опцій чистки та оптимізації, а також забезпечує можливість багатопотокової обробки файлів на багатоядерних процесорах.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="51"/>
        <source>Important! The internal image viewer in SVG Cleaner uses the QtSvg module for rendering SVG images. Qt supports only the static features of SVG 1.2 Tiny, and that imposes a number of restrictions on rendering of advanced features. For instance, elements such as clipPath, mask, filters etc. will not be rendered at all.</source>
        <translation>Увага! Внутрішній переглядач зображень SVG Cleaner використовує для рендерінга модуль QtSvg. На даний час Qt підтримує лише статичну функціональність у межах SVG 1.2 Tiny, що не дозволяє використовувати розширені функції стандарту SVG при формуванні зображень. Тому варто враховувати, що такі елементи, як clipPath (відсічні контури), mask (маски), filter (фільтри) і т.п. взагалі не будуть відображатися.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="48"/>
        <source>Images cleaned by SVG Cleaner are typically 40-60 percent smaller than the original ones.</source>
        <translation>Розмір очищених за допомогою SVG Cleaner зображень зазвичай може бути на 40-60% меншим від початкового значення.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="57"/>
        <source>Please send bug reports to </source>
        <translation>Будь ласка, відправляйте звіти про виявлені помилки на </translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="63"/>
        <source>Developers:</source>
        <translation>Розробники:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="66"/>
        <source>Previous developers:</source>
        <translation>Попередні розробники:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="69"/>
        <source>Special thanks:</source>
        <translation>Особлива подяка:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="72"/>
        <source>Logo design:</source>
        <translation>Дизайн логотипу:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="75"/>
        <source>Translators:</source>
        <translation>Перекладачі:</translation>
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
        <translation>Застереження</translation>
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
        <translation>Ви можете додавати тільки svg(z) файли або папки.</translation>
    </message>
</context>
<context>
    <name>Keys</name>
    <message>
        <location filename="../src/cli/keys.cpp" line="140"/>
        <source>Remove unused elements in &apos;defs&apos; element</source>
        <translation>Видаляти елементи з секції &apos;defs&apos;, що не використовуються</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="134"/>
        <source>Remove XML prolog</source>
        <translation>Видаляти пролог XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="136"/>
        <source>Remove XML comments</source>
        <translation>Видаляти XML коментарі</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="138"/>
        <source>Remove XML processing instruction</source>
        <translation>Видаляти інструкції обробки XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="142"/>
        <source>Remove non SVG elements</source>
        <translation>Видаляти не SVG елементи</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="144"/>
        <source>Remove metadata elements</source>
        <translation>Видаляти елементи метаданих</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="146"/>
        <source>Remove Inkscape namespaced elements</source>
        <translation>Видаляти елементи з простору імен Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="148"/>
        <source>Remove SodiPodi namespaced elements</source>
        <translation>Видаляти елементи з простору імен SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="150"/>
        <source>Remove Adobe Illustrator namespaced elements</source>
        <translation>Видаляти елементи з простору імен Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="152"/>
        <source>Remove CorelDRAW namespaced elements</source>
        <translation>Видаляти елементи з простору імен CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="154"/>
        <source>Remove MS Visio namespaced elements</source>
        <translation>Видаляти елементи з простору імен MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="156"/>
        <source>Remove Sketch namespaced elements</source>
        <translation>Видаляти елементи з простору імен Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="158"/>
        <source>Remove invisible elements</source>
        <translation>Видаляти невидимі елементи</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="160"/>
        <source>Remove empty containers elements</source>
        <translation>Видаляти порожні елементи-контейнери</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="162"/>
        <source>Remove elements which is outside the viewbox</source>
        <translation>Видаляти елементи поза області видимості</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="164"/>
        <source>Replace equal elements by the &apos;use&apos;</source>
        <translation>Замінювати ідентичні елементи на елемент &apos;use&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="166"/>
        <source>Ungroup container elements, when possible</source>
        <translation>Розгруповувати елементи-контейнери, якщо можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="168"/>
        <source>Remove duplicate elements in the &apos;defs&apos; element</source>
        <translation>Видаляти елементи що повторюються в секції &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="170"/>
        <source>Merge &apos;linearGradient&apos; into &apos;radialGradient&apos;, when possible</source>
        <translation>Об&apos;єднувати &apos;linearGradient&apos; та &apos;radialGradient&apos;, коли це можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="172"/>
        <source>Remove Gaussian blur filters with deviation lower than</source>
        <translation>Видаляти фільтри Гаусового розмивання, коли значення нище за</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="175"/>
        <source>Remove SVG version</source>
        <translation>Видаляти версію SVG</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="177"/>
        <source>Remove unreferenced id&apos;s</source>
        <translation>Видаляти зайві id у елементів</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="179"/>
        <source>Trim &apos;id&apos; attributes</source>
        <translation>Скорочувати атрибут &apos;id&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="181"/>
        <source>Keep unreferenced id&apos;s which contains only letters</source>
        <translation>Залишати атрибути &apos;id&apos;, що містять лише літери</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="183"/>
        <source>Remove not applied attributes</source>
        <translation>Видаляти атрибути що не використовуються</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="185"/>
        <source>Remove attributes with default values</source>
        <translation>Видаляти атрибути зі значеннями за замовчуванням</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="187"/>
        <source>Remove Inkscape namespaced attributes</source>
        <translation>Видаляти атрибути з простору імен Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="189"/>
        <source>Remove SodiPodi namespaced attributes</source>
        <translation>Видаляти атрибути з простору імен SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="191"/>
        <source>Remove Adobe Illustrator namespaced attributes</source>
        <translation>Видаляти атрибути з простору імен Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="193"/>
        <source>Remove CorelDRAW namespaced attributes</source>
        <translation>Видаляти атрибути з простору імен CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="195"/>
        <source>Remove MS Visio namespaced attributes</source>
        <translation>Видаляти атрибути з простору імен MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="197"/>
        <source>Remove Sketch namespaced attributes</source>
        <translation>Видаляти атрибути з простору імен Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="199"/>
        <source>Remove stroke properties when no stroking</source>
        <translation>Видаляти властивості обведення, якщо його немає</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="201"/>
        <source>Remove fill properties when no filling</source>
        <translation>Видаляти властивості заповнення, якщо його немає</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="203"/>
        <source>Remove XLinks which pointed to nonexistent elements</source>
        <translation>Видаляти посилання, які вказують на неіснуючі елементи</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="205"/>
        <source>Group elements by style properties</source>
        <translation>Групувати елементи з подібними стилями</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="207"/>
        <source>Merge style properties into &apos;style&apos; attribute</source>
        <translation>Об&apos;єднувати властивості стилю в атрибут &apos;style&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="209"/>
        <source>Simplify transform matrices into short equivalent, when possible</source>
        <translation>Спрощувати матриці трансформацій, коли це можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="211"/>
        <source>Apply transform matrices to gradients, when possible</source>
        <translation>Застосовувати матриці трансформацій до градиентів, коли це можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="213"/>
        <source>Apply transform matrices to basic shapes, when possible</source>
        <translation>Застосовувати матриці трансформацій до базових форм, коли це можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="216"/>
        <source>Convert absolute coordinates into relative ones</source>
        <translation>Переводити абсолютні координати у відносні</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="218"/>
        <source>Remove unneeded symbols in &apos;d&apos; attribute</source>
        <translation>Видаляти непотрібні символи з атрибуту &apos;d&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="220"/>
        <source>Remove tiny or empty segments</source>
        <translation>Видаляти крихітні або порожні сегменти</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="222"/>
        <source>Convert segments into shorter equivalent, when possible</source>
        <translation>Переводити сегменти в короткі еквіваленти, коли це можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="224"/>
        <source>Apply transform matrices, when possible</source>
        <translation>Застосовувати матриці трансформацій, коли це можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="227"/>
        <source>Convert &apos;height&apos; and &apos;width&apos; attributes into &apos;viewBox&apos; attribute</source>
        <translation>Переводити атрибути &apos;height&apos; та &apos;width&apos; в атрибут &apos;viewBox&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="229"/>
        <source>Convert colors into #RRGGBB format</source>
        <translation>Переводити кольори у формат # RRGGBB</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="231"/>
        <source>Convert #RRGGBB colors into #RGB format, when possible</source>
        <translation>Переводити кольори у форматі #RRGGBB у формат #RGB, коли це можливо</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="233"/>
        <source>Convert &apos;polygon&apos;, &apos;polyline&apos;, &apos;line&apos;, &apos;rect&apos; into paths</source>
        <translation>Переводити &apos;polygon&apos;, &apos;polyline&apos;, &apos;line&apos;, &apos;rect&apos; до елементу &apos;path&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="235"/>
        <source>Set rounding precision for transformations</source>
        <translation>Точність округлення для трансформацій</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="237"/>
        <source>Set rounding precision for coordinates</source>
        <translation>Точність округлення для координат</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="239"/>
        <source>Set rounding precision for attributes</source>
        <translation>Точність округлення для атрибутів</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="241"/>
        <source>Save file with only required whitespace and newlines</source>
        <translation>Зберегти файл тільки з необхідними пробілами і символами нового рядка</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="243"/>
        <source>Sort elements by name in &apos;defs&apos; element</source>
        <translation>Відсортувати елементи в секції &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="455"/>
        <source>&lt;b&gt;Basic&lt;/b&gt; preset are designed to remove all unnecessary data from SVG file, without changing it structure. Allows you to continue editing of the file.&lt;br&gt;Сan not damage your files. Otherwise, please send this files to our email.</source>
        <translation>Передустановка &lt;b&gt; Базова &lt;/b&gt; призначена для видалення непотрібної інформації з SVG файлу, без зміни його структури. Надаючи можливість подальшого редагування файлу.&lt;br&gt; Не може зіпсувати Ваш файл. В іншому випадку, будь ласка, надішліть зіпсовані файли нам на email.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="460"/>
        <source>&lt;b&gt;Complete&lt;/b&gt; preset are designed to create a file which will be used only for showing. This preset completely change file structure, what in most cases prevents future editing.&lt;br&gt;Should not damage your files. Otherwise, please send this files to our email.</source>
        <translation>Передустановка &lt;b&gt; Повна &lt;/b&gt; призначена для створення файлу тільки для відображення. Дана предустановка повністю змінює структуру файлу, що, в більшості випадків, перешкоджає подальшому редагуванню.&lt;br&gt; Не повинна зіпсувати Ваш файл. В іншому випадку, будь ласка, надішліть зіпсовані файли нам на email.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="464"/>
        <source>&lt;b&gt;Extreme&lt;/b&gt; preset does the same that &lt;b&gt;Complete&lt;/b&gt; do, but also enables some unstable features.&lt;br&gt;It will definitely change displaying of your file and could even damage it.</source>
        <translation>Передустановка &lt;b&gt; Екстремальна &lt;/b&gt; робить теж що і &lt;b&gt; Повна &lt;/b&gt;, але додатково задіює експерементальні можливості.&lt;br&gt; Файл швидше за все буде викривлений і може навіть повністю зіпсуватися.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="467"/>
        <source>&lt;b&gt;Custom&lt;/b&gt; preset is used to store your own cleaning options. By default all options are off.</source>
        <translation>Передустановка &lt;b&gt; Своя &lt;/b&gt; призначена для зберігання власних настройок очищення. За замовчуванням всі опції відключені.</translation>
    </message>
</context>
<context>
    <name>LineEdit</name>
    <message>
        <source>%1 files</source>
        <translation type="obsolete">%1 файлів</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="131"/>
        <source>Statistics</source>
        <translation>Статистика</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="147"/>
        <source>Files</source>
        <translation>Файли</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="153"/>
        <source>Total count:</source>
        <translation>Загальна кількість:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="176"/>
        <source>Cleaned:</source>
        <translation>Оброблено:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="199"/>
        <source>Crashed:</source>
        <translation>Пропущено:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="222"/>
        <source>Size before:</source>
        <translation>Початковий розмір:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="232"/>
        <source>Size after:</source>
        <translation>Підсумковий розмір:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="277"/>
        <source>Cleaned</source>
        <translation>Очищено</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="283"/>
        <location filename="../src/gui/mainwindow.ui" line="377"/>
        <source>Maximum:</source>
        <translation>Максимум:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="306"/>
        <location filename="../src/gui/mainwindow.ui" line="397"/>
        <source>Minimum:</source>
        <translation>Мінімум:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="329"/>
        <location filename="../src/gui/mainwindow.ui" line="367"/>
        <source>Total:</source>
        <translation>Всього:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="361"/>
        <source>Processing time</source>
        <translation>Час обробки</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="387"/>
        <source>Average:</source>
        <translation>Середнє:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="407"/>
        <location filename="../src/gui/mainwindow.ui" line="420"/>
        <location filename="../src/gui/mainwindow.ui" line="433"/>
        <location filename="../src/gui/mainwindow.ui" line="452"/>
        <source>000ms</source>
        <translation>000мс</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="544"/>
        <source>Information</source>
        <translation>Інформація</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="568"/>
        <source>Compare view</source>
        <translation>Порівняння зображень</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="62"/>
        <source>Sort by name</source>
        <translation>Сортувати за ім&apos;ям</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="63"/>
        <source>Sort by size</source>
        <translation>Сортувати за розміром</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="64"/>
        <source>Sort by cleaning</source>
        <translation>Сортувати за ступенем очищення</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="65"/>
        <source>Sort by attributes</source>
        <translation>Сортувати за атрибутами</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="66"/>
        <source>Sort by elements</source>
        <translation>Сортувати за елементами</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="67"/>
        <source>Sort by time</source>
        <translation>Сортувати за часом</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="78"/>
        <source>Open the wizard</source>
        <translation>Відкрити Майстер</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="79"/>
        <source>Start processing</source>
        <translation>Розпочати обробку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="80"/>
        <source>Pause processing</source>
        <translation>Призупинити обробку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="81"/>
        <source>Stop cleaning</source>
        <translation>Завершити обробку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="281"/>
        <source>Compare view: on</source>
        <translation>Порівняння зображень: увімкнено</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="284"/>
        <source>Compare view: off</source>
        <translation>Порівняння зображень: вимкнено</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="411"/>
        <source>Warning</source>
        <translation>Застереження</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="412"/>
        <source>You can drop only svg(z) files or folders.</source>
        <translation>Ви можете додавати тільки svg(z) файли або папки.</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="429"/>
        <source>Cleaning is not finished.
Did you really want to exit?</source>
        <translation>Очищення ще не закінчилось.
Ви дійсно бажаете вийти?</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/gui/someutils.cpp" line="35"/>
        <source>B</source>
        <translation>Б</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="35"/>
        <source>KiB</source>
        <translation>КіБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="36"/>
        <source>MiB</source>
        <translation>МіБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="46"/>
        <source>h</source>
        <translation>г</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="48"/>
        <source>m</source>
        <translation>х</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="50"/>
        <source>s</source>
        <translation>с</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="51"/>
        <source>ms</source>
        <translation>мс</translation>
    </message>
</context>
<context>
    <name>ThumbWidget</name>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="55"/>
        <source>&lt;b&gt;Name:&lt;/b&gt;</source>
        <translation>&lt;b&gt;Ім&apos;я&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="93"/>
        <source>Sizes:</source>
        <translation>Розміри:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="109"/>
        <source>Attributes:</source>
        <translation>Атрибути:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="125"/>
        <source>Elements:</source>
        <translation>Елементи:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="141"/>
        <source>Processing time:</source>
        <translation>Час обробки:</translation>
    </message>
</context>
<context>
    <name>WizardDialog</name>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="20"/>
        <source>Wizard - SVG Cleaner</source>
        <translation>Майстер - SVG Cleaner</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="107"/>
        <source>Set an output folder</source>
        <translation>Встановити папку для результаів</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="251"/>
        <source>Saving method:</source>
        <translation>Метод збереження:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="342"/>
        <location filename="../src/gui/wizarddialog.cpp" line="602"/>
        <source>Add files</source>
        <translation>Додати файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="377"/>
        <location filename="../src/gui/wizarddialog.cpp" line="617"/>
        <source>Add folder</source>
        <translation>Додати папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="430"/>
        <source>Preset:</source>
        <translation>Передустановка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="438"/>
        <location filename="../src/gui/wizarddialog.cpp" line="304"/>
        <location filename="../src/gui/wizarddialog.cpp" line="444"/>
        <source>Basic</source>
        <translation>Базова</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="443"/>
        <location filename="../src/gui/wizarddialog.cpp" line="145"/>
        <location filename="../src/gui/wizarddialog.cpp" line="306"/>
        <location filename="../src/gui/wizarddialog.cpp" line="446"/>
        <location filename="../src/gui/wizarddialog.cpp" line="483"/>
        <source>Complete</source>
        <translation>Повна</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="448"/>
        <location filename="../src/gui/wizarddialog.cpp" line="308"/>
        <location filename="../src/gui/wizarddialog.cpp" line="448"/>
        <source>Extreme</source>
        <translation>Екстремальна</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="453"/>
        <location filename="../src/gui/wizarddialog.cpp" line="310"/>
        <location filename="../src/gui/wizarddialog.cpp" line="573"/>
        <source>Custom</source>
        <translation>Своя</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="494"/>
        <source>Compressing</source>
        <translation>Стиснення</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="509"/>
        <source>Compress level:</source>
        <translation>Рівень стиснення:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="526"/>
        <source>fastest</source>
        <translation>найшвидший</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="531"/>
        <source>fast</source>
        <translation>швидкий</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="536"/>
        <source>normal</source>
        <translation>звичайний</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="541"/>
        <source>maximum</source>
        <translation>максимальний</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="546"/>
        <source>ultra</source>
        <translation>ультра</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="563"/>
        <source>Compress all processed files</source>
        <translation>Стискати усі оброблені файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="582"/>
        <source>Compress processed .svgz files only</source>
        <translation>Стискати лише оброблені svgz-файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="604"/>
        <source>Multithreading</source>
        <translation>Багатопоточність</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="628"/>
        <source>The number of threads:</source>
        <translation>Кількість потоків:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="215"/>
        <location filename="../src/gui/wizarddialog.cpp" line="283"/>
        <source>filename</source>
        <translation>filename</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="228"/>
        <source>For example: </source>
        <translation>Наприклад: </translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="235"/>
        <source>Set prefix and/or suffix:</source>
        <translation>Додавати префікс та/або суфікс:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="273"/>
        <source>save cleaned files into the output folder</source>
        <translation>зберігати очищені файли у вхідній папці</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="286"/>
        <source>save cleaned files into the current folder</source>
        <translation>зберігати очищені файли у вихідній папці</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="299"/>
        <source>overwrite original files</source>
        <translation>переписати оригінальні файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="130"/>
        <source>Output folder:</source>
        <translation>Вихідна папка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="320"/>
        <source>Search for SVG files in subfolders</source>
        <translation>Обробляти файли із вкладених папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="323"/>
        <source>Recursive folder scanning</source>
        <translation>Рекурсивна перевірка папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="82"/>
        <source>prefix</source>
        <translation>префікс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="83"/>
        <source>suffix</source>
        <translation>суфікс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="91"/>
        <source>Files</source>
        <translation>Файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="92"/>
        <source>Preferences</source>
        <translation>Налаштування</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="93"/>
        <source>Elements</source>
        <translation>Елементи</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="94"/>
        <source>Attributes</source>
        <translation>Атрибути</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="95"/>
        <source>Paths</source>
        <translation>Шляхи</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="96"/>
        <source>Optimizations</source>
        <translation>Оптимізації</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="199"/>
        <source>Additional:</source>
        <translation>Додатково:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="200"/>
        <source>Options below do not increase cleaning value, but can be useful.</source>
        <translation>Дані опції не покращують очищення, але можуть бути корисні.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="510"/>
        <source>No files are selected.</source>
        <translation>Жодного файлу не вибрано.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="523"/>
        <source>The &apos;svgcleaner-cli&apos; executable is not found.</source>
        <translation>Виконуваний файл &apos;svgcleaner-cli&apos; не знайдений.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="530"/>
        <source>The &apos;7za&apos; executable is not found.

You will not be able to clean the SVGZ files.</source>
        <translation>Виконуваний файл &apos;7za&apos; не знайдений.

Ви не зможете обробляти SVGZ файли.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="282"/>
        <source>For example</source>
        <translation>Наприклад</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="499"/>
        <source>Select an output folder</source>
        <translation>Обрати вихідну папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="513"/>
        <source>An output folder is not selected.</source>
        <translation>Не обрано вихідну папку.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="516"/>
        <source>You have to set a prefix or a suffix for this save method.</source>
        <translation>Необхідно задати префікс та/або суфікс.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="534"/>
        <source>Selected output folder is not writable.</source>
        <translation>Обрана вихідна папка недоступна для запису.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="542"/>
        <source>Warning</source>
        <translation>Застереження</translation>
    </message>
</context>
</TS>
