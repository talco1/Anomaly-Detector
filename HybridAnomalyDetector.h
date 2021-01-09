

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
public:
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();
	void setThreshold(float n);
protected:
	virtual bool checkAnomaly(Point p, correlatedFeatures cf);
	virtual void updateCf(correlatedFeatures* cf, Point** points, int size);
	virtual bool isCorrelated(float pearson);

};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
