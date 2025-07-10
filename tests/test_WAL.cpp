#include <gtest/gtest.h>
#include <fstream>
#include <thread>
#include <chrono>
#include "storage/WAL.h"

class WALTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file = "test_wal.log";
        // Clean up any existing test file
        std::remove(test_file.c_str());
        wal = std::make_unique<WAL>(test_file);
    }
    
    void TearDown() override {
        wal.reset();
        std::remove(test_file.c_str());
    }
    
    std::string test_file;
    std::unique_ptr<WAL> wal;
};

TEST_F(WALTest, BasicAppendAndReplay) {
    wal->append("SET", "key1", "value1");
    wal->append("SET", "key2", "value2");
    wal->append("DEL", "key1");
    
    // Force file close and reopen for replay
    wal.reset();
    wal = std::make_unique<WAL>(test_file);
    
    auto ops = wal->replay();
    
    ASSERT_EQ(ops.size(), 3);
    
    EXPECT_EQ(std::get<0>(ops[0]), "SET");
    EXPECT_EQ(std::get<1>(ops[0]), "key1");
    EXPECT_EQ(std::get<2>(ops[0]), "value1");
    
    EXPECT_EQ(std::get<0>(ops[1]), "SET");
    EXPECT_EQ(std::get<1>(ops[1]), "key2");
    EXPECT_EQ(std::get<2>(ops[1]), "value2");
    
    EXPECT_EQ(std::get<0>(ops[2]), "DEL");
    EXPECT_EQ(std::get<1>(ops[2]), "key1");
    EXPECT_EQ(std::get<2>(ops[2]), ""); // DEL has no value
}

TEST_F(WALTest, DeleteOperationWithoutValue) {
    wal->append("DEL", "testkey");
    
    // Force file close and reopen for replay
    wal.reset();
    wal = std::make_unique<WAL>(test_file);
    
    auto ops = wal->replay();
    
    ASSERT_EQ(ops.size(), 1);
    EXPECT_EQ(std::get<0>(ops[0]), "DEL");
    EXPECT_EQ(std::get<1>(ops[0]), "testkey");
    EXPECT_EQ(std::get<2>(ops[0]), "");
}

TEST_F(WALTest, EmptyWALReplay) {
    auto ops = wal->replay();
    EXPECT_EQ(ops.size(), 0);
}

TEST_F(WALTest, LargeOperationSet) {
    const int num_ops = 10000;
    
    for (int i = 0; i < num_ops; ++i) {
        wal->append("SET", "key" + std::to_string(i), "value" + std::to_string(i));
    }
    
    // Force file close and reopen for replay
    wal.reset();
    wal = std::make_unique<WAL>(test_file);
    
    auto ops = wal->replay();
    
    EXPECT_EQ(ops.size(), num_ops);
    
    // Check first and last operations
    EXPECT_EQ(std::get<0>(ops[0]), "SET");
    EXPECT_EQ(std::get<1>(ops[0]), "key0");
    EXPECT_EQ(std::get<2>(ops[0]), "value0");
    
    EXPECT_EQ(std::get<0>(ops[num_ops-1]), "SET");
    EXPECT_EQ(std::get<1>(ops[num_ops-1]), "key9999");
    EXPECT_EQ(std::get<2>(ops[num_ops-1]), "value9999");
}

TEST_F(WALTest, ConcurrentWrites) {
    const int num_threads = 4;
    const int ops_per_thread = 1000;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; ++i) {
                std::string key = "thread" + std::to_string(t) + "_key" + std::to_string(i);
                std::string value = "thread" + std::to_string(t) + "_value" + std::to_string(i);
                wal->append("SET", key, value);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Force file close and reopen for replay
    wal.reset();
    wal = std::make_unique<WAL>(test_file);
    
    auto ops = wal->replay();
    
    EXPECT_EQ(ops.size(), num_threads * ops_per_thread);
}

TEST_F(WALTest, WALTruncation) {
    wal->append("SET", "key1", "value1");
    wal->append("SET", "key2", "value2");
    
    wal->truncate();
    
    auto ops = wal->replay();
    EXPECT_EQ(ops.size(), 0);
}

TEST_F(WALTest, WALSync) {
    wal->append("SET", "key1", "value1");
    wal->sync(); // Force sync to disk
    
    // Should be able to read immediately
    std::ifstream file(test_file);
    std::string line;
    std::getline(file, line);
    
    EXPECT_FALSE(line.empty());
    EXPECT_NE(line.find("SET"), std::string::npos);
    EXPECT_NE(line.find("key1"), std::string::npos);
    EXPECT_NE(line.find("value1"), std::string::npos);
}
