#include <iostream>
#include <fcntl.h>    // for open()
#include <sys/mman.h> // for mmap()
#include <sys/stat.h> // for fstat()
#include <unistd.h>   // for close()
#include <cstring>    // for memcpy()

int main() {
    const char* filePath = "output.txt";
    const char* data = "Hello, mmap!";
    size_t dataSize = strlen(data);

    // Step 1: Open the file with read-write permissions and create it if it doesn't exist
    int fd = open(filePath, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    // Step 2: Resize the file to hold the data
    if (ftruncate(fd, dataSize) == -1) {
        std::cerr << "Error resizing file\n";
        close(fd);
        return 1;
    }

    // Step 3: Memory-map the file
    char* map = (char*) mmap(nullptr, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        std::cerr << "Error mapping file\n";
        close(fd);
        return 1;
    }

    // Step 4: Write data to the memory-mapped region
//    memcpy(map, data, dataSize);
    for(uint8_t i=0;i<dataSize;i++)
        map[i] = i;
    // Step 5: Sync changes with disk
    if (msync(map, dataSize, MS_SYNC) == -1) {
        std::cerr << "Could not sync the file to disk\n";
    } else {
        std::cout << "Data successfully synced to disk\n";
    }

    // Step 6: Unmap the file
    if (munmap(map, dataSize) == -1) {
        std::cerr << "Error unmapping the file\n";
    }

    // Step 7: Close the file descriptor
    close(fd);

    return 0;
}
