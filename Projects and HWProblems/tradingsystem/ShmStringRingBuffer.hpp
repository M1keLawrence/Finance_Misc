#ifndef SHM_STRING_RING_BUFFER_HPP
#define SHM_STRING_RING_BUFFER_HPP

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include <cstring>
#include <stdexcept>
#include <string>

namespace bip = boost::interprocess;

template <std::size_t Capacity, std::size_t MsgSize>
struct ShmStringRingBuffer {
  bip::interprocess_mutex mutex;
  bip::interprocess_condition not_empty;
  bip::interprocess_condition not_full;

  std::size_t head = 0;
  std::size_t tail = 0;
  std::size_t count = 0;

  std::size_t len[Capacity];
  char data[Capacity][MsgSize];
};

// Wrapper that maps/owns shared memory
template <std::size_t Capacity, std::size_t MsgSize>
class ShmQueueHandle {
 public:
  using Queue = ShmStringRingBuffer<Capacity, MsgSize>;

  static void Remove(const std::string& name) {
    bip::shared_memory_object::remove(name.c_str());
  }

  // create=true => create and truncate; create=false => open existing
  ShmQueueHandle(const std::string& name, bool create) : name_(name) {
    if (create) {
      bip::shared_memory_object::remove(name.c_str());
      shm_ = bip::shared_memory_object(bip::create_only, name.c_str(), bip::read_write);
      shm_.truncate(sizeof(Queue));
      region_ = bip::mapped_region(shm_, bip::read_write);

      // placement-new init
      void* addr = region_.get_address();
      queue_ = new (addr) Queue();
    } else {
      shm_ = bip::shared_memory_object(bip::open_only, name.c_str(), bip::read_write);
      region_ = bip::mapped_region(shm_, bip::read_write);
      queue_ = static_cast<Queue*>(region_.get_address());
    }
  }

  Queue& Get() { return *queue_; }

  void Push(const std::string& msg) {
    if (msg.size() >= MsgSize) throw std::runtime_error("SHM msg too large");

    bip::scoped_lock<bip::interprocess_mutex> lock(queue_->mutex);
    while (queue_->count == Capacity) queue_->not_full.wait(lock);

    std::size_t i = queue_->tail;
    queue_->len[i] = msg.size();
    std::memcpy(queue_->data[i], msg.data(), msg.size());
    queue_->data[i][msg.size()] = '\0';

    queue_->tail = (queue_->tail + 1) % Capacity;
    ++queue_->count;
    queue_->not_empty.notify_one();
  }

  std::string Pop() {
    bip::scoped_lock<bip::interprocess_mutex> lock(queue_->mutex);
    while (queue_->count == 0) queue_->not_empty.wait(lock);

    std::size_t i = queue_->head;
    std::string msg(queue_->data[i], queue_->len[i]);

    queue_->head = (queue_->head + 1) % Capacity;
    --queue_->count;
    queue_->not_full.notify_one();
    return msg;
  }

 private:
  std::string name_;
  bip::shared_memory_object shm_;
  bip::mapped_region region_;
  Queue* queue_ = nullptr;
};

#endif
