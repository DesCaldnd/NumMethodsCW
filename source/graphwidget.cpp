
// You may need to build the project (run Qt uic code generator) to get "ui_graphwidget.h" resolved

#include "../include/graphwidget.h"
//#include "ui_graphwidget.h"
#include <QString>
#include <algorithm>
#include <iostream>

graphwidget::graphwidget(QWidget *parent) :
	QWidget(parent) {}

void graphwidget::setData(const std::vector<long double> &time,
						  const std::vector<long double> &temperature,
						  long double rad)
{
	timeValues = time;
	temperatureValues = temperature;

	radius = rad;

	pointsToShow = temperatureValues.size();
}

void graphwidget::updateGraphStep()
{
	int pointsToAdd = std::min(1000, static_cast<int>(timeValues.size()) - pointsToShow);
	if (pointsToShow < timeValues.size())
	{
		pointsToShow += pointsToAdd;
	}
}

void graphwidget::update_total_time(long double new_total_time)
{
	total_time = new_total_time;
	copy_total_time = new_total_time;
}

void graphwidget::setTargetTemperature(long double targetTemperature)
{
	this->targetTemperature = targetTemperature;
}

void graphwidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::white);

	if (timeValues.empty() || temperatureValues.empty())
	{
		return;
	}

	int intersectionIndex = -1;
	auto it = std::find_if(temperatureValues.begin(), temperatureValues.end(), [this](auto val) {
	  return std::abs(val - targetTemperature) < 0.001;
	});

	if (it != temperatureValues.end())
		intersectionIndex = std::distance(temperatureValues.begin(), it);

	long double intersectionTime = timeValues[intersectionIndex];

	long double maxTime = intersectionTime;
	long double maxTemperature = *std::max_element(temperatureValues.begin(), temperatureValues.end());
	int width = this->width();
	int height = this->height();

	painter.drawLine(50, height - 50, width - 50, height - 50); // Ось X
	painter.drawLine(50, 50, 50, height - 50); // Ось Y

	painter.setPen(Qt::white);

	painter.drawText(width - 100, height - 10, "Time (s)");
	painter.drawText(10, 30, "Temperature (K)");

	long double tempStep = std::ceil(maxTemperature / 1.0) * 0.1;

	for (long double temp = 0; temp <= maxTemperature; temp += tempStep)
	{
		int y = height - 50 - static_cast<int>((temp / maxTemperature) * (height - 100));
		painter.drawLine(45, y, 50, y);
		painter.drawText(10, y + 5, QString::number(temp, 'f', 1));
	}

	painter.setPen(Qt::lightGray);
	int xStart = 5 + static_cast<int>((timeValues[0] / maxTime) * (width - 100));
	int yStart = height - 45 - static_cast<int>((temperatureValues[0] / maxTemperature) * (height - 100));

	painter.setPen(Qt::green);
	painter.drawText(xStart, yStart - 10, QString::number(temperatureValues[0], 'f', 2));


	painter.setPen(Qt::lightGray);
	for (size_t i = 1; i < pointsToShow && i < timeValues.size() && i < intersectionIndex; ++i)
	{
		int x1 = 50 + static_cast<int>((timeValues[i - 1] / intersectionTime) * (width - 100));
		int y1 = height - 50 - static_cast<int>((temperatureValues[i - 1] / maxTemperature) * (height - 100));

		int x2 = 50 + static_cast<int>((timeValues[i] / intersectionTime) * (width - 100));
		int y2 = height - 50 - static_cast<int>((temperatureValues[i] / maxTemperature) * (height - 100));

		painter.drawLine(x1, y1, x2, y2);
	}

	int yTarget = height - 50 - static_cast<int>((targetTemperature / maxTemperature) * (height - 100));

	painter.setPen(Qt::red);
	painter.drawLine(50, yTarget, width - 50, yTarget);
	painter.setPen(Qt::white);

	painter.drawText(5, yTarget, QString::number(targetTemperature, 'f', 2));

	if (intersectionIndex != -1)
	{
		long double tmp_total = copy_total_time;

		total_time = intersectionTime;

		long double stepSize = (maxTime) / 8.0;
		int halfTicks = 6;

		for (int k = -halfTicks - 2; k <= halfTicks - 4; ++k)
		{
			long double tickTime = total_time + k * stepSize;

			painter.setPen(Qt::white);
			if (tickTime >= 0 && tickTime <= maxTime)
			{
				int xTick = 50 + static_cast<int>((tickTime / maxTime) * (width - 100));
				auto tmp = tickTime * tmp_total / total_time;

				painter.drawText(xTick, height - 30, QString::number(tmp, 'f', 1));
				painter.drawLine(xTick, height - 50, xTick, height - 45);
			}
		}

		painter.setPen(Qt::white);
		QString totalTimeText = "Total Time: " + QString::number(tmp_total, 'f', 2) + " s";
		painter.drawText(50, height, totalTimeText);
		return;
	}
}


