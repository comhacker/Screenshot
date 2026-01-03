#pragma once

#include <QImage>
#include <QString>

class OcrService
{
public:
	static bool recognize(const QImage& img, const QString& lang, QString& outText, QString& outError);
};
