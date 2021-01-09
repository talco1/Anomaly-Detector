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
	int numOfLines;
};

class LinkingClass {
public:
	linkerStruct linker;
	LinkingClass(){}
	~LinkingClass(){}
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
	LinkingClass* data;
public:
	string description;
	Command(DefaultIO* dio, LinkingClass* data):dio(dio), data(data){}
	virtual void execute()=0;
	virtual ~Command(){}
};

// implement here your command classes

class Command1:public Command{
public:
	Command1(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "1. upload a time series csv file\n";
	}
	virtual void execute() {
		//train file
		dio->write("Please uplaod your local train CSV file.\n");
		writeFile("anomalyTrain.csv");
		//test file
		dio->write("Please uplaod your local test CSV file.\n");
		data->linker.numOfLines = writeFile("anomalyTest.csv");
		//save files in timeseries
		data->setTrainTs("anomalyTrain.csv");
		data->setTestTs("anomalyTest.csv");
	}
private:
	int writeFile(string fileName) {
		ofstream out(fileName);
		int lines = 0;
		string input = dio->read();
		while (input != "done") {
			lines++;
			out << input << endl;
			input = dio->read();
		}
		out.close();
		dio->write("Upload Complete.\n");
		return lines;
	}
};

class Command2:public Command {
public:
	Command2(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "2. algorithm settings\n";
	}
	virtual void execute() {
		dio->write("The current correlation threshold is 0.9\n");
		string input = dio->read();
		while(stof(input) < 0 || stof(input) > 1) {
			dio->write("please choose a value between 0 and 1.\n");
			dio->read(); // check if needed or not --- for enter
			dio->write("The current correlation threshold is 0.9\n");
			input = dio->read();
		}
		data->linker.detector.setThreshold(stof(input)); //update threshold
	}
};

class Command3:public Command {
public:
	Command3(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "3. detect anomalies\n";
	}
	virtual void execute() {
		data->linker.detector.learnNormal(*data->linker.trainTs);
		data->linker.detector.detect(*data->linker.testTs);
		dio->write("anomaly detection complete.\n");
	}
};

class Command4:public Command {
public:
	Command4(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "4. display results\n";
	}
	virtual void execute() {
		// check if the reports are in the correct order 
		vector<AnomalyReport> reports = data->linker.detector.detect(*data->linker.testTs);
		for (auto it = reports.begin(); it != reports.end(); it++) {
			string output = it->timeStep + "	" + it->description + "\n";
			dio->write(output);
		}
		dio->write("Done.\n");
	}
};

class Command5:public Command {
public:
	Command5(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "5. upload anomalies and analyze results\n";
	}
	virtual void execute() {
		vector<pair<long, long>> trueAnomlies;
		vector<pair<long,long>> reportedAnomalies;
		int P = 0, N = 0, TP = 0, FP = 0, sumOfAnomalies = 0;
		dio->write("Please upload your local anomalies file.\n");
		string input = dio->read();
		while(input != "done") {
			P++;
			size_t pos = input.find(',');
			long str1 = stol(input.substr(0, pos)), str2 = stol(input.substr(pos + 1));
			trueAnomlies.push_back({str1, str2}); //timesteps of the anomaly
			sumOfAnomalies += (str2 - str1 + 1);
			input = dio->read();
		}
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
			reportedAnomalies.push_back({start, end});
		}
		bool isReported[P]; // if true anomaly was detected
		for (auto it = reportedAnomalies.begin(); it != reportedAnomalies.end(); it++) {
			int i = 0;
			bool isTrueReport = false; // if the reported anomaly is true
			for (auto it2 = trueAnomlies.begin(); it2 != trueAnomlies.end(); it2++) {
				if ((it->first >= it2->first && it->first <= it2->second) 
				|| (it->second >= it2->first && it->second <= it2->second)) {
					isTrueReport = true; // the reported anomaly is true
					if(!isReported[i]) {
						TP++; // this frame of anomaly was reported for the first time
					}
					isReported[i] = true;
				}
				i++;
			}
			if (!isTrueReport) {
				FP++;
			}
		}
		N = data->linker.numOfLines - sumOfAnomalies;
		float tpr = floor(1000*(TP/P))/1000, far = floor(1000*(FP/N))/1000;
		string output = "True Positive Rate: " + to_string(tpr) + "\n";
		dio->write(output);
		output = "False Positive Rate: " + to_string(far) + "\n";
		dio->write(output);
	}
};

class Command6:public Command {
public:
	Command6(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "6. exit\n";
	}
	virtual void execute() {
	}
};

class UploadCommand:public Command {
public:
	UploadCommand(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "Welcome to the Anomaly Detection Server.\nPlease choose an option:\n";
	}
	virtual void execute() {
	}
};

#endif /* COMMANDS_H_ */
