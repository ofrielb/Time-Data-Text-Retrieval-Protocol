# Time Data and Text Retrieval Protocol (TDTRP) Specification

## 1. Introduction
The *Time Data and Text Retrieval Protocol (TDTRP)* is designed to enable reliable text data exchange between a client and server over a TCP/IP network. It provides clients with access to predefined data sets stored locally or fetched from external sources upon request. TDTRP also includes mechanisms for measuring performance metrics, such as round-trip time (RTT) and server processing time.

## 2. Protocol Overview
TDTRP operates over TCP, ensuring reliable, ordered, and error-checked data transmission. It handles two primary request types:
- **JSON Requests**: Fetches JSON data from a local file (`file2.txt`) or retrieves it from an external source if not cached.
- **Anything Requests**: Returns data based on the "Anything" command from a local file (`file1.txt`) or an external URL if unavailable locally.

### 2.1. Port Assignment
TDTRP services listen for client connections on **TCP port 27015**.

## 3. Operation

### 3.1. Server Operation
The server listens for incoming client connections and processes requests as follows:
- **JSON Request**: Retrieves data from `file2.txt` or fetches from an external source if the file is missing.
- **Anything Request**: Retrieves data from `file1.txt` or an external URL when the file is absent.
- **Error Handling**: For unsupported requests, the server responds with a specific error message.

### 3.2. Client Operation
The client can:
1. Send either a `JSON` or `Anything` request.
2. Receive text data or error messages based on the server's response.
3. Measure and display RTT for requests, maintaining an average RTT value for performance analysis.

## 4. Request and Response Structure

### 4.1. Client Requests
Clients send simple string commands to request specific data:
- **Command**: `"JSON"` or `"Anything"`
- **Example**: Sending `"JSON"` requests data linked to the JSON identifier.

### 4.2. Server Responses
Server responses depend on the request type:
- **Data Response**: Returns requested data, appending "(Local Copy)" when data is served from the cache.
- **Error Message**: Returns `"Error: Unsupported request"` for unrecognized requests.

## 5. Error Handling and Stability
The server detects client disconnections or crashes and automatically closes the socket to ensure stability. This prevents resource leaks and keeps the server responsive to new connections.

## 6. Performance Measurements

- **Round-Trip Time (RTT)**: The client measures RTT for each request and displays an average RTT value based on historical data.
- **Server Processing Time**: Server-side processing time is measured and displayed separately, giving insights into the server’s efficiency independent of network latency.
  
### Example
An "Anything" request might result in an RTT of `0.964742` seconds and a server processing time of `0.001443` seconds.

## 7. Security Considerations
Currently, TDTRP does not support encryption or authentication. Therefore, it is recommended to use TDTRP within secure, private networks or with additional layers of security (e.g., VPN) if deployed in potentially insecure environments.

## 8. Implementation Details

- **File Caching**: Repeated requests for the same data are served from a local cache, reducing response time.
- **High Precision Timing**: Uses high-resolution timers to accurately measure RTT and server processing times, leveraging Windows-specific APIs for precision.
- **Error Logging**: Error messages are printed with function-specific details for easier debugging and maintenance.

## 9. Compatibility
TDTRP is designed for **Windows environments** and relies on Winsock2 for TCP network interactions. Compatibility with non-Windows systems would require modifications, specifically for the socket handling and timing functions.

## 10. Conclusion
The *Time Data and Text Retrieval Protocol (TDTRP)* provides a structured method for textual data retrieval over TCP/IP, with detailed performance metrics (RTT and server processing time) that allow for in-depth operation analysis. Its design balances efficiency with functionality, making it suitable for scenarios requiring reliable text data exchange and performance monitoring.
