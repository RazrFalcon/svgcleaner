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
        <location filename="../src/gui/aboutdialog.cpp" line="38"/>
        <source>Generally, SVG files produced by vector editors contain a lot of unused elements and attributes that just blow up their size without providing better visible quality.</source>
        <translation>Как правило, созданные редакторами векторной графики SVG-файлы содержат достаточно большое количество неиспользуемых элементов и атрибутов, которые не имеют какого-либо практического значения и только лишь раздувают размер файлов.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="42"/>
        <source>SVG Cleaner could help you to clean up your SVG files from unnecessary data. It has a lot of options for cleanup and optimization, works in batch mode, provides threaded processing on the multicore processors and basically does two things:&lt;br /&gt;- removing elements and attributes that don&apos;t contribute to the final rendering;&lt;br /&gt;- making those elements and attributes in use more compact.</source>
        <translation>SVG Cleaner может помочь очистить ваши SVG-файлы от указанных неиспользуемых данных. Он работает в пакетном режиме и по сути делает две вещи:&lt;br /&gt;- удаляет элементы и атрибуты, не участвующие в конечном изображении; &lt;br /&gt;- приводит задействованные элементы и атрибуты к более компактному виду.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="50"/>
        <source>Images cleaned by SVG Cleaner are typically 10-60 percent smaller than the original ones.</source>
        <translation>Размер очищенных при помощи SVG Cleaner изображений обычно может составлять 10-60% от исходного значения.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="53"/>
        <source>Important! The internal image viewer in SVG Cleaner uses the QtSvg module for rendering SVG images. Qt supports only the static features of SVG 1.2 Tiny, and that imposes a number of restrictions on rendering of advanced features. For instance, elements such as clipPath, mask, filters etc. will not be rendered at all.</source>
        <translation>Внимание! Внутренний просмотрщик изображений SVG Cleaner использует для рендеринга модуль QtSvg. На данный момент Qt поддерживает только лишь статическую функциональность в рамках SVG 1.2 Tiny, что не позволяет использовать расширенные функции стандарта SVG при формировании изображений. Поэтому следует учитывать, что такие элементы как clipPath (обтравочные контуры), mask (маски), filter (фильтры) и т.п. вообще не будут отображаться.</translation>
    </message>
    <message>
        <source>We apologize for any bugs in advance. Please send bug reports to </source>
        <translation type="obsolete">Мы заранее приносим свои извинения за возможные ошибки. Пожалуйста, отправляйте отчеты о выявленных ошибках на </translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="59"/>
        <source>Please send bug reports to </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="65"/>
        <source>Developers:</source>
        <translation>Разработчики:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="68"/>
        <source>Previous developers:</source>
        <translation>Бывшие разработчики:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="71"/>
        <source>Special thanks:</source>
        <translation>Отдельная благодарность:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="74"/>
        <source>Logo design:</source>
        <translation>Дизайн логотипа:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="77"/>
        <source>Translators:</source>
        <translation>Переводчики:</translation>
    </message>
</context>
<context>
    <name>CleanerThread</name>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="144"/>
        <location filename="../src/gui/cleanerthread.cpp" line="153"/>
        <source>Crashed</source>
        <translation>Очистка завершилась неудачно</translation>
    </message>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="147"/>
        <source>Input file does not exist.</source>
        <translation>Исходный файл не существует.</translation>
    </message>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="149"/>
        <source>Output folder does not exist.</source>
        <translation>Итоговая папка не существует.</translation>
    </message>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="151"/>
        <source>It&apos;s a not well-formed SVG file.</source>
        <translation>Некорректный формат файла.</translation>
    </message>
</context>
<context>
    <name>LineEdit</name>
    <message>
        <location filename="../src/gui/lineedit.cpp" line="66"/>
        <source>%1 files</source>
        <translation>%1 файлов</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="124"/>
        <source>Statistics</source>
        <translation>Статистика</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="140"/>
        <source>Files</source>
        <translation>Файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="146"/>
        <source>Total count:</source>
        <translation>Общее количество:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="169"/>
        <source>Cleaned:</source>
        <translation>Обработано:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="192"/>
        <source>Crashed:</source>
        <translation>Не подлежит обработке:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="215"/>
        <source>Size before:</source>
        <translation>Начальный размер:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="225"/>
        <source>Size after:</source>
        <translation>Итоговый размер:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="270"/>
        <source>New file sizes</source>
        <translation>Размер обработанных файлов</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="276"/>
        <location filename="../src/gui/mainwindow.ui" line="370"/>
        <source>Maximum:</source>
        <translation>Максимальное значение:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="299"/>
        <location filename="../src/gui/mainwindow.ui" line="390"/>
        <source>Minimum:</source>
        <translation>Минимальное значение:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="322"/>
        <location filename="../src/gui/mainwindow.ui" line="360"/>
        <source>Total:</source>
        <translation>Итоговое значение:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="354"/>
        <source>Processing time</source>
        <translation>Время обработки файлов</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="380"/>
        <source>Average:</source>
        <translation>Среднее значение:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="400"/>
        <location filename="../src/gui/mainwindow.ui" line="413"/>
        <location filename="../src/gui/mainwindow.ui" line="426"/>
        <location filename="../src/gui/mainwindow.ui" line="445"/>
        <source>000ms</source>
        <translation>000мс</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="537"/>
        <source>Information</source>
        <translation>Информация</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="561"/>
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
        <source>Sort by compression</source>
        <translation>Сортировать по очистке</translation>
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
        <location filename="../src/gui/mainwindow.cpp" line="74"/>
        <source>Open the wizard</source>
        <translation>Открыть Мастер</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="75"/>
        <source>Start processing</source>
        <translation>Начать обработку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="76"/>
        <source>Pause processing</source>
        <translation>Pause processing</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="77"/>
        <source>Stop cleaning</source>
        <translation>Stop cleaning</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="307"/>
        <source>Compare view: on</source>
        <translation>Сравнение изображений: включено</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="310"/>
        <source>Compare view: off</source>
        <translation>Сравнение изображений: выключено</translation>
    </message>
</context>
<context>
    <name>SomeUtils</name>
    <message>
        <location filename="../src/gui/someutils.cpp" line="43"/>
        <source>B</source>
        <translation>Б</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="43"/>
        <source>KiB</source>
        <translation>КиБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="43"/>
        <source>MiB</source>
        <translation>МиБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="53"/>
        <source>%1h %2m %3s %4ms</source>
        <translation>%1ч %2м %3с %4мс</translation>
    </message>
</context>
<context>
    <name>ThumbWidget</name>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="90"/>
        <source>Sizes:</source>
        <translation>Размеры:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="106"/>
        <source>Attributes:</source>
        <translation>Атрибуты:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="122"/>
        <source>Elements:</source>
        <translation>Элементы:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="138"/>
        <source>Processing time:</source>
        <translation>Время обработки:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.cpp" line="37"/>
        <source>Name:</source>
        <translation>Имя:</translation>
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
        <location filename="../src/gui/wizarddialog.ui" line="65"/>
        <source>Compressing</source>
        <translation>Сжатие</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="80"/>
        <source>Compress level:</source>
        <translation>Уровень сжатия:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="97"/>
        <source>fastest</source>
        <translation>самый быстрый</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="102"/>
        <source>fast</source>
        <translation>быстрый</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="107"/>
        <source>normal</source>
        <translation>нормальный</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="112"/>
        <source>maximum</source>
        <translation>максимальный</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="117"/>
        <source>ultra</source>
        <translation>ультра</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="134"/>
        <source>Compress all processed files</source>
        <translation>Сжимать все обработанные файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="153"/>
        <source>Compress processed .svgz files only</source>
        <translation>Сжимать только обработанные svgz-файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="175"/>
        <source>Multithreading</source>
        <translation>Многопоточность</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="199"/>
        <source>The number of threads:</source>
        <translation>Количество потоков:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="249"/>
        <source>Open files from the input folder and:</source>
        <translation>Открывать файлы из исходной папки и:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="325"/>
        <location filename="../src/gui/wizarddialog.cpp" line="184"/>
        <source>filename</source>
        <translation>filename</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="332"/>
        <source>For example: </source>
        <translation>Например:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="339"/>
        <source>Set prefix and/or suffix:</source>
        <translation>Добавлять префикс и/или суффикс:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="364"/>
        <source>save cleaned files into the output folder</source>
        <translation>сохранять очищенные файлы в итоговую папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="377"/>
        <source>save cleaned files into the current folder</source>
        <translation>сохранять очищенные файлы в исходную папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="390"/>
        <source>overwrite original files</source>
        <translation>перезаписывать исходные файлы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="443"/>
        <source>Open an input folder</source>
        <translation>Открыть исходную папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="468"/>
        <source>Input folder:</source>
        <translation>Исходная папка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="511"/>
        <source>Open an output folder</source>
        <translation>Открыть итоговую папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="530"/>
        <source>Output folder:</source>
        <translation>Итоговая папка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="549"/>
        <source>Search for SVG files in subfolders</source>
        <translation>Обрабатывать файлы из вложенных папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="552"/>
        <source>Recursive folder scanning</source>
        <translation>Рекурсивная проверка папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="589"/>
        <source>Select a preset:</source>
        <translation>Выбранный пресет:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="614"/>
        <source>Enter preset name:</source>
        <translation>Введите имя пресета:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="624"/>
        <source>Save</source>
        <translation>Сохранить</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="650"/>
        <source>Remove</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="670"/>
        <source>Preset description:</source>
        <translation>Описание пресета:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="708"/>
        <source>Remove prolog</source>
        <translation>Удалять пролог</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="721"/>
        <source>Remove comments</source>
        <translation>Удалять комментарии</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="734"/>
        <source>Remove processing instruction</source>
        <translation>Удалять инструкции обработки</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="747"/>
        <source>Remove unused definitions</source>
        <translation>Удалять неиспользуемые определения</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="760"/>
        <source>Remove non-SVG elements</source>
        <translation>Удалять не-SVG элементы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="773"/>
        <source>Remove metadata elements</source>
        <translation>Удалять элементы метаданных</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="783"/>
        <source>Remove elements from namespaces by the following graphics editors:</source>
        <translation>Удалять элементы из пространств имен графических редакторов:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1129"/>
        <source>Remove IDs that contain only letters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1326"/>
        <source>skip-ids-trim</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1329"/>
        <source>Trim the id attributes into hexadecimal format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1515"/>
        <source>Convert &apos;curveto&apos; segment to short one, when possible</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Merge multiply matrices into one and simplify it</source>
        <translation type="obsolete">Объединять матрицы трансформаций и упрощать их</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1502"/>
        <source>Remove empty segments</source>
        <translation>Удалять пустые сегменты</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="879"/>
        <source>Remove invisible elements</source>
        <translation>Удалять неотображаемые элементы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="892"/>
        <source>Remove empty container elements</source>
        <translation>Удалять пустые элементы-контейнеры</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="905"/>
        <source>Collapse groups when possible</source>
        <translation>Разгруппировывать группы, когда возможно</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="918"/>
        <source>Remove duplicated definitions</source>
        <translation>Удалять дублирующиеся определения</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="931"/>
        <source>Remove gradients that are only referenced by one other gradient</source>
        <translation>Объединять градиенты, когда возможно</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="944"/>
        <source>Remove Gaussian blur filters</source>
        <translation>Удалять фильтры размытия по Гауссу</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="987"/>
        <source>if their standard deviation is less than:</source>
        <translation>если их стандартное отклонение меньше чем:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1073"/>
        <source>Remove the SVG language version</source>
        <translation>Удалять версию языка SVG</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1086"/>
        <source>Remove unreferenced IDs</source>
        <translation>Удалять неиспользуемые идентификаторы</translation>
    </message>
    <message>
        <source>Always keep IDs that contain only letters</source>
        <translation type="obsolete">Оставлять идентификаторы, содержащие только буквы</translation>
    </message>
    <message>
        <source>Remove non-SVG attributes</source>
        <translation type="obsolete">Удалять не-SVG атрибуты</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1145"/>
        <source>Remove attributes that are not applicable to elements</source>
        <translation>Удалять атрибуты, которые не используются элементами</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1158"/>
        <source>Remove attributes with default SVG values</source>
        <translation>Удалять атрибуты со значениями по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1168"/>
        <source>Remove attributes from namespaces by the following graphics editors:</source>
        <translation>Удалять атрибуты из пространств имен графических редакторов:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1264"/>
        <source>Remove all stroke properties when there&apos;s no stroking</source>
        <translation>Удалять все свойства обводки, когда она отсутствует</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1277"/>
        <source>Remove all fill properties when there&apos;s no filling</source>
        <translation>Удалять все свойства заливки, когда она отсутствует</translation>
    </message>
    <message>
        <source>Remove all unused properties from children of clipPath elements</source>
        <translation type="obsolete">Удалять все лишние свойства у потомков элементов clipPath</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1290"/>
        <source>Remove gradient coordinates when possible</source>
        <translation>Удалять координаты градиентов, когда возможно</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1303"/>
        <source>Remove XLinks which pointed to nonexistent element</source>
        <translation>Удалять XLinks, которые указывают на несуществующие елементы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1316"/>
        <source>Group elements by style properties</source>
        <translation>Группировать элементы с подобными стилями</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1393"/>
        <source>Keep existing paths data</source>
        <translation>Оставлять существующие данные путей</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1433"/>
        <location filename="../src/gui/wizarddialog.ui" line="1489"/>
        <source>Remove unnecessary whitespace between commands and coordinates</source>
        <translation>Удалять лишние пробелы между командами и координатами</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1452"/>
        <source>Convert absolute paths into relative ones</source>
        <translation>Преобразовывать абсолютные пути в относительные</translation>
    </message>
    <message>
        <source>Convert lines into horizontal/vertical equivalents when possible</source>
        <translation type="obsolete">Преобразовывать линии в короткие гор./верт. эквиваленты</translation>
    </message>
    <message>
        <source>Convert straight curves into lines when possible</source>
        <translation type="obsolete">Преобразовывать прямые кривые в линии</translation>
    </message>
    <message>
        <source>Convert cubic curve segments into shorthand equivalents when possible</source>
        <translation type="obsolete">Преобразовывать сегменты кубических кривых в короткие эквиваленты</translation>
    </message>
    <message>
        <source>Convert quadratic curve segments into shorthand equivalents when possible</source>
        <translation type="obsolete">Преобразовывать сегменты квадратичных кривых в короткие эквиваленты</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1576"/>
        <source>Convert width/height into a viewBox when possible</source>
        <translation>Преобразовывать атрибуты width и height во viewBox</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1589"/>
        <source>Convert style properties into SVG attributes</source>
        <translation>Преобразовывать свойства стилей в атрибуты SVG</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1602"/>
        <source>Convert colors to #RRGGBB format</source>
        <translation>Преобразовывать цвета в формат #RRGGBB</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1633"/>
        <source>Convert colors to #RGB format when possible</source>
        <translation>Преобразовывать цвета в формат #RGB, когда возможно</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1649"/>
        <source>Convert basic shapes into paths</source>
        <translation>Преобразовывать основные фигуры в пути</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1662"/>
        <source>Apply transform matrices to the elements when possible</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Recalculate coordinates and remove transform attributes when possible</source>
        <translation type="obsolete">Перерасчитывать координаты и удалять атрибуты трансформации</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1675"/>
        <source>Sort elements by name inside the defs section</source>
        <translation>Сортировать по имени элементы из секции defs</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1688"/>
        <source>Round numbers to a given precision</source>
        <translation>Округлять числа до заданной точности</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1775"/>
        <source>inside transform attributes:</source>
        <translation>внутри атрибутов трансформации:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1785"/>
        <source>inside coordinate attributes:</source>
        <translation>внутри атрибутов координат:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1795"/>
        <source>inside other attributes:</source>
        <translation>внутри остальных атрибутов:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1891"/>
        <source>If this option is set, then output is created with only required whitespace and newlines.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1894"/>
        <source>not-compact</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1897"/>
        <source>Compact output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The indentation for the pretty print style:</source>
        <translation type="obsolete">Размер отступа в структуре данных:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="106"/>
        <source>prefix</source>
        <translation>префикс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="107"/>
        <source>suffix</source>
        <translation>суффикс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="115"/>
        <source>Main</source>
        <translation>Главная</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="116"/>
        <source>Presets</source>
        <translation>Пресеты</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="117"/>
        <source>Elements</source>
        <translation>Элементы</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="118"/>
        <source>Attributes</source>
        <translation>Атрибуты</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="119"/>
        <source>Paths</source>
        <translation>Пути</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="120"/>
        <source>Optimization</source>
        <translation>Оптимизация</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="121"/>
        <source>Output</source>
        <translation>Вывод</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="145"/>
        <source>Warning! The original files will be destroyed!</source>
        <translation>Внимание! Исходные файлы будут уничтожены!</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="165"/>
        <location filename="../src/gui/wizarddialog.cpp" line="183"/>
        <source>For example</source>
        <translation>Пример</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="165"/>
        <source>before</source>
        <translation>до</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="167"/>
        <source>after</source>
        <translation>после</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="373"/>
        <source>Select an input folder</source>
        <translation>Выберите исходную папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="386"/>
        <source>Select an output folder</source>
        <translation>Выберите итоговую папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="397"/>
        <source>An input folder is not selected.</source>
        <translation>Не выбрана исходная папка.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="400"/>
        <source>An output folder is not selected.</source>
        <translation>Не выбрана итоговая папка.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="403"/>
        <source>You have to set a prefix or a suffix for this save method.</source>
        <translation>Необходимо установить префикс или суффикс.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="406"/>
        <source>An input folder is not exist.</source>
        <translation>Исходная папка не существует.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="409"/>
        <source>An input folder did not contain any svg, svgz files.</source>
        <translation>Исходная папка не содержит файлов svg и svgz.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="412"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="420"/>
        <source>You can not handle the SVGZ files.</source>
        <translation>Вы не сможете обрабатывать SVGZ файлы.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="423"/>
        <source>Selected output folder is not writable.</source>
        <translation>Выбранная итоговая папка недоступна для записи.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="417"/>
        <location filename="../src/gui/wizarddialog.cpp" line="431"/>
        <location filename="../src/gui/wizarddialog.cpp" line="450"/>
        <source>Warning</source>
        <translation>Предупреждение</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="413"/>
        <source>The &apos;svgcleaner-cli&apos; executable is not found in these folders:
</source>
        <translation>Исполняемый файл &apos;svgcleaner-cli&apos; не найден в данных директориях:
</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="418"/>
        <source>The &apos;7za&apos; executable is not found in these folders:
</source>
        <translation>Исполняемый файл &apos;7za&apos; не найден в данных директориях:
</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="442"/>
        <source>You have to set preset name.</source>
        <translation>Необходимо задать имя пресета.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="451"/>
        <source>This preset already exists.
Overwrite?</source>
        <translation>Данный пресет уже существует. Перезаписать?</translation>
    </message>
</context>
</TS>
