import QtQuick
import QtWebView 1.0

Rectangle {
    WebView {
        anchors.fill: parent
        url: alarmHtmlUrl
    }
}
