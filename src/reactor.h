#ifndef SPARKLE_REACTOR_H
#define SPARKLE_REACTOR_H

#include "actor.h"
#include "group.h"

namespace sparkle {

    template<typename T>
    class Reactor : public Actor {
    public:
        using Context = typename Actor::Context;

        class Builder : public Group<Reactor<T>>::template Builder<Builder> {
        public:
            using size_type = typename Reactor::size_type;
            using BaseBuilder = typename Group<Reactor<T>>::template Builder<Builder>;

            Builder(ActorSystem &actor_system) : BaseBuilder(actor_system), mailbox_size_{0},
                                                 on_setup_([](const Context &) {}),
                                                 on_shutdown_([](const Context &) {}),
                                                 on_receive_([](T &, const Context &) {}) {}

            Builder MailboxSize(size_type mailbox_size) {
                mailbox_size_ = mailbox_size;
                return *this;
            }

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

            Builder
            OnReceiveWithContext(const std::function<void(T &, const Context &)> &on_receive) {
                on_receive_ = on_receive;
                return *this;
            }

            Builder OnReceive(const std::function<void(T &)> &on_receive) {
                return OnReceiveWithContext(
                        [on_receive](T &message, const Context &) { on_receive(message); });
            }

            std::shared_ptr<Reactor> CreateWithContext(const Context &context) {
                assert(mailbox_size_ > 0);
                auto reactor = std::make_shared<Reactor>(
                        Reactor(context, mailbox_size_, on_setup_, on_shutdown_, on_receive_));
                this->Register(reactor);
                return reactor;
            }

        private:
            size_type mailbox_size_;
            std::function<void(const Context &)> on_setup_;
            std::function<void(const Context &)> on_shutdown_;
            std::function<void(T &, const Context &)> on_receive_;
        };

        using size_type = typename bounded_buffer<T>::size_type;

        Reactor(const Context &context,
                size_type mailbox_size,
                const std::function<void(const Context &)> &on_setup,
                const std::function<void(const Context &)> &on_shutdown,
                const std::function<void(T &, const Context &)> &on_receive)
                : Actor(context, on_setup, on_shutdown), mailbox_(mailbox_size),
                  on_receive_(on_receive) {}

        void Run() override {
            thread_ = std::thread([this] {
                on_setup_(context_);
                while (true) {
                    auto message = this->mailbox_.pop_back();
                    on_receive_(message, context_);
                }
            });
        }

        void Wait() override {
            thread_.join();
        }

        void Send(const T &message) {
            mailbox_.push_front(message);
        }

        void Send(T &&message) {
            mailbox_.push_front(std::move(message));
        }

    protected:
        std::function<void(T &, const Context &)> on_receive_;

    private:
        std::thread thread_;
        bounded_buffer<T> mailbox_;
    };

}

#endif //SPARKLE_REACTOR_H
