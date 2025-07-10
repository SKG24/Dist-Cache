#include <gtest/gtest.h>
#include <fstream>
#include <unordered_map>
#include "storage/MMapPersistence.h"

class MMapPersistenceTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file = "test_snapshot.dat";
        persistence = std::make_unique<MMapPersistence>(test_file);
    }
    
    void TearDown() override {
        persistence.reset();
        std::remove(test_file.c_str());
    }
    
    std::string test_file;
    std::unique_ptr<MMapPersistence> persistence;
};

TEST_F(MMapPersistenceTest, BasicSnapshotAndLoad) {
    std::unordered_map<std::string, std::string> original_data = {
        {"key1", "value1"},
        {"key2", "value2"},
        {"key3", "value3"}
    };
    
    persistence->snapshot(original_data);
    auto loaded_data = persistence->load();
    
    EXPECT_EQ(loaded_data.size(), 3);
    EXPECT_EQ(loaded_data["key1"], "value1");
    EXPECT_EQ(loaded_data["key2"], "value2");
    EXPECT_EQ(loaded_data["key3"], "value3");
}

TEST_F(MMapPersistenceTest, EmptySnapshot) {
    std::unordered_map<std::string, std::string> empty_data;
    
    persistence->snapshot(empty_data);
    auto loaded_data = persistence->load();
    
    EXPECT_EQ(loaded_data.size(), 0);
}

TEST_F(MMapPersistenceTest, LargeDataSet) {
    std::unordered_map<std::string, std::string> large_data;
    
    // Create 10,000 entries
    for (int i = 0; i < 10000; ++i) {
        large_data["key" + std::to_string(i)] = "value" + std::to_string(i);
    }
    
    persistence->snapshot(large_data);
    auto loaded_data = persistence->load();
    
    EXPECT_EQ(loaded_data.size(), 10000);
    EXPECT_EQ(loaded_data["key0"], "value0");
    EXPECT_EQ(loaded_data["key9999"], "value9999");
}

TEST_F(MMapPersistenceTest, SpecialCharacters) {
    std::unordered_map<std::string, std::string> special_data = {
        {"key with spaces", "value with spaces"},
        {"key\twith\ttabs", "value\twith\ttabs"},
        {"key_with_underscore", "value_with_underscore"}
    };
    
    persistence->snapshot(special_data);
    auto loaded_data = persistence->load();
    
    EXPECT_EQ(loaded_data.size(), 3);
    EXPECT_EQ(loaded_data["key with spaces"], "value with spaces");
    EXPECT_EQ(loaded_data["key_with_underscore"], "value_with_underscore");
}

TEST_F(MMapPersistenceTest, OverwriteExistingFile) {
    std::unordered_map<std::string, std::string> data1 = {{"key1", "value1"}};
    std::unordered_map<std::string, std::string> data2 = {{"key2", "value2"}};
    
    persistence->snapshot(data1);
    persistence->snapshot(data2);
    
    auto loaded_data = persistence->load();
    
    EXPECT_EQ(loaded_data.size(), 1);
    EXPECT_EQ(loaded_data["key2"], "value2");
}