1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

It uses delimiters, length-prefixing, or structured message formats. Partial reads are handled continuously until the expected completion signal is detected.


2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

By using delimiters, length-prefixing, or structured formats. Without proper handling, message fragmentation or concatenation can cause incorrect parsing and security risks.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols maintain session context (e.g., TCP), while stateless protocols treat each request independently (e.g., HTTP). Stateful is reliable; stateless is scalable.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

It’s faster and lower-latency, making it ideal for streaming, gaming, and real-time applications where occasional packet loss is acceptable.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The sockets API, which allows applications to send, receive, and manage network connections over TCP/UDP.