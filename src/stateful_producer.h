#ifndef SPARKLE_STATEFUL_PRODUCER_H
#define SPARKLE_STATEFUL_PRODUCER_H

#include "producer.h"

namespace sparkle {

    template<typename S>
    class StatefulProducer : public Producer {
    public:
        static_assert(std::is_default_constructible<S>::value,
                      "State in StatefulProducer must have a default constructor");

        using Context = typename Actor::Context;

        class Builder : public Group<StatefulProducer<S>>::template Builder<Builder> {
        public:
            using BaseBuilder = typename Group<StatefulProducer<S>>::template Builder<Builder>;

            Builder(ActorSystem &actor_system) : BaseBuilder(actor_system),
                                                 on_setup_([](S &, const Context &) {}),
                                                 on_shutdown_([](S &, const Context &) {}),
                                                 on_run_([](S &, const Context &) {}) {}

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

            Builder OnRunWithContext(const std::function<void(S &, const Context &)> &on_run) {
                on_run_ = on_run;
                return *this;
            }

            Builder OnRun(const std::function<void(S &)> &on_run) {
                return OnRunWithContext(
                        [on_run](S &state, const Context &context) { on_run(state); });
            }

            std::shared_ptr<StatefulProducer> CreateWithContext(const Context &context) {
                auto producer = std::make_shared<StatefulProducer>(
                        StatefulProducer(context, on_setup_, on_shutdown_, on_run_));
                this->Register(producer);
                return producer;
            }

        private:
            std::function<void(S &, const Context &)> on_setup_;
            std::function<void(S &, const Context &)> on_shutdown_;
            std::function<void(S &, const Context &)> on_run_;
        };

        StatefulProducer(const Context &actor_context,
                         const std::function<void(S &, const Context &)> &on_setup,
                         const std::function<void(S &, const Context &)> &on_shutdown,
                         const std::function<void(S &, const Context &)> &on_run)
                : Producer(actor_context,
                           [this, on_setup](const Context &context) { on_setup(state_, context); },
                           [this, on_shutdown](const Context &context) {
                               on_shutdown(state_, context);
                           },
                           [this, on_run](const Context &context) { on_run(state_, context); }),
                  state_(S()) {}

    private:
        S state_;

    };

}

#endif //SPARKLE_STATEFUL_PRODUCER_H
