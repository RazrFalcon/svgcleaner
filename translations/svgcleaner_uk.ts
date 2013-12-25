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
        <location filename="../src/gui/aboutdialog.cpp" line="38"/>
        <source>Generally, SVG files produced by vector editors contain a lot of unused elements and attributes that just blow up their size without providing better visible quality.</source>
        <translation>Зазвичай, створені редакторами векторної графіки SVG-файли містять досить велику кількість елементів і атрибутів, які не мають будь-якого практичного значення і лише надмірно збільшують розмір файлів.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="42"/>
        <source>SVG Cleaner could help you to clean up your SVG files from unnecessary data. It has a lot of options for cleanup and optimization, works in batch mode, provides threaded processing on the multicore processors and basically does two things:&lt;br /&gt;- removing elements and attributes that don&apos;t contribute to the final rendering;&lt;br /&gt;- making those elements and attributes in use more compact.</source>
        <translation>SVG Cleaner може допомогти очистити ваші SVG-файли від зазначених зайвих даних. Він працює у пакетному режимі і, власне кажучи, робить дві речі:&lt;br /&gt;- видаляє елементи й атрибути, які не приймають участь у формуванні кінцевого зображення; &lt;br /&gt;- приводить задіяні елементи й атрибути до більш компактного вигляду.
Програма містить велику кількість опцій чистки та оптимізації, а також забезпечує можливість багатопотокової обробки файлів на багатоядерних процесорах.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="50"/>
        <source>Images cleaned by SVG Cleaner are typically 10-60 percent smaller than the original ones.</source>
        <translation>Розмір очищених за допомогою SVG Cleaner зображень зазвичай може становити 10-60% від початкового значення.</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="53"/>
        <source>Important! The internal image viewer in SVG Cleaner uses the QtSvg module for rendering SVG images. Qt supports only the static features of SVG 1.2 Tiny, and that imposes a number of restrictions on rendering of advanced features. For instance, elements such as clipPath, mask, filters etc. will not be rendered at all.</source>
        <translation>Увага! Внутрішній переглядач зображень SVG Cleaner використовує для рендерінга модуль QtSvg. На даний час Qt підтримує лише статичну функціональність у межах SVG 1.2 Tiny, що не дозволяє використовувати розширені функції стандарту SVG при формуванні зображень. Тому варто враховувати, що такі елементи, як clipPath (відсічні контури), mask (маски), filter (фільтри) і т.п. взагалі не будуть відображатися.</translation>
    </message>
    <message>
        <source>We apologize for any bugs in advance. Please send bug reports to </source>
        <translation type="obsolete">Ми заздалегідь приносимо свої вибачення за можливі помилки. Будь ласка, відправляйте звіти про виявлені помилки на </translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="59"/>
        <source>Please send bug reports to </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="65"/>
        <source>Developers:</source>
        <translation>Розробники:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="68"/>
        <source>Previous developers:</source>
        <translation>Попередні розробники:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="71"/>
        <source>Special thanks:</source>
        <translation>Особлива подяка:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="74"/>
        <source>Logo design:</source>
        <translation>Дизайн логотипу:</translation>
    </message>
    <message>
        <location filename="../src/gui/aboutdialog.cpp" line="77"/>
        <source>Translators:</source>
        <translation>Перекладачі:</translation>
    </message>
</context>
<context>
    <name>CleanerThread</name>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="144"/>
        <location filename="../src/gui/cleanerthread.cpp" line="153"/>
        <source>Crashed</source>
        <translation>Не підлягає обробці</translation>
    </message>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="147"/>
        <source>Input file does not exist.</source>
        <translation>Заданий вхідний файл не існує.</translation>
    </message>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="149"/>
        <source>Output folder does not exist.</source>
        <translation>Задана вихідна папка не існує.</translation>
    </message>
    <message>
        <location filename="../src/gui/cleanerthread.cpp" line="151"/>
        <source>It&apos;s a not well-formed SVG file.</source>
        <translation>Некоректний формат файлу.</translation>
    </message>
</context>
<context>
    <name>LineEdit</name>
    <message>
        <location filename="../src/gui/lineedit.cpp" line="66"/>
        <source>%1 files</source>
        <translation>%1 файлів</translation>
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
        <translation>Файли</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="146"/>
        <source>Total count:</source>
        <translation>Загальна кількість:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="169"/>
        <source>Cleaned:</source>
        <translation>Оброблено:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="192"/>
        <source>Crashed:</source>
        <translation>Не підлягає обробці:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="215"/>
        <source>Size before:</source>
        <translation>Початковий розмір:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="225"/>
        <source>Size after:</source>
        <translation>Підсумковий розмір:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="270"/>
        <source>New file sizes</source>
        <translation>Розмір оброблених файлів</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="276"/>
        <location filename="../src/gui/mainwindow.ui" line="370"/>
        <source>Maximum:</source>
        <translation>Найбільше значення:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="299"/>
        <location filename="../src/gui/mainwindow.ui" line="390"/>
        <source>Minimum:</source>
        <translation>Найменше значення:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="322"/>
        <location filename="../src/gui/mainwindow.ui" line="360"/>
        <source>Total:</source>
        <translation>Підсумкове значення:</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="354"/>
        <source>Processing time</source>
        <translation>Час обробки файлів</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="380"/>
        <source>Average:</source>
        <translation>Середнє значення:</translation>
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
        <translation>Інформація</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.ui" line="561"/>
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
        <source>Sort by compression</source>
        <translation>Сортувати за очищенням</translation>
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
        <location filename="../src/gui/mainwindow.cpp" line="74"/>
        <source>Open the wizard</source>
        <translation>Відкрити Майстер</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="75"/>
        <source>Start processing</source>
        <translation>Розпочати обробку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="76"/>
        <source>Pause processing</source>
        <translation>Призупинити обробку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="77"/>
        <source>Stop cleaning</source>
        <translation>Завершити обробку</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="307"/>
        <source>Compare view: on</source>
        <translation>Порівняння зображень: увімкнено</translation>
    </message>
    <message>
        <location filename="../src/gui/mainwindow.cpp" line="310"/>
        <source>Compare view: off</source>
        <translation>Порівняння зображень: вимкнено</translation>
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
        <translation>КіБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="43"/>
        <source>MiB</source>
        <translation>МіБ</translation>
    </message>
    <message>
        <location filename="../src/gui/someutils.cpp" line="53"/>
        <source>%1h %2m %3s %4ms</source>
        <translation>%1г %2х %3с %4мс</translation>
    </message>
</context>
<context>
    <name>ThumbWidget</name>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="90"/>
        <source>Sizes:</source>
        <translation>Розміри:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="106"/>
        <source>Attributes:</source>
        <translation>Атрибути:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="122"/>
        <source>Elements:</source>
        <translation>Елементи:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.ui" line="138"/>
        <source>Processing time:</source>
        <translation>Час обробки:</translation>
    </message>
    <message>
        <location filename="../src/gui/thumbwidget.cpp" line="37"/>
        <source>Name:</source>
        <translation>Ім&apos;я:</translation>
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
        <location filename="../src/gui/wizarddialog.ui" line="65"/>
        <source>Compressing</source>
        <translation>Стиснення</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="80"/>
        <source>Compress level:</source>
        <translation>Рівень стиснення:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="97"/>
        <source>fastest</source>
        <translation>найшвидший</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="102"/>
        <source>fast</source>
        <translation>швидкий</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="107"/>
        <source>normal</source>
        <translation>звичайний</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="112"/>
        <source>maximum</source>
        <translation>максимальний</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="117"/>
        <source>ultra</source>
        <translation>ультра</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="134"/>
        <source>Compress all processed files</source>
        <translation>Стискати усі оброблені файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="153"/>
        <source>Compress processed .svgz files only</source>
        <translation>Стискати лише оброблені svgz-файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="175"/>
        <source>Multithreading</source>
        <translation>Багатопоточність</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="199"/>
        <source>The number of threads:</source>
        <translation>Кількість потоків:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="249"/>
        <source>Open files from the input folder and:</source>
        <translation>Відкривати файли із вхідної папки й:</translation>
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
        <translation>Наприклад: </translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="339"/>
        <source>Set prefix and/or suffix:</source>
        <translation>Додавати префікс та/або суфікс:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="364"/>
        <source>save cleaned files into the output folder</source>
        <translation>зберігати очищені файли у вхідній папці</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="377"/>
        <source>save cleaned files into the current folder</source>
        <translation>зберігати очищені файли у вихідній папці</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="390"/>
        <source>overwrite original files</source>
        <translation>переписати оригінальні файли</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="443"/>
        <source>Open an input folder</source>
        <translation>Відкрити вхідну папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="468"/>
        <source>Input folder:</source>
        <translation>Вхідна папка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="511"/>
        <source>Open an output folder</source>
        <translation>Відкрити вихідну папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="530"/>
        <source>Output folder:</source>
        <translation>Вихідна папка:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="549"/>
        <source>Search for SVG files in subfolders</source>
        <translation>Обробляти файли із вкладених папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="552"/>
        <source>Recursive folder scanning</source>
        <translation>Рекурсивна перевірка папок</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="589"/>
        <source>Select a preset:</source>
        <translation>Обрати передустановку:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="614"/>
        <source>Enter preset name:</source>
        <translation>Уведіть ім&apos;я передустановки:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="624"/>
        <source>Save</source>
        <translation>Зберегти</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="650"/>
        <source>Remove</source>
        <translation>Видалити</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="670"/>
        <source>Preset description:</source>
        <translation>Опис передустановки:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="708"/>
        <source>Remove prolog</source>
        <translation>Видаляти пролог</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="721"/>
        <source>Remove comments</source>
        <translation>Видаляти коментарі</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="734"/>
        <source>Remove processing instruction</source>
        <translation>Видаляти інструкції обробки</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="747"/>
        <source>Remove unused definitions</source>
        <translation>Видаляти визначення, що не використовуються</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="760"/>
        <source>Remove non-SVG elements</source>
        <translation>Видаляти не-SVG елементи</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="773"/>
        <source>Remove metadata elements</source>
        <translation>Видаляти елементи метаданих</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="783"/>
        <source>Remove elements from namespaces by the following graphics editors:</source>
        <translation>Видаляти елементи із просторів імен графічних редакторів:</translation>
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
        <translation type="obsolete">Об&apos;єднувати декілька матриць трансформацій в одну та спрощувати її</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1502"/>
        <source>Remove empty segments</source>
        <translation>Видаляти порожні сегменти ліній і кривих</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="879"/>
        <source>Remove invisible elements</source>
        <translation>Видаляти невидимі елементи</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="892"/>
        <source>Remove empty container elements</source>
        <translation>Видаляти порожні елементи-контейнери</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="905"/>
        <source>Collapse groups when possible</source>
        <translation>Розгруповувати групи, якщо можливо</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="918"/>
        <source>Remove duplicated definitions</source>
        <translation>Видаляти визначення, що дублюються</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="931"/>
        <source>Remove gradients that are only referenced by one other gradient</source>
        <translation>Поєднувати градієнти, якщо можливо</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="944"/>
        <source>Remove Gaussian blur filters</source>
        <translation>Видаляти фільтри Гаусового розмивання</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="987"/>
        <source>if their standard deviation is less than:</source>
        <translation>якщо їх стандартне відхилення менше ніж:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1073"/>
        <source>Remove the SVG language version</source>
        <translation>Видаляти версію мови SVG</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1086"/>
        <source>Remove unreferenced IDs</source>
        <translation>Видаляти зайві ідентіфікатори</translation>
    </message>
    <message>
        <source>Always keep IDs that contain only letters</source>
        <translation type="obsolete">Залишати ідентіфікатори, що містять лише літери</translation>
    </message>
    <message>
        <source>Remove non-SVG attributes</source>
        <translation type="obsolete">Видаляти не-SVG атрибути</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1145"/>
        <source>Remove attributes that are not applicable to elements</source>
        <translation>Видаляти атрибути, що не застосовуються елементами</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1158"/>
        <source>Remove attributes with default SVG values</source>
        <translation>Видаляти атрибути із значеннями за замовчуванням</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1168"/>
        <source>Remove attributes from namespaces by the following graphics editors:</source>
        <translation>Видаляти атрибути із просторів імен графічних редакторів:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1264"/>
        <source>Remove all stroke properties when there&apos;s no stroking</source>
        <translation>Видаляти усі властивості обведення, якщо його не має</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1277"/>
        <source>Remove all fill properties when there&apos;s no filling</source>
        <translation>Видаляти усі властивості заповнення, якщо його не має</translation>
    </message>
    <message>
        <source>Remove all unused properties from children of clipPath elements</source>
        <translation type="obsolete">Видаляти усі зайві властивості у нащадків елементів clipPath</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1290"/>
        <source>Remove gradient coordinates when possible</source>
        <translation>Видаляти координати градієнтів, якщо можливо</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1303"/>
        <source>Remove XLinks which pointed to nonexistent element</source>
        <translation>Видаляти XLinks, які вказують на неіснуючий елемент</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1316"/>
        <source>Group elements by style properties</source>
        <translation>Згрупувати елементи з подібними стилями</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1393"/>
        <source>Keep existing paths data</source>
        <translation>Залишати існуючи дані шляхів</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1433"/>
        <location filename="../src/gui/wizarddialog.ui" line="1489"/>
        <source>Remove unnecessary whitespace between commands and coordinates</source>
        <translation>Видаляти зайві пробіли між командами і координатами</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1452"/>
        <source>Convert absolute paths into relative ones</source>
        <translation>Конвертувати абсолютні шляхи у відносні</translation>
    </message>
    <message>
        <source>Convert lines into horizontal/vertical equivalents when possible</source>
        <translation type="obsolete">Конвертувати лінії у короткі гор./верт. еквіваленти</translation>
    </message>
    <message>
        <source>Convert straight curves into lines when possible</source>
        <translation type="obsolete">Конвертувати прямі криві у лінії</translation>
    </message>
    <message>
        <source>Convert cubic curve segments into shorthand equivalents when possible</source>
        <translation type="obsolete">Конвертувати сегменти кубічних кривих у короткі еквіваленти</translation>
    </message>
    <message>
        <source>Convert quadratic curve segments into shorthand equivalents when possible</source>
        <translation type="obsolete">Конвертувати сегменти квадратичних кривих у короткі еквіваленти</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1576"/>
        <source>Convert width/height into a viewBox when possible</source>
        <translation>Конвертувати атрибути width і height у viewBox</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1589"/>
        <source>Convert style properties into SVG attributes</source>
        <translation>Конвертувати властивості стилів в атрибути SVG</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1602"/>
        <source>Convert colors to #RRGGBB format</source>
        <translation>Конвертувати кольори у формат #RRGGBB</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1633"/>
        <source>Convert colors to #RGB format when possible</source>
        <translation>Конвертувати кольори у формат #RGB, якщо можливо</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1649"/>
        <source>Convert basic shapes into paths</source>
        <translation>Конвертувати основні фігури у шляхи</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1662"/>
        <source>Apply transform matrices to the elements when possible</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Recalculate coordinates and remove transform attributes when possible</source>
        <translation type="obsolete">Перераховувати координати і видаляти атрибути трансформації</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1675"/>
        <source>Sort elements by name inside the defs section</source>
        <translation>Сортувати за ім&apos;ям елементи із секції defs</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1688"/>
        <source>Round numbers to a given precision</source>
        <translation>Округляти числа до заданої точності</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1775"/>
        <source>inside transform attributes:</source>
        <translation>всередині атрибутів трансформації:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1785"/>
        <source>inside coordinate attributes:</source>
        <translation>всередині атрибутів координат:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.ui" line="1795"/>
        <source>inside other attributes:</source>
        <translation>всередині інших атрибутів:</translation>
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
        <translation type="obsolete">Розмір відступу у структурі даних:</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="106"/>
        <source>prefix</source>
        <translation>префікс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="107"/>
        <source>suffix</source>
        <translation>суфікс</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="115"/>
        <source>Main</source>
        <translation>Головна</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="116"/>
        <source>Presets</source>
        <translation>Передустановки</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="117"/>
        <source>Elements</source>
        <translation>Елементи</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="118"/>
        <source>Attributes</source>
        <translation>Атрибути</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="119"/>
        <source>Paths</source>
        <translation>Шляхи</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="120"/>
        <source>Optimization</source>
        <translation>Оптимізація</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="121"/>
        <source>Output</source>
        <translation>Вихідний формат</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="145"/>
        <source>Warning! The original files will be destroyed!</source>
        <translation>Увага! Оригінальні файли будуть знищені!</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="165"/>
        <location filename="../src/gui/wizarddialog.cpp" line="183"/>
        <source>For example</source>
        <translation>Наприклад</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="165"/>
        <source>before</source>
        <translation>до</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="167"/>
        <source>after</source>
        <translation>після</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="373"/>
        <source>Select an input folder</source>
        <translation>Обрати вхідну папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="386"/>
        <source>Select an output folder</source>
        <translation>Обрати вихідну папку</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="397"/>
        <source>An input folder is not selected.</source>
        <translation>Не обрано вхідну папку.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="400"/>
        <source>An output folder is not selected.</source>
        <translation>Не обрано вихідну папку.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="403"/>
        <source>You have to set a prefix or a suffix for this save method.</source>
        <translation>Необхідно задати додаваємий префікс та/або суфікс.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="406"/>
        <source>An input folder is not exist.</source>
        <translation>Задана вхідна папка не існує.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="409"/>
        <source>An input folder did not contain any svg, svgz files.</source>
        <translation>Вхідна папка не містить жодних SVG-файлів.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="412"/>
        <source>Error</source>
        <translation>Помилка</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="420"/>
        <source>You can not handle the SVGZ files.</source>
        <translation>Ви не зможете обробляти SVGZ файли.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="423"/>
        <source>Selected output folder is not writable.</source>
        <translation>Обрана вихідна папка недоступна для запису.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="417"/>
        <location filename="../src/gui/wizarddialog.cpp" line="431"/>
        <location filename="../src/gui/wizarddialog.cpp" line="450"/>
        <source>Warning</source>
        <translation>Застереження</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="413"/>
        <source>The &apos;svgcleaner-cli&apos; executable is not found in these folders:
</source>
        <translation>Файл &apos;svgcleaner-cli&apos; не знайден у цих папках:
</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="418"/>
        <source>The &apos;7za&apos; executable is not found in these folders:
</source>
        <translation>Файл &apos;7za&apos; не знайден у цих папках:
</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="442"/>
        <source>You have to set preset name.</source>
        <translation>Потрібно задати ім&apos;я передустановки.</translation>
    </message>
    <message>
        <location filename="../src/gui/wizarddialog.cpp" line="451"/>
        <source>This preset already exists.
Overwrite?</source>
        <translation>Ця передустановка вже існує.
Переписати її?</translation>
    </message>
</context>
</TS>
