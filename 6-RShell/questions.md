1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

EOF markers sent by the server. Some techniques are to use recv() in a loop until the entire message is received, checking for the EOF marker, and buffering.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

The protocol can use new line charachers or it can send the length of the message before the message itself. Some challenges include buffer overflow and incomplete command execution.

3. Describe the general differences between stateful and stateless protocols.

Stateful: Retains session information
Stateless: No session infomation, each request is independent

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

It has lower latency and overhead compared to TCP, making it suitable for applications that require fast and real time communication.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides sockets, which allow applications to send and receive data over network protocols.