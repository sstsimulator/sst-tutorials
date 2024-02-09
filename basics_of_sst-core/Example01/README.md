Example project for creating a component for SST

Created for SST-Core Version (9.1.0)  
Also works with SST-Core v10.0.0

This is a very simple component that demonstrates the use of a link
between two components.  Each component registers a clock and sends 
messages to the other component from the clock event handler. When 
a component receives the number of messages specified on the command
line the component indicates the simulation can end.  When both 
components indicate this condition, the simulation ends.
