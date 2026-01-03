#pragma once

#include <QDialog>

class QPlainTextEdit;
class QPushButton;

class OcrResultDialog : public QDialog
{
public:
	explicit OcrResultDialog(QWidget* parent = nullptr);
	void setText(const QString& text);
	QString text() const;

private:
	QPlainTextEdit* edit;
	QPushButton* btnCopy;
	QPushButton* btnClose;
};
