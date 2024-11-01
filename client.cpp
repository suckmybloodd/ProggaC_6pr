#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Использование: " << argv[0] << " <IP адрес сервера> <порт>" << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int port = std::atoi(argv[2]);
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Преобразование IP адреса
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Неверный адрес/адрес не поддерживается" << std::endl;
        return -1;
    }
if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Ошибка подключения к серверу" << std::endl;
        return -1;
    }

    while (true) {
        std::cout << "Введите ваше число (или 'exit' для выхода): ";
        std::string input;
        std::cin >> input;

        if (input == "exit") {
            break;
        }

        send(sock, input.c_str(), input.length(), 0);
        char buffer[BUFFER_SIZE] = {0};
        int valread = read(sock, buffer, BUFFER_SIZE);
        std::cout << buffer;
    }

    close(sock);
    return 0;
}