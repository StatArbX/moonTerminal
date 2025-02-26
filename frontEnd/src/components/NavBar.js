// src/components/NavBar.js
import React from 'react';
import { Link } from 'react-router-dom';

const NavBar = () => {
  return (
    <nav style={{ backgroundColor: '#f8f9fa', padding: '10px' }}>
      <ul style={{ listStyle: 'none', display: 'flex', gap: '20px', margin: 0 }}>
        <li><Link to="/">Home</Link></li>
        <li><Link to="/chart">Chart</Link></li>
        <li><Link to="/about">About</Link></li>
      </ul>
    </nav>
  );
};

export default NavBar;
