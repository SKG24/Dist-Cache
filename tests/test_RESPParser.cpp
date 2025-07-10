#include <gtest/gtest.h>
#include "network/RESPParser.h"

class RESPParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = std::make_unique<RESPParser>();
    }
    
    void TearDown() override {
        parser.reset();
    }
    
    std::unique_ptr<RESPParser> parser;
};

TEST_F(RESPParserTest, BasicCommandParsing) {
    auto [cmd, args] = parser->parse("SET mykey myvalue");
    
    EXPECT_EQ(cmd, "SET");
    EXPECT_EQ(args.size(), 2);
    EXPECT_EQ(args[0], "mykey");
    EXPECT_EQ(args[1], "myvalue");
}

TEST_F(RESPParserTest, GetCommandParsing) {
    auto [cmd, args] = parser->parse("GET testkey");
    
    EXPECT_EQ(cmd, "GET");
    EXPECT_EQ(args.size(), 1);
    EXPECT_EQ(args[0], "testkey");
}

TEST_F(RESPParserTest, DeleteCommandParsing) {
    auto [cmd, args] = parser->parse("DEL key1 key2 key3");
    
    EXPECT_EQ(cmd, "DEL");
    EXPECT_EQ(args.size(), 3);
    EXPECT_EQ(args[0], "key1");
    EXPECT_EQ(args[1], "key2");
    EXPECT_EQ(args[2], "key3");
}

TEST_F(RESPParserTest, EmptyCommand) {
    auto [cmd, args] = parser->parse("");
    
    EXPECT_TRUE(cmd.empty());
    EXPECT_TRUE(args.empty());
}

TEST_F(RESPParserTest, CommandWithNoArgs) {
    auto [cmd, args] = parser->parse("PING");
    
    EXPECT_EQ(cmd, "PING");
    EXPECT_TRUE(args.empty());
}

TEST_F(RESPParserTest, CaseInsensitiveCommands) {
    auto [cmd1, args1] = parser->parse("set mykey myvalue");
    auto [cmd2, args2] = parser->parse("SET mykey myvalue");
    auto [cmd3, args3] = parser->parse("Set mykey myvalue");
    
    EXPECT_EQ(cmd1, "SET");
    EXPECT_EQ(cmd2, "SET");
    EXPECT_EQ(cmd3, "SET");
}

TEST_F(RESPParserTest, ResponseSerialization) {
    std::string ok_response = parser->serialize("OK");
    std::string value_response = parser->serialize("hello world");
    std::string error_response = parser->serialize_error("ERR unknown command");
    std::string nil_response = parser->serialize_nil();
    
    EXPECT_EQ(ok_response, "+OK\r\n");
    EXPECT_EQ(value_response, "+hello world\r\n");
    EXPECT_EQ(error_response, "-ERR unknown command\r\n");
    EXPECT_EQ(nil_response, "$-1\r\n");
}

TEST_F(RESPParserTest, BulkStringSerialization) {
    std::string bulk_response = parser->serialize_bulk("hello");
    std::string expected = "$5\r\nhello\r\n";
    
    EXPECT_EQ(bulk_response, expected);
}

TEST_F(RESPParserTest, IntegerSerialization) {
    std::string int_response = parser->serialize_integer(42);
    std::string expected = ":42\r\n";
    
    EXPECT_EQ(int_response, expected);
}
