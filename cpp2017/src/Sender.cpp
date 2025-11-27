#include<iostream>
#include<fstream>
#include<string>
#include<windows.h>

#ifdef max
#undef max
#endif
#include<limits>
#include<vector>
#include<QueueOperations.h>
using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3) exit(-1);

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    string binFileName(argv[1]);
    uintptr_t raw = stoull(argv[2]);
    HANDLE hEvent = reinterpret_cast<HANDLE>(raw);
    SetEvent(hEvent);

    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Global\\QueueMutex");
    HANDLE hEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Global\\QueueEmpty");
    HANDLE hFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Global\\QueueFull");

    QueueFile queue = { binFileName, 0, 0, 0 };

    while (true) {
        int operation = 0;
        cout << "Введите операцию:\n1:Отправка сообщения\n2:Выйти из программы\n";

        if (!(cin >> operation)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (operation) {
        case 1:
        {
            WaitForSingleObject(hEmpty, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            if (!QueueOperations::initializeQueueFile(queue)) {
                cout << "Ошибка чтения состояния очереди\n";
                ReleaseMutex(hMutex);
                ReleaseSemaphore(hEmpty, 1, NULL);
                break;
            }

            char message[maxMessageLength];
            cin.getline(message, maxMessageLength);

            if (QueueOperations::writeMessage(queue, queue.tail, message)) {
                int newTail = QueueOperations::getNextIndex(queue.tail, queue.capacity);
                if (QueueOperations::updateTail(queue, newTail)) {
                    queue.tail = newTail;
                }
                else {
                    cout << "Ошибка обновления tail\n";
                }
            }
            else {
                cout << "Ошибка записи сообщения\n";
            }

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hFull, 1, NULL);
            break;
        }
        case 2:
            exit(0);
            break;
        default:
            cout << "Неизвестная операция\n";
        }
    }
    return 0;
}