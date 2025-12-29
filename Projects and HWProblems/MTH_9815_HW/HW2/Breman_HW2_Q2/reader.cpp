#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

int main() {
    // Open shared memory created by writer
    boost::interprocess::shared_memory_object shm(
        boost::interprocess::open_only,
        "MySharedMemory",
        boost::interprocess::read_only
    );

    // Map region
    boost::interprocess::mapped_region region(
        shm,
        boost::interprocess::read_only
    );

    // Read value
    int* number = static_cast<int*>(region.get_address());

    std::cout << "Reader: read value = " << *number << "\n";
    return 0;
}
