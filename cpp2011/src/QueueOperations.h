#pragma once
#include <string>
#include <fstream>

const int maxMessageLength = 20;

struct QueueFile {
    std::string filename;
    int head;
    int tail;
    int capacity;
};

namespace QueueOperations {
    bool createQueueFile(const std::string& filename, int numberOfMessages);
    bool initializeQueueFile(QueueFile& queue);
    bool readMessage(QueueFile& queue, int index, char* message);
    bool writeMessage(QueueFile& queue, int index, const char* message);
    bool updateHead(QueueFile& queue, int newHead);
    bool updateTail(QueueFile& queue, int newTail);
    bool readQueueHeader(QueueFile& queue);
    int getNextIndex(int current, int capacity);
    bool validateMessage(const std::string& message);
}