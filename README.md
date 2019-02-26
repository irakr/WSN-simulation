# WSN-simulation
Simulation of a routing protocol for Wireless Sensor Networks(WSN).

# Description
The WSN simulator is network simulator specifically designed for wireless sensor networks simulations.
It is specifically designed for my own research works. The results may or may not be accurate as other
widely renown simulation tools like ns2, netsim, opnet, etc.
It uses a multithreaded approach of running events of a network. It was the best solution that I thought
which could also help in easily producing randomness in the event scheduling.
It takes a configuration file as input of which the format is defined by the software itself.
To know the details of the configuration format checkout the file 'input.config'.
This file is used to create simulation environment according to the user's preferences.


# Implementation
- Core system language: C++.
- Input configuration script: My own defined format

# How to use
- make sim
  (OR)
- make debug   (For debugging)
- ./sim input_file.config
  The simulation will start according to parameters set in the input file. Out is a trace file
  quite similar to ns2 trace-file format with the name input_file.tr.
- Trace files can be parsed using your own scripts written in any scripting language like awk,
  python, etc.
  
#						ENJOY
