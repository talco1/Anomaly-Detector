#include "minCircle.h"
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

//find the distance between two points
float distance(float x1, float x2, float y1, float y2){
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

//find the circle's center and radius given 1,2 or 3 points
Circle findCircle(const vector<Point> &boundarypoints){
    if (boundarypoints.size() == 0){
        return Circle(Point(0, 0), 0);
    }
    if (boundarypoints.size() == 1){
        return Circle(boundarypoints.at(0), 0);
    }
    float x1 = boundarypoints.at(0).x;
    float x2 = boundarypoints.at(1).x;
    float y1 = boundarypoints.at(0).y;
    float y2 = boundarypoints.at(1).y;
    if (boundarypoints.size() == 2){
        //the center is in the middle of the line stretches between the two points
        float x = (x1 + x2) / 2;
        float y = (y1 + y2) / 2;
        float radius = distance(x, x1, y, y1);
        return Circle(Point(x, y), radius);
    }
    if (boundarypoints.size() == 3){
        float x3 = boundarypoints.at(2).x;
        float y3 = boundarypoints.at(2).y;
        //find the circle's center by placing the 3 points in the circle
        //equation: (x-x_center)^2+(y-y_center)^2=r^2
        float a = 2 * x1 - 2 * x2;
        float b = 2 * y1 - 2 * y2;
        float c = x2 * x2 + y2 * y2 - x1 * x1 - y1 * y1;
        float e = 2 * x1 - 2 * x3;
        float f = 2 * y1 - 2 * y3;
        float g = x3 * x3 + y3 * y3 - x1 * x1 - y1 * y1;
        float y = (c * e - a * g) / (-1 * e * b + a * f);
        float x = (-1 * c - 1 * b * y) / a;
        float radius = distance(x, x1, y, y1);
        return Circle(Point(x, y), radius);
    }
    return Circle(Point(0, 0), 0);
}

// recursive helper function
Circle recursiveFindMinCircle(Point **points, size_t size, vector<Point> boundaryPoints){
    //if points is empty or if we have enough points to calculate the circle equation
    if (size == 0 || boundaryPoints.size() == 3){
        return findCircle(boundaryPoints);
    }
    //generate random point
    size_t randomIndex = rand() % size;
    //swap the random point with the point in the end of the array
    Point *temp = points[randomIndex];
    points[randomIndex] = points[size - 1];
    points[size - 1] = temp;
    //we send the array "without" the random point
    Circle tempCircle = recursiveFindMinCircle(points, size - 1, boundaryPoints);
    //find the distance between the random point and the center of the circle
    float dis = distance(points[size - 1]->x, tempCircle.center.x, points[size - 1]->y, tempCircle.center.y);
    //if the distance is not bigger then the radius it means the point is in the circle
    if (dis <= tempCircle.radius){
        return tempCircle;
    }
    //if the point is not in the circle, it should be on its boundary
    boundaryPoints.push_back(Point(points[size - 1]->x, points[size - 1]->y));
    return recursiveFindMinCircle(points, size - 1, boundaryPoints);
}

// implement
Circle findMinCircle(Point **points, size_t size){
    vector<Point> p;
    return recursiveFindMinCircle(points, size, p);
}