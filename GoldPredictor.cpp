

#include "GoldPredictor.h"

#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

GoldPredictor::GoldPredictor()
{
    slope = 0;
    intercept = 0;
}

void GoldPredictor::loadData(string filename)
{
    ifstream file(filename);

    if(!file.is_open())
    {
        return;
    }

    string line;

    getline(file, line);

    while(getline(file, line))
    {
        stringstream ss(line);

        string open;
        string high;
        string low;
        string close;

        getline(ss, open, ',');
        getline(ss, high, ',');
        getline(ss, low, ',');
        getline(ss, close, ',');

        openPrice.push_back(stod(open));
        highPrice.push_back(stod(high));
        lowPrice.push_back(stod(low));
        closePrice.push_back(stod(close));
    }

    file.close();
}

int GoldPredictor::getRows()
{
    return openPrice.size();
}

DatasetStatistics GoldPredictor::getStatistics()
{
    DatasetStatistics s;

    auto calculate =
    [](vector<double>& data,
       double& mn,
       double& mx,
       double& mean)
    {
        mn = data[0];
        mx = data[0];

        double sum = 0;

        for(double value : data)
        {
            mn = min(mn, value);
            mx = max(mx, value);

            sum += value;
        }

        mean = sum / data.size();
    };

    calculate(
    openPrice,
    s.openMin,
    s.openMax,
    s.openMean);

    calculate(
    highPrice,
    s.highMin,
    s.highMax,
    s.highMean);

    calculate(
    lowPrice,
    s.lowMin,
    s.lowMax,
    s.lowMean);

    calculate(
    closePrice,
    s.closeMin,
    s.closeMax,
    s.closeMean);

    return s;
}

void GoldPredictor::trainModel()
{
    int n =
    openPrice.size();

    if(n == 0)
        return;

    double meanX = 0;
    double meanY = 0;

    for(int i = 0; i < n; i++)
    {
        meanX += openPrice[i];
        meanY += closePrice[i];
    }

    meanX /= n;
    meanY /= n;

    double numerator = 0;
    double denominator = 0;

    for(int i = 0; i < n; i++)
    {
        numerator +=
        (openPrice[i] - meanX)
        *
        (closePrice[i] - meanY);

        denominator +=
        (openPrice[i] - meanX)
        *
        (openPrice[i] - meanX);
    }

    slope =
    numerator /
    denominator;

    intercept =
    meanY -
    slope * meanX;
}

double GoldPredictor::predict(double open)
{
    return
    slope * open
    + intercept;
}

double GoldPredictor::getSlope()
{
    return slope;
}

double GoldPredictor::getIntercept()
{
    return intercept;
}

double GoldPredictor::getMAE()
{
    int n =
    openPrice.size();

    double mae = 0;

    for(int i = 0; i < n; i++)
    {
        double pred =
        predict(openPrice[i]);

        mae +=
        fabs(
        closePrice[i]
        -
        pred);
    }

    return mae / n;
}

double GoldPredictor::getRMSE()
{
    int n =
    openPrice.size();

    double mse = 0;

    for(int i = 0; i < n; i++)
    {
        double pred =
        predict(openPrice[i]);

        double error =
        closePrice[i]
        -
        pred;

        mse +=
        error * error;
    }

    return sqrt(
    mse / n);
}

double GoldPredictor::correlation(
vector<double>& x,
vector<double>& y)
{
    int n =
    x.size();

    double meanX = 0;
    double meanY = 0;

    for(int i = 0; i < n; i++)
    {
        meanX += x[i];
        meanY += y[i];
    }

    meanX /= n;
    meanY /= n;

    double numerator = 0;
    double denomX = 0;
    double denomY = 0;

    for(int i = 0; i < n; i++)
    {
        numerator +=
        (x[i] - meanX)
        *
        (y[i] - meanY);

        denomX +=
        (x[i] - meanX)
        *
        (x[i] - meanX);

        denomY +=
        (y[i] - meanY)
        *
        (y[i] - meanY);
    }

    return
    numerator /
    sqrt(
    denomX * denomY);
}

double GoldPredictor::openCorrelation()
{
    return
    correlation(
    openPrice,
    closePrice);
}

double GoldPredictor::highCorrelation()
{
    return
    correlation(
    highPrice,
    closePrice);
}

double GoldPredictor::lowCorrelation()
{
    return
    correlation(
    lowPrice,
    closePrice);
}

vector<double>
GoldPredictor::getActualValues()
{
    return closePrice;
}

vector<double>
GoldPredictor::getPredictedValues()
{
    vector<double> predicted;

    for(double open : openPrice)
    {
        predicted.push_back(
        predict(open));
    }

    return predicted;
}