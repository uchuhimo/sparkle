#ifndef SPARKLE_GROUP_H
#define SPARKLE_GROUP_H

#include <vector>
#include "actor.h"
#include "actor_system.h"

namespace sparkle {

template<typename T>
class Group {
 public:
  static_assert(std::is_base_of<Actor, T>::value, "Group can only contain Actor");

  template<typename Self>
  class Builder {
   public:
    virtual std::shared_ptr<T> CreateWithContext(const Actor::Context &context) = 0;

    std::shared_ptr<T> Create() {
      return CreateWithContext({});
    }

    Group<T> CreateGroup(std::size_t size) {
      std::vector<std::shared_ptr<T>> actors;
      for (int32_t i = 0; i < size; ++i) {
        actors.push_back(CreateWithContext({i, context_.name + "-" + std::to_string(i)}));
      }
      return {actors};
    }

    Self Id(int32_t id) {
      context_.id = id;
      return *this;
    }

    Self Name(std::string name) {
      context_.name = name;
      return *this;
    }

   protected:
    Builder(ActorSystem &actor_system) : actor_system_(actor_system) {}

    void Register(std::shared_ptr<T> actor) {
      actor_system_.Register(actor);
    }

   private:
    ActorSystem &actor_system_;
    Actor::Context context_;
  };

  Group(const std::vector<std::shared_ptr<T>> &actors) : size_(actors.size()), actors_(actors) {}

  std::shared_ptr<T> Get(size_t id) {
    assert(id >= 0 && id < size_);
    return actors_[id];
  }

  size_t size() {
    return size_;
  }

 private:
  size_t size_;
  std::vector<std::shared_ptr<T>> actors_;
};

}

#endif //SPARKLE_GROUP_H
