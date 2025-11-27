#include "QueueOperations.h"
#include <fstream>
#include <stdexcept>

using namespace std;

bool QueueOperations::createQueueFile(const string& filename, int numberOfMessages) {
    if (numberOfMessages <= 0) {
        throw invalid_argument("число сообщений должно быть положительным");
    }

    fstream file(filename, ios::binary | ios::trunc | ios::out);
    if (!file.is_open()) {
        return false;
    }

    int head = 0;
    int tail = 0;

    file.write((char*)&head, sizeof(int));
    file.write((char*)&tail, sizeof(int));
    file.write((char*)&numberOfMessages, sizeof(int));

    string empty(maxMessageLength, '\0');
    for (int i = 0; i < numberOfMessages; i++) {
        file.write(empty.c_str(), maxMessageLength);
    }

    return file.good();
}

bool QueueOperations::initializeQueueFile(QueueFile& queue) {
    fstream file(queue.filename, ios::binary | ios::in);
    if (!file.is_open()) {
        return false;
    }

    file.read((char*)&queue.head, sizeof(int));
    file.read((char*)&queue.tail, sizeof(int));
    file.read((char*)&queue.capacity, sizeof(int));

    return file.good();
}

bool QueueOperations::readMessage(QueueFile& queue, int index, char* message) {
    if (index < 0 || index >= queue.capacity || message == nullptr) {
        return false;
    }

    fstream file(queue.filename, ios::binary | ios::in);
    if (!file.is_open()) {
        return false;
    }

    int offset = 3 * sizeof(int) + index * maxMessageLength;
    file.seekg(offset, ios::beg);
    file.read(message, maxMessageLength);

    return file.good();
}

bool QueueOperations::writeMessage(QueueFile& queue, int index, const char* message) {
    if (index < 0 || index >= queue.capacity || message == nullptr) {
        return false;
    }

    fstream file(queue.filename, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        return false;
    }

    int offset = 3 * sizeof(int) + index * maxMessageLength;
    file.seekp(offset, ios::beg);

    
    string safeMessage(message);
    if (safeMessage.length() > maxMessageLength) {
        safeMessage.resize(maxMessageLength);
    }

    file.write(safeMessage.c_str(), maxMessageLength);
    return file.good();
}

bool QueueOperations::updateHead(QueueFile& queue, int newHead) {
    if (newHead < 0 || newHead >= queue.capacity) {
        return false;
    }

    fstream file(queue.filename, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        return false;
    }

    file.seekp(0, ios::beg);
    file.write((char*)&newHead, sizeof(int));
    queue.head = newHead;

    return file.good();
}

bool QueueOperations::updateTail(QueueFile& queue, int newTail) {
    if (newTail < 0 || newTail >= queue.capacity) {
        return false;
    }

    fstream file(queue.filename, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        return false;
    }

    file.seekp(sizeof(int), ios::beg); 
    file.write((char*)&newTail, sizeof(int));
    queue.tail = newTail;

    return file.good();
}

bool QueueOperations::readQueueHeader(QueueFile& queue) {
    return initializeQueueFile(queue);
}

int QueueOperations::getNextIndex(int current, int capacity) {
    return (current + 1) % capacity;
}

bool QueueOperations::validateMessage(const string& message) {
    return !message.empty() && message.length() <= maxMessageLength;
}