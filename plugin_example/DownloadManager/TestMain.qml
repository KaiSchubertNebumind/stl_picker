import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import DownloadManager 1.0

Window {
    width: 640
    height: 200
    visible: true
    title: qsTr("Download Manager Test Project")

    DownloadManagerControls {
        anchors.fill: parent
        anchors.margins: 20
    }
}
