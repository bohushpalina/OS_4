#include "../headers/SharedQueue.h"
#include <iostream>
#include <string>
#include <clocale>

using namespace std;

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    cout << "Консоль: Sender\n";
    if (argc < 2) {
        cout << "Использование: sender.exe <имя_файла>\n";
        return 1;
    }
    string fileName = argv[1];

    SharedQueue queue;
    if (!queue.OpenAsSender(fileName)) {
        cerr << "Ошибка: не удалось подключиться к приемнику.\n";
        return 1;
    }

    if (!queue.SignalSenderReady()) {
        cerr << "Ошибка: сбой отправки сигнала готовности.\n";
        return 1;
    }
    cout << "Успешное подключение.\n\nДоступны команды: \"send\" - отправить , \"exit\" - закрыть.\n";

    while (true) {
        cout << "Введите команду (send/exit): ";
        string cmdLine;
        if (!getline(cin, cmdLine)) break;
        if (cmdLine == "send") {
            if (queue.IsShuttingDown()) {
                cout << "Приемник отключился. Выход.\n";
                break;
            }
            cout << "Введите текст сообщения (до 19 символов): ";
            string msg;
            if (!getline(cin, msg)) break;
            if (msg.size() >= MAX_MESSAGE_LEN) {
                cout << "Слишком длинное сообщение. Отказ.\n";
                continue;
            }
            if (!queue.PushMessage(msg, true)) {
                cout << "Сбой передачи.\n";
                if (queue.IsShuttingDown()) break;
            }
        }
        else if (cmdLine == "exit") {
            cout << "Завершение процесса.\n";
            break;
        }
        else {
            cout << "Неизвестная команда.\n";
        }
    }
    return 0;
}