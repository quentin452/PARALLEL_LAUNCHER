#include "src/ui/download-dialog.hpp"

DownloadDialog::DownloadDialog(const QString label, const QUrl &url,
                               const QString &filePath)
    : QProgressDialog() {
  m_networkManager = new QNetworkAccessManager();
  m_stream = m_networkManager->get(QNetworkRequest(url));
  m_result = {false, 0, "Request cancelled"};

  m_file = new QFile(filePath);
  m_file->open(QIODevice::WriteOnly | QIODevice::Truncate);

  setLabelText(label);
  setRange(0, 1);
  setValue(0);

  connect(m_stream, &QNetworkReply::readyRead, this,
          &DownloadDialog::dataReceived);
  connect(m_stream, &QNetworkReply::downloadProgress, this,
          &DownloadDialog::onDownloadProgress);
  connect(m_stream, &QNetworkReply::finished, this,
          &DownloadDialog::onDownloadFinished);
}

DownloadDialog::~DownloadDialog() {
  m_stream->deleteLater();
  m_networkManager->deleteLater();
  m_file->close();
  m_file->deleteLater();
  m_file = nullptr;
}

void DownloadDialog::onDownloadProgress(qint64 downloaded, qint64 total) {
  setMaximum((int)total);
  setValue((int)downloaded);
}

void DownloadDialog::dataReceived() {
  if (m_file != nullptr) {
    m_file->write(m_stream->readAll());
  }
}

void DownloadDialog::onDownloadFinished() {
  bool hasStatusCode;
  const int statusCode =
      m_stream->attribute(QNetworkRequest::HttpStatusCodeAttribute)
          .toInt(&hasStatusCode);

  m_result.statusCode = hasStatusCode ? statusCode : 0;
  if (m_stream->error() == QNetworkReply::NoError) {
    m_result.success = true;
    m_result.errorMessage.clear();
    accept();
  } else {
    m_result.success = false;
    m_result.errorMessage = m_stream->errorString().toStdString();
    reject();
  }
}

DownloadResult DownloadDialog::download(const string &label, const string &url,
                                        const fs::path &filePath) {
  fs::create_directories(filePath.parent_path());
  DownloadDialog dialog(tr(label.c_str()), QUrl(url.c_str()),
                        filePath.u8string().c_str());
  dialog.exec();
  return dialog.m_result;
}
