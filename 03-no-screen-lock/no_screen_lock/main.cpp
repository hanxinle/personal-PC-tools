#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QLabel>
#include <Windows.h>
#include <QMovie>



// 模拟按下 NumLock.
void pressNumLock() {
	// keybd_event 已弃用，但仍能用；也可以换 SendInput.
	keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
	keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

// 模拟按下两次 NumLock，中间间隔 2 秒，确保 NumLock 状态不变.
void pressNumLockTwice() {
	pressNumLock();
	// 延迟 2 秒后再次按下，避免两次按键间隔过快导致问题.
	QTimer::singleShot(2000, []() {
		pressNumLock();
	});
}

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//QIcon icon("C:\\Users\\xinle.han\\Pictures\\111.png");  // 支持 .ico、.png 等，路径支持资源或外部路径
	//a.setWindowIcon(icon);


	QWidget w;
	w.setWindowTitle("硬座");
	w.resize(400, 300);

	// 用 QLabel + QMovie 做背景
	QLabel* background = new QLabel(&w);
	background->setScaledContents(true); // 自适应窗口大小
	background->resize(w.size());

	QMovie* movie = new QMovie(":/no_screen_lock/bk.gif");  // 资源或本地路径
	background->setMovie(movie);
	movie->start();

	// 在背景之上放按钮（用布局管理）
	QPushButton* btnExit = new QPushButton("退出程序", &w);
	btnExit->setStyleSheet("QPushButton { background-color: rgba(255,255,255,180); }");
	btnExit->hide();

	QVBoxLayout* layout = new QVBoxLayout(&w);
	layout->addStretch();
	layout->addWidget(btnExit, 0, Qt::AlignCenter);
	layout->addStretch();

	// 定时按 NumLock.
	QTimer* timer = new QTimer(&w);
	QObject::connect(timer, &QTimer::timeout, []() {
		pressNumLockTwice();
		});
	timer->start(300000); // 每 5min 触发一次，每次按两次 NumLock.

	QObject::connect(btnExit, &QPushButton::clicked, [&]() {
		QApplication::quit();
		});

	w.show();
	return a.exec();
}