// .h header file.
#pragma once

#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QTimeEdit>
#include <QTime>
#include <QListWidget>

class lex_clock : public QWidget
{
	Q_OBJECT

public:
	lex_clock(QWidget* parent = nullptr);
	~lex_clock();

private:
	QPushButton* setAlarmButton;  // 设置闹钟按钮
	QPushButton* deleteAlarmButton;  // 删除选中闹钟按钮
	QTimeEdit* timeEdit;  // 时间选择控件
	QTime alarmTime;  // 用于存储用户输入的闹钟时间
	bool isAlarmTriggered;  // 用于标记是否已经弹出过提示

	QList<QTime> alarmList;  // 存储所有闹钟时间
	QListWidget* alarmListWidget;  // 显示闹钟列表的控件
	QList<QTime> triggeredAlarms;  // 存储已经弹出的闹钟时间

private slots:
	void onTimerTick();  // 定时器槽函数
	void onSetAlarmButtonClicked();  // 设置闹钟按钮槽函数
	void onDeleteAlarmButtonClicked();  // 删除选中闹钟按钮槽函数
};


