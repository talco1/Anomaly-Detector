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
	vector<AnomalyReport> reports;
};

class LinkingClass {
public:
	linkerStruct linker;
	LinkingClass(){}
	~LinkingClass(){}
	void setTrainTs(const char* CSVfileName) {
		TimeSeries* ts = new TimeSeries(CSVfileName);
		linker.trainTs = ts;
	}
	void setTestTs(const char* CSVfileName) {
		TimeSeries* ts = new TimeSeries (CSVfileName);
		linker.testTs = ts;
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
		description = "1.upload a time series csv file\n";
	}
	virtual void execute() {
		//train file
		dio->write("Please upload your local train CSV file.\n");
		writeFile("anomalyTrain.csv");
		//test file
		dio->write("Please upload your local test CSV file.\n");
		data->linker.numOfLines = writeFile("anomalyTest.csv");
		//save files in timeseries
		data->setTrainTs("anomalyTrain.csv");
		data->setTestTs("anomalyTest.csv");
	}
private:
	// method to create file
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
		dio->write("Upload complete.\n");
		return lines;
	}
};

class Command2:public Command {
public:
	Command2(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "2.algorithm settings\n";
	}
	virtual void execute() {
		dio->write("The current correlation threshold is 0.9\n");
		string input = dio->read(); //read new correlation thershold
		while(stof(input) < 0 || stof(input) > 1) { //it threshold is in the wrong range
			dio->write("please choose a value between 0 and 1.\n");
			dio->read(); //for enter
			dio->write("The current correlation threshold is 0.9\n");
			input = dio->read();
		}
		dio->write("Type a new threshold\n");
		data->linker.detector.setThreshold(stof(input)); //update threshold
	}
};

class Command3:public Command {
public:
	Command3(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "3.detect anomalies\n";
	}
	virtual void execute() {
		data->linker.detector.learnNormal(*data->linker.trainTs);
		data->linker.reports = data->linker.detector.detect(*data->linker.testTs); //save in linking class
		dio->write("anomaly detection complete.\n");
	}
};

class Command4:public Command {
public:
	Command4(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "4.display results\n";
	}
	virtual void execute() {
		//write the anomaly reports
		for (auto it = data->linker.reports.begin(); it != data->linker.reports.end(); it++) {
			string output = to_string(it->timeStep) + " 	" + it->description + "\n";
			dio->write(output);
		}
		dio->write("Done.\n");
	}
};

class Command5:public Command {
public:
	Command5(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "5.upload anomalies and analyze results\n";
	}
	virtual void execute() {
		vector<pair<long, long>> trueAnomlies; //save the true anomalies given by the user
		vector<pair<long,long>> reportedAnomalies; //save the anomalies reported by the detector
		int P = 0, N = 0, TP = 0, FP = 0, sumOfAnomalies = 0;
		dio->write("Please upload your local anomalies file.\n");
		string input = dio->read();
		//read the anomalies
		while(input != "done") {
			P++; //amount of anomalies
			size_t pos = input.find(',');
			long str1 = stol(input.substr(0, pos)), str2 = stol(input.substr(pos + 1));
			trueAnomlies.push_back({str1, str2}); //save the range of the anomaly
			sumOfAnomalies += (str2 - str1 + 1); //sum of all the timesteps of the anomalies
			input = dio->read();
		}
		dio->write("Upload complete.\n");
		auto it = data->linker.reports.begin();
		//merge all the continuous reports of the same description
		while (it != data->linker.reports.end()) {
			int start = it->timeStep, end = it->timeStep;
			string desc = it->description;
			it++;
			while ((it->description == desc) && (end == (it->timeStep - 1))) {
				end = it->timeStep;
				it++;
			}
			reportedAnomalies.push_back({start, end}); //save the range of the report
		}
		bool isReported[P]; // if true anomaly was detected
		for (auto it = reportedAnomalies.begin(); it != reportedAnomalies.end(); it++) {
			int i = 0;
			bool isTrueReport = false; // if the reported anomaly is true
			for (auto it2 = trueAnomlies.begin(); it2 != trueAnomlies.end(); it2++) {
				// the reported anomaly is in the time frame of the true anomalies
				if ((it->first >= it2->first && it->first <= it2->second) 
				|| (it->second >= it2->first && it->second <= it2->second) 
				|| (it->first <= it2->first && it->second >= it2->second)) {
					isTrueReport = true;
					if(!isReported[i]) {
						TP++; // this time frame of anomaly was reported for the first time
					}
					isReported[i] = true; //this time frame was detected
				}
				i++;
			}
			if (!isTrueReport) {
				FP++; //false report
			}
		}
		N = data->linker.numOfLines - sumOfAnomalies; //time atep without anomaly
		float tpr = (float)TP/P, fpr = (float)FP/N;
		//round tpr and fpr
		tpr = floor(tpr * 1000.0) / 1000.0;
		fpr = floor(fpr * 1000.0) / 1000.0;
		stringstream s1, s2;
		s1 << tpr;
		s2 << fpr;
		string output = "True Positive Rate: " + s1.str() + "\n";
		dio->write(output);
		output = "False Positive Rate: " + s2.str() + "\n";
		dio->write(output);
	}
};

class Command6:public Command {
public:
	Command6(DefaultIO* dio, LinkingClass* data):Command(dio, data) {
		description = "6.exit\n";
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
