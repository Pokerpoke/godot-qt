#include <spdlog/spdlog.h>
#include "GodotBridge.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <thread>
#include "GodotVKItem.h"

int main(int argc, char *argv[])
{
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);

    qmlRegisterType<GodotVKItem>("GodotQt", 1, 0, "GodotVKItem");

    // 创建 Qt 应用程序实例
    QGuiApplication app(argc, argv);

    // 初始化 QML 引擎
    QQmlApplicationEngine engine;

    // 加载 QML 文件（根据实际路径修改）
    const QUrl url(QStringLiteral("./qml/GodotQt/main.qml"));
    engine.load(url);

    return app.exec();
}
