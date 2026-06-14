#include "crow.h"
// #include <crow.h>
#include "GoldPredictor.h"

#include <fstream>
#include <sstream>

using namespace std;

int main()
{
    crow::SimpleApp app;

    GoldPredictor predictor;

    predictor.loadData("gold_train.csv");
    ifstream test("gold_train.csv");

if(test.is_open())
{
    cout << "CSV FOUND\n";
}
else
{
    cout << "CSV NOT FOUND\n";
}
    cout << "Rows Loaded = "<< predictor.getRows()<< endl;

    // =========================
    // Home Page
    // =========================

    CROW_ROUTE(app, "/")
    ([]()
    {
        ifstream file(
        "index.html");

        stringstream buffer;

        buffer << file.rdbuf();

        return crow::response(
        buffer.str());
    });

    // =========================
    // Static Files
    // =========================

    CROW_ROUTE(app,
    "/static/<string>")
    ([](string filename)
    {
        ifstream file(
        "static/" +
        filename);

        if(!file.is_open())
        {
            return crow::response(
            404);
        }

        stringstream buffer;

        buffer << file.rdbuf();

        crow::response res(
        buffer.str());

        if(filename.find(".css")
        != string::npos)
        {
            res.set_header(
            "Content-Type",
            "text/css");
        }

        if(filename.find(".js")
        != string::npos)
        {
            res.set_header(
            "Content-Type",
            "application/javascript");
        }

        return res;
    });

    // =========================
    // Overview
    // =========================

    CROW_ROUTE(app, "/overview")
    ([&predictor]()
    {
        crow::json::wvalue x;

        x["rows"] =
        predictor.getRows();

        x["columns"] = 4;

        return x;
    });

    // =========================
    // Statistics
    // =========================

    CROW_ROUTE(app, "/statistics")
    ([&predictor]()
    {
        auto stats =
        predictor.getStatistics();

        crow::json::wvalue x;

        x["open_min"] =
        stats.openMin;

        x["open_max"] =
        stats.openMax;

        x["open_mean"] =
        stats.openMean;

        x["high_min"] =
        stats.highMin;

        x["high_max"] =
        stats.highMax;

        x["high_mean"] =
        stats.highMean;

        x["low_min"] =
        stats.lowMin;

        x["low_max"] =
        stats.lowMax;

        x["low_mean"] =
        stats.lowMean;

        x["close_min"] =
        stats.closeMin;

        x["close_max"] =
        stats.closeMax;

        x["close_mean"] =
        stats.closeMean;

        return x;
    });

    // =========================
    // Train Model
    // =========================

    CROW_ROUTE(app, "/train")
    ([&predictor]()
    {
        predictor.trainModel();

        crow::json::wvalue x;

        x["slope"] =
        predictor.getSlope();

        x["intercept"] =
        predictor.getIntercept();

        return x;
    });

    // =========================
    // Evaluation
    // =========================

    CROW_ROUTE(app, "/evaluation")
    ([&predictor]()
    {
        predictor.trainModel();

        crow::json::wvalue x;

        x["mae"] =
        predictor.getMAE();

        x["rmse"] =
        predictor.getRMSE();

        return x;
    });

    // =========================
    // Correlation
    // =========================

    CROW_ROUTE(app, "/correlation")
    ([&predictor]()
    {
        crow::json::wvalue x;

        x["open"] =
        predictor.openCorrelation();

        x["high"] =
        predictor.highCorrelation();

        x["low"] =
        predictor.lowCorrelation();

        return x;
    });

    // =========================
    // Chart Data
    // =========================

    CROW_ROUTE(app, "/chartdata")
    ([&predictor]()
    {
        predictor.trainModel();

        auto actual =
        predictor.getActualValues();

        auto predicted =
        predictor.getPredictedValues();

        crow::json::wvalue result;

        for(size_t i=0;
            i<actual.size();
            i++)
        {
            result["actual"][i] =
            actual[i];

            result["predicted"][i] =
            predicted[i];
        }

        return result;
    });

    // =========================
    // Prediction API
    // =========================

    CROW_ROUTE(app, "/predict")
    .methods("POST"_method)
    ([&predictor]
    (const crow::request& req)
    {
        auto body =
        crow::json::load(
        req.body);

        if(!body)
        {
            return crow::response(
            400);
        }

        double open =
        body["open"].d();

        double prediction =
        predictor.predict(
        open);

        crow::json::wvalue x;

        x["prediction"] =
        prediction;

        return crow::response(
        x);
    });

    // =========================
    // Start Server
    // =========================

    cout << endl;
    cout << "=====================================\n";
    cout << " Gold Price Prediction Server\n";
    cout << "=====================================\n";
    cout << "Running at:\n";
    cout << "http://localhost:18080\n";
    cout << "=====================================\n";

    app.port(18080)
       .multithreaded()
       .run();

    return 0;
}