#include "timeseries.h"

//update the members of the class according to the data from the CSV file
void TimeSeries::loadData(const char* CSVfileName) {
    ifstream file(CSVfileName); 
    string line, word;
    int colIndex = 0;
    bool firstCol = true; //features line
    map<int, string> colName; //map between number of column to feature's name
    while (getline(file, line)) {
        stringstream ss(line);
        //features column
        if (firstCol) {
            while (getline(ss, word, ',')) {
                this->data[word]; //create new key of this feature
                colName.insert({colIndex++, word});
            }
        }
        else {
            colIndex = 0; //initialize column before  while loop
            while (getline(ss, word, ',')) {
                string name = colName.find(colIndex++)->second; //name of feature in the column
                this->data[name].push_back(stof(word)); //add value to he map
            }
        }
        firstCol = false; // after first loop
    }
    file.close();
    //update list of features
    map<string, vector<float>>::const_iterator it;
    for (it = data.begin(); it != data.end(); it++) {
        this->features.push_back(it->first);
    }
}

//return the data from the CSV file
const map<string, vector<float>>& TimeSeries::getData() const {
    return data;
}

//return the features from the CSV file
const list<string>& TimeSeries::getFeatures() const {
    return features;
}