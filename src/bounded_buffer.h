#ifndef SPARKLE_BOUNDED_BUFFER_H_
#define SPARKLE_BOUNDED_BUFFER_H_

#include <boost/circular_buffer.hpp>
#include <mutex>
#include <boost/call_traits.hpp>
#include <condition_variable>

namespace sparkle {

template<typename T>
class bounded_buffer {
 public:

  using buffer_type = boost::circular_buffer<T>;
  using size_type = typename buffer_type::size_type;
  using value_type = typename buffer_type::value_type;
  using reference = typename buffer_type::reference;
  using pointer = typename buffer_type::pointer;
  using rvalue_type = typename buffer_type::rvalue_type;
  using param_type = typename boost::call_traits<value_type>::const_reference;

  explicit bounded_buffer(size_type capacity)
      : unread_num_(0), underlying_buffer_(capacity) {}

  bounded_buffer(bounded_buffer &&other) {
    *this = std::move(other);
  }

  bounded_buffer &operator=(bounded_buffer &&other) {
    unread_num_ = other.unread_num_;
    underlying_buffer_ = std::move(other.underlying_buffer_);
    return *this;
  }

  void push_front(param_type item) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_full_.wait(lock, [this] { return unread_num_ < underlying_buffer_.capacity(); });
      underlying_buffer_.push_front(item);
      ++unread_num_;
    }
    not_empty_.notify_one();
  }

  void push_front(rvalue_type item) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_full_.wait(lock, [this] { return unread_num_ < underlying_buffer_.capacity(); });
      underlying_buffer_.push_front(std::move(item));
      ++unread_num_;
    }
    not_empty_.notify_one();
  }

  value_type pop_back() {
    value_type result;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_empty_.wait(lock, [this] { return unread_num_ > 0; });
      result = std::move(underlying_buffer_[--unread_num_]);
    }
    not_full_.notify_one();
    return result;
  }

 private:
  bounded_buffer(const bounded_buffer &) = delete; // Disabled copy constructor.
  bounded_buffer &operator=(const bounded_buffer &) = delete; // Disabled assign operator.

  size_type unread_num_;
  buffer_type underlying_buffer_;
  std::mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;
};

}

#endif //SPARKLE_BOUNDED_BUFFER_H
