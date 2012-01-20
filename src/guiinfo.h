#ifndef GUIINFO_H
#define GUIINFO_H

#include <QObject>
#include <QDomDocument>
#include <QSizePolicy>
#include <QCheckBox>
#include <QMap>

template <class SPIN>
QWidget* F1(float value,float min,float max,float step, QString tagName)
{
    SPIN* spinBox = new SPIN;
    spinBox->setValue(value);
    spinBox->setMinimum(min);
    spinBox->setMaximum(max);
    spinBox->setSingleStep(step);
    spinBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    spinBox->setAccessibleName(tagName);
    return spinBox;
}

class GuiInfo : public QObject
{
    Q_OBJECT

public:
    explicit GuiInfo(QObject *parent = 0);
    float defaultSpintValue(const QString &preset, const QString &name);
    int defaultValue(const QString &preset, const QString &name);
    QStringList presets();
    QString presetInfo(const QString &preset);
    QString valueAt(const QString &name, const int &pos);
    void start();

private:
    QMap<QString,QDomDocument> presetMap;

    int interDefValue(const QDomNode &node);
    int presetDefValue(const QString &name, const QString &string);
    QCheckBox* createCheckBox(const QDomNode &node);
    QString loadFile(const QString &file);
    QString findInterface();
    QWidget* createComboBox(const QDomNode &node);
    QWidget* createSpinBox(const QDomNode &node);
    QWidget* createSub(const QDomNode &node);

signals:
    void newPage(const QString &name, const QString &iconName, QWidget *widget);
};

#endif // GUIINFO_H
