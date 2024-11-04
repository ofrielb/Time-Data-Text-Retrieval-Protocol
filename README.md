# Time Data and Text Retrieval Protocol (TDTRP)

**A custom protocol over TCP/IP for efficient, reliable text data retrieval with built-in performance metrics**

The *Time Data and Text Retrieval Protocol (TDTRP)* is a C-based client-server application designed to demonstrate expertise in network programming, efficient data retrieval, and performance analysis. TDTRP handles specific requests to retrieve text data, sourced either from local storage or external URLs, with added features to measure and display round-trip time (RTT) and server processing times.

## Key Features
- **Reliable TCP Communication**: TDTRP uses TCP to ensure reliable, ordered data transmission, making it suitable for environments requiring high data integrity.
- **Request Types**: The protocol supports:
  - **JSON Requests** – Retrieves JSON data from local storage (`file2.txt`) or fetches live from an external API.
  - **Anything Requests** – Returns data from a local file (`file1.txt`) or an external source, demonstrating dynamic data handling.
- **Performance Measurement**: Both client and server measure and display RTT and server-side processing times, giving real-time insights into protocol performance.
- **Persistent Connections**: TDTRP maintains connections across multiple requests, reducing overhead and enhancing efficiency for repeated client interactions.

## Project Structure
- **Client**: Implements request commands, collects RTT measurements, and displays averages.
- **Server**: Listens on TCP port 27015, handles client requests, performs file caching for efficiency, and logs performance metrics.

## Technical Highlights
- **File Caching**: Optimizes repeated requests by storing fetched data locally.
- **High Precision Timing**: Leveraged to calculate RTT and processing time, using Windows-specific APIs for accuracy.
- **Error Handling**: Graceful error management for unsupported requests and unexpected client disconnections.

## Installation
1. **Clone Repository**: `git clone https://github.com/yourusername/TDTRP.git`
2. **Compile Code**: Use a C compiler on Windows (requires Winsock2) to compile `TCP_Client.c` and `TCP_BlockingServer.c`.
3. **Run**:
   - Start the server (`TCP_BlockingServer`) first.
   - Launch the client (`TCP_Client`) and select options from the command-line menu.

## Example Usage
1. **JSON Request**:
   - Client sends "JSON" request.
   - Server responds with data from `file2.txt` or an external API if the file is unavailable.
2. **RTT Measurement**:
   - Client displays RTT for each request and an average RTT over time.

## Sample Code Structure
- `TCP_Client.c` – Handles client-side operations, including RTT logging and command selection.
- `TCP_BlockingServer.c` – Manages server operations, request handling, and response formatting.

## TDTRP Protocol Specification
See the full protocol specification in [docs/TDTRP_Specification.md](docs/TDTRP_Specification.md).

---

This project showcases skills in low-level network programming, C-based TCP/IP communication, and performance profiling, making it an ideal addition to portfolios focused on systems and network engineering.
