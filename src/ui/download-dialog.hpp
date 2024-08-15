#ifndef SRC_UI_DOWNLOAD_DIALOG_HPP_
#define SRC_UI_DOWNLOAD_DIALOG_HPP_

#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include "src/core/filesystem.hpp"
#include "src/types.hpp"

struct DownloadResult {
	bool success;
	int statusCode;
	string errorMessage;
};

class DownloadDialog : public QProgressDialog {
	Q_OBJECT

	private:
	QNetworkAccessManager *m_networkManager;
	QNetworkReply *m_stream;
	QFile *m_file;
	DownloadResult m_result;

	DownloadDialog(
		const QString label,
		const QUrl &url,
		const QString &filePath
	);

	public:
	~DownloadDialog();

	static DownloadResult download(
		const string &label,
		const string &url,
		const fs::path &filePath
	);

	private slots:
	void onDownloadProgress( qint64 downloaded, qint64 total );
	void onDownloadFinished();
	void dataReceived();

};



#endif /* SRC_UI_DOWNLOAD_DIALOG_HPP_ */
