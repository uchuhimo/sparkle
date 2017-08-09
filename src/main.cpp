#include <iostream>
#include <thread>
#include <boost/progress.hpp>
#include "bounded_buffer.h"
#include "reactor.h"
#include "producer.h"
#include "stateful_producer.h"
#include "stateful_reactor.h"
#include "actor_builder.h"

const int64_t QUEUE_SIZE = 1000L;
const int64_t TOTAL_ELEMENTS = QUEUE_SIZE * 1000L;

template<typename T>
void check_class() {
  std::cout << std::boolalpha
            << "is move-assignable? "
            << std::is_move_assignable<T>::value << '\n'
            << "is nothrow move-assignable? "
            << std::is_nothrow_move_assignable<T>::value << '\n'
            << "is trivally move-assignable? "
            << std::is_trivially_move_assignable<T>::value << '\n'
            << "is copy-assignable? "
            << std::is_copy_assignable<T>::value << '\n'
            << "is nothrow copy-assignable? "
            << std::is_nothrow_copy_assignable<T>::value << '\n'
            << "is trivally copy-assignable? "
            << std::is_trivially_copy_assignable<T>::value << '\n';
}

struct Message {
  int64_t data;

  Message() {
//    std::cout << "default construct" << std::endl;
  }

  Message(int64_t _data) : data(_data) {
//    std::cout << "construct from data: " << data << std::endl;
  }

  ~Message() {
//    std::cout << "destruct" << std::endl;
  }
};

void test_long() {
  sparkle::ActorSystem actor_system;
  auto &&consumer = sparkle::reactor<int64_t>(actor_system)
      .MailboxSize(QUEUE_SIZE)
      .OnReceive(
          [](int64_t &x) {
//            std::cout << x << std::endl;
          })
      .Create();
  auto &&producer = sparkle::producer(actor_system)
      .OnRun(
          [&consumer] {
            for (int64_t i = 0L; i < TOTAL_ELEMENTS; ++i) {
              consumer->Send(i + 2);
            }
          })
      .Create();
  actor_system.Start();
}

void test_unique_ptr() {
  sparkle::ActorSystem actor_system;
  auto &&consumer = sparkle::reactor<std::unique_ptr<Message>>(actor_system)
      .MailboxSize(QUEUE_SIZE)
      .OnReceive(
          [](std::unique_ptr<Message> &x) {
//            std::cout << x->data << std::endl;
          })
      .Create();
  auto &&producer = sparkle::producer(actor_system)
      .OnRun(
          [&consumer] {
            for (int64_t i = 0L; i < TOTAL_ELEMENTS; ++i) {
              consumer->Send(std::unique_ptr<Message>(new Message(i + 2)));
            }
          })
      .Create();
  actor_system.Start();
}

void test_shared_ptr() {
  sparkle::ActorSystem actor_system;
  auto &&consumer = sparkle::reactor<std::shared_ptr<Message>>(actor_system)
      .MailboxSize(QUEUE_SIZE)
      .OnReceive(
          [](std::shared_ptr<Message> &x) {
//            std::cout << x->data << std::endl;
          })
      .Create();
  auto &&producer = sparkle::producer(actor_system)
      .OnRun(
          [&consumer] {
            for (int64_t i = 0L; i < TOTAL_ELEMENTS; ++i) {
              consumer->Send(std::make_shared<Message>(i + 2));
            }
          })
      .Create();
  actor_system.Start();
}

struct NoDefaultConstructor {
//  NoDefaultConstructor(int x) : x(x) {}

  int x;
};

void test_stateful_producer() {
  sparkle::ActorSystem actor_system;
  auto &&stateful_producer = sparkle::producer<NoDefaultConstructor>(actor_system).Create();
}

void test_stateful_reactor() {
  sparkle::ActorSystem actor_system;
  auto &&stateful_reactor = sparkle::reactor<int32_t, NoDefaultConstructor>(actor_system).Create();
}

struct State {
  int64_t counter;
};

void test_stateful() {
  sparkle::ActorSystem actor_system;
  auto &&consumer = sparkle::reactor<std::unique_ptr<Message>, State>(actor_system)
      .MailboxSize(QUEUE_SIZE)
      .OnReceive(
          [](std::unique_ptr<Message> &message, State &state) {
            state.counter += message->data;
            if (state.counter % 100000 == 0) {
              std::cout << state.counter << std::endl;
            }
          })
      .Create();
  auto &&producer = sparkle::producer<int32_t>(actor_system)
      .OnRun(
          [&consumer](int32_t &state) {
            for (int64_t i = 0L; i < TOTAL_ELEMENTS; ++i) {
              consumer->Send(std::unique_ptr<Message>(new Message(i)));
            }
          })
      .Create();
  actor_system.Start();
}

void test_group() {
  sparkle::ActorSystem actor_system;
  auto &&consumers = sparkle::reactor<std::unique_ptr<Message>>(actor_system)
      .MailboxSize(2)
      .OnReceiveWithContext(
          [](std::unique_ptr<Message> &x, const sparkle::Actor::Context context) {
            std::cout << context.id << ":" << x->data << std::endl;
          })
      .CreateGroup(2);
  auto &&producers = sparkle::producer(actor_system)
      .OnRun(
          [&consumers] {
            for (int64_t i = 0L; i < 6; ++i) {
              consumers.Get(i % consumers.size())->Send(std::unique_ptr<Message>(new Message(i)));
            }
          })
      .CreateGroup(4);
  std::cout << producers.Get(3)->name() << std::endl;
  actor_system.Start();
}

int main() {
  boost::progress_timer progress;

//  check_class<std::vector<int>>();

//  test_long();
//  test_unique_ptr();
//  test_shared_ptr();
  test_stateful();
//  test_group();

  return 0;
}
