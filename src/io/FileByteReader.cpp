#include "FileByteReader.h"
#include "../exceptions/IOException.h"

FileByteReader::FileByteReader(std::string filename) {
    fileStream.open(filename, std::ios::binary);
    if (!fileStream.is_open()) {
        throw IOException("Unable to open file");
    }
}
/**
 * @return the number of bytes successfully read
 */
long FileByteReader::readToBuffer(uint8_t *buffer, int offset, int size) {
    if (!fileStream.read((char *) buffer + offset, size)) {
        return fileStream.gcount();
    }
    return size;
}

FileByteReader::~FileByteReader() {
    fileStream.close();
}
