#pragma once

#include <QImage>
#include <QString>

class QWidget;

class OcrModule
{
public:
    static bool run(QWidget* parent, const QImage& img, const QString& lang);
};
