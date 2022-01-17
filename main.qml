import QtQuick 2.9
import QtQuick.Window 2.2
import Live2DView 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    // 必须与Animation连用以使模型能够运动
    Live2DView {
        anchors.fill: parent;
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 1000; }
            NumberAnimation { to: 0; duration: 1000; }
            loops: Animation.Infinite;
            running: true;
        }
        resourcePath: "E:/xiaoyaosheny/live2d/live2d-model-master/aidang_2/aidang_2.model3.json"
    }
}
