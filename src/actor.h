#ifndef SPARKLE_ACTOR_H
#define SPARKLE_ACTOR_H

#include <thread>
#include "bounded_buffer.h"

namespace sparkle {

class Actor {
 public:
  struct Context {
    int32_t id = 0;
    std::string name = "anon";
  };

  Actor(const Context &context,
        const std::function<void(const Context &)> &on_setup,
        const std::function<void(const Context &)> &on_shutdown)
      : context_(context), on_setup_(on_setup), on_shutdown_(on_shutdown) {}

  virtual void Run()= 0;

  virtual void Wait() = 0;

  int32_t id() {
    return context_.id;
  }

  std::string name() {
    return context_.name;
  }

 protected:
  Context context_;
  std::function<void(const Context &)> on_setup_;
  std::function<void(const Context &)> on_shutdown_;
};

}

#endif //SPARKLE_ACTOR_H
