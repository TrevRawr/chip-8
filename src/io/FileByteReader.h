#ifndef CHIP_8_FILEREADER_H
#define CHIP_8_FILEREADER_H

#include <string>
#include <fstream>

class FileByteReader {
public:
    FileByteReader(std::string filename);
    long readToBuffer(uint8_t* buffer, int offset, int size);
    virtual ~FileByteReader();

private:
    std::ifstream fileStream;
};


#endif //CHIP_8_FILEREADER_H
