import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Controls.Material
import DownloadManager 1.0
import Qt.labs.platform 1.1

Item {
    id: root
    state: "default"

    DownloadManager {
        id: downloadManager
        onStarted: root.state = "downloading"
        onAborted: root.state = "finished"
        onFinished: function (result) {
            root.state = "finished"
        }
    }

    RowLayout {
        id: stateDefault
        anchors.fill: parent
        opacity: 0
        enabled: false
        TextField {
            id: tfDownloadUrl
            placeholderText: qsTr("Download URL")
            text: "https://releases.ubuntu.com/20.04.3/ubuntu-20.04.3-desktop-amd64.iso"
            selectByMouse: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }

        Button {
            text: qsTr("Download")
            highlighted: true
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                let urlFileName = StandardPaths.writableLocation(
                        StandardPaths.DocumentsLocation) + "/" + downloadManager.parseFileName(
                        tfDownloadUrl.text)
                fileDialog.currentFile = urlFileName
                fileDialog.open()
            }
        }

        FileDialog {
            id: fileDialog
            fileMode: FileDialog.SaveFile
            onAccepted: {
                downloadManager.download(tfDownloadUrl.text,
                                         fileDialog.currentFile)
            }
        }
    }
    RowLayout {
        id: stateDownloading
        anchors.fill: parent
        opacity: 0
        enabled: false
        spacing: 20
        ProgressBar {
            from: 0
            to: 100
            value: downloadManager.progressPercentage
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }
        Text {
            text: bytesToSize(downloadManager.readFileSize) + "/" + bytesToSize(
                      downloadManager.fileSize)
            function bytesToSize(bytes) {
                var sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB']
                if (bytes === 0)
                    return '0 Byte'
                var i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)))
                return Math.round(bytes / Math.pow(1024, i), 2) + ' ' + sizes[i]
            }
        }

        Button {
            text: qsTr("Abort")
            highlighted: true
            onClicked: downloadManager.abort()
            Layout.alignment: Qt.AlignVCenter
        }
    }
    RowLayout {
        id: stateFinished
        anchors.fill: parent
        opacity: 0
        enabled: false
        Text {
            id: name
            text: qsTr("Finished...")
            Layout.fillHeight: true
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    states: [
        State {
            name: "default"
            PropertyChanges {
                target: stateDefault
                opacity: 1
                enabled: true
            }
        },
        State {
            name: "downloading"
            PropertyChanges {
                target: stateDownloading
                opacity: 1
                enabled: true
            }
        },
        State {
            name: "finished"
            PropertyChanges {
                target: stateFinished
                opacity: 1
                enabled: true
            }
        }
    ]
    transitions: [
        Transition {
            from: "default"
            to: "downloading"
            PropertyAnimation {
                targets: [stateDefault, stateDownloading]
                property: "opacity"
                duration: 300
            }
        },
        Transition {
            from: "downloading"
            to: "finished"
            SequentialAnimation {
                PropertyAnimation {
                    targets: [stateDownloading, stateFinished]
                    property: "opacity"
                    duration: 300
                }
                PauseAnimation {
                    duration: 1000
                }
                ScriptAction {
                    script: root.state = "default"
                }
            }
        },
        Transition {
            from: "finished"
            to: "default"
            PropertyAnimation {
                targets: [stateFinished, stateDefault]
                property: "opacity"
                duration: 300
            }
        }
    ]
}
