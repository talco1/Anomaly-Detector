#include <iostream> 
#include <iterator> 
#include <map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <fstream>
#include <sstream>
#include <list>

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

using namespace std;

class TimeSeries{
	map<string, vector<float>> data;
	list<string> features;
public:

	TimeSeries(const char* CSVfileName){
		loadData(CSVfileName);
	}
	const map<string, vector<float>>& getData() const;
	const list<string>& getFeatures() const;

private:
	void loadData(const char* CSVfileName);
};



#endif /* TIMESERIES_H_ */
