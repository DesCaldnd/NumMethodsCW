//
// Created by fedor on 31.10.2024.
//

#ifndef CWNUM_MAINWINDOW_H
#define CWNUM_MAINWINDOW_H

#include <QMainWindow>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "funcs.h"
#include "graphwidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class mainwindow; }
QT_END_NAMESPACE

class mainwindow : public QMainWindow {
Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr);

    ~mainwindow() override;

private slots:
    void updateGraph();

private:
    Ui::mainwindow *ui;
    long double S = 110.4; // добавка к температурам
    long double T0 = 273.15; // эталонная температура
    long double v0 = 1.81e-5; // вязкость при эталонной температуре
    long double Cp_air = 1.006e3; // удельная теплоемкость воздуха
    long double k = 0.023; // коэффициент теплоотдачи воздуха
    long double density_air = 1.2754; // плотность воздуха
    long double g = 9.81; // ускорение свободного падения
    long double eps = 1e-9; // точность


    long double density_material = 7874.0;
    long double Cp_Fe = 460.0;


    QDoubleSpinBox *radiusInput;
    QDoubleSpinBox *temperatureInput;
    QDoubleSpinBox *maxTemperatureInput;
    graphwidget* graphwidget;


    std::string filename = "data.txt";


    std::string name_material;

    void calculateTemperatures(long double radius, long double current_temperature);
};


#endif //CWNUM_MAINWINDOW_H
