#include "DownloadManager.h"

#include <QDir>
#include <QFileInfo>
#include <QStorageInfo>

DownloadManager::DownloadManager()
{
    qInfo() << "Init!";
}

DownloadManager::~DownloadManager()
{
    qInfo() << "Destructor!";
}

/*!
 * \brief
 */
void DownloadManager::download(const QUrl& url, const QString& absoluteFilePath, const QString& fileName, const bool override)
{
    // Removes file:///
    const QString targetAbsoluteFilePath = QUrl::fromUserInput(absoluteFilePath).toLocalFile();

    QFileInfo targetFile(targetAbsoluteFilePath);
    m_absoluteFilePath = targetAbsoluteFilePath;
    m_fileName = targetFile.fileName();

    QDir targetPath(targetFile.path());
    if (!targetPath.exists()) {
        if (targetPath.mkpath(url.toLocalFile())) {
            emit finished(DownloadManager::DownloadStatus::TargetPathCreationFailure);
            return;
        }
    }

    if (targetFile.exists() && !override) {
        emit finished(DownloadManager::DownloadStatus::FileExists);
        return;
    }

    // Creates an empty file and checks for writeability
    m_targetFile.setFileName(targetAbsoluteFilePath);
    if (!m_targetFile.open(QIODevice::WriteOnly)) {
        emit finished(DownloadManager::DownloadStatus::TargetFileBusy);
        return;
    }
    m_targetFile.close();

    // Remove hardcoded value
    QStorageInfo storage(targetFile.path());
    if (storage.bytesAvailable() < 100000) {
        emit finished(DownloadManager::DownloadStatus::MissingSpace);
        return;
    }

    reset();
    m_reply.reset(m_networkAccessManager.get(QNetworkRequest(url)));

    QObject::connect(m_reply.get(), &QNetworkReply::readyRead, this, &DownloadManager::replyReadyRead);
    QObject::connect(m_reply.get(), &QNetworkReply::finished, this, &DownloadManager::replyFinished);
    QObject::connect(m_reply.get(), &QNetworkReply::errorOccurred, this, &DownloadManager::replyErrorOccurred);
    QObject::connect(m_reply.get(), &QNetworkReply::downloadProgress, this, &DownloadManager::replyDownloadProgress);

    emit started();
}

void DownloadManager::abort()
{
    m_reply->abort();
    emit aborted();
}

QString DownloadManager::parseFileName(const QUrl& url)
{
    return url.fileName();
}

/*!
 * \brief Save small chuncs so we do not need to load all data into RAM
 */
void DownloadManager::replyReadyRead()
{
    const QByteArray data = m_reply->readAll();
    if (data.size() <= 0) {
        return;
    }

    m_targetFile.setFileName(m_absoluteFilePath);
    m_targetFile.open(QIODevice::Append);
    m_targetFile.write(data);
    m_targetFile.close();
}

/*!
 * \brief
 */
void DownloadManager::replyFinished()
{
    emit finished(DownloadManager::DownloadStatus::Success);
}

/*!
 * \brief
 */
void DownloadManager::replyDownloadProgress(qint64 read, qint64 total)
{
    // Do not devide by zero.
    if (total <= 0)
        return;

    const float progressPercentage = (static_cast<float>(read) / static_cast<float>(total));
    const int progress = static_cast<int>(progressPercentage * 100.0f);
    setProgressPercentage(progress);
    setReadFileSize(read);
    setFileSize(total);
}

void DownloadManager::reset()
{
    setProgressPercentage(0);
    setFileSize(0);
    setReadFileSize(0);
}

/*!
 * \brief
 */
void DownloadManager::replyErrorOccurred()
{
    emit finished(DownloadManager::DownloadStatus::ConnectionFailure);
}

int DownloadManager::progressPercentage() const { return m_progressPercentage; }
quint64 DownloadManager::fileSize() const { return m_fileSize; }
quint64 DownloadManager::readFileSize() const { return m_readFileSize; }

void DownloadManager::setProgressPercentage(int progressPercentage)
{
    if (m_progressPercentage == progressPercentage)
        return;
    m_progressPercentage = progressPercentage;
    emit progressPercentageChanged(m_progressPercentage);
}

void DownloadManager::setFileSize(quint64 fileSize)
{
    if (m_fileSize == fileSize)
        return;
    m_fileSize = fileSize;
    emit fileSizeChanged(m_fileSize);
}

void DownloadManager::setReadFileSize(quint64 readFileSize)
{
    if (m_readFileSize == readFileSize)
        return;
    m_readFileSize = readFileSize;
    emit readFileSizeChanged(m_readFileSize);
}
