#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <windows.h>

#define MAX_RTT_VALUES  20


const int TIME_PORT = 27015;

double rttValues[MAX_RTT_VALUES];
int rttIndex = 0;
int totalRequests = 0;





void printHighPrecisionRTT(LARGE_INTEGER start, LARGE_INTEGER end) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    double interval = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    printf("RTT: %.6f seconds\n", interval);

    rttValues[rttIndex] = interval;
    rttIndex = (rttIndex + 1) % MAX_RTT_VALUES;
}


void printLastRTTValues() {
    double sum = 0.0;
    int validCount = 0;
    int startIndex = totalRequests > MAX_RTT_VALUES ? totalRequests % MAX_RTT_VALUES : 0;
    int actualRequestNumber;

    printf("RTT values with real request number:\n");
    for (int i = 0; i < MAX_RTT_VALUES; ++i) {
        int index = (startIndex + i) % MAX_RTT_VALUES;
        if (rttValues[index] > 0.0) {
            actualRequestNumber = totalRequests - MAX_RTT_VALUES + i + 1;
            if (totalRequests < MAX_RTT_VALUES) {
                actualRequestNumber = i + 1;
            }
            printf("Request %d: %.6f seconds\n", actualRequestNumber, rttValues[index]);
            sum += rttValues[index];
            validCount++;
        }
    }

    if (validCount > 0) {
        double average = sum / validCount;
        printf("Average RTT: %.6f seconds (from %d valid samples)\n", average, validCount);
    } else {
        printf("No RTT values recorded.\n");
    }
}


void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}



bool checkForAnError(int bytesResult, char* ErrorAt, SOCKET socket){
    if (SOCKET_ERROR == bytesResult) {
        printf("Time Client: Error at %s(): ",ErrorAt);
        printf("%d", WSAGetLastError());
        closesocket(socket);
        WSACleanup();
        return true;
    }
    return false;
}


void main() {
    WSADATA wsaData;
    SOCKET connSocket;
    struct sockaddr_in server;
    int bytesSent, bytesRecv;
    char sendBuff[2000], recvBuff[2000], inputBuffer[100];
    char option = 0;
    LARGE_INTEGER start, end;


    if (NO_ERROR != WSAStartup(MAKEWORD(2, 0), &wsaData)) {
        printf("Time Client: Error at WSAStartup()\n");
        return;
    }

    connSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == connSocket) {
        printf("Time Client: Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(TIME_PORT);

    if (SOCKET_ERROR == connect(connSocket, (SOCKADDR *)&server, sizeof(server))) {
        printf("Time Client: Error at connect(): %d\n", WSAGetLastError());
        closesocket(connSocket);
        WSACleanup();
        return;
    }

    printf("Connection established successfully.\n");

    while (option != '4') {
        printf("\nPlease insert an option:\n");
        printf(" 1 : JSON.\n");
        printf(" 2 : Anything.\n");
        printf(" 3 : RTT.\n");
        printf(" 4 : Exit.\n");
        printf("Your option: ");

        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
            if (sscanf(inputBuffer, "%c", &option) != 1) {
                printf("\nInvalid input. Please try again.\n");
                continue;
            }
        }
        printf("\n");

        if (option == '1') {
            strcpy(sendBuff, "JSON");
        } else if (option == '2') {
            strcpy(sendBuff, "Anything");
        } else if (option == '3') {
            printf("RTT statistics:\n");
            printLastRTTValues();
            continue;
        } else if (option == '4') {
            strcpy(sendBuff, "Exit");
            break;
        } else {
            printf("\n*-*-* Please enter a valid option only. *-*-*\n");
            continue;
        }

        if (option == '1' || option == '2') {
            totalRequests++;
            QueryPerformanceCounter(&start);
            bytesSent = send(connSocket, sendBuff, strlen(sendBuff), 0);
            if (checkForAnError(bytesSent, "send", connSocket)) {
                closesocket(connSocket);
                WSACleanup();
                return;
            }

            bytesRecv = recv(connSocket, recvBuff, sizeof(recvBuff) - 1, 0);
            QueryPerformanceCounter(&end);
            if (checkForAnError(bytesRecv, "recv", connSocket)) {
                closesocket(connSocket);
                WSACleanup();
                return;
            }
            recvBuff[bytesRecv] = '\0';
            printHighPrecisionRTT(start, end);
            printf("Received: %s\n", recvBuff);
        }

        printf("\n");
    }

    closesocket(connSocket);
    WSACleanup();
    return 0;
}
