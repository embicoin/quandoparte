import QtQuick 1.1
import com.nokia.meego 1.1
import "uiconstants.js" as UiConstants

BorderImage {
    property alias text: label.text
    width: parent.width
    border.left: 10
    border.right: 10
    border.top: 10
    border.bottom: 10
    height: UiConstants.SearchBarDefaultHeight
    source: style.background

    ToolBarStyle {
        id: style
    }
    Item {
        id: spacerTop
        width: parent.width
        height: UiConstants.DefaultMargin
    }
    Item {
        id: spacerLeft
        anchors.left: parent.left
        width: UiConstants.DefaultMargin
        height: searchField.height
    }
    Item {
        id: spacerRight
        anchors.right: parent.right
        width: UiConstants.DefaultMargin
        height: searchField.height
    }
    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: spacerLeft.right
        anchors.right: spacerRight.left
        text: "Lorem Ipsum"
        font {
            pixelSize: UiConstants.HeaderFontPixelSize
        }
    }
}
