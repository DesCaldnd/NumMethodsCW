

#ifndef CWNUM_GRAPHWIDGET_H
#define CWNUM_GRAPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include <QTimer>




class graphwidget : public QWidget {
Q_OBJECT

public:
    explicit graphwidget(QWidget *parent = nullptr);

    void setData(const std::vector<long double>& time, const std::vector<long double>& temperature, long double radius);
    void setTargetTemperature(long double targetTemperature);
    void update_total_time(long double new_total_time);


protected:
    void paintEvent(QPaintEvent *event) override;

private slots:

    void updateGraphStep();


private:
    std::vector<long double> timeValues;
    std::vector<long double> temperatureValues;

    long double targetTemperature = 0;
    int pointsToShow = 0;
//    QTimer* timer;
    long double total_time;
    long double copy_total_time;
    long double radius;


};


#endif //CWNUM_GRAPHWIDGET_H
