// // src/ChartComponent.js
// import React, { useEffect, useState } from 'react';
// import io from 'socket.io-client';
// import { Line } from 'react-chartjs-2';
// import 'chart.js/auto';

// // Connect to our backend Socket.IO server
// const socket = io('http://localhost:4000');

// const ChartComponent = () => {
//   const [chartData, setChartData] = useState({
//     labels: [],
//     datasets: [{
//       label: 'Price',
//       data: [],
//       fill: false,
//       backgroundColor: 'rgb(75, 192, 192)',
//       borderColor: 'rgba(75, 192, 192, 0.2)',
//     }]
//   });

//   useEffect(() => {
//     socket.on('redisData', (data) => {
//       // Extract the price (p) and time (t)
//       // Assuming `t` is a Unix timestamp in seconds if it is 10 digits long
//       let timestamp;
//       if (String(data.t).length === 13) {
//         timestamp = new Date(data.t);
//       } else {
//         timestamp = new Date(data.t * 1000);
//       }
      
//       const istTime = timestamp.toLocaleString("en-IN", { timeZone: "Asia/Kolkata" });
      
//       setChartData(prevState => ({
//         labels: [...prevState.labels, istTime],
//         datasets: [{
//           ...prevState.datasets[0],
//           data: [...prevState.datasets[0].data, parseFloat(data.p)]
//         }]
//       }));
//     });

//     return () => {
//       socket.off('redisData');
//     }
//   }, []);

//   return (
//     <div>
//       <h2>Live Price Chart</h2>
//       <Line data={chartData} />
//     </div>
//   );
// };

// export default ChartComponent;
