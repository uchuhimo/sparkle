#ifndef SPARKLE_ACTOR_SYSTEM_H
#define SPARKLE_ACTOR_SYSTEM_H

#include "actor.h"

namespace sparkle {

    class ActorSystem {
    public:
        template<typename T>
        void Register(const std::shared_ptr<T> &actor) {
            actors_.push_back(actor);
        }

        void Start() {
            for (auto &&actor : actors_) {
                actor->Run();
            }
            for (auto &&actor : actors_) {
                actor->Wait();
            }
        }

    private:
        std::vector<std::shared_ptr<Actor>> actors_;
    };

}

#endif //SPARKLE_ACTOR_SYSTEM_H
