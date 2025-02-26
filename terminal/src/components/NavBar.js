// src/components/NavBar.js
import React from 'react';
import { Link } from 'react-router-dom';
import '../style/NavBar.css';


const NavBar = () => {
  return (
    <nav className='navbar'>
      <ul>
        <li><Link to="/" style = {{color: 'white'}}>Home</Link></li>
        <li><Link to="/quotes" style = {{color: 'white'}}>Quotes</Link></li>
        <li><Link to="/research" style = {{color: 'white'}}>Research</Link></li>
        <li><Link to="/login" style = {{color: 'white'}}>Login</Link></li>
        <li><Link to="/about" style = {{color: 'white'}}>About</Link></li>
      </ul>
    </nav>
  );
};

export default NavBar;
