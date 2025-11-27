#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "QueueOperations.h"  
#include<vector>
#ifdef max
#undef max
#endif
#include <limits>

using namespace std;

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    string binFileName;
    cout << "Введите имя бинарного файла\n";
    cin >> binFileName;

    cout << "Введите Число процессов\n";
    int numberOfProcesses;
    cin >> numberOfProcesses;

    int numberOfMessages;
    cout << "Введите Число сообщений\n";
    cin >> numberOfMessages;

    try {
        if (!QueueOperations::createQueueFile(binFileName, numberOfMessages)) {
            cout << "Ошибка создания файла очереди\n";
            return -1;
        }
    }
    catch (const invalid_argument& e) {
        cout << "Ошибка: " << e.what() << endl;
        return -1;
    }


    QueueFile queue{ binFileName, 0, 0, 0 };
    if (!QueueOperations::initializeQueueFile(queue)) {
        cout << "Ошибка инициализации очереди\n";
        return -1;
    }


    HANDLE* CreationEvents = new HANDLE[numberOfProcesses];
    STARTUPINFO* siRec = new STARTUPINFO[numberOfProcesses];
    PROCESS_INFORMATION* piRec = new PROCESS_INFORMATION[numberOfProcesses];

    HANDLE hEmpty = CreateSemaphore(NULL, numberOfMessages, numberOfMessages, "Global\\QueueEmpty");
    HANDLE hFull = CreateSemaphore(NULL, 0, numberOfMessages, "Global\\QueueFull");
    HANDLE hMutex = CreateMutex(NULL, FALSE, "Global\\QueueMutex");

    for (int i = 0; i < numberOfProcesses; i++) {
        ZeroMemory(&siRec[i], sizeof(STARTUPINFO));
        siRec[i].cb = sizeof(STARTUPINFO);
        SECURITY_ATTRIBUTES sa;
        ZeroMemory(&sa, sizeof(sa));
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        CreationEvents[i] = CreateEvent(&sa, FALSE, FALSE, NULL);
        uintptr_t hValue = reinterpret_cast<uintptr_t>(CreationEvents[i]);
        string consoleArgs = "Sender11.exe " + binFileName + " " + to_string(hValue);
        vector<char> cmd(consoleArgs.begin(), consoleArgs.end());
        cmd.push_back('\0');
        if (!CreateProcess(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &siRec[i], &piRec[i])) {
            cout << "Не удалось создать Sender\n";
        }
    }

    WaitForMultipleObjects(numberOfProcesses, CreationEvents, TRUE, INFINITE);


    while (true) {
        int operation = 0;
        cout << "Введите операцию:\n1:Чтение сообщения\n2:Выйти из программы\n";

        if (!(cin >> operation)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (operation) {
        case 1: {
            WaitForSingleObject(hFull, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);


            char message[maxMessageLength];


            if (!QueueOperations::initializeQueueFile(queue)) {
                cout << "Ошибка чтения состояния очереди\n";
                ReleaseMutex(hMutex);
                ReleaseSemaphore(hEmpty, 1, NULL);
                break;
            }


            if (QueueOperations::readMessage(queue, queue.head, message)) {
                cout << "Получено: " << message << endl;


                int newHead = QueueOperations::getNextIndex(queue.head, queue.capacity);


                if (QueueOperations::updateHead(queue, newHead)) {
                    queue.head = newHead;
                }
                else {
                    cout << "Ошибка обновления указателя head\n";
                }
            }
            else {
                cout << "Ошибка чтения сообщения\n";
            }

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hEmpty, 1, NULL);
            break;
        }
        case 2: {

            for (int i = 0; i < numberOfProcesses; i++) {
                CloseHandle(piRec[i].hProcess);
                CloseHandle(piRec[i].hThread);
                CloseHandle(CreationEvents[i]);
            }
            delete[] piRec;
            delete[] siRec;
            delete[] CreationEvents;

            CloseHandle(hEmpty);
            CloseHandle(hFull);
            CloseHandle(hMutex);

            exit(0);
            break;
        }
        default:
            cout << "Неизвестная операция\n";
        }
    }
}