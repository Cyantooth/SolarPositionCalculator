#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "spa.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onTimer();
    void onCurrentDate(bool checked);
    void onDateTimeChanged();
    void onCalendarChanged();
    void onCoordChanged();
    void onElevationChanged(int value);
    void onPressureChanged(int value);
    void onTemperatureChanged(int value);
    void onSlopeChanged(int value);
    void onAzmRotChanged(int value);
    void onRefractChanged(int value);
    void onTimeZoneChanged(int value);
    void onDeltaTChanged(int value);

private:
    void setDateTime();
    void doCalc();

    Ui::MainWindow *ui;
    QTimer *_timer;
    spa_data spa;
};

#endif // MAINWINDOW_H
