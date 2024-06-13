Example project for creating a networked simulation configuratin in SST.

This simulation contains three components, each with two subcomponents.
Functionality used to send and receive messages has been moved to the
subcomponents and links between network nodes are made to and from the
subcomponents.

Like previous examples, messages contain a single value.  Received 
messages are processed in event handlers.  If the received value equals 
the one specified on the command line that subcomponent will set a 
flag.  When both subcomponent flags are set, the component will indicate 
the simulation can end.  When all components meet this condition 
the simulation will end.

The primary difference between this and Example04 is the use of 
subcomponents to send and receive the messages.
