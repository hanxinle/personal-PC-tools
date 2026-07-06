#pragma once

#include <QtWidgets/QWidget>
#include "ui_catclockpc.h"

QT_BEGIN_NAMESPACE
class QQuickWidget;
QT_END_NAMESPACE

class CatClockPC : public QWidget
{
    Q_OBJECT

public:
    CatClockPC(QWidget *parent = nullptr);
    ~CatClockPC();

private:
    Ui::CatClockPCClass ui;
    QQuickWidget *m_quickWidget;
};
