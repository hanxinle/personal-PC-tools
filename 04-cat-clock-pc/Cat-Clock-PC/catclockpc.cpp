#include "catclockpc.h"

#include <QQuickWidget>
#include <QQmlContext>
#include <QVBoxLayout>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QUrl>
#include <QtWebView>

CatClockPC::CatClockPC(QWidget *parent)
    : QWidget(parent)
    , m_quickWidget(nullptr)
{
    ui.setupUi(this);

    // 初始化 Qt WebView 模块，Windows 平台会加载 Edge WebView2.
    QtWebView::initialize();

    // 创建垂直布局，使 QML 视图填满整个窗口.
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 创建 QuickWidget 用于嵌入 QML WebView.
    m_quickWidget = new QQuickWidget(this);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    layout->addWidget(m_quickWidget);

    // 计算小猫闹钟 HTML 文件路径，优先使用输出目录中的副本.
    QDir appDir(QCoreApplication::applicationDirPath());
    QString htmlPath = appDir.filePath(QStringLiteral("01-cat-clock-h5/小猫闹钟.html"));

    // 如果输出目录中不存在，则回退到仓库根目录下的原始文件.
    if (!QFile::exists(htmlPath)) {
        QDir projectDir(QCoreApplication::applicationDirPath());
        // 从 x64/Debug 或 x64/Release 上溯到仓库根目录.
        for (int i = 0; i < 4; ++i) {
            projectDir.cdUp();
        }
        htmlPath = projectDir.filePath(QStringLiteral("01-cat-clock-h5/小猫闹钟.html"));
    }

    // 将本地文件 URL 暴露给 QML.
    m_quickWidget->rootContext()->setContextProperty(QStringLiteral("alarmHtmlUrl"), QUrl::fromLocalFile(htmlPath));
    m_quickWidget->setSource(QUrl(QStringLiteral("qrc:/main.qml")));

    setWindowTitle(QStringLiteral("小猫闹钟"));
    resize(560, 840);
}

CatClockPC::~CatClockPC()
{
}
