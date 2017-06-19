#include <gtest/gtest.h>

/**
 * A simple main function to kick off testcase execution.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}