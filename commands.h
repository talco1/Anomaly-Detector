#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

	// you may add additional methods here
};

// you may add here helper classes

struct linkerStruct {
	HybridAnomalyDetector detector;
	TimeSeries* trainTs;
	TimeSeries* testTs;
};

class LinkingClass {
public:
	linkerStruct linker;
	LinkingClass();
	~LinkingClass();
	void setTrainTs(const char* CSVfileName) {
		TimeSeries* ts = new TimeSeries (CSVfileName);
		*linker.trainTs = *ts;
	}
	void setTestTs(const char* CSVfileName) {
		TimeSeries* ts = new TimeSeries (CSVfileName);
		*linker.testTs = *ts;
	}
};

// you may edit this class
class Command{
protected:
	DefaultIO* dio;
	string description;
	LinkingClass* data;
public:
	Command(DefaultIO* dio, LinkingClass* data):dio(dio), data(data){}
	virtual void execute()=0;
	virtual ~Command(){}
};

// implement here your command classes

class Command1:public Command{
private:
	static int numOfLines;
public:
	Command1(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "1. upload a time series csv file";
		numOfLines = 0;
	}
	virtual void execute() {
		//train file
		dio->write("Please uplaod your local train CSV file.");
		ofstream out("anomalyTrain.csv");
		string input = dio->read();
		while (input != "done") {
			out << input << endl;
			input = dio->read();
		}
		out.close();
		dio->write("Upload Complete.");
		//test file
		dio->write("Please uplaod your local test CSV file.");
		ofstream out("anomalyTest.csv");
		input = dio->read();
		while (input != "done") {
			numOfLines++;
			out << input << endl;
			input = dio->read();
		}
		out.close();
		dio->write("Upload Complete.");
		//save files in timeseries
		data->setTrainTs("anomalyTrain.csv");
		data->setTestTs("anomalyTest.csv");
	}
	static int getNumOfLines() {
		return numOfLines;
	}
};

class Command2:public Command {
	Command2(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "2. algorithm settings";
	}
	virtual void execute() {
		dio->write("The current correlation threshold is 0.9");
		string input = dio->read();
		while(stof(input) < 0 || stof(input) > 1) {
			dio->write("please choose a value between 0 and 1.");
			dio->read(); // check if needed or not --- for enter
			dio->write("The current correlation threshold is 0.9");
			input = dio->read();
		}
		data->linker.detector.setThreshold(stof(input)); //update threshold
	}
};

class Command3:public Command {
	Command3(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "3. detect anomalies";
	}
	virtual void execute() {
		data->linker.detector.learnNormal(*data->linker.trainTs);
		data->linker.detector.detect(*data->linker.testTs);
	}
};

class Command4:public Command {
	Command4(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "4. display results";
	}
	virtual void execute() {
		// check id the reports are in the correct order 
		vector<AnomalyReport> reports = data->linker.detector.detect(*data->linker.testTs);
		for (auto it = reports.begin(); it != reports.end(); it++) {
			string output = it->timeStep + "	" + it->description;
			dio->write(output);
		}
		dio->write("Done.");
	}
};

class Command5:public Command {
	Command5(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "5. upload anomalies and analyze results";
	}
	virtual void execute() {
		map<vector<pair<long, long>>, bool> anomaly;
		vector<pair<long, long>> anomalies; // to save the reported anomalies
		map<string, vector<pair<long, long>>> detectorAnomalies;
		int arr[2];
		int p = 0, sum = 0; // p is number of anomalies, sum is amount of anomalies
		dio->write("Please upload your local anomalies file.");
		string input = dio->read();
		// read the file
		while (input != "done") {
			p++;
			int i = 0;
			stringstream s(input);
			string subString;
			// save the time steps
			while (getline(s, subString, ',')) {
				arr[i] = stol(subString);
				i++;
			}
			anomalies.push_back({arr[0], arr[1]});
			sum += ((arr[1] - arr[0]) + 1);
			input = dio->read();
		}
		// save reported anomalies according to timestep
		vector<AnomalyReport> reports = data->linker.detector.detect(*data->linker.testTs);
		auto it = reports.begin();
		while (it != reports.end()) {
			int start = it->timeStep, end = it->timeStep;
			string desc = it->description;
			it++;
			while ((it->description == desc) && (end == (it->timeStep - 1))) {
				end = it->timeStep;
				it++;
			}
			detectorAnomalies[desc].push_back({start, end});
		}
		int n = Command1::getNumOfLines() - 1;
		int N = n - sum, FP = 0, i = 0, TP = 0;
		bool* arr[anomalies.size()];
		for (auto it = detectorAnomalies.begin(); it != detectorAnomalies.end(); it++) { //desctription
			for (auto itVec = it->second.begin(); itVec!= it->second.end(); itVec++) { //anomalies
				long x = itVec->first, y = itVec->second;
				bool isReported = false;
				i = 0;
				for (auto itAno = anomalies.begin(); itAno != anomalies.end(); itAno++) {
					if ((x >= itAno->first && x <= itAno->second) || (y >= itAno->first && y <= itAno->second)) {
						isReported = true;
						if (!arr[i]) {
							TP++;
						}
						arr[i] = true;
					}
					if (!isReported) {
						FP++;
					}
					i++;
				}
			}
		}
		float tpr = TP/p, far = FP/N;
		tpr *= 1000;
		tpr = floor(tpr);
		tpr /= 1000;
		far *= 1000;
		far = floor(tpr);
		far /= 1000;
		string output = "True Positive Rate: " + to_string(tpr);
		dio->write(output);
		output = "False Positive Rate: " + to_string(far);
		dio->write(output);
	}
};

class Command6:public Command {
	Command6(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "6. exit";
	}
	virtual void execute() {

	}
};


#endif /* COMMANDS_H_ */
