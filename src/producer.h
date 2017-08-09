#ifndef SPARKLE_PRODUCER_H
#define SPARKLE_PRODUCER_H

#include "actor.h"
#include "group.h"
#include "actor_system.h"

namespace sparkle {

class Producer : public Actor {
 public:
  using Context = typename Actor::Context;

  class Builder;

  Producer(const Context &context,
           const std::function<void(const Context &)> &on_setup,
           const std::function<void(const Context &)> &on_shutdown,
           const std::function<void(const Context &)> &on_run)
      : Actor(context, on_setup, on_shutdown), on_run_(on_run) {}

  void Run() {
    thread_ = std::thread([this] {
      on_setup_(context_);
      on_run_(context_);
    });
  }

  void Wait() {
    thread_.join();
  }

 private:
  std::thread thread_;
  std::function<void(const Context &)> on_run_;
};

class Producer::Builder : public Group<Producer>::template Builder<Producer::Builder> {
 public:
  using Context = Actor::Context;
  using BaseBuilder = typename Group<Producer>::template Builder<Producer::Builder>;

  Builder(ActorSystem &actor_system)
      : BaseBuilder(actor_system), on_setup_([](const Context &) {}), on_shutdown_([](const Context &) {}),
        on_run_([](const Context &) {}) {}

  Builder OnSetupWithContext(const std::function<void(const Context &)> &on_setup) {
    on_setup_ = on_setup;
    return *this;
  }

  Builder OnSetup(const std::function<void()> &on_setup) {
    return OnSetupWithContext([on_setup](const Context &) { on_setup(); });
  }

  Builder OnShutdownWithContext(const std::function<void(const Context &)> &on_shutdown) {
    on_shutdown_ = on_shutdown;
    return *this;
  }

  Builder OnShutdown(const std::function<void()> &on_shutdown) {
    return OnShutdownWithContext([on_shutdown](const Context &) { on_shutdown(); });
  }

  Builder OnRunWithContext(const std::function<void(const Context &)> &on_run) {
    on_run_ = on_run;
    return *this;
  }

  Builder OnRun(const std::function<void()> &on_run) {
    return OnRunWithContext([on_run](const Context &) { on_run(); });
  }

  std::shared_ptr<Producer> CreateWithContext(const Context &context) {
    auto producer = std::make_shared<Producer>(Producer(context, on_setup_, on_shutdown_, on_run_));
    Register(producer);
    return producer;
  }

 private:
  std::function<void(const Context &)> on_setup_;
  std::function<void(const Context &)> on_shutdown_;
  std::function<void(const Context &)> on_run_;
};

}

#endif //SPARKLE_PRODUCER_H
