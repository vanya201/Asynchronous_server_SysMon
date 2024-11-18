#pragma once
#include "Common.h"
#include "boost/lockfree/queue.hpp"
#include <boost/thread.hpp>

namespace Net
{
    template<typename T>
    class INTERFACE_COMPONENT Queue
    {
    public:
      virtual ~Queue() = default;
      virtual bool pop(T&) noexcept(false) = 0;
      virtual bool push(const T&) = 0;
      virtual void notify_stop() = 0;
    };



    template<typename T>
    class SERVER_COMPONENT BlockPopQueue : public Queue<T>
    {
    public:
      BlockPopQueue(uint64_t size);

      virtual bool pop(T&) noexcept(false) override;
      virtual bool push(const T&) override;
      virtual void notify_stop() override;

    private:
      lockfree::queue<T> queue;
      std::condition_variable consumer;
      std::mutex mutex;
      volatile bool done = true;
    };



    template<typename T>
    inline BlockPopQueue<T>::BlockPopQueue(uint64_t size) 
      :
      queue(size)
    {
    }

    template<typename T>
    inline bool BlockPopQueue<T>::push(const T& elem)
    {
      bool state = queue.push(elem);
      if (state) {
        consumer.notify_one();
      }
      return state;
    }

    template<typename T>
    inline void BlockPopQueue<T>::notify_stop()
    {
      done = false;
      consumer.notify_all();
    }

    template<typename T>
    inline bool BlockPopQueue<T>::pop(T& elem)
    {
      std::unique_lock<std::mutex> lock(mutex);
      consumer.wait(lock, [this] { return !done || !queue.empty(); });
      lock.unlock();

      return queue.pop(elem);
    }


};


