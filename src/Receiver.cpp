#include "../headers/SharedQueue.h"
#include <iostream>
#include <string>
#include <vector>
#include <clocale>

using namespace std;

static unsigned int to_uint(const string& s) {
    return static_cast<unsigned int>(atoi(s.c_str()));
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    cout << "Консоль: Receiver\n";
    cout << "Задайте имя бинарного файла: ";
    string fileName;
    getline(cin, fileName);

    std::cout << "Укажите вместимость очереди (макс. число записей): ";
    string tmp;
    unsigned int capacity = 0;

    while (true) {
        getline(std::cin, tmp);

        if (tmp.empty()) {
            std::cout << "Пустая строка недопустима. Введите заново: ";
            continue;
        }
        bool allDigits = true;
        for (char c : tmp) {
            if (c < '0' || c > '9') {
                allDigits = false;
                break;
            }
        }
        if (!allDigits) {
            std::cout << "Требуется ввести число. Повторите: ";
            continue;
        }
        capacity = static_cast<unsigned int>(atoi(tmp.c_str()));

        if (capacity == 0) {
            std::cout << "Размер должен быть больше нуля. Введите заново: ";
            continue;
        }
        break;
    }

    const unsigned int MAX_SENDERS = 10;
    std::cout << "Сколько процессов-отправителей запустить? (от 1 до " << MAX_SENDERS << "): ";
    string tmp2;
    unsigned int senderCount = 0;

    while (true) {
        getline(std::cin, tmp2);
        if (tmp2.empty()) {
            std::cout << "Пустая строка недопустима. Введите заново: ";
            continue;
        }
        bool allDigits = true;
        for (char c : tmp2) {
            if (c < '0' || c > '9') {
                allDigits = false;
                break;
            }
        }
        if (!allDigits) {
            std::cout << "Требуется ввести число. Повторите: ";
            continue;
        }
        senderCount = static_cast<unsigned int>(atoi(tmp2.c_str()));

        if (senderCount == 0) {
            std::cout << "Количество должно быть больше нуля. Введите заново: ";
            continue;
        }
        if (senderCount > MAX_SENDERS) {
            std::cout << "Превышен лимит. Допускается максимум " << MAX_SENDERS << ". Повторите: ";
            continue;
        }
        break;
    }

    SharedQueue queue;
    if (!queue.CreateAsReceiver(fileName, capacity, senderCount)) {
        cerr << "Ошибка: не удалось инициализировать очередь.\n";
        return 1;
    }

    cout << "Старт процессов (Sender): " << senderCount << " шт...\n";
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    string fullPath(path);
    size_t pos = fullPath.find_last_of("\\/");
    fullPath = fullPath.substr(0, pos + 1);

    string exeName = fullPath + "sender.exe";
    for (unsigned int i = 0; i < senderCount; ++i) {
        string cmd = "\"" + exeName + "\" \"" + fileName + "\"";
        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        std::vector<char> buf(cmd.begin(), cmd.end());
        buf.push_back('\0');
        DWORD creationFlags = CREATE_NEW_CONSOLE;

        if (!CreateProcessA(NULL, &buf[0], NULL, NULL, FALSE, creationFlags, NULL, NULL, &si, &pi)) {
            PrintLastErrorA("Сбой запуска отправителя");
        }
        else {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }

    queue.WaitAllSendersReady(INFINITE);
    cout << "Все на связи.\n\n";
    cout << "Доступны команды: \"read\" - прочитать, \"exit\" - выйти.\n";

    while (true) {
        cout << "Ввод команды (read/exit): ";
        string cmdLine;
        if (!getline(cin, cmdLine)) break;
        if (cmdLine == "read") {
            string msg;
            bool ok = queue.PopMessage(msg, true);
            if (!ok) {
                if (queue.IsShuttingDown())
                    cout << "Завершение работы...\n";
                else
                    cout << "Очередь пуста\n";
            }
        }
        else if (cmdLine == "exit") {
            cout << "Посылаем сигнал на закрытие.\n";
            queue.SignalShutdown();
            break;
        }
        else {
            cout << "Команда не распознана.\n";
        }
    }
    return 0;
}