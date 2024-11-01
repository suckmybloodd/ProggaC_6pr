#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <random>
#include <time.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Использование: " << argv[0] << " <порт>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Привязка сокета к порту
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Ошибка настройки параметров сокета");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки сокета");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Ошибка в прослушивании");
        exit(EXIT_FAILURE);
    }

    std::cout << "Сервер запущен на порту " << port << std::endl;

    // Генерация случайного числа
    srand(time(NULL));   // Initialization, should only be called once.
    int min_num = 0;
    int max_num = 10;
    int secret_number = rand() % (max_num - min_num + 1) + min_num; 

    while (true) {
        std::cout << "Ожидание подключения клиента..." << std::endl;

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Ошибка при подключении");
            exit(EXIT_FAILURE);
        }

        std::cout << "Клиент подключен: " << inet_ntoa(address.sin_addr) << std::endl;
        int attempts = 0;

        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            // int valread = read(new_socket, buffer, BUFFER_SIZE);
            int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
            if (valread <= 0) {
                std::cout << "Клиент отключен: " << inet_ntoa(address.sin_addr) << std::endl;
                break;
            }

            attempts++;
            int guess = std::atoi(buffer);
            std::cout << inet_ntoa(address.sin_addr) << ": " << guess << std::endl;

            if (guess < secret_number) {
                send(new_socket, "Мое число больше\n", 32, 0);
            } else if (guess > secret_number) {
                send(new_socket, "Мое число меньше\n", 32, 0);
            } else {
                std::string message = "Поздравляю! Вы угадали число " + std::to_string(secret_number) + " за " + std::to_string(attempts) + " попыток.\n";
                send(new_socket, message.c_str(), message.size(), 0);
                break;
            }
        }

        secret_number = rand() % (max_num - min_num + 1) + min_num;  // Генерация нового числа
        close(new_socket);
    }

    return 0;
}
