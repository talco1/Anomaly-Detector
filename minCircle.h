
#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_

#include <iostream>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "anomaly_detection_util.h"

using namespace std;


// ------------ DO NOT CHANGE -----------
//class Point{
//public:
//	float x,y;
//	Point(float x,float y):x(x),y(y){}
//};

class Circle{
public:
	Point center;
	float radius;
	Circle(Point c,float r):center(c),radius(r){}
};
// --------------------------------------



float dist(Point a, Point b);

Circle from2points(Point a,Point b);

Point circumcenter(Point b, Point c);

Circle from3Points(Point a, Point b, Point c);

Circle trivial(vector<Point>& P);

Circle welzl(Point** P,vector<Point> R, size_t n);

Circle findMinCircle(Point** points,size_t size);

Circle recursiveFindMinCircle(Point **points, size_t size, vector<Point> boundaryPoints);

Circle findCircle(const vector<Point> &boundarypoints);

float distance(float x1, float x2, float y1, float y2);

#endif /* MINCIRCLE_H_ */
