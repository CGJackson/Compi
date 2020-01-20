# Kumquat
Performs efficient complex valued numerical integration of real varaibles in python. 

Essentially a thin wrapper for the C++ Boost numerical quadriture package

## Requirements

Kumquat requires Python 3.7 or later and C++ boost version 1.66.0 or later

## Installation

To install Kumquat simply clone the repository, navigate to its location and run 
```bash
$ python setup.py
```

Kumquat requires C++ boost to be installed. If the setup script is unable to locate the required header files, it will ask for a filepath to their location.