#include <QtMath>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    memset(&spa, 0, sizeof(spa));
    ui->setupUi(this);

    // Setup default values
    spa.timezone      = ui->sldTimeZone->value();
    spa.delta_t       = ui->sldDeltaT->value();
    spa.elevation     = ui->sldElevation->value();
    spa.pressure      = ui->sldPressure->value();
    spa.temperature   = ui->sldTemperature->value();
    spa.slope         = ui->sldSlope->value();
    spa.azm_rotation  = ui->sldAzmRot->value();
    spa.atmos_refract = ui->sldRefract->value() / 100.0;
    spa.latitude      = ui->spnLatitude->value();
    spa.longitude     = ui->spnLongitude->value();
    spa.function = SPA_ALL;

    ui->lblTimeZoneVal    -> setText(((spa.timezone > 0) ? "+" : "") + QString::number(spa.timezone));
    ui->lblDeltaTVal      -> setText(QString::number(spa.delta_t));
    ui->lblElevationVal   -> setText(QString::number(spa.elevation));
    ui->lblPressureVal    -> setText(QString::number(spa.pressure));
    ui->lblTemperatureVal -> setText(QString::number(spa.temperature));
    ui->lblSlopeVal       -> setText(QString::number(spa.slope));
    ui->lblAzmRotVal      -> setText(QString::number(spa.azm_rotation));
    ui->lblRefractVal     -> setText(QString::number(spa.atmos_refract));

    setDateTime();
    connect(ui->chbCurrentTime, SIGNAL(toggled(bool)), this, SLOT(onCurrentDate(bool)));
    ui->chbCurrentTime->setChecked(true);

    connect(ui->spnYear, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    connect(ui->spnMonth, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    connect(ui->spnDay, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    connect(ui->spnHour, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    connect(ui->spnMinute, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    connect(ui->spnSecond, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));

    connect(ui->spnLatitude, SIGNAL(valueChanged(double)), this, SLOT(onCoordChanged()));
    connect(ui->spnLongitude, SIGNAL(valueChanged(double)), this, SLOT(onCoordChanged()));

    connect(ui->calendarWidget, SIGNAL(selectionChanged()), this, SLOT(onCalendarChanged()));

    connect(ui->sldElevation, SIGNAL(valueChanged(int)), this, SLOT(onElevationChanged(int)));
    connect(ui->sldPressure, SIGNAL(valueChanged(int)), this, SLOT(onPressureChanged(int)));
    connect(ui->sldTemperature, SIGNAL(valueChanged(int)), this, SLOT(onTemperatureChanged(int)));
    connect(ui->sldSlope, SIGNAL(valueChanged(int)), this, SLOT(onSlopeChanged(int)));
    connect(ui->sldAzmRot, SIGNAL(valueChanged(int)), this, SLOT(onAzmRotChanged(int)));
    connect(ui->sldRefract, SIGNAL(valueChanged(int)), this, SLOT(onRefractChanged(int)));
    connect(ui->sldTimeZone, SIGNAL(valueChanged(int)), this, SLOT(onTimeZoneChanged(int)));
    connect(ui->sldDeltaT, SIGNAL(valueChanged(int)), this, SLOT(onDeltaTChanged(int)));

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    _timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete _timer;
    delete ui;
}

void MainWindow::onTimer()
{
    if (ui->chbCurrentTime->isChecked())
        setDateTime();
}

void MainWindow::onCurrentDate(bool checked)
{
    ui->calendarWidget->setEnabled(!checked);
    ui->spnYear->setEnabled(!checked);
    ui->spnMonth->setEnabled(!checked);
    ui->spnDay->setEnabled(!checked);
    ui->spnHour->setEnabled(!checked);
    ui->spnMinute->setEnabled(!checked);
    ui->spnSecond->setEnabled(!checked);
}

void MainWindow::onDateTimeChanged()
{
    disconnect(ui->calendarWidget, SIGNAL(selectionChanged()), this, SLOT(onCalendarChanged()));
    ui->calendarWidget->setSelectedDate(QDate(ui->spnYear->value(), ui->spnMonth->value(), ui->spnDay->value()));
    spa.year = ui->spnYear->value();
    spa.month = ui->spnMonth->value();
    spa.day = ui->spnDay->value();
    spa.hour = ui->spnHour->value();
    spa.minute = ui->spnMinute->value();
    spa.second = ui->spnSecond->value();
    doCalc();
    connect(ui->calendarWidget, SIGNAL(selectionChanged()), this, SLOT(onCalendarChanged()));
}

void MainWindow::onCalendarChanged()
{
    disconnect(ui->spnYear, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    disconnect(ui->spnMonth, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    disconnect(ui->spnDay, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));

    QDate d = ui->calendarWidget->selectedDate();
    ui->spnYear->setValue(d.year());
    ui->spnMonth->setValue(d.month());
    ui->spnDay->setValue(d.day());

    spa.year = d.year();
    spa.month = d.month();
    spa.day = d.day();
    doCalc();

    connect(ui->spnYear, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    connect(ui->spnMonth, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
    connect(ui->spnDay, SIGNAL(valueChanged(int)), this, SLOT(onDateTimeChanged()));
}

void MainWindow::onCoordChanged()
{
    spa.latitude = ui->spnLatitude->value();
    spa.longitude = ui->spnLongitude->value();
    doCalc();
}

void MainWindow::onElevationChanged(int value)
{
    ui->lblElevationVal->setText(QString::number(value));
    spa.elevation = value;
    doCalc();
}

void MainWindow::onPressureChanged(int value)
{
    ui->lblPressureVal->setText(QString::number(value));
    spa.pressure = value;
    doCalc();
}

void MainWindow::onTemperatureChanged(int value)
{
    ui->lblTemperatureVal->setText(QString::number(value));
    spa.temperature = value;
    doCalc();
}

void MainWindow::onSlopeChanged(int value)
{
    ui->lblSlopeVal->setText(QString::number(value));
    spa.slope = value;
    doCalc();
}

void MainWindow::onAzmRotChanged(int value)
{
    ui->lblAzmRotVal->setText(QString::number(value));
    spa.azm_rotation = value;
    doCalc();
}

void MainWindow::onRefractChanged(int value)
{
    double val = value / 100.0;
    ui->lblRefractVal->setText(QString::number(val));
    spa.atmos_refract = val;
    doCalc();
}

void MainWindow::onTimeZoneChanged(int value)
{
    QString tz = QString::number(value);
    if (value > 0)
        tz.prepend("+");
    ui->lblTimeZoneVal->setText(tz);
    spa.timezone = value;
    doCalc();
}

void MainWindow::onDeltaTChanged(int value)
{
    ui->lblDeltaTVal->setText(QString::number(value));
    spa.delta_t = value;
    doCalc();
}

void MainWindow::setDateTime()
{
    QDateTime d = QDateTime::currentDateTime();

    ui->calendarWidget->setSelectedDate(d.date());
    ui->spnYear->setValue(d.date().year());
    ui->spnMonth->setValue(d.date().month());
    ui->spnDay->setValue(d.date().day());
    ui->spnHour->setValue(d.time().hour());
    ui->spnMinute->setValue(d.time().minute());
    ui->spnSecond->setValue(d.time().second());
}

void MainWindow::doCalc()
{
    int result = spa_calculate(&spa);
    if (result == 0)
    {
        ui->txtResult->clear();
        ui->txtResult->appendPlainText("Julian Day: " + QString::number(spa.jd, 'f'));
        ui->txtResult->appendPlainText("L: " + QString::number(spa.l) + "°");
        ui->txtResult->appendPlainText("B: " + QString::number(spa.b) + "°");
        ui->txtResult->appendPlainText("R: " + QString::number(spa.r) + "AU");
        ui->txtResult->appendPlainText("H: " + QString::number(spa.h) + "°");
        ui->txtResult->appendPlainText("Delta Psi: " + QString::number(spa.del_psi) + "°");
        ui->txtResult->appendPlainText("Delta Epsilon: " + QString::number(spa.del_epsilon) + "°");
        ui->txtResult->appendPlainText("Epsilon: " + QString::number(spa.epsilon) + "°");
        ui->txtResult->appendPlainText("Zenith: " + QString::number(spa.zenith) + "°");
        ui->txtResult->appendPlainText("Azimuth: " + QString::number(spa.azimuth) + "°");
        ui->txtResult->appendPlainText("Incidence: " + QString::number(spa.incidence) + "°");

        double min, sec;
        min = 60.0 * modf(spa.sunrise, nullptr);
        sec = 60.0 * modf(min, nullptr);
        ui->txtResult->appendPlainText(QString("Sunrise: %1:%2:%3 Local Time")
                                       .arg(spa.sunrise, 2, 'f', 0, '0')
                                       .arg(min, 2, 'f', 0, '0')
                                       .arg(sec, 2, 'f', 0, '0'));

        min = 60.0 * modf(spa.sunset, nullptr);
        sec = 60.0 * modf(min, nullptr);
        ui->txtResult->appendPlainText(QString("Sunset: %1:%2:%3 Local Time")
                                       .arg(spa.sunset, 2, 'f', 0, '0')
                                       .arg(min, 2, 'f', 0, '0')
                                       .arg(sec, 2, 'f', 0, '0'));
    } else
    {
        ui->txtResult->setPlainText("Error: " + QString::number(result));
    }
}
