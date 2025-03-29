#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <utility>

namespace Detail {

template <class T>
class Observer;

template <class T>
class Observable;

template <class T>
class Observer {
  using Observable = Observable<T>;
  friend Observable;

public:
  void Unsubscribe() {
    if (!IsSubscribed()) {
      return;
    }
    observable_->detach_(this);
    observable_ = nullptr;
    notify_action_ = nullptr;
  }

  bool IsSubscribed() const {
    return observable_;
  }

  void SetNotifyAction(std::function<void()>&& new_func) {
    notify_action_ = std::move(new_func);
  }

  ~Observer() {
    Unsubscribe();
  }

private:
  void attach_(Observable* observable) {
    observable_ = observable;
  }

  Observable* observable_;
  std::function<void()> notify_action_;
};

template <class T>
class Observable {
  using Observer = Observer<T>;
  friend Observer;

public:
  Observable() = default;

  Observable(const Observable&) = delete;
  Observable& operator=(const Observable&) = delete;
  Observable(Observable&&) noexcept = delete;
  Observable& operator=(Observable&&) noexcept = delete;

  void SubscribeObserver(Observer& observer) {
    if (observer.IsSubscribed()) {

      observer.Unsubscribe();
    }
    observers_.push_back(&observer);
    observer.attach_(this);
  }

  void NotifyAll() const {
    for (auto it : observers_) {
      it->notify_action_();
    }
  }

  ~Observable() {
    while (!observers_.empty()) {
      observers_.front()->Unsubscribe();
    }
  }

private:
  void detach_(Observer* observer) {
    observers_.remove(observer);
  }

  std::list<Observer*> observers_;
};

}  // namespace Detail
