#include <spdlog/spdlog.h>
#include "GodotBridge.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>

int main(int argc, char *argv[])
{
    // 创建 Qt 应用程序实例
    QGuiApplication app(argc, argv);

    // 初始化 QML 引擎
    QQmlApplicationEngine engine;

    // 加载 QML 文件（根据实际路径修改）
    const QUrl url(QStringLiteral("./qml/main.qml"));
    engine.load(url);

    auto th = std::make_shared<std::thread>(
        []()
        {
            LibGodot godot("godot-qt", "../project");
            // godot.get_root()->hide();
            while (true)
            {
                godot.m_godot_instance->iteration();
            }
        });
    return app.exec();
}
