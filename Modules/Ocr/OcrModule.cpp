#include "OcrModule.h"

#include <QMessageBox>

#include "OcrService.h"
#include "OcrResultDialog.h"

bool OcrModule::run(QWidget* parent, const QImage& img, const QString& lang)
{
    if (img.isNull()) {
        QMessageBox::warning(parent, "OCR", "Image is empty.");
        return false;
    }

    QString text;
    QString error;
    if (!OcrService::recognize(img, lang, text, error)) {
        QMessageBox::warning(parent, "OCR", error.isEmpty() ? "OCR failed." : error);
        return false;
    }

    OcrResultDialog dlg(parent);
    dlg.setText(text);
    dlg.exec();
    return true;
}
