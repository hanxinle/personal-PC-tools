#pragma once

#include <QtWidgets/QWidget>
#include "ui_no_screen_lock.h"

class no_screen_lock : public QWidget
{
    Q_OBJECT

public:
    no_screen_lock(QWidget *parent = nullptr);
    ~no_screen_lock();

private:
    Ui::no_screen_lockClass ui;
};

