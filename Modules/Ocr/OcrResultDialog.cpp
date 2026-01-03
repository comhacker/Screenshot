#include "OcrResultDialog.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

OcrResultDialog::OcrResultDialog(QWidget* parent)
	: QDialog(parent)
	, edit(new QPlainTextEdit(this))
	, btnCopy(new QPushButton("Copy", this))
	, btnClose(new QPushButton("Close", this))
{
	setWindowTitle("OCR Result");
	setMinimumSize(520, 360);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

	edit->setReadOnly(false);

	auto* btnLayout = new QHBoxLayout();
	btnLayout->addStretch();
	btnLayout->addWidget(btnCopy);
	btnLayout->addWidget(btnClose);

	auto* layout = new QVBoxLayout(this);
	layout->addWidget(edit);
	layout->addLayout(btnLayout);
	setLayout(layout);

	QObject::connect(btnCopy, &QPushButton::clicked, this, [this]() {
		auto* cb = QGuiApplication::clipboard();
		if (cb) {
			cb->setText(edit->toPlainText());
		}
	});
	QObject::connect(btnClose, &QPushButton::clicked, this, [this]() {
		accept();
	});
}

void OcrResultDialog::setText(const QString& text)
{
	edit->setPlainText(text);
}

QString OcrResultDialog::text() const
{
	return edit->toPlainText();
}
