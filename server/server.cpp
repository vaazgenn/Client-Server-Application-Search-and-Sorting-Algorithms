#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;

void quick_sort(int* arr, int size, int& permutations, int& comparison) {
    if (size <= 1) return;
    
    int pivot = arr[size / 2];
    int i = 0;
    int j = size - 1;

    while (i <= j) {
        while (arr[i] < pivot) { i++; comparison++; }
        while (arr[j] > pivot) { j--; comparison++; }
        
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            permutations++;
            i++;
            j--;
        }
    }

    quick_sort(arr, j + 1, permutations, comparison);
    quick_sort(arr + i, size - i, permutations, comparison);
}

int selection_sort(int* arr, int size, long long& comparison) {
    int permutations = 0;
    
    for (int i = 0; i < size; i++) {
        int min = i;
        for (int j = i + 1; j < size; j++) {
            comparison++;
            if (arr[j] < arr[min]) {
                min = j;
            }
        }
        
        comparison++;
        if (i != min) {
            std::swap(arr[i], arr[min]);
            permutations++;
        }
    }
    
    return permutations;
}

int sequential_search_ordered_array(int* arr, int size, int value, long long& comparison) {
    int* temp = new int[size];
    std::copy(arr, arr + size, temp);
    std::sort(temp, temp + size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            comparison++;
            if (arr[i] == temp[j] && arr[i] == value) {
                delete[] temp;
                return i;
            }
        }
    }

    delete[] temp;
    return -1;
}

int binary_search(int* arr, int size, int value, int& comparison) {
    std::sort(arr, arr + size);
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        comparison++;
        
        if (arr[mid] == value) {
            return mid;
        }
        else if (arr[mid] < value) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
        comparison++;
    }

    return -1;
}

int fast_sequential_search(int* arr, int size, int value) {
    int temp = arr[size - 1];
    arr[size - 1] = value;

    int i = 0;
    while (arr[i] != value) {
        i++;
    }

    arr[size - 1] = temp;
    if (i < size - 1 || arr[size - 1] == value) {
        return i;
    }
    
    return -1;
}

int sequential_search(int* arr, int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return i;
        }
    }
    return -1;
}

SOCKET setup_server() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return INVALID_SOCKET;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    std::cout << "Server started on port " << PORT << std::endl;
    return serverSocket;
}

void run_server() {
    SOCKET serverSocket = setup_server();
    if (serverSocket == INVALID_SOCKET) {
        return;
    }

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    int arraySize;
    recv(clientSocket, (char*)&arraySize, sizeof(arraySize), 0);

    int* dataArray = new int[arraySize];
    for (int i = 0; i < arraySize; i++) {
        recv(clientSocket, (char*)&dataArray[i], sizeof(int), 0);
    }

    int operation;
    recv(clientSocket, (char*)&operation, sizeof(operation), 0);

    int value;
    recv(clientSocket, (char*)&value, sizeof(value), 0);

    int result = -1;
    int permutations = 0;
    int intComparisons = 0;
    long long longComparisons = 0;

    switch (operation) {
        case 1:
            result = sequential_search(dataArray, arraySize, value);
            send(clientSocket, (char*)&result, sizeof(result), 0);
            break;
            
        case 2:
            result = fast_sequential_search(dataArray, arraySize, value);
            send(clientSocket, (char*)&result, sizeof(result), 0);
            break;
            
        case 3:
            result = binary_search(dataArray, arraySize, value, intComparisons);
            send(clientSocket, (char*)&result, sizeof(result), 0);
            send(clientSocket, (char*)&intComparisons, sizeof(intComparisons), 0);
            break;
            
        case 4:
            result = sequential_search_ordered_array(dataArray, arraySize, value, longComparisons);
            send(clientSocket, (char*)&result, sizeof(result), 0);
            send(clientSocket, (char*)&longComparisons, sizeof(longComparisons), 0);
            break;
            
        case 5:
            permutations = selection_sort(dataArray, arraySize, longComparisons);
            send(clientSocket, (char*)&permutations, sizeof(permutations), 0);
            send(clientSocket, (char*)&longComparisons, sizeof(longComparisons), 0);
            break;
            
        case 6:
            quick_sort(dataArray, arraySize, permutations, intComparisons);
            send(clientSocket, (char*)&permutations, sizeof(permutations), 0);
            send(clientSocket, (char*)&intComparisons, sizeof(intComparisons), 0);
            break;
            
        default:
            std::cerr << "Invalid operation" << std::endl;
    }

    delete[] dataArray;
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    run_server();
    return 0;
}