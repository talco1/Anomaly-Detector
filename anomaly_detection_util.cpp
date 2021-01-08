/*
 * animaly_detection_util.cpp
 *
 * Author: Tal Cohen 319021457
 */

#include <math.h>
#include "anomaly_detection_util.h"

float avg(float* x, int size){
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += x[i];
	}
	return sum / size;
}

// returns the variance of X and Y
float var(float* x, int size){
	float doubleSum = 0;
    for (int i=0; i < size; i++) {
        doubleSum += pow(x[i], 2);
    }
    float average = avg(x, size), doubleAvg = doubleSum / size;
    float variance = doubleAvg - pow (average, 2);
    return variance;
}

// returns the covariance of X and Y
float cov(float* x, float* y, int size){
	float sum = 0;
	float avgX = avg(x, size);
	float avgY = avg(y, size);
    for (int i=0; i < size; i++) {
        sum += (x[i] - avgX) * (y[i] - avgY);
    }
    return sum / size;
}

// returns the Pearson correlation coefficient of X and Y
float pearson(float* x, float* y, int size){
	float c = cov(x, y, size);
    float varX = var(x, size), varY = var(y, size);
    return c / (sqrt(varX) * sqrt(varY));
}

// performs a linear regression and returns the line equation
Line linear_reg(Point** points, int size){
	float* xPoints = new float[size];
    float* yPoints = new float[size];
    for (int i=0; i < size; i++) {
        xPoints[i] = points[i]->x;
        yPoints[i] = points[i]->y;
    }
    float a = cov(xPoints, yPoints, size) / var(xPoints, size);
    float avgX = avg(xPoints, size), avgY = avg(yPoints, size);
    float b = avgY - (a * avgX);
	delete[] xPoints;
	delete[] yPoints;
    return Line(a, b);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p,Point** points, int size){
	Line l = linear_reg(points, size);
    float result = l.f(p.x) - p.y;
    if (result < 0) {
        result *= -1;
    }
    return result;
}

// returns the deviation between point p and the line
float dev(Point p,Line l){
	float result = l.f(p.x) - p.y;
    if (result < 0) {
        result *= -1;
    }
    return result;
}