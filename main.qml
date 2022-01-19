import QtQuick 2.9
import QtQuick.Window 2.2
import Qt.labs.platform 1.1
import Live2DView 1.0

Window {
	id: root;
	visible: true;
	width: 640;
	height: 480;
	minimumWidth: 100;
	minimumHeight: 100;
	title: qsTr("Hello World");
	flags: Qt.Window | Qt.FramelessWindowHint | Qt.Tool | Qt.X11BypassWindowManagerHint;
	color: "transparent";
	property bool fixed: false;

	SystemTrayIcon {
		visible: true;
		icon.source: "qrc:/sysTray.png";

		menu: Menu {
			MenuItem {
				text: qsTr("Quit");
				onTriggered: Qt.quit();
			}
		}
	}

	// 必须与Animation连用以使模型能够运动
	Live2DView {
		id: live2DView;
		anchors.fill: parent;
		resourcePath: "D:/CubismSdkForNative-4-r.2/Samples/Resources/Haru/Haru.model3.json";

		SequentialAnimation on t {
			NumberAnimation { to: 1; duration: 1000; }
			NumberAnimation { to: 0; duration: 1000; }
			loops: Animation.Infinite;
			running: true;
		}
	}

	Rectangle {
		id: backRect;
		anchors.fill: parent;
		color: "lightgray";
		opacity: 0.0;

		DropArea {
			anchors.fill: parent;
			onDropped: {
				if(drop.hasUrls) {
					live2DView.resourcePath = drop.urls[0];
				}
			}
		}

		MouseArea {
			anchors.fill: parent;
			acceptedButtons: Qt.LeftButton | Qt.RightButton;
			hoverEnabled: true;
			property int clickPosX: 0;
			property int clickPosY: 0;
			property var mousePosType: CustomEnum.MousePositionType.None;
			onEntered: {
				backRect.opacity = 0.3;
			}
			onExited: {
				if(!this.pressed) {
					backRect.opacity = 0.0;
				}
			}
			onClicked: {
				if(mouse.button === Qt.RightButton)
					contextMenu.open();
			}
			onPressed: {
				if(mouse.button === Qt.LeftButton) {
					this.clickPosX = mouse.x;
					this.clickPosY = mouse.y;

					if(mouse.x < 5 && mouse.y < 5) {
						this.mousePosType = CustomEnum.MousePositionType.LeftTop;
					} else if(mouse.x < 5 && mouse.y > 5 && mouse.y < root.height - 5) {
						this.mousePosType = CustomEnum.MousePositionType.Left;
					} else if(mouse.x < 5 && mouse.y > root.height - 5) {
						this.mousePosType = CustomEnum.MousePositionType.LeftBottom;
					} else if(mouse.x > 5 && mouse.x < root.width - 5 && mouse.y < 5) {
						this.mousePosType = CustomEnum.MousePositionType.Top;
					} else if(mouse.x > 5 && mouse.x < root.width - 5 && mouse.y > root.height - 5) {
						this.mousePosType = CustomEnum.MousePositionType.Bottom;
					} else if(mouse.x > root.width - 5 && mouse.y < 5) {
						this.mousePosType = CustomEnum.MousePositionType.RightTop;
					} else if(mouse.x > root.width - 5 && mouse.y > 5 && mouse.y < root.height - 5) {
						this.mousePosType = CustomEnum.MousePositionType.Right;
					} else if(mouse.x > root.width - 5 && mouse.y > root.height - 5) {
						this.mousePosType = CustomEnum.MousePositionType.RightBottom;
					} else {
						this.mousePosType = CustomEnum.MousePositionType.Center;
					}
				}
				live2DView.handleMousePressEvent(mouse.button, mouse.x, mouse.y);
			}
			onReleased: {
				if(mouse.button === Qt.LeftButton) {
					this.mousePosType = CustomEnum.MousePositionType.None;
				}
				live2DView.handleMouseReleaseEvent(mouse.button, mouse.x, mouse.y);
			}
			onPositionChanged: {
				if(!root.fixed && this.pressed) {
					var w = 100;
					var h = 100;
					switch(this.mousePosType) {
					case CustomEnum.MousePositionType.Center:
						root.x = root.x + mouse.x - this.clickPosX;
						root.y = root.y + mouse.y - this.clickPosY;
						w = root.width;
						h = root.height;
						break;
					case CustomEnum.MousePositionType.LeftTop:
						root.x = root.x + mouse.x - this.clickPosX;
						root.y = root.y + mouse.y - this.clickPosY;
						w = root.width - mouse.x + this.clickPosX;
						h = root.height - mouse.y + this.clickPosY;
						break;
					case CustomEnum.MousePositionType.Left:
						root.x = root.x + mouse.x - this.clickPosX;
						w = root.width - mouse.x + this.clickPosX;
						h = root.height;
						break;
					case CustomEnum.MousePositionType.LeftBottom:
						root.x = root.x + mouse.x - this.clickPosX;
						w = root.width - mouse.x + this.clickPosX;
						h = mouse.y;
						break;
					case CustomEnum.MousePositionType.Top:
						root.y = root.y + mouse.y - this.clickPosY;
						w = root.width;
						h = root.height - mouse.y + this.clickPosY;
						break;
					case CustomEnum.MousePositionType.Bottom:
						w = root.width;
						h = mouse.y;
						break;
					case CustomEnum.MousePositionType.RightTop:
						root.y = root.y + mouse.y - this.clickPosY;
						w = mouse.x;
						h = root.height - mouse.y + this.clickPosY;
						break;
					case CustomEnum.MousePositionType.Right:
						w = mouse.x;
						h = root.height;
						break;
					case CustomEnum.MousePositionType.RightBottom:
						w = mouse.x;
						h = mouse.y;
						break;
					}
					root.width = w > 100 ? w : 100;
					root.height = h > 100 ? h : 100;
				} else if(root.fixed) {
					live2DView.handleMouseMoveEvent(mouse.button, mouse.x, mouse.y);
				}
			}

			Menu {
				id: contextMenu;
			
				MenuItem {
					text: qsTr("Pin the Window");
					checkable: true;
					onTriggered: {
						root.fixed = this.checked;
					}
				}

				MenuItem {
					text: qsTr("Always on the Top");
					checkable: true;
					onTriggered: {
						if(this.checked) {
							root.flags = Qt.Window | Qt.FramelessWindowHint | Qt.Tool | Qt.X11BypassWindowManagerHint | Qt.WindowStaysOnTopHint;
						} else {
							root.flags = Qt.Window | Qt.FramelessWindowHint | Qt.Tool | Qt.X11BypassWindowManagerHint;
						}
					}
				}

				MenuSeparator {}
				
				MenuItem {
					text: qsTr("Quit");
					onTriggered: Qt.quit();
				}
			}
		}
	}
}
