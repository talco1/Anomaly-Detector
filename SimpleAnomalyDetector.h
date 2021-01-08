

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
	string feature1,feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
	Point* center;
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
protected:
	vector<correlatedFeatures> cf;
	float threshold;
public:
	SimpleAnomalyDetector();
	virtual ~SimpleAnomalyDetector();

	virtual void learnNormal(const TimeSeries& ts);
	virtual vector<AnomalyReport> detect(const TimeSeries& ts);

	vector<correlatedFeatures> getNormalModel(){
		return cf;
	}
protected:
	virtual void updateCf(correlatedFeatures* cf, Point** points, int size);

	void findCorrelatedFeatures(const map<string, vector<float>>& m, const list<string>& features);
	float* vectorToArray(vector<float> m, int size);
	Point** arrayToPoints(float* x, float* y, int size);
	float findThreshold(Point** points, int size, Line line);
	virtual bool checkAnomaly(Point p, correlatedFeatures cf);
	
	virtual bool isCorrelated(float pearson);
};



#endif /* SIMPLEANOMALYDETECTOR_H_ */
