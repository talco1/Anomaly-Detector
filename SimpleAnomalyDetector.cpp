#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::SimpleAnomalyDetector() {
	threshold = 0.9;
}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
	// TODO Auto-generated destructor stub
}

float* SimpleAnomalyDetector::vectorToArray(vector<float> vec, int size) {
	float* arr = new float[size];
	std::copy(vec.begin(), vec.end(), arr);
	return arr;
}

Point** SimpleAnomalyDetector::arrayToPoints(float* x, float* y, int size) {
	Point** points = new Point*[size];
	for(int i = 0; i < size; i++) {
		points[i] = new Point(x[i], y[i]);
	}
	return points;	
}

float SimpleAnomalyDetector::findThreshold(Point** points, int size, Line line) {
	float max = 0;
	for (int i = 0; i < size; i++) {
		float temp = dev(*points[i], line);
		if (temp > max) {
			max = temp;
		}
	}
	return max;
}

void SimpleAnomalyDetector::updateCf(correlatedFeatures* cf, Point** points, int size) {
	cf->lin_reg = linear_reg(points, size); // find linear regression of points
	cf->threshold = findThreshold(points, size, cf->lin_reg)*1.1; // find threshold of features
}

bool SimpleAnomalyDetector::checkAnomaly(Point p, correlatedFeatures cf) {
	return dev(p, cf.lin_reg) > cf.threshold;
}

 bool SimpleAnomalyDetector::isCorrelated(float pearson) {
	 return fabs(pearson) > threshold;
 }

//find for correlated features and update the member cf
void SimpleAnomalyDetector::findCorrelatedFeatures(const map<string, vector<float>>& m, const list<string>& features) {
	string relative; //name of relative feature
	float maxPear = 0;
	int i = 0, j = 0, relativeIdx; //indexes of for loops and features
	list<string>:: const_iterator itFirst;
	list<string>:: const_iterator itSecond;
	for (itFirst=features.begin(); itFirst !=features.end(); itFirst++) {
		//intialize arguments of inner loop
		j = i + 1;
		maxPear = 0;
		int size = m.at(*itFirst).size();
		// conver the first feature's vector to float*
		float* firstValues = vectorToArray(m.at(*itFirst), size);
		for (itSecond=itFirst, ++itSecond; itSecond != features.end(); itSecond++) {
			// conver the second feature's vector to float*
			float* secondValues = vectorToArray(m.at(*itSecond), size);
			float tempPear = pearson(firstValues, secondValues, size); //find pearson of both features
			//find biggest pearson of feature1
			if (fabs(tempPear) > fabs(maxPear)) {
				relativeIdx = j;
				maxPear = tempPear;
				relative = *itSecond;
			}
			delete[] secondValues;
			j++;
		}
		//update cf to the new correlated features
		if (isCorrelated(maxPear)) {
			//create new correlated features
			struct correlatedFeatures cfNew = {};
			cfNew.feature1 = *itFirst;
			cfNew.feature2 = relative;
			cfNew.corrlation = maxPear;
			//create points array from the data of the features
			float* y = vectorToArray(m.at(relative), size);
			Point** points = arrayToPoints(firstValues, y, size);
			updateCf(&cfNew, points, size);
			cf.push_back(cfNew); // add new cf
			delete[] y;
			for (int i = 0; i < size; i++) {
				delete points[i];
			}
		}
		delete[] firstValues;
		i++;
	}
}

//learn and define the normal data of the time series
void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
	map<string, vector<float>> m = ts.getData();
	list<string> features = ts.getFeatures();
	findCorrelatedFeatures(m, features);
}

//detect if there is anomaly in the timeseries and return an AnomalyReport vector of anomalies
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
	const map<string, vector<float>> m = ts.getData();
	vector<AnomalyReport> anomalies;
	if (!this->cf.empty()) {
		for (auto it = cf.begin(); it != cf.end(); it++) {
			const int size = m.at(it->feature1).size();
			//float arrays to create points from feature 1 values and feature 2 values
			float* x = vectorToArray(m.at(it->feature1), size);
			float* y = vectorToArray(m.at(it->feature2), size);
			//create point and check its deviation from the line regression
			for (int i = 0; i < size; i++) {
				//if the point is deviated - update the anomaly report
				if (checkAnomaly(Point(x[i], y[i]), *it)) {
					string description = it->feature1 + "-" + it->feature2;
					anomalies.push_back(AnomalyReport(description, (i + 1)));
				}
			}
		}
	}
	return anomalies;
}