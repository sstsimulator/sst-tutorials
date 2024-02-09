Example project for creating a component for SST

Created for SST-Core Version (9.1.0)  
Also works with SST-Core v10.0.0

This is a very simple component.  It registers a clock and sends
a message between two components.  The message has a payload that
contains a single value.  If that value equals the one specified
on the command line that component will indicate the simulation can
end.  When both components meet this condition the simulation will
end.

The primary difference between this and Example01 is the use of the
payload in the message.
