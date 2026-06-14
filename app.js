let openPrices = [];
let highPrices = [];
let lowPrices = [];
let closePrices = [];

let slope = 0;
let intercept = 0;

function loadCSV()
{
    const file =
    document.getElementById(
    "csvFile").files[0];

    const reader =
    new FileReader();

    reader.onload =
    function(e)
    {
        const lines =
        e.target.result
        .trim()
        .split('\n');

        openPrices = [];
        highPrices = [];
        lowPrices = [];
        closePrices = [];

        for(let i=1;i<lines.length;i++)
        {
            const row =
            lines[i].split(',');

            openPrices.push(
            parseFloat(row[0]));

            highPrices.push(
            parseFloat(row[1]));

            lowPrices.push(
            parseFloat(row[2]));

            closePrices.push(
            parseFloat(row[3]));
        }

        document
        .getElementById("records")
        .innerHTML =
        openPrices.length;

        calculateStatistics();

        trainModel();

        calculateCorrelation();

        drawChart();
    };

    reader.readAsText(file);
}

function mean(arr)
{
    return arr.reduce(
    (a,b)=>a+b,0)
    / arr.length;
}

function calculateStatistics()
{
    const stats = [
        ["Open",openPrices],
        ["High",highPrices],
        ["Low",lowPrices],
        ["Close",closePrices]
    ];

    let html='';

    stats.forEach(item=>
    {
        html += `
        <tr>
            <td>${item[0]}</td>
            <td>${Math.min(...item[1]).toFixed(2)}</td>
            <td>${Math.max(...item[1]).toFixed(2)}</td>
            <td>${mean(item[1]).toFixed(2)}</td>
        </tr>
        `;
    });

    document
    .getElementById(
    "statsTable")
    .innerHTML = html;
}

function trainModel()
{
    const meanX =
    mean(openPrices);

    const meanY =
    mean(closePrices);

    let numerator=0;
    let denominator=0;

    for(let i=0;i<openPrices.length;i++)
    {
        numerator +=
        (openPrices[i]-meanX)
        *
        (closePrices[i]-meanY);

        denominator +=
        (openPrices[i]-meanX)
        *
        (openPrices[i]-meanX);
    }

    slope =
    numerator /
    denominator;

    intercept =
    meanY -
    slope*meanX;

    document
    .getElementById(
    "slope")
    .innerHTML =
    slope.toFixed(4);

    document
    .getElementById(
    "intercept")
    .innerHTML =
    intercept.toFixed(2);

    calculateRMSE();
}

function calculateRMSE()
{
    let mse = 0;

    for(let i=0;i<openPrices.length;i++)
    {
        const pred =
        slope*
        openPrices[i]
        +
        intercept;

        const error =
        closePrices[i]
        -
        pred;

        mse +=
        error*error;
    }

    const rmse =
    Math.sqrt(
    mse/openPrices.length);

    document
    .getElementById(
    "rmse")
    .innerHTML =
    rmse.toFixed(2);
}

function correlation(x,y)
{
    const meanX = mean(x);
    const meanY = mean(y);

    let num=0;
    let denX=0;
    let denY=0;

    for(let i=0;i<x.length;i++)
    {
        num +=
        (x[i]-meanX)
        *
        (y[i]-meanY);

        denX +=
        (x[i]-meanX)**
        2;

        denY +=
        (y[i]-meanY)**
        2;
    }

    return num /
    Math.sqrt(
    denX*denY);
}

function calculateCorrelation()
{
    document
    .getElementById(
    "corrOpen")
    .innerHTML =
    "Open → Close : "
    +
    correlation(
    openPrices,
    closePrices)
    .toFixed(4);

    document
    .getElementById(
    "corrHigh")
    .innerHTML =
    "High → Close : "
    +
    correlation(
    highPrices,
    closePrices)
    .toFixed(4);

    document
    .getElementById(
    "corrLow")
    .innerHTML =
    "Low → Close : "
    +
    correlation(
    lowPrices,
    closePrices)
    .toFixed(4);
}

function predictPrice()
{
    const open =
    parseFloat(
    document
    .getElementById(
    "openInput")
    .value);

    const prediction =
    slope*open
    +
    intercept;

    document
    .getElementById(
    "predictionResult")
    .innerHTML =
    "Predicted Close Price : ₹ "
    +
    prediction.toFixed(2);
}

let chart;

function drawChart()
{
    const predicted =
    openPrices.map(
    x=>slope*x+intercept);

    const ctx =
    document
    .getElementById(
    "predictionChart");

    if(chart)
    {
        chart.destroy();
    }

    chart =
    new Chart(ctx,
    {
        type:'line',

        data:
        {
            labels:
            closePrices.map(
            (_,i)=>i+1),

            datasets:
            [
                {
                    label:'Actual',
                    data:closePrices
                },

                {
                    label:'Predicted',
                    data:predicted
                }
            ]
        }
    });
}
