#ifndef CHIP_8_FILEREADER_H
#define CHIP_8_FILEREADER_H

#include <string>
#include <fstream>

/**
 * A class for simplifying the reading of files. It allows for easy opening of a file as well as support for reading a file's data into a memory buffer.
 */
class FileByteReader {
public:
    FileByteReader(std::string filename);
    long readToBuffer(uint8_t* buffer, int offset, int size);
    virtual ~FileByteReader();

private:
    std::ifstream fileStream;
};


#endif //CHIP_8_FILEREADER_H
