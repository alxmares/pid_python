# **PID Temperature Control System with Real-Time Graphs**

![](/images/graph.png)

## Objective
This project aims to develop a PID control system to maintain the air temperature within a specified range using Arduino Nano. The system controls the temperature of the airflow expelled through a tube by a fan, regulated between 35°C and 45°C using a halogen lamp.

## Control Method
The system utilizes a discrete PID controller, allowing real-time adjustment of Kp, Ki, and Kd values via potentiometers displayed on an LCD

![](/images/system2.jpg)

## Hardware Requirements
* Arduino Nano
* Push Button
* Potentiometers
* LCD 20x4
* MOSFET IRFZ44N
* Optocoupler PC817C
* LM35 Temperature Sensor
* 12v 50W Halogen Lamp
* 12v 0.98mA Fan
* 12v 5A Power Supply
  
## Electronic desing

![](/images/diagram.jpg)
![](/images/system1.jpg)

## Contributing
Whether you're interested in adapting the system for use with other microprocessors or in developing a graphical interface that allows users to not only visualize real-time graphs but also change all PID parameters interactively, your input is valuable. If you have ideas, code enhancements, or want to help with these improvements, please feel free to fork the repository, make your changes, and submit a pull request. For more details on contributing, check out our contributing guidelines.