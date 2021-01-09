#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector() {
	// TODO Auto-generated constructor stub
}

HybridAnomalyDetector::~HybridAnomalyDetector() {
	// TODO Auto-generated destructor stub
}

bool HybridAnomalyDetector::isCorrelated(float pearson) {
	return fabs(pearson) > 0.5;
}

void HybridAnomalyDetector::updateCf(correlatedFeatures* cf, Point** points, int size) {
	if (cf->corrlation >= threshold) {
		SimpleAnomalyDetector::updateCf(cf, points, size);
	} else {
		Circle minCircle = findMinCircle(points, size);
		cf->center = new Point(minCircle.center.x, minCircle.center.y);
		cf->threshold = minCircle.radius*1.1;
	}
}

bool HybridAnomalyDetector::checkAnomaly(Point p, correlatedFeatures cf){
	if (cf.corrlation >= threshold) {
		return SimpleAnomalyDetector::checkAnomaly(p, cf);
	}
	float distance = sqrt((p.x - cf.center->x)*(p.x - cf.center->x) + (p.y - cf.center->y)*(p.y - cf.center->y));
	return distance > cf.threshold;
}

void HybridAnomalyDetector::setThreshold(float n) {
	threshold = n;
}