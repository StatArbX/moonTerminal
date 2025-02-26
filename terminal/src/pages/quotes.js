import React, { useState, useEffect } from "react";
import Chart from "react-apexcharts";

const SpotQuotes = () => {
  const [candleData, setCandleData] = useState([]);

  useEffect(() => {
    const eventSource = new EventSource("http://localhost:4000/spotDataStream");

    eventSource.onmessage = (event) => {
      const data = JSON.parse(event.data);
      const T = parseInt(data.T);
      const spot = parseFloat(data.p);
      const q = parseFloat(data.q);
      const x = getIntervalStart(T);

      setCandleData((prev) => {
        // If no data or a new interval candle
        if (prev.length === 0 || x > prev[prev.length - 1].x) {
          const newCandle = { 
            x, 
            y: [spot, spot, spot, spot], 
            volume: q 
          };
          const newData = [...prev, newCandle];
          if (newData.length > 50) {
            newData.shift();
          }
          return newData;
        } 
        // If the incoming data belongs to the current candle
        else if (x === prev[prev.length - 1].x) {
          const lastCandle = { ...prev[prev.length - 1] };
          const [open, high, low] = lastCandle.y;
          const newHigh = Math.max(high, spot);
          const newLow = Math.min(low, spot);
          // Update candle's OHLC and add volume
          lastCandle.y = [open, newHigh, newLow, spot];
          lastCandle.volume = (lastCandle.volume || 0) + q;
          return [...prev.slice(0, -1), lastCandle];
        } else {
          return prev;
        }
      });
    };

    return () => {
      eventSource.close();
    };
  }, []);

  // Calculate the start of the minute in UTC based on the IST adjustment
  const getIntervalStart = (T) => {
    const T_sec = Math.floor(T / 1000);
    const IST_sec = T_sec + 19800; // 5.5 hours in seconds
    const minute_ist = Math.floor(IST_sec / 60);
    const interval_start_ist_sec = minute_ist * 60;
    const interval_start_utc_sec = interval_start_ist_sec - 19800;
    return interval_start_utc_sec * 1000;
  };

  // Determine the current price from the latest candle.
  const currentPrice =
    candleData.length > 0 ? candleData[candleData.length - 1].y[3] : null;

  // Candlestick chart options with dark theme, no grid lines,
  // and an annotation for the current price.
  const candlestickOptions = {
    chart: {
      type: "candlestick",
      height: 350,
      background: "#000", // Dark background
      foreColor: "#fff",  // White text
    },
    annotations: {
        yaxis: currentPrice !== null ? [
          {
            y: currentPrice,
            borderColor: "white", 
            label: {
              text: `Price: ${currentPrice}`,
              style: {
                fontSize: "12px",
                color: "#fff",
                background: "transparent"  
              },
              borderColor: "transparent"   
            }
          }
        ] : []
      },
    title: {
      text: "Real-Time Spot Chart",
      align: "center",
      offsetY: 10, // adds 20px spacing from the top
      style: {
        color: "#fff",
      },
    },
    xaxis: {
      type: "datetime",
      labels: {
        formatter: function (val) {
          return new Date(val).toLocaleString("en-IN", {
            timeZone: "Asia/Kolkata",
            hour: "2-digit",
            minute: "2-digit",
          });
        },
      },
    },
    yaxis: {
      tooltip: {
        enabled: true,
      },
    },
    grid: {
      show: false, // Remove grid lines
    },
    // Removed global candlestick color settings to allow per-candle styling.
  };

  // Prepare candlestick series with hollow candle logic.
  const candlestickSeries = [
    {
      data: candleData.map((candle) => {
        const open = candle.y[0];
        const close = candle.y[3];
        if (close >= open) {
          // Bullish candle: hollow look (fill same as background, outlined in green).
          return {
            x: candle.x,
            y: candle.y,
            fillColor: "#000",    // Matches chart background for a hollow effect
            lineColor: "#00B746", // Upward (green) color outline
          };
        } else {
          // Bearish candle: fully filled with red.
          return {
            x: candle.x,
            y: candle.y,
            fillColor: "#EF403C", // Filled red
            lineColor: "#EF403C",
          };
        }
      }),
    },
  ];

  // Create volume data from candleData.
  const volumeData = candleData.map((candle) => {
    const open = candle.y[0];
    const close = candle.y[3];
    return {
      x: candle.x,
      y: candle.volume,
      fillColor: close >= open ? "#00B746" : "#EF403C",
    };
  });

  // Volume chart options with dark theme and no grid lines.
  const volumeOptions = {
    chart: {
      type: "bar",
      height: 100,
      background: "#000",
      foreColor: "#fff",
    },
    plotOptions: {
      bar: {
        columnWidth: "80%",
        colors: {
          ranges: [
            {
              from: -1000,
              to: 1000000000,
              color: undefined,
            },
          ],
        },
      },
    },
    dataLabels: {
      enabled: false,
    },
    xaxis: {
      type: "datetime",
    },
    yaxis: {
      labels: {
        show: false,
      },
      title: {
        text: undefined,
      },
    },
    grid: {
      show: false,
    },
  };

  const volumeSeries = [
    {
      name: "Volume",
      data: volumeData,
    },
  ];

  return (
    <div>
      <Chart
        options={candlestickOptions}
        series={candlestickSeries}
        type="candlestick"
        height={500}
      />
      <Chart
        options={volumeOptions}
        series={volumeSeries}
        type="bar"
        height={300}
      />
    </div>
  );
};

export default SpotQuotes;