#include "../headers/SharedQueue.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

static unsigned int to_uint(const string& s) {
    return static_cast<unsigned int>(atoi(s.c_str()));
}

int main() {
    cout << "=== Receiver ===\n";
    cout << "Input binary file name: ";
    string fileName;
    getline(cin, fileName);

    std::cout << "Input records count (capacity): ";
    string tmp;
    unsigned int capacity = 0;

    while (true) {
        getline(std::cin, tmp);

        if (tmp.empty()) {
            std::cout << "Input cannot be empty. Try again: ";
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
            std::cout << "Invalid input. Only digits are allowed. Try again: ";
            continue;
        }
        capacity = static_cast<unsigned int>(atoi(tmp.c_str()));

        if (capacity == 0) {
            std::cout << "Number must be positive. Try again: ";
            continue;
        }
        break;
    }

    const unsigned int MAX_SENDERS = 10;
    std::cout << "Input desired Sender process count (1-" << MAX_SENDERS << "): ";
    string tmp2;
    unsigned int senderCount = 0;

    while (true) {
        getline(std::cin, tmp2);
        if (tmp2.empty()) {
            std::cout << "Input cannot be empty. Try again: ";
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
            std::cout << "Invalid input. Only digits are allowed. Try again: ";
            continue;
        }
        senderCount = static_cast<unsigned int>(atoi(tmp2.c_str()));

        if (senderCount == 0) {
            std::cout << "Number must be positive. Try again: ";
            continue;
        }
        if (senderCount > MAX_SENDERS) {
            std::cout << "Number too large. Max allowed: " << MAX_SENDERS << ". Try again: ";
            continue;
        }
        break;
    }

    std::cout << "Sender count is: " << senderCount << "\n";


    SharedQueue queue;
    if (!queue.CreateAsReceiver(fileName, capacity, senderCount)) {
        cerr << "Error creating queue.\n";
        return 1;
    }

    cout << "Launching " << senderCount << " instances of sender process...\n";
    string exeName = ".\\sender.exe";
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
            PrintLastErrorA("CreateProcess sender");
        } else {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }

    cout << "Waiting for all Senders to be ready...\n";
    queue.WaitAllSendersReady(INFINITE);
    cout << "All Senders ready. Commands: r - read, q - quit.\n";

    while (true) {
        cout << "[Receiver] Command (r/q): ";
        string cmdLine;
        if (!getline(cin, cmdLine)) break;
        if (cmdLine == "r") {
            string msg;
            bool ok = queue.PopMessage(msg, true);
            if (!ok) {
                if (queue.IsShuttingDown())
                    cout << "[Receiver] Quitting...\n";
                else
                    cout << "[Receiver] No messages\n";
            }
        } else if (cmdLine == "q") {
            cout << "[Receiver] Finishing. shutdown signal.\n";
            queue.SignalShutdown();
            break;
        } else {
            cout << "Unknown command.\n";
        }
    }
    return 0;
}