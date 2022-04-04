# Live2DQMLDemo
### 工程说明
* 本工程参考使用和修改Cubism公司开放的C++版本的[Sample源码](https://github.com/Live2D/CubismNativeSamples)，以及Qt官方开放的[OpenGL Under QML](https://doc.qt.io/qt-5.15/qtquick-scenegraph-openglunderqml-example.html)项目，实现使用```Qt Quick```显示模型及动画的功能。
* 本工程将与Live2D模型显示相关的功能封装为自定义的QML组件，即继承自```QQuickItem```类的```LAppLive2DView```类，同时搭配```SequentialAnimation```组件实现动画功能，具体参考```main.qml```文件。自定义QML组件的名称为```Live2DView```，其属性```resourcePath```用于设置Live2D模型文件的路径。
* Live2D模型文件的放置请参考Cubism公司在Sample工程中的放置方式，并将```.model3.json```文件的绝对路径作为```resourcePath```属性的值，注意使用```"/"```分隔。
### 工程配置
* Qt版本： 5.15.2
* 编译器版本：MSVC 2019 64-bit
* 开发工具：Visual Studio 2022
* 本工程需在**Debug**模式下的 **属性页** -> **链接器** -> **输入** -> **附加依赖项** 中添加```Core/lib/Live2DCubismCore_MDd.lib```，并在**Debug**模式中运行。**Release**模式未进行测试。
### 演示视频
[https://www.bilibili.com/video/BV1mL4y1u75V?spm_id_from=333.999.0.0](https://www.bilibili.com/video/BV1mL4y1u75V?spm_id_from=333.999.0.0)

---
**Sorry for my poor English!(T_T)**
### Project Description
* Thanks for the [sample source code](https://github.com/Live2D/CubismNativeSamples) of C++ version opened by Cubism company and the project '[OpenGL Under QML](https://doc.qt.io/qt-5.15/qtquick-scenegraph-openglunderqml-example.html)'. This project refers to above two projects and modifies some code to realize the function that using ```Qt Quick``` to show models and animations.
* This project encapsulates above function into a custom QML component which is called ```Live2DView```. You should collocate this QML component with the component ```SequentialAnimation``` to play the animation. Please read the file ```main.qml``` for reference.
* The property called ```resourcePath``` in the custom component ```Live2DView``` is used to set the path of Live2D model files. To set the path of Live2D model files, please refer to the sample project of Cubism company. Additionally, please let the path of ```.model3.json``` file as the value of the property ```resourcePath```.
### Project Settings
* Qt Version: 5.15.2
* Compiler Version: MSVC 2019 64-bit
* IDE: Visual Studio 2019
* Other Settings: **Debug** Mode, **Porperty Page** -> **Linker** -> **Input** -> **Additional Dependencies**: Add ```Core/lib/Live2DCubismCore_MDd.lib```.
### Presentation Video
[https://www.bilibili.com/video/BV1mL4y1u75V?spm_id_from=333.999.0.0](https://www.bilibili.com/video/BV1mL4y1u75V?spm_id_from=333.999.0.0)
