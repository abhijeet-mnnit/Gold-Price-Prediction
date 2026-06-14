


#ifndef GOLD_PREDICTOR_H
#define GOLD_PREDICTOR_H

#include <vector>
#include <string>

using namespace std;

struct DatasetStatistics
{
    double openMin;
    double openMax;
    double openMean;

    double highMin;
    double highMax;
    double highMean;

    double lowMin;
    double lowMax;
    double lowMean;

    double closeMin;
    double closeMax;
    double closeMean;
};

class GoldPredictor
{
private:

    vector<double> openPrice;
    vector<double> highPrice;
    vector<double> lowPrice;
    vector<double> closePrice;

    double slope;
    double intercept;

public:

    GoldPredictor();

    void loadData(string filename);

    int getRows();

    DatasetStatistics getStatistics();

    void trainModel();

    double predict(double open);

    double getSlope();

    double getIntercept();

    double getMAE();

    double getRMSE();

    double correlation(
        vector<double>& x,
        vector<double>& y);

    double openCorrelation();

    double highCorrelation();

    double lowCorrelation();

    vector<double> getActualValues();

    vector<double> getPredictedValues();
};

#endif