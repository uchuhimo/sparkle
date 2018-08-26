#ifndef SPARKLE_STATEFUL_REACTOR_H
#define SPARKLE_STATEFUL_REACTOR_H

#include "reactor.h"

namespace sparkle {

    template<typename T, typename S>
    class StatefulReactor : public Reactor<T> {
    public:
        static_assert(std::is_default_constructible<S>::value,
                      "State in StatefulReactor must have a default constructor");

        using Context = typename Actor::Context;

        class Builder : public Group<StatefulReactor<T, S>>::template Builder<Builder> {
        public:
            using size_type = typename StatefulReactor<T, S>::size_type;
            using BaseBuilder = typename Group<StatefulReactor<T, S>>::template Builder<Builder>;

            Builder(ActorSystem &actor_system)
                    : BaseBuilder(actor_system), mailbox_size_{0},
                      on_setup_([](S &, const Context &) {}),
                      on_shutdown_([](S &, const Context &) {}),
                      on_receive_([](T &, S &, const Context &) {}) {}

            Builder MailboxSize(size_type mailbox_size) {
                mailbox_size_ = mailbox_size;
                return *this;
            }

            Builder OnSetupWithContext(const std::function<void(S &, const Context &)> &on_setup) {
                on_setup_ = on_setup;
                return *this;
            }

            Builder OnSetup(const std::function<void(S &)> &on_setup) {
                return OnSetupWithContext(
                        [on_setup](S &state, const Context &context) { on_setup(state); });
            }

            Builder
            OnShutdownWithContext(const std::function<void(S &, const Context &)> &on_shutdown) {
                on_shutdown_ = on_shutdown;
                return *this;
            }

            Builder OnShutdown(const std::function<void(S &)> &on_shutdown) {
                return OnShutdownWithContext(
                        [on_shutdown](S &state, const Context &context) { on_shutdown(state); });
            }

            Builder
            OnReceiveWithContext(const std::function<void(T &, S &, const Context &)> &on_receive) {
                on_receive_ = on_receive;
                return *this;
            }

            Builder OnReceive(const std::function<void(T &, S &)> &on_receive) {
                return OnReceiveWithContext(
                        [on_receive](T &message, S &state, const Context &context) {
                            on_receive(message, state);
                        });
            }

            std::shared_ptr<StatefulReactor> CreateWithContext(const Context &context) {
                assert(mailbox_size_ > 0);
                auto reactor = std::make_shared<StatefulReactor>(
                        StatefulReactor(context, mailbox_size_, on_setup_, on_shutdown_,
                                        on_receive_));
                this->Register(reactor);
                return reactor;
            }

        private:
            size_type mailbox_size_;
            std::function<void(S &, const Context &)> on_setup_;
            std::function<void(S &, const Context &)> on_shutdown_;
            std::function<void(T &, S &, const Context &)> on_receive_;
        };

        StatefulReactor(const Context &actor_context,
                        typename Reactor<T>::size_type mailbox_size,
                        const std::function<void(S &, const Context &)> &on_setup,
                        const std::function<void(S &, const Context &)> &on_shutdown,
                        const std::function<void(T &, S &, const Context &)> &on_receive)
                : Reactor<T>(actor_context,
                             mailbox_size,
                             [this, on_setup](const Context &context) {
                                 on_setup(state_, context);
                             },
                             [this, on_shutdown](const Context &context) {
                                 on_shutdown(state_, context);
                             },
                             [this, on_receive](T &message, const Context &context) {
                                 on_receive(message, state_, context);
                             }),
                  state_(S()) {
        }

    private:
        S state_;

    };

}

#endif //SPARKLE_STATEFUL_REACTOR_H
