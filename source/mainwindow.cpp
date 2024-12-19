// You may need to build the project (run Qt uic code generator) to get "ui_mainwindow.h" resolved

#include "../include/mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <ranges>
#include <QLabel>
#include <QComboBox>


mainwindow::mainwindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::mainwindow)
{
    ui->setupUi(this);

    setWindowTitle("Particle relaxation");

	density_Fe = 7874;
    Cp_Fe = 460;

    name_material = "Ferrum";

    QLabel *radiusLabel = new QLabel("Radius (м):", this);

    QLabel *temperatureLabel = new QLabel("Start particle temperature (K):", this);
    QLabel *maxTemperatureLabel = new QLabel("Stream temperature (K):", this);


    radiusInput = new QDoubleSpinBox(this);
    radiusInput->setRange(0.000'001, 0.1);
    radiusInput->setDecimals(6);
    radiusInput->setSingleStep(0.000'001);
    radiusInput->setValue(0.0001);


    temperatureInput = new QDoubleSpinBox(this);
    temperatureInput->setRange(298.15, 2000.0);
    temperatureInput->setValue(298.15);

    maxTemperatureInput = new QDoubleSpinBox(this);
    maxTemperatureInput->setRange(298.15, 2000.0);
    maxTemperatureInput->setValue(2000.0);

    connect(radiusInput, &QDoubleSpinBox::editingFinished, this, &mainwindow::updateGraph);
    connect(temperatureInput, &QDoubleSpinBox::editingFinished, this, &mainwindow::updateGraph);
    connect(maxTemperatureInput,&QDoubleSpinBox::editingFinished, this, &mainwindow::updateGraph);

    QHBoxLayout * radiusLayout = new QHBoxLayout();
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(radiusInput);


    QHBoxLayout * maxTemperatureLayout = new QHBoxLayout();
    maxTemperatureLayout->addWidget(maxTemperatureLabel);
    maxTemperatureLayout->addWidget(maxTemperatureInput);


    QHBoxLayout * temperatureLayout = new QHBoxLayout();
    temperatureLayout->addWidget(temperatureLabel);
    temperatureLayout->addWidget(temperatureInput);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addLayout(temperatureLayout);
    layout->addLayout(radiusLayout);
    layout->addLayout(maxTemperatureLayout);

    graphwidget = new class graphwidget(this);
    layout->addWidget(graphwidget);

    QWidget * centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);


    updateGraph();
}

mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::updateGraph()
{
    long double radius = radiusInput->value();
    long double current_temperature = temperatureInput->value();


    calculateTemperatures(radius, current_temperature);
}

void mainwindow::calculateTemperatures(long double radius, long double current_temperature)
{
    auto calc_start_time = std::chrono::high_resolution_clock::now();
    long double temperature_gas = maxTemperatureInput->value();
    std::cout << "Start calculating, radius: " << radius << ", particle t: " << current_temperature << ", gas t: " << temperature_gas << std::endl;
    long double previous_temperature;

    long double step = 0.1;
    long double total_time = 0.0;
    size_t counter = 0;
    std::vector<long double> time_values{0};
    std::vector<long double> temperature_values{current_temperature};
    auto beg_temperature = current_temperature;


    while (std::abs(temperature_gas - current_temperature) > eps)
    {
        previous_temperature = current_temperature;

        current_temperature = runge_kutta_4th_order(previous_temperature, step, eps,
													temperature_gas, radius,
													density_air, density_Fe, Cp_Fe,
													Cp_air, k, S, T0, v0, g);

		auto new_time = time_values.back() + step;
        time_values.push_back(new_time);
        temperature_values.push_back(current_temperature);


        long double delta_temp = std::abs(current_temperature - previous_temperature);

        counter++;

        if (delta_temp > 0.5)
        {
            step *= std::max((long double) 0.5, 0.45 / delta_temp);
            current_temperature = previous_temperature;
            --counter;
            time_values.pop_back();
            temperature_values.pop_back();

        } else if (delta_temp < 0.001 && step < 1.0)
        {
            step *= 1.2;
        }
    }


    auto h = get_h(eps, k, radius, temperature_gas, density_air, S, T0, v0, Cp_air, g);

    if (beg_temperature < temperature_gas)
    {

        total_time = (density_Fe * Cp_Fe * radius * radius * 2) / (3 * h) *
                     std::log((temperature_gas - beg_temperature) / (0.01 * temperature_gas));
    } else
    {
        total_time = -(density_Fe * Cp_Fe * radius * radius * 2) / (3 * h) *
                     std::log((0.01 * temperature_gas) / (beg_temperature - temperature_gas));
    }

    auto calc_end_time = std::chrono::high_resolution_clock::now();
    auto duration = calc_end_time - calc_start_time;

    std::cout << "total_time = " << total_time << " sec, iterations: " << counter << ", time: " << duration.count() / 1'000'000 << "ms" << std::endl;

    graphwidget->update_total_time(total_time);
    graphwidget->setTargetTemperature(temperature_gas);
    graphwidget->setData(time_values, temperature_values, radius);
	graphwidget->update();

    size_t intersectionIndex = -1;

    auto it = std::find_if(temperature_values.begin(), temperature_values.end(), [temperature_gas](auto val) {
       return std::abs(val - temperature_gas) < 0.001;
    });

    if (it != temperature_values.end())
        intersectionIndex = std::distance(temperature_values.begin(), it);

    if (intersectionIndex != -1)
    {
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Can't open file!");

        file << "#" << beg_temperature << " -> " << temperature_gas << " for radius = " << std::setw(6) << radius
             << " for material: " << name_material << std::endl;

        file << "Counter"
             << "," << "Time"
             << "," << "Temperature"
             << std::endl;


        for (size_t i = 0; i < intersectionIndex; i += std::max<size_t>(counter / 150, 10))
        {

            file << i
                 << "," << std::fixed << std::setprecision(6)
                 << (time_values[i] * total_time) / (time_values[intersectionIndex])
                 << "," << temperature_values[i]
                 << std::endl;
        }


        file << counter - 1
             << "," << total_time
             << "," << temperature_values[intersectionIndex]
             << std::endl;

        file << "\n\n\n" << std::endl;

        file.close();
    } else
    {
        throw std::runtime_error("No intersection found");
    }
}
