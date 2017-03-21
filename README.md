# Real-Time-Systems-Assignment-1.4

For the assignment 1.4 of Real Time System I have to create a startup process that will fork seven processes. Each process is used build a solar heating system

# Each process has an individual job:
1) Will act as a sensor for the tank level.
2) This will act as a sensor for the air temperature on the outside. This is relevant as if its cold outside the solar panel will no produce any heat to heat the water.
3) This will turn on and off the pump if air outside is too cold or the water in the solar panel connector is cold as you don’t want cold water entering the tank as it would lower the temperature inside the tank.
4) This will act as a sensor to increase/decrease the temperature of the tank if the pump is off.
5) This act as a sensor for the water temperature inside the tank. If the levels drop below a certain degrees it will send a signal to turn off the pump.
6) This process changes the tilt of the solar panel to obtain he best heating surface on the solar panel.
7) This wait for a signal from process one, if the water levels reach to high the valve will open to release some water, if the water levels reach to low the valve will close.
