#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;
const std::string SERVER_IP = "127.0.0.1";
const std::string FILENAME = "data.txt";
const int ARRAY_SIZE = 100000;

void select_operation(int& value, int& operation) {
    std::cout << "Algorithms Demonstration\n";
    std::cout << "1) Search Algorithms\n";
    std::cout << "2) Sorting Algorithms\n";
    std::cout << "Enter choice (1 or 2): ";
    
    int choice;
    std::cin >> choice;
    std::cout << std::endl;

    if (choice == 1) {
        std::cout << "Search Algorithms:\n";
        std::cout << "1) Sequential Search\n";
        std::cout << "2) Fast Sequential Search\n";
        std::cout << "3) Binary Search\n";
        std::cout << "4) Sequential Search (Ordered Array)\n";
        std::cout << "Enter algorithm (1-4): ";
        
        int searchChoice;
        std::cin >> searchChoice;
        
        if (searchChoice >= 1 && searchChoice <= 4) {
            operation = searchChoice;
            std::cout << "Enter value to search: ";
            std::cin >> value;
        } else {
            std::cerr << "Invalid choice" << std::endl;
            exit(1);
        }
    } 
    else if (choice == 2) {
        std::cout << "Sorting Algorithms:\n";
        std::cout << "1) Selection Sort\n";
        std::cout << "2) Quick Sort\n";
        std::cout << "Enter algorithm (1 or 2): ";
        
        int sortChoice;
        std::cin >> sortChoice;
        
        if (sortChoice == 1) operation = 5;
        else if (sortChoice == 2) operation = 6;
        else {
            std::cerr << "Invalid choice" << std::endl;
            exit(1);
        }
    } 
    else {
        std::cerr << "Invalid choice" << std::endl;
        exit(1);
    }
}

void generate_data() {
    std::ofstream fout(FILENAME);
    if (!fout) {
        std::cerr << "Failed to open file" << std::endl;
        exit(1);
    }

    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        fout << rand() << "\n";
    }

    fout.close();
}

std::vector<int> read_data() {
    std::ifstream fin(FILENAME);
    if (!fin) {
        std::cerr << "Failed to open file" << std::endl;
        exit(1);
    }

    std::vector<int> data;
    int value;
    while (fin >> value) {
        data.push_back(value);
    }

    fin.close();
    return data;
}

void run_client() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    generate_data();
    std::vector<int> data = read_data();

    int value = 0, operation = 0;
    select_operation(value, operation);

    int arraySize = static_cast<int>(data.size());
    send(clientSocket, (char*)&arraySize, sizeof(arraySize), 0);

    for (int num : data) {
        send(clientSocket, (char*)&num, sizeof(num), 0);
    }

    send(clientSocket, (char*)&operation, sizeof(operation), 0);
    send(clientSocket, (char*)&value, sizeof(value), 0);

    int result;
    int intResult;
    long long longResult;

    switch (operation) {
        case 1: case 2:
            recv(clientSocket, (char*)&result, sizeof(result), 0);
            std::cout << "Element index: " << result << std::endl;
            std::cout << "Comparisons: " << result + 1 << std::endl;
            break;
            
        case 3:
            recv(clientSocket, (char*)&result, sizeof(result), 0);
            recv(clientSocket, (char*)&intResult, sizeof(intResult), 0);
            std::cout << "Element index: " << result << std::endl;
            std::cout << "Comparisons: " << intResult << std::endl;
            break;
            
        case 4:
            recv(clientSocket, (char*)&result, sizeof(result), 0);
            recv(clientSocket, (char*)&longResult, sizeof(longResult), 0);
            std::cout << "Element index: " << result << std::endl;
            std::cout << "Comparisons: " << longResult << std::endl;
            break;
            
        case 5:
            recv(clientSocket, (char*)&result, sizeof(result), 0);
            recv(clientSocket, (char*)&longResult, sizeof(longResult), 0);
            std::cout << "Permutations: " << result << std::endl;
            std::cout << "Comparisons: " << longResult << std::endl;
            break;
            
        case 6:
            recv(clientSocket, (char*)&intResult, sizeof(intResult), 0);
            recv(clientSocket, (char*)&result, sizeof(result), 0);
            std::cout << "Permutations: " << intResult << std::endl;
            std::cout << "Comparisons: " << result << std::endl;
            break;
    }

    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    run_client();
    return 0;
}