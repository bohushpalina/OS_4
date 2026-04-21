#include "../headers/SharedQueue.h"
#include <iostream>
#include <string>

using namespace solution;

int main(int argc, char* argv[]) {
    cout << "=== Sender ===\n";
    if (argc < 2) {
        cout << "Usage: sender.exe <fileName>\n";
        return 1;
    }
    string fileName = argv[1];

    SharedQueue queue;
    if (!queue.OpenAsSender(fileName)) {
        cerr << "[Sender] Can't open queue.\n";
        return 1;
    }

    if (!queue.SignalSenderReady()) {
        cerr << "[Sender] Can't send ready signal.\n";
        return 1;
    }
    cout << "[Sender] Ready. Commands: s - send, q - quit.\n";

    while (true) {
        cout << "[Sender] Command (s/q): ";
        string cmdLine;
        if (!getline(cin, cmdLine)) break;
        if (cmdLine == "s") {
            if (queue.IsShuttingDown()) {
                cout << "[Sender] Receiver finishes. Quitting.\n";
                break;
            }
            cout << "Input message (<20 symbols): ";
            string msg;
            if (!getline(cin, msg)) break;
            if (msg.size() >= MAX_MESSAGE_LEN) {
                cout << "[Sender] Message too long.\n";
                continue;
            }
            if (!queue.PushMessage(msg, true)) {
                cout << "[Sender] Error.\n";
                if (queue.IsShuttingDown()) break;
            }
        } else if (cmdLine == "q") {
            cout << "[Sender] Quitting.\n";
            break;
        } else {
            cout << "Unknown commands.\n";
        }
    }
    return 0;
}