<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="ru_RU">
<defaultcodec>UTF-8</defaultcodec>
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="14"/>
        <source>Information</source>
        <translation>Информация</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="45"/>
        <source>About</source>
        <translation>О программе</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="59"/>
        <source>Authors</source>
        <translation>Авторы</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.ui" line="73"/>
        <source>License</source>
        <translation>Лицензия</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="36"/>
        <source>Generally, SVG files produced by vector editors contain a lot of unused elements and attributes that just blow up their size without providing better visible quality.</source>
        <translation>Как правило, созданные редакторами векторной графики SVG-файлы содержат достаточно большое количество неиспользуемых элементов и атрибутов, которые не имеют какого-либо практического значения и только лишь раздувают размер файлов.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="40"/>
        <source>SVG Cleaner could help you to clean up your SVG files from unnecessary data. It has a lot of options for cleanup and optimization, works in batch mode, provides threaded processing on the multicore processors and basically does two things:&lt;br /&gt;- removing elements and attributes that don&apos;t contribute to the final rendering;&lt;br /&gt;- making those elements and attributes in use more compact.</source>
        <translation>SVG Cleaner может помочь очистить ваши SVG-файлы от указанных неиспользуемых данных. Он работает в пакетном режиме и по сути делает две вещи:&lt;br /&gt;- удаляет элементы и атрибуты, не участвующие в конечном изображении; &lt;br /&gt;- приводит задействованные элементы и атрибуты к более компактному виду.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="51"/>
        <source>Important! The internal image viewer in SVG Cleaner uses the QtSvg module for rendering SVG images. Qt supports only the static features of SVG 1.2 Tiny, and that imposes a number of restrictions on rendering of advanced features. For instance, elements such as clipPath, mask, filters etc. will not be rendered at all.</source>
        <translation>Внимание! Внутренний просмотрщик изображений SVG Cleaner использует для рендеринга модуль QtSvg. На данный момент Qt поддерживает только лишь статическую функциональность в рамках SVG 1.2 Tiny, что не позволяет использовать расширенные функции стандарта SVG при формировании изображений. Поэтому следует учитывать, что такие элементы как clipPath (обтравочные контуры), mask (маски), filter (фильтры) и т.п. вообще не будут отображаться.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="48"/>
        <source>Images cleaned by SVG Cleaner are typically 40-60 percent smaller than the original ones.</source>
        <translation>Размер очищенных при помощи SVG Cleaner изображений обычно может составлять 40-60% от исходного значения.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="57"/>
        <source>Please send bug reports to </source>
        <translation>Пожалуйста, отправляйте отчеты о выявленных ошибках на </translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="63"/>
        <source>Developers:</source>
        <translation>Разработчики:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="66"/>
        <source>Previous developers:</source>
        <translation>Бывшие разработчики:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="69"/>
        <source>Special thanks:</source>
        <translation>Отдельная благодарность:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="72"/>
        <source>Logo design:</source>
        <translation>Дизайн логотипа:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="75"/>
        <source>Translators:</source>
        <translation>Переводчики:</translation>
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
        <translation>Предупреждение</translation>
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
        <translation>Вы можете добавлять только svg(z) файлы или папки.</translation>
    </message>
</context>
<context>
    <name>Keys</name>
    <message>
        <location filename="../src/cli/keys.cpp" line="140"/>
        <source>Remove unused elements in &apos;defs&apos; element</source>
        <translation>Удалять неиспользуемые элементы из секции &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="134"/>
        <source>Remove XML prolog</source>
        <translation>Удалять XML пролог</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="136"/>
        <source>Remove XML comments</source>
        <translation>Удалять XML комментарии</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="138"/>
        <source>Remove XML processing instruction</source>
        <translation>Удалять инструкции обработки XML</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="142"/>
        <source>Remove non SVG elements</source>
        <translation>Удалять не SVG элементы</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="144"/>
        <source>Remove metadata elements</source>
        <translation>Удалять элементы метаданных</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="146"/>
        <source>Remove Inkscape namespaced elements</source>
        <translation>Удалять элементы из пространства имен Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="148"/>
        <source>Remove SodiPodi namespaced elements</source>
        <translation>Удалять элементы из пространства имен SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="150"/>
        <source>Remove Adobe Illustrator namespaced elements</source>
        <translation>Удалять элементы из пространства имен Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="152"/>
        <source>Remove CorelDRAW namespaced elements</source>
        <translation>Удалять элементы из пространства имен CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="154"/>
        <source>Remove MS Visio namespaced elements</source>
        <translation>Удалять элементы из пространства имен MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="156"/>
        <source>Remove Sketch namespaced elements</source>
        <translation>Удалять элементы из пространства имен Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="158"/>
        <source>Remove invisible elements</source>
        <translation>Удалять невидимые элементы</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="160"/>
        <source>Remove empty containers elements</source>
        <translation>Удалять пустые элементы-контейнеры</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="162"/>
        <source>Remove elements which is outside the viewbox</source>
        <translation>Удалять элементы вне области видимости</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="164"/>
        <source>Replace equal elements by the &apos;use&apos;</source>
        <translation>Заменять идентичные элементы на элемент &apos;use&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="166"/>
        <source>Ungroup container elements, when possible</source>
        <translation>Разгруппировывать элементы-контейнеры, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="168"/>
        <source>Remove duplicate elements in the &apos;defs&apos; element</source>
        <translation>Удалять дублирующиеся элементы в секции &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="170"/>
        <source>Merge &apos;linearGradient&apos; into &apos;radialGradient&apos;, when possible</source>
        <translation>Объединять &apos;linearGradient&apos; и &apos;radialGradient&apos;, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="172"/>
        <source>Remove Gaussian blur filters with deviation lower than</source>
        <translation>Удалять фильтры размытия по Гауссу со значением меньше</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="175"/>
        <source>Remove SVG version</source>
        <translation>Удалять версию SVG</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="177"/>
        <source>Remove unreferenced id&apos;s</source>
        <translation>Удалять неиспользуемые id элементов</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="179"/>
        <source>Trim &apos;id&apos; attributes</source>
        <translation>Сокращать атрибут &apos;id&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="181"/>
        <source>Keep unreferenced id&apos;s which contains only letters</source>
        <translation>Оставлять атрибуты &apos;id&apos;, содержащие только буквы</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="183"/>
        <source>Remove not applied attributes</source>
        <translation>Удалять неиспользуемые атрибуты</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="185"/>
        <source>Remove attributes with default values</source>
        <translation>Удалять атрибуты со значениями по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="187"/>
        <source>Remove Inkscape namespaced attributes</source>
        <translation>Удалять атрибуты из пространства имен Inkscape</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="189"/>
        <source>Remove SodiPodi namespaced attributes</source>
        <translation>Удалять атрибуты из пространства имен SodiPodi</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="191"/>
        <source>Remove Adobe Illustrator namespaced attributes</source>
        <translation>Удалять атрибуты из пространства имен Adobe Illustrator</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="193"/>
        <source>Remove CorelDRAW namespaced attributes</source>
        <translation>Удалять атрибуты из пространства имен CorelDRAW</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="195"/>
        <source>Remove MS Visio namespaced attributes</source>
        <translation>Удалять атрибуты из пространства имен MS Visio</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="197"/>
        <source>Remove Sketch namespaced attributes</source>
        <translation>Удалять атрибуты из пространства имен Sketch</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="199"/>
        <source>Remove stroke properties when no stroking</source>
        <translation>Удалять свойства обводки, когда она отсутствует</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="201"/>
        <source>Remove fill properties when no filling</source>
        <translation>Удалять свойства заливки, когда она отсутствует</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="203"/>
        <source>Remove XLinks which pointed to nonexistent elements</source>
        <translation>Удалять ссылки, которые указывают на несуществующие элементы</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="205"/>
        <source>Group elements by style properties</source>
        <translation>Группировать элементы с подобными стилями</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="207"/>
        <source>Merge style properties into &apos;style&apos; attribute</source>
        <translation>Объединять атрибуты стиля в атрибут &apos;style&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="209"/>
        <source>Simplify transform matrices into short equivalent, when possible</source>
        <translation>Упрощать матрицы трансформаций, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="211"/>
        <source>Apply transform matrices to gradients, when possible</source>
        <translation>Применять матрицы трансформаций к градиентам, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="213"/>
        <source>Apply transform matrices to basic shapes, when possible</source>
        <translation>Применять матрицы трансформаций к базовым формам, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="216"/>
        <source>Convert absolute coordinates into relative ones</source>
        <translation>Переводить абсолютные координаты в относительные</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="218"/>
        <source>Remove unneeded symbols in &apos;d&apos; attribute</source>
        <translation>Удалять ненужные символы из атрибута &apos;d&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="220"/>
        <source>Remove tiny or empty segments</source>
        <translation>Удалять крошечные или пустые сегменты</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="222"/>
        <source>Convert segments into shorter equivalent, when possible</source>
        <translation>Переводить сегменты в короткие эквиваленты, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="224"/>
        <source>Apply transform matrices, when possible</source>
        <translation>Применять матрицы трансформаций, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="227"/>
        <source>Convert &apos;height&apos; and &apos;width&apos; attributes into &apos;viewBox&apos; attribute</source>
        <translation>Переводить атрибуты &apos;height&apos; и &apos;width&apos; в атрибут &apos;viewBox&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="229"/>
        <source>Convert colors into #RRGGBB format</source>
        <translation>Переводить цвета в формат #RRGGBB</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="231"/>
        <source>Convert #RRGGBB colors into #RGB format, when possible</source>
        <translation>Переводить цвета в формате #RRGGBB в формат #RGB, когда это возможно</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="233"/>
        <source>Convert &apos;polygon&apos;, &apos;polyline&apos;, &apos;line&apos;, &apos;rect&apos; into paths</source>
        <translation>Переводить &apos;polygon&apos;, &apos;polyline&apos;, &apos;line&apos;, &apos;rect&apos; в пути</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="235"/>
        <source>Set rounding precision for transformations</source>
        <translation>Точность округления для трансформаций</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="237"/>
        <source>Set rounding precision for coordinates</source>
        <translation>Точность округления для координат</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="239"/>
        <source>Set rounding precision for attributes</source>
        <translation>Точность округления для атрибутов</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="241"/>
        <source>Save file with only required whitespace and newlines</source>
        <translation>Сохранить файл только с необходимыми пробелами и символами новой строки</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="243"/>
        <source>Sort elements by name in &apos;defs&apos; element</source>
        <translation>Отсортировать элементы в секции &apos;defs&apos;</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="455"/>
        <source>&lt;b&gt;Basic&lt;/b&gt; preset are designed to remove all unnecessary data from SVG file, without changing it structure. Allows you to continue editing of the file.&lt;br&gt;Сan not damage your files. Otherwise, please send this files to our email.</source>
        <translation>Предустановка &lt;b&gt;Базовая&lt;/b&gt; предназначена для удаления ненужной информации из SVG файла, без изменения его структуры. Предоставляя возможность дальнейшего редактирования файла.&lt;br&gt;Не может испортить Ваш файл. В противном случае, пожалуйста, пришлите испорченные файлы нам на email.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="460"/>
        <source>&lt;b&gt;Complete&lt;/b&gt; preset are designed to create a file which will be used only for showing. This preset completely change file structure, what in most cases prevents future editing.&lt;br&gt;Should not damage your files. Otherwise, please send this files to our email.</source>
        <translation>Предустановка &lt;b&gt;Полная&lt;/b&gt; предназначена для создания файла только для отображения. Данная предустановка полностью меняет структуру файла, что, в большинстве случаев, препятствует дальнейшему редактированию.&lt;br&gt;Не должна испортить Ваш файл. В противном случае, пожалуйста, пришлите испорченные файлы нам на email.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="464"/>
        <source>&lt;b&gt;Extreme&lt;/b&gt; preset does the same that &lt;b&gt;Complete&lt;/b&gt; do, but also enables some unstable features.&lt;br&gt;It will definitely change displaying of your file and could even damage it.</source>
        <translation>Предустановка &lt;b&gt;Экстремальная&lt;/b&gt; делает тоже что и &lt;b&gt;Полная&lt;/b&gt;, но дополнительно задействует экспериментальные возможности.&lt;br&gt;Файла скорее всего будет искажен и может даже полностью испортиться.</translation>
    </message>
    <message>
        <location filename="../src/cli/keys.cpp" line="467"/>
        <source>&lt;b&gt;Custom&lt;/b&gt; preset is used to store your own cleaning options. By default all options are off.</source>
        <translation>Предустановка &lt;b&gt;Своя&lt;/b&gt; предназначена для хранения Ваших собственных настроек очистки. По умолчанию все опции отключены.</translation>
    </message>
</context>
<context>
    <name>LineEdit</name>
    <message>
        <source>%1 files</source>
        <translation type="obsolete">%1 файлов</translation>
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
        <translation>Файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="153"/>
        <source>Total count:</source>
        <translation>Общее количество:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="176"/>
        <source>Cleaned:</source>
        <translation>Очищено:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="199"/>
        <source>Crashed:</source>
        <translation>Пропущено:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="222"/>
        <source>Size before:</source>
        <translation>Начальный размер:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="232"/>
        <source>Size after:</source>
        <translation>Итоговый размер:</translation>
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
        <translation>Минимум:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="329"/>
        <location filename="../src/gui/mainwindow.ui" line="367"/>
        <source>Total:</source>
        <translation>Всего:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="361"/>
        <source>Processing time</source>
        <translation>Время обработки</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="387"/>
        <source>Average:</source>
        <translation>Среднее:</translation>
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
        <translation>Информация</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="568"/>
        <source>Compare view</source>
        <translation>Сравнение изображений</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="62"/>
        <source>Sort by name</source>
        <translation>Сортировать по имени</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="63"/>
        <source>Sort by size</source>
        <translation>Сортировать по размеру</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="64"/>
        <source>Sort by cleaning</source>
        <translation>Сортировать по степени очистки</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="65"/>
        <source>Sort by attributes</source>
        <translation>Сортировать по атрибутам</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="66"/>
        <source>Sort by elements</source>
        <translation>Сортировать по элементам</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="67"/>
        <source>Sort by time</source>
        <translation>Сортировать по времени</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="78"/>
        <source>Open the wizard</source>
        <translation>Открыть Мастер</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="79"/>
        <source>Start processing</source>
        <translation>Начать обработку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="80"/>
        <source>Pause processing</source>
        <translation>Приостановить обработку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="81"/>
        <source>Stop cleaning</source>
        <translation>Stop cleaning</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="281"/>
        <source>Compare view: on</source>
        <translation>Сравнение изображений: включено</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="284"/>
        <source>Compare view: off</source>
        <translation>Сравнение изображений: выключено</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="411"/>
        <source>Warning</source>
        <translation>Предупреждение</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="412"/>
        <source>You can drop only svg(z) files or folders.</source>
        <translation>Вы можете добавлять только svg(z) файлы или папки.</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="429"/>
        <source>Cleaning is not finished.
Did you really want to exit?</source>
        <translation>Очистка не завершена.
Вы действительно хотите выйти?</translation>
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
        <translation>КиБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="36"/>
        <source>MiB</source>
        <translation>МиБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="46"/>
        <source>h</source>
        <translation>ч</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="48"/>
        <source>m</source>
        <translation>м</translation>
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
        <translation>&lt;b&gt;Имя:&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="93"/>
        <source>Sizes:</source>
        <translation>Размеры:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="109"/>
        <source>Attributes:</source>
        <translation>Атрибуты:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="125"/>
        <source>Elements:</source>
        <translation>Элементы:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="141"/>
        <source>Processing time:</source>
        <translation>Время обработки:</translation>
    </message>
</context>
<context>
    <name>WizardDialog</name>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="20"/>
        <source>Wizard - SVG Cleaner</source>
        <translation>Мастер - SVG Cleaner</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="107"/>
        <source>Set an output folder</source>
        <translation>Задать итоговую папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="251"/>
        <source>Saving method:</source>
        <translation>Метод сохранения:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="342"/>
        <location filename="../src/gui/wizarddialog.cpp" line="602"/>
        <source>Add files</source>
        <translation>Добавить файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="377"/>
        <location filename="../src/gui/wizarddialog.cpp" line="617"/>
        <source>Add folder</source>
        <translation>Добавить папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="430"/>
        <source>Preset:</source>
        <translation>Предустановка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="438"/>
        <location filename="../src/gui/wizarddialog.cpp" line="304"/>
        <location filename="../src/gui/wizarddialog.cpp" line="444"/>
        <source>Basic</source>
        <translation>Базовая</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="443"/>
        <location filename="../src/gui/wizarddialog.cpp" line="145"/>
        <location filename="../src/gui/wizarddialog.cpp" line="306"/>
        <location filename="../src/gui/wizarddialog.cpp" line="446"/>
        <location filename="../src/gui/wizarddialog.cpp" line="483"/>
        <source>Complete</source>
        <translation>Полная</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="448"/>
        <location filename="../src/gui/wizarddialog.cpp" line="308"/>
        <location filename="../src/gui/wizarddialog.cpp" line="448"/>
        <source>Extreme</source>
        <translation>Экстремальная</translation>
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
        <translation>Сжатие</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="509"/>
        <source>Compress level:</source>
        <translation>Уровень сжатия:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="526"/>
        <source>fastest</source>
        <translation>самый быстрый</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="531"/>
        <source>fast</source>
        <translation>быстрый</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="536"/>
        <source>normal</source>
        <translation>нормальный</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="541"/>
        <source>maximum</source>
        <translation>максимальный</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="546"/>
        <source>ultra</source>
        <translation>ультра</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="563"/>
        <source>Compress all processed files</source>
        <translation>Сжимать все обработанные файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="582"/>
        <source>Compress processed .svgz files only</source>
        <translation>Сжимать только обработанные svgz-файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="604"/>
        <source>Multithreading</source>
        <translation>Многопоточность</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="628"/>
        <source>The number of threads:</source>
        <translation>Количество потоков:</translation>
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
        <translation>Например:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="235"/>
        <source>Set prefix and/or suffix:</source>
        <translation>Добавлять префикс и/или суффикс:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="273"/>
        <source>save cleaned files into the output folder</source>
        <translation>сохранять очищенные файлы в итоговую папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="286"/>
        <source>save cleaned files into the current folder</source>
        <translation>сохранять очищенные файлы в исходную папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="299"/>
        <source>overwrite original files</source>
        <translation>перезаписывать исходные файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="130"/>
        <source>Output folder:</source>
        <translation>Итоговая папка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="320"/>
        <source>Search for SVG files in subfolders</source>
        <translation>Обрабатывать файлы из вложенных папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="323"/>
        <source>Recursive folder scanning</source>
        <translation>Рекурсивная проверка папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="82"/>
        <source>prefix</source>
        <translation>префикс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="83"/>
        <source>suffix</source>
        <translation>суффикс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="91"/>
        <source>Files</source>
        <translation>Файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="92"/>
        <source>Preferences</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="93"/>
        <source>Elements</source>
        <translation>Элементы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="94"/>
        <source>Attributes</source>
        <translation>Атрибуты</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="95"/>
        <source>Paths</source>
        <translation>Пути</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="96"/>
        <source>Optimizations</source>
        <translation>Оптимизации</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="199"/>
        <source>Additional:</source>
        <translation>Дополнительно:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="200"/>
        <source>Options below do not increase cleaning value, but can be useful.</source>
        <translation>Данные опции не увеличивают степерь очистки, но могут быть полезны.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="510"/>
        <source>No files are selected.</source>
        <translation>Ни одного файла не выбрано.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="523"/>
        <source>The &apos;svgcleaner-cli&apos; executable is not found.</source>
        <translation>Исполняемый файл &apos;svgcleaner-cli&apos; не найден.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="530"/>
        <source>The &apos;7za&apos; executable is not found.

You will not be able to clean the SVGZ files.</source>
        <translation>Исполняемый файл &apos;7za&apos; не найден.

Вы не сможете обрабатывать SVGZ файлы.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="282"/>
        <source>For example</source>
        <translation>Пример</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="499"/>
        <source>Select an output folder</source>
        <translation>Выберите итоговую папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="513"/>
        <source>An output folder is not selected.</source>
        <translation>Не выбрана итоговая папка.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="516"/>
        <source>You have to set a prefix or a suffix for this save method.</source>
        <translation>Необходимо установить префикс или суффикс.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="534"/>
        <source>Selected output folder is not writable.</source>
        <translation>Выбранная итоговая папка недоступна для записи.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="542"/>
        <source>Warning</source>
        <translation>Предупреждение</translation>
    </message>
</context>
</TS>
