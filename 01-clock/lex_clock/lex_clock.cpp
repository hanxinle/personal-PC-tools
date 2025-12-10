// .cpp file
#include "lex_clock.h"

#include <windows.h>
#include <mmsystem.h>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>
#include <QDialog>
#include <QPushButton>

lex_clock::lex_clock(QWidget* parent)
	: QWidget(parent), isAlarmTriggered(false)
{
	// 创建控件
	setAlarmButton = new QPushButton("设置闹钟", this);
	deleteAlarmButton = new QPushButton("删除选中闹钟", this);
	timeEdit = new QTimeEdit(this);
	alarmListWidget = new QListWidget(this);

	// 设置默认时间
	timeEdit->setTime(QTime(12, 0));  // 默认时间为 12:00

	// 创建布局并将控件添加到布局中
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(timeEdit);
	layout->addWidget(setAlarmButton);
	layout->addWidget(alarmListWidget);
	layout->addWidget(deleteAlarmButton);

	// 设置窗口布局
	setLayout(layout);

	// 每秒检查一次时间
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &lex_clock::onTimerTick);
	timer->start(1000);  // 每秒触发一次

	// 连接设置闹钟按钮的点击事件
	connect(setAlarmButton, &QPushButton::clicked, this, &lex_clock::onSetAlarmButtonClicked);
	connect(deleteAlarmButton, &QPushButton::clicked, this, &lex_clock::onDeleteAlarmButtonClicked);
}

lex_clock::~lex_clock() {}

void lex_clock::onTimerTick()
{
	QTime currentTime = QTime::currentTime();

	// 遍历闹钟列表，检查是否有闹钟时间到达
	for (int i = 0; i < alarmList.size(); ++i) {
		// 如果当前时间的时和分与闹钟的时和分相同
		if (currentTime.hour() == alarmList[i].hour() && currentTime.minute() == alarmList[i].minute()) {
			// 如果这个时间还没有被触发过
			if (!triggeredAlarms.contains(alarmList[i])) {
				// 创建一个新的 QDialog 用于显示 GIF
				QDialog* gifDialog = new QDialog(this);  // 创建独立的对话框

				// 创建 QLabel 用于显示 GIF
				QLabel* label = new QLabel(gifDialog);  // 创建标签并将其放置到新的对话框中

				// 创建 QMovie 来加载 GIF 文件
				QMovie* movie = new QMovie(":/lex_clock/bk.gif");  // 请替换为实际 GIF 文件的路径
				label->setMovie(movie);

				// 设置 QLabel 大小以适应 400x300 大小
				label->setFixedSize(400, 300);  // 设置标签大小为 400x300

				// 启动 GIF 动画
				movie->start();

				// 创建文本标签
				QLabel* textLabel = new QLabel("闹钟时间：" + alarmList[i].toString(), gifDialog);
				textLabel->setAlignment(Qt::AlignCenter);  // 文字居中显示
				textLabel->setStyleSheet("font-size: 18px; color: white;");  // 设置字体大小和颜色

				// 创建布局，并将 QLabel 和文字添加到对话框中
				QVBoxLayout* dialogLayout = new QVBoxLayout(gifDialog);
				dialogLayout->addWidget(label);  // 将 GIF 添加到布局
				dialogLayout->addWidget(textLabel);  // 将文本添加到布局

				// 设置 QDialog 的大小为 400x300
				gifDialog->setFixedSize(400, 300);  // 设置对话框大小为 400x300

				// 设置定时器，32秒后关闭窗口
				QTimer::singleShot(32000, gifDialog, &QDialog::accept);  // 32秒后自动关闭

				// 显示对话框，不阻塞主窗口
				gifDialog->show();

				// 标记这个闹钟时间已经触发过
				triggeredAlarms.append(alarmList[i]);

				// 从闹钟列表中删除已触发的闹钟
				alarmList.removeAt(i);  // 从闹钟列表中删除触发的闹钟

				// 同步删除 QListWidget 中的闹钟项
				delete alarmListWidget->takeItem(i);  // 从 QListWidget 中删除对应的项
				// 播放铃声
				PlaySound(TEXT("bell.wav"), NULL, SND_FILENAME | SND_ASYNC);
				break;  // 停止进一步检查
			}
		}
	}
}

void lex_clock::onSetAlarmButtonClicked()
{
	// 获取用户输入的时间
	alarmTime = timeEdit->time();  // 获取 QTimeEdit 中的时间

	// 将闹钟时间添加到列表
	alarmList.append(alarmTime);

	// 更新 QListWidget 显示
	alarmListWidget->addItem(alarmTime.toString());

	// 重置 flag，以便用户重新设置闹钟
	isAlarmTriggered = false;
}

void lex_clock::onDeleteAlarmButtonClicked()
{
	// 获取当前选中的闹钟项
	QListWidgetItem* selectedItem = alarmListWidget->currentItem();
	if (selectedItem) {
		// 从列表中删除该项
		int row = alarmListWidget->row(selectedItem);
		alarmListWidget->takeItem(row);

		// 从闹钟列表中删除
		alarmList.removeAt(row);

		// 从已触发的闹钟列表中删除
		triggeredAlarms.removeAll(alarmList[row]);
	}
}
