#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <winsock2.h>

const int TIME_PORT = 27015;

bool checkForAnError(int bytesResult, char *ErrorAt, SOCKET socket_1, SOCKET socket_2)
{
    if (SOCKET_ERROR == bytesResult)
    {
        printf("Time Server: Error at %s(): %d\n", ErrorAt, WSAGetLastError());
        closesocket(socket_1);
        closesocket(socket_2);
        WSACleanup();
        return true;
    }
    return false;
}

bool fileExists(const char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

long readFileContents(const char *filename, char *buffer, size_t bufferSize)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return -1;

    long bytesRead = fread(buffer, 1, bufferSize - 1, file);
    buffer[bytesRead] = '\0';

    fclose(file);
    return bytesRead;
}

void saveToFile(const char *filename, const char *data)
{
    FILE *file = fopen(filename, "wb");
    if (file)
    {
        fwrite(data, 1, strlen(data), file);
        fclose(file);
    }
}

bool fetchDataFromServer(const char *filename, const char *url, char *buffer, size_t bufferSize)
{
    if (fileExists(filename))
    {
        return readFileContents(filename, buffer, bufferSize) > 0;
    }
    else
    {
        FILE *curl_output = _popen(url, "r");
        if (curl_output == NULL)
        {
            printf("Failed to execute curl command.\n");
            return false;
        }

        size_t bytesRead = fread(buffer, 1, bufferSize - 1, curl_output);
        buffer[bytesRead] = '\0';

        if (bytesRead > 0)
        {
            saveToFile(filename, buffer);
        }
        else
        {
            printf("Failed to receive data from main server.\n");
        }

        fclose(curl_output);
        return bytesRead > 0;
    }
}

int main()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    WSADATA wsaData;
    SOCKET listenSocket;
    struct sockaddr_in serverService;


    if (NO_ERROR != WSAStartup(MAKEWORD(2, 0), &wsaData))
    {
        printf("Time Server: Error at WSAStartup()\n");
        return 1;
    }

    listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (INVALID_SOCKET == listenSocket)
    {
        printf("Time Server: Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    memset(&serverService, 0, sizeof(serverService));
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = htonl(INADDR_ANY);
    serverService.sin_port = htons(TIME_PORT);

    if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR *)&serverService, sizeof(serverService)))
    {
        printf("Time Server: Error at bind(): %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (SOCKET_ERROR == listen(listenSocket, 5))
    {
        printf("Time Server: Error at listen(): %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    while (1)
    {
        struct sockaddr_in from;
        int fromLen = sizeof(from);

        printf("Time Server: Wait for clients' requests.\n");

        SOCKET msgSocket = accept(listenSocket, (struct sockaddr *)&from, &fromLen);
        if (INVALID_SOCKET == msgSocket)
        {
            printf("Time Server: Error at accept(): %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("Time Server: Client is connected.\n");
        while (1)
        {
            int bytesSent = 0;
            int bytesRecv = 0;
            char *sendBuff;
            char recvBuff[2000];

            bytesRecv = recv(msgSocket, recvBuff, 2000, 0);
            if (bytesRecv == SOCKET_ERROR)
            {
                printf("Time Server: Error at recv(): %d\n", WSAGetLastError());
                closesocket(msgSocket);
                break;
            }
            else if (bytesRecv == 0)
            {
                printf("Time Server: Client disconnected.\n");
                closesocket(msgSocket);
                break;
            }

            if (checkForAnError(bytesRecv, "recv", listenSocket, msgSocket))
                return 1;

            LARGE_INTEGER start, end;
            QueryPerformanceCounter(&start);

            double rtt = 0.0;

            if (strncmp(recvBuff, "JSON", 4) == 0)
            {
                const char *filename = "file2.txt";

                if (fileExists(filename))
                {
                    long bytesRead = readFileContents(filename, recvBuff, sizeof(recvBuff));
                    if (bytesRead > 0)
                    {
                        sendBuff = recvBuff;
                        strcat(sendBuff, " (Local Copy)");
                    }
                    else
                    {
                        printf("Failed to read local file.\n");
                        closesocket(msgSocket);
                        continue;
                    }
                }
                else
                {
                    FILE *curl_output = _popen("curl -s -X GET \"https://httpbin.org/json\" -H \"accept: application/json\"", "r");
                    if (curl_output == NULL)
                    {
                        printf("Failed to execute curl command.\n");
                        closesocket(msgSocket);
                        continue;
                    }

                    size_t bytesRead = fread(recvBuff, 1, sizeof(recvBuff), curl_output);
                    if (bytesRead > 0)
                    {
                        saveToFile(filename, recvBuff);
                        sendBuff = recvBuff;
                    }
                    else
                    {
                        printf("Failed to receive data from main server.\n");
                        fclose(curl_output);
                        closesocket(msgSocket);
                        continue;
                    }

                    fclose(curl_output);

                    QueryPerformanceCounter(&end);
                    rtt = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;


                    printf("RTT: %.6f seconds\n", rtt);
                }
            }
            else if (strncmp(recvBuff, "Anything", 8) == 0)
            {
                const char *filename = "file1.txt";

                if (fileExists(filename))
                {
                    long bytesRead = readFileContents(filename, recvBuff, sizeof(recvBuff));
                    if (bytesRead > 0)
                    {
                        sendBuff = recvBuff;
                        strcat(sendBuff, " (Local Copy)");
                    }
                    else
                    {
                        printf("Failed to read local file.\n");
                        closesocket(msgSocket);
                        continue;
                    }
                }
                else
                {
                    FILE *curl_output = _popen("curl -s -X GET \"https://httpbin.org/anything\" -H \"accept: application/json\"", "r");
                    if (curl_output == NULL)
                    {
                        printf("Failed to execute curl command.\n");
                        closesocket(msgSocket);
                        continue;
                    }

                    size_t bytesRead = fread(recvBuff, 1, sizeof(recvBuff), curl_output);
                    if (bytesRead > 0)
                    {
                        saveToFile(filename, recvBuff);
                        sendBuff = recvBuff;
                    }
                    else
                    {
                        printf("Failed to receive data from main server.\n");
                        fclose(curl_output);
                        closesocket(msgSocket);
                        continue;
                    }

                    fclose(curl_output);

                    QueryPerformanceCounter(&end);
                    rtt = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

                    printf("RTT: %.6f seconds\n", rtt);
                }
            }
            else
            {
                const char *errorMessage = "Error: Unsupported request";
                bytesSent = send(msgSocket, errorMessage, strlen(errorMessage), 0);
                if (checkForAnError(bytesSent, "send", listenSocket, msgSocket))
                    return 1;
                printf("Time Server: Unsupported request received.\n");
                closesocket(msgSocket);
                break;
            }

            printf("Received: %s\n", recvBuff);

            bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
            if (checkForAnError(bytesRecv, "send", listenSocket, msgSocket))
                return 1;

            fflush(stdin);
            sendBuff = "";

            QueryPerformanceCounter(&end);
            double elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart - rtt;
            printf("Processing Time: %.6f seconds\n\n", elapsedTime);
            printf("---------------------------\n\n", elapsedTime);
        }
    }
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
