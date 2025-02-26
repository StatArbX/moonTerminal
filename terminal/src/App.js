import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import NavBar from './components/NavBar';
// import ChartComponent from './components/ChartComponents';
import './App.css';
import SpotQuotes from './pages/quotes';

// const Home = () => <h1>Home Page</h1>;
// const About = () => <h1>About Page</h1>;

const Home = () => <h1></h1>;
const About = () => <h1></h1>;


function App() {
  return (
    <Router>
      <NavBar />
      <Routes>
        <Route path="/" element={<Home />} />
        <Route path="/quotes" element={<SpotQuotes />} />
        {/* <Route path="/chart" element={<ChartComponent />} /> */}
        <Route path="/about" element={<About />} />
      </Routes>
    </Router>
  );
}

export default App;
