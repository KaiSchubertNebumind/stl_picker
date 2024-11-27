#pragma once
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <memory>

class DownloadManager : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int progressPercentage READ progressPercentage WRITE setProgressPercentage NOTIFY progressPercentageChanged)
    Q_PROPERTY(quint64 fileSize READ fileSize WRITE setFileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(quint64 readFileSize READ readFileSize WRITE setReadFileSize NOTIFY readFileSizeChanged)

public:
    DownloadManager();
    ~DownloadManager();

    enum class DownloadStatus {
        Empty, // Nothing downloaded yet
        Success,
        Aborted,
        MissingSpace,
        FileExists,
        TargetPathCreationFailure,
        TargetFileBusy,
        ConnectionFailure,
    };

    int progressPercentage() const;

    quint64 fileSize() const;

    quint64 readFileSize() const;

public slots:
    void download(const QUrl& url, const QString& absoluteFilePath, const QString& fileName = "", const bool override = true);
    void abort();

    QString parseFileName(const QUrl& url);

signals:
    void started();
    void aborted();
    void finished(DownloadManager::DownloadStatus result);

public slots:
    void setProgressPercentage(int progressPercentage);

    void setFileSize(quint64 fileSize);

    void setReadFileSize(quint64 readFileSize);

private slots:
    // Prefix with reply because we want to use
    // the name "finished" for qml signaling
    void replyReadyRead();
    void replyErrorOccurred();
    void replyFinished();
    void replyDownloadProgress(qint64 read, qint64 total);

private:
    void reset();

signals:
    void progressPercentageChanged(int progress);

    void fileSizeChanged(quint64 fileSize);

    void readFileSizeChanged(quint64 readFileSize);

private:
    int m_progressPercentage = 0;
    QNetworkAccessManager m_networkAccessManager;
    std::unique_ptr<QNetworkReply> m_reply;
    QByteArray m_data;
    QUrl m_targetFilePath;
    QString m_fileName;
    QString m_absoluteFilePath; // Combination of path + file
    QFile m_targetFile;
    quint64 m_fileSize;
    quint64 m_readFileSize;
};
