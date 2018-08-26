#ifndef SPARKLE_ACTOR_BUILDER_H
#define SPARKLE_ACTOR_BUILDER_H

#include "producer.h"
#include "stateful_producer.h"
#include "reactor.h"
#include "stateful_reactor.h"

namespace sparkle {

    Producer::Builder producer(ActorSystem &actor_system) {
        return {actor_system};
    }

    template<typename S>
    typename StatefulProducer<S>::Builder producer(ActorSystem &actor_system) {
        return {actor_system};
    }

    template<typename T>
    typename Reactor<T>::Builder reactor(ActorSystem &actor_system) {
        return {actor_system};
    }

    template<typename T, typename S>
    typename StatefulReactor<T, S>::Builder reactor(ActorSystem &actor_system) {
        return {actor_system};
    }

}

#endif //SPARKLE_ACTOR_BUILDER_H
