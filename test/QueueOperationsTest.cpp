#include <gtest/gtest.h>
#include <fstream>
#include "QueueOperations.h"

class QueueOperationsTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_filename = "test_queue.bin";
        std::remove(test_filename.c_str());
    }

    void TearDown() override {
        std::remove(test_filename.c_str());
    }

    std::string test_filename;
    const int test_capacity = 5;
};


TEST_F(QueueOperationsTest, CreateQueueFile_Success) {
    EXPECT_TRUE(QueueOperations::createQueueFile(test_filename, test_capacity));

    
    std::ifstream file(test_filename, std::ios::binary);
    EXPECT_TRUE(file.is_open());

    int head, tail, capacity;
    file.read(reinterpret_cast<char*>(&head), sizeof(int));
    file.read(reinterpret_cast<char*>(&tail), sizeof(int));
    file.read(reinterpret_cast<char*>(&capacity), sizeof(int));

    EXPECT_EQ(head, 0);
    EXPECT_EQ(tail, 0);
    EXPECT_EQ(capacity, test_capacity);
}


TEST_F(QueueOperationsTest, InitializeQueueFile_Success) {
  
    ASSERT_TRUE(QueueOperations::createQueueFile(test_filename, test_capacity));

    QueueFile queue{ test_filename, 0, 0, 0 };
    EXPECT_TRUE(QueueOperations::initializeQueueFile(queue));

    EXPECT_EQ(queue.head, 0);
    EXPECT_EQ(queue.tail, 0);
    EXPECT_EQ(queue.capacity, test_capacity);
}


TEST_F(QueueOperationsTest, WriteAndReadMessage_Success) {
    ASSERT_TRUE(QueueOperations::createQueueFile(test_filename, test_capacity));

    QueueFile queue{ test_filename, 0, 0, test_capacity };
    const char* test_message = "Test message";

   
    EXPECT_TRUE(QueueOperations::writeMessage(queue, 0, test_message));

   
    char buffer[maxMessageLength] = { 0 };
    EXPECT_TRUE(QueueOperations::readMessage(queue, 0, buffer));

    EXPECT_STREQ(buffer, test_message);
}

TEST_F(QueueOperationsTest, UpdateHeadAndTail_Success) {
    ASSERT_TRUE(QueueOperations::createQueueFile(test_filename, test_capacity));

    QueueFile queue{ test_filename, 0, 0, test_capacity };

 
    EXPECT_TRUE(QueueOperations::updateHead(queue, 2));

    
    EXPECT_TRUE(QueueOperations::updateTail(queue, 3));

    
    QueueFile updated_queue{ test_filename, 0, 0, 0 };
    EXPECT_TRUE(QueueOperations::initializeQueueFile(updated_queue));

    EXPECT_EQ(updated_queue.head, 2);
    EXPECT_EQ(updated_queue.tail, 3);
}


TEST_F(QueueOperationsTest, ValidationAndNextIndex) {

    EXPECT_TRUE(QueueOperations::validateMessage("Valid message"));
    EXPECT_FALSE(QueueOperations::validateMessage(""));
    EXPECT_FALSE(QueueOperations::validateMessage(std::string(maxMessageLength + 1, 'A')));

   
    EXPECT_EQ(QueueOperations::getNextIndex(0, 5), 1);
    EXPECT_EQ(QueueOperations::getNextIndex(4, 5), 0); 
    EXPECT_EQ(QueueOperations::getNextIndex(2, 5), 3);
}