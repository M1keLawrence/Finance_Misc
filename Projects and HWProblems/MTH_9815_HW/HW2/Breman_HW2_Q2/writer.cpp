#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

int main() {
    // Remove any previous shared memory with the same name
    boost::interprocess::shared_memory_object::remove("MySharedMemory");

    // Create shared memory (read/write)
    boost::interprocess::shared_memory_object shm(
        boost::interprocess::create_only,
        "MySharedMemory",
        boost::interprocess::read_write
    );

    // Allocate enough space for one int
    shm.truncate(sizeof(int));

    // Map it into this process
    boost::interprocess::mapped_region region(
        shm,
        boost::interprocess::read_write
    );

    // Get pointer to shared memory buffer
    int* number = static_cast<int*>(region.get_address());

    // Write the integer
    *number = 1235;

    std::cout << "Writer: wrote 1235 to shared memory\n";
    return 0;
}
