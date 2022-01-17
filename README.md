# Live2DQMLDemo
### 工程说明
---
* 本工程参考使用和修改Cubism公司开放的C++版本的[Sample源码](https://github.com/Live2D/CubismNativeSamples)，以及Qt官方开放的[OpenGL Under QML](https://doc.qt.io/qt-5.15/qtquick-scenegraph-openglunderqml-example.html)项目，实现使用```Qt Quick```显示模型及动画的功能。
* 本工程将与Live2D模型显示相关的功能封装为自定义的QML组件，即继承自```QQuickItem```类的```LAppLive2DView```类，同时搭配```SequentialAnimation```组件实现动画功能，具体参考```main.qml```文件。自定义QML组件的名称为```Live2DView```，其属性```resourcePath```用于设置Live2D模型文件的路径。
* Live2D模型文件的放置请参考Cubism公司在Sample工程中的放置方式，并将```.model3.json```文件的绝对路径作为```resourcePath```属性的值。
* 本工程暂未实现点击同步播放音频的功能。
### 工程配置
---
* Qt版本： 5.15.2
* 编译器版本：MSVC 2019 64-bit
* 开发工具：Visual Studio 2022
* 本工程需在**Debug**模式下的 **属性页** -> **链接器** -> **输入** -> **附加依赖项** 中添加```CubismSDK/Core/lib/Live2DCubismCore_MDd.lib```，并在**Debug**模式中运行
