#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "=== DistCache Enhanced Test Suite ===" << std::endl;
    std::cout << "Running comprehensive unit tests..." << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    
    // Custom test listener for better output
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    
    int result = RUN_ALL_TESTS();
    
    if (result == 0) {
        std::cout << "\n🎉 All tests passed! System is ready for production." << std::endl;
    } else {
        std::cout << "\n❌ Some tests failed. Please review the output above." << std::endl;
    }
    
    return result;
}