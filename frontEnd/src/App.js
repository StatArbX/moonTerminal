// src/App.js
import React from 'react';
import { BrowserRouter as Router, Route, Switch } from 'react-router-dom';
import NavBar from './components/NavBar';
import ChartComponent from './ChartComponent';

const Home = () => <h1>Home Page</h1>;
const About = () => <h1>About Page</h1>;

const App = () => {
  return (
    <Router>
        <NavBar />
        <div className="main-content">
            <Routes>
            <Route path="/" element={<Home />} />
            <Route path="/chart" element={<ChartComponent />} />
            <Route path="/about" element={<About />} />
            </Routes>
        </div>
        </Router>
    );
};

export default App;
