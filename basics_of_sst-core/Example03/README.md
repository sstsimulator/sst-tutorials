Example project for creating a component for SST

Created for SST-Core Version (9.1.0)  
Also works with SST-Core v10.0.0

This is a very simple simulation.  It contains two components.  For
each component it registers a clock and within each clock event 
handler it sends a message to the other component.  The message has 
a payload that contains a single value.  The message is processed 
in a port event handler.  If that value equals the one specified on 
the command line that component will indicate the simulation can end.  
When both components meet this condition the simulation will end.

The primary difference between this and Example02 is the use of the
message event handler.
