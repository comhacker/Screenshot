#include "OcrService.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QProcess>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QUuid>

#include <climits>

bool OcrService::recognize(const QImage& img, const QString& lang, QString& outText, QString& outError)
{
	outText.clear();
	outError.clear();

	QTemporaryDir tempDir;
	if (!tempDir.isValid()) {
		outError = "Failed to create temp directory.";
		return false;
	}

	auto inputPath = QDir(tempDir.path()).filePath("ocr_input.png");
	QImage workImg = img;
	if (!workImg.isNull()) {
		const int maxW = 4000;
		const int maxH = 4000;
		const int minDim = qMin(workImg.width(), workImg.height());
		if (minDim > 0 && minDim < 700) {
			const double factor = qMin(2.0, 900.0 / double(minDim));
			int w = qMin(maxW, int(workImg.width() * factor));
			int h = qMin(maxH, int(workImg.height() * factor));
			workImg = workImg.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		{
			const qint64 px = qint64(workImg.width()) * qint64(workImg.height());
			const qint64 maxPx = 3000000;
			if (px > maxPx) {
				const double scale = qSqrt(double(maxPx) / double(px));
				int w = qMax(1, int(workImg.width() * scale));
				int h = qMax(1, int(workImg.height() * scale));
				workImg = workImg.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}
		}
		workImg = workImg.convertToFormat(QImage::Format_Grayscale8);
		const int margin = 10;
		QImage padded(workImg.width() + margin * 2, workImg.height() + margin * 2, QImage::Format_RGB32);
		padded.fill(Qt::white);
		{
			QPainter p(&padded);
			p.drawImage(margin, margin, workImg);
		}
		workImg = padded;
	}
	inputPath = QDir(tempDir.path()).filePath("ocr_input.bmp");
	if (!workImg.save(inputPath, "BMP")) {
		outError = "Failed to save temp image.";
		return false;
	}

	auto outBasePath = QDir(tempDir.path()).filePath("ocr_out");

	QStringList candidates;
	const auto appDir = QCoreApplication::applicationDirPath();
	const auto bundledDir = QDir(appDir).filePath("tesseract");
	const auto bundledExe1 = QDir(bundledDir).filePath("tesseract.exe");
	const auto bundledExe2 = QDir(appDir).filePath("tesseract.exe");
	if (QFileInfo::exists(bundledExe1)) candidates << bundledExe1;
	if (QFileInfo::exists(bundledExe2)) candidates << bundledExe2;
	candidates << "tesseract";

	QStringList requiredFiles;
	if (lang.contains("chi_sim")) requiredFiles << "chi_sim.traineddata";
	if (lang.contains("eng")) requiredFiles << "eng.traineddata";

	auto isValidTessdataDir = [&](const QString& dirPath) -> bool {
		if (dirPath.isEmpty()) return false;
		QDir d(dirPath);
		if (!d.exists()) return false;
		for (const auto& f : requiredFiles) {
			if (!QFileInfo::exists(d.filePath(f))) return false;
		}
		return true;
	};

	QString chosenTessdataDir;
	{
		auto envPrefix = QProcessEnvironment::systemEnvironment().value("TESSDATA_PREFIX").trimmed();
		if (isValidTessdataDir(envPrefix)) {
			chosenTessdataDir = envPrefix;
		}
		else if (!envPrefix.isEmpty()) {
			auto nested = QDir(envPrefix).filePath("tessdata");
			if (isValidTessdataDir(nested)) {
				chosenTessdataDir = nested;
			}
		}
	}

	if (chosenTessdataDir.isEmpty()) {
		const QStringList tessdataCandidates{
			QDir(bundledDir).filePath("tessdata"),
			QDir(appDir).filePath("tessdata"),
			QDir(QDir(appDir).filePath("..")).filePath("tesseract/tessdata")
		};
		for (const auto& c : tessdataCandidates) {
			if (isValidTessdataDir(c)) {
				chosenTessdataDir = c;
				break;
			}
		}
	}

	auto runOnce = [&](const QString& outBase, const QString& runLang, const QString& psm, const QStringList& extraArgs, QString& text, QString& err) -> bool {
		QStringList args;
		args << inputPath << outBase;
		args << "--oem" << "1";
		args << "--psm" << psm;
		args << "-c" << "user_defined_dpi=200";
		if (!chosenTessdataDir.isEmpty()) {
			args << "--tessdata-dir" << chosenTessdataDir;
		}
		args << extraArgs;
		args << "-l" << runLang;

		bool started = false;
		QString usedProgram;
		QStringList tried;
		QProcess proc;
		proc.setProcessChannelMode(QProcess::MergedChannels);
		for (const auto& program : candidates) {
			tried << program;
			{
				QFileInfo fi(program);
				if (fi.isAbsolute()) {
					proc.setWorkingDirectory(fi.absolutePath());
				}
				else {
					proc.setWorkingDirectory(appDir);
				}
			}
			QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
			if (!chosenTessdataDir.isEmpty()) {
				env.insert("TESSDATA_PREFIX", chosenTessdataDir);
			}
			proc.setProcessEnvironment(env);
			proc.start(program, args);
			if (proc.waitForStarted(3000)) {
				started = true;
				usedProgram = program;
				break;
			}
		}
		if (!started) {
			err = "Failed to start tesseract. Ensure it is installed or bundled with the app. Tried: " + tried.join(", ");
			return false;
		}
		if (!proc.waitForFinished(60000)) {
			proc.kill();
			proc.waitForFinished(1000);
			err = "Tesseract timeout.";
			return false;
		}

		const auto exitCode = proc.exitCode();
		const auto log = QString::fromLocal8Bit(proc.readAll());
		if (exitCode != 0) {
			err = "Tesseract failed. program=" + usedProgram + " tessdataDir=" + chosenTessdataDir + "\n" + log;
			return false;
		}

		QFile outFile(outBase + ".txt");
		if (!outFile.open(QIODevice::ReadOnly)) {
			err = "Failed to read tesseract output file.";
			return false;
		}
		text = QString::fromUtf8(outFile.readAll());
		outFile.close();
		return true;
	};

	auto postProcess = [&](QString text, const QString& runLang) -> QString {
		static const QString cjkRange = "[\\x{3400}-\\x{4DBF}\\x{4E00}-\\x{9FFF}\\x{F900}-\\x{FAFF}]";
		const QString hspaces = "[\\t \\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]";

		const QString cnPunct = "，。！？；：、（）【】《》“”‘’";
		const QString asciiPunct = ",\\.\\!\\?;:\\(\\)\\[\\]\\{\\}\"'";
		const QString punctClass = "[" + cnPunct + asciiPunct + "]";

		QStringList lines = text.split("\n");
		for (auto& line : lines) {
			QString s = line;

			if (runLang.contains("chi_sim")) {
				// basic CJK cleanup (no newline removal)
				s.replace(QRegularExpression("(" + cjkRange + ")" + hspaces + "+(" + cjkRange + ")"), "\\1\\2");
				s.replace(QRegularExpression("(" + cjkRange + ")" + hspaces + "+(" + punctClass + ")"), "\\1\\2");
				s.replace(QRegularExpression("(" + punctClass + ")" + hspaces + "+(" + cjkRange + ")"), "\\1\\2");
			}

			// detect spaced-out CJK lines and fix aggressively
			const int cjkCount = s.count(QRegularExpression(cjkRange));
			const int cjkSpacedPairs = s.count(QRegularExpression(cjkRange + hspaces + "+" + cjkRange));
			if (cjkCount >= 12 && cjkSpacedPairs * 2 >= cjkCount) {
				s.replace(QRegularExpression(hspaces + "+"), "");
				s.replace(QRegularExpression("(" + cjkRange + ")([A-Za-z0-9])"), "\\1 \\2");
				s.replace(QRegularExpression("([A-Za-z0-9])(" + cjkRange + ")"), "\\1 \\2");
				s.replace(QRegularExpression("(" + punctClass + ")(" + cjkRange + ")"), "\\1\\2");
				s.replace(QRegularExpression("(" + cjkRange + ")(" + punctClass + ")"), "\\1\\2");
			}

			// detect code/path-like lines and remove spaces between code characters
			const int codeish = s.count(QRegularExpression("[\\\\/_\\-.=:+*]"));
			if (codeish >= 4) {
				s.replace(QRegularExpression("([A-Za-z0-9\\\\/_\\-.=:+*])" + hspaces + "+([A-Za-z0-9\\\\/_\\-.=:+*])"), "\\1\\2");
			}

			// normalize multi spaces (within the line)
			s.replace(QRegularExpression(hspaces + "{2,}"), " ");
			s = s.trimmed();
			line = s;
		}

		text = lines.join("\n");
		return text.trimmed();
	};

	auto scoreText = [&](const QString& text) -> int {
		static const QString cjkRange = "[\\x{3400}-\\x{4DBF}\\x{4E00}-\\x{9FFF}\\x{F900}-\\x{FAFF}]";
		int score = 0;
		score += text.count(QRegularExpression(cjkRange)) * 3;
		score += text.count(QRegularExpression("[A-Za-z0-9]")) * 1;
		score -= text.count(QRegularExpression("[\\uFFFD]")) * 10;
		score -= text.count(QRegularExpression("[\\x00-\\x08\\x0B\\x0C\\x0E-\\x1F]")) * 10;
		score -= text.count(QRegularExpression(cjkRange + "[\\t \\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]+" + cjkRange)) * 8;
		return score;
	};

	auto isGoodEnough = [&](const QString& text) -> bool {
		static const QString cjkRange = "[\\x{3400}-\\x{4DBF}\\x{4E00}-\\x{9FFF}\\x{F900}-\\x{FAFF}]";
		if (text.trimmed().isEmpty()) return false;
		if (text.count(QRegularExpression("[\\uFFFD]")) > 0) return false;
		if (text.count(QRegularExpression("[\\x00-\\x08\\x0B\\x0C\\x0E-\\x1F]")) > 0) return false;
		if (text.count(QRegularExpression(cjkRange + "[\\t \\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]+" + cjkRange)) > 3) return false;
		return true;
	};

	QString bestText;
	QString bestErr;
	QString bestLang;
	int bestScore = INT_MIN;

	auto tryUpdateBest = [&](const QString& outBase, const QString& runLang, const QString& psm, const QStringList& extraArgs = {}) {
		QString t;
		QString e;
		if (!runOnce(outBase, runLang, psm, extraArgs, t, e)) {
			if (bestErr.isEmpty()) bestErr = e;
			return;
		}
		t = postProcess(t, runLang);
		const int s = scoreText(t);
		if (s > bestScore) {
			bestScore = s;
			bestText = t;
			bestLang = runLang;
			bestErr.clear();
		}
	};

	tryUpdateBest(outBasePath + "_a", lang, "6");
	if (isGoodEnough(bestText)) {
		outText = postProcess(bestText, bestLang.isEmpty() ? lang : bestLang);
		return true;
	}
	tryUpdateBest(outBasePath + "_b", lang, "3");
	if (isGoodEnough(bestText)) {
		outText = postProcess(bestText, bestLang.isEmpty() ? lang : bestLang);
		return true;
	}
	if (lang.contains("chi_sim") && lang.contains("eng")) {
		tryUpdateBest(outBasePath + "_c", "chi_sim", "6");
		if (isGoodEnough(bestText)) {
			outText = postProcess(bestText, bestLang.isEmpty() ? lang : bestLang);
			return true;
		}
		if (bestText.count(QRegularExpression("[\\\\/_\\-.=:+*]")) >= 4) {
			tryUpdateBest(
				outBasePath + "_d",
				"eng",
				"6",
				{
					"-c",
					"preserve_interword_spaces=1",
					"-c",
					"tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\\\\/_\\\\-.=:+*()[]{}<>@#%&,'\"`~!?$^|;:\\t "
				}
			);
		}
	}

	if (bestText.isEmpty()) {
		outError = bestErr.isEmpty() ? "Tesseract failed." : bestErr;
		return false;
	}
	outText = postProcess(bestText, bestLang.isEmpty() ? lang : bestLang);
	return true;
}
