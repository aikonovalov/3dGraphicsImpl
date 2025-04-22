#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <utility>

namespace Detail {

template <class Data, typename ID_Data>
class Observer;

template <class Data, typename ID_Data>
class Observable;

template <class Data, typename ID_Data>
class Observer {
  using Observable = Observable<Data, ID_Data>;
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

  void SetNotifyAction(std::function<void(Data&)>&& new_func) {
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
  std::function<void(Data&)> notify_action_;
};

template <class Data, typename ID_Data>
class Observable {
  using Observer = Observer<Data, ID_Data>;
  friend Observer;

public:
  Observable() = default;

  Observable(const Observable&) = delete;
  Observable& operator=(const Observable&) = delete;
  Observable(Observable&&) noexcept = delete;
  Observable& operator=(Observable&&) noexcept = delete;

  void SubscribeObserver(Observer& observer, ID_Data data) {
    if (observer.IsSubscribed()) {
      observer.Unsubscribe();
    }
    observers_.push_back({&observer, data});
    observer.attach_(this);
  }

  void NotifyAll(Data& data) const {
    for (auto it : observers_) {
      it->first.notify_action_(data);
    }
  }

  void NotifyOne(Observer* observer, Data& data) {

    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
      if (it->first == observer) {
        it->first->notify_action_(data);
        return;
      }
    }
  }

  std::list<std::pair<Observer*, ID_Data>>& GetObserversList() {
    return observers_;
  }

  ID_Data GetDataByID(Observer* observer) {
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
      if (it->first == observer) {
        observers_.erase(it);
      }
    }
  }

  void SetDataByID(Observer* observer, ID_Data new_data) {
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
      if (it->first == observer) {
        it->second = new_data;
      }
    }
  }

  ~Observable() {
    while (!observers_.empty()) {
      observers_.front().first->Unsubscribe();
    }
  }

private:
  void detach_(Observer* observer) {
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
      if (it->first == observer) {
        observers_.erase(it);
        return;
      }
    }
  }

  std::list<std::pair<Observer*, ID_Data>> observers_;
};

}  // namespace Detail
