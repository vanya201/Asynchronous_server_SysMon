#pragma once
#include "Common.h"
#include "Queue.h"
#include "Router.h"

namespace Net
{
  class Session;
  class Message;

   typedef std::pair<std::shared_ptr<Session>, std::shared_ptr<Request>>  transact_queue;
   using SharedQueue = Queue<transact_queue*>;
   using BlockPopSharedQueue = BlockPopQueue<transact_queue*>;


    class INTERFACE_COMPONENT Server 
    {
    public:
      virtual ~Server() = default;
    
    public:
      virtual void init() noexcept(false) = 0;
      virtual void stop() = 0;
      virtual void runtime() = 0;
    };



    class SERVER_COMPONENT SvServer : public Server
    {
    public:
      SvServer(std::string ip, uint16_t port, const std::shared_ptr<Routing::Router>&);

    public:
      virtual void init() override;
      virtual void stop() override;
      virtual void runtime() override;

    public:
      void set_router(const std::shared_ptr<Routing::Router>&);

    private:
      void accept();
      bool allow_connect(const asio::ip::tcp::socket& socket );
      void clear_sessions(const system::error_code& err, asio::steady_timer* timer);

    private:
      std::shared_ptr<SharedQueue> m_queue;
      std::shared_ptr<Routing::Router> m_router;
      volatile bool done = true;

      std::shared_ptr<asio::io_context> m_asioContext;
      asio::ip::tcp::acceptor m_asioAcceptor;
      std::thread m_threadContext;

      std::list<std::shared_ptr<Session>> sessions;
      const uint64_t time_seconds_clear = calculate_time_seconds_clear<100000>();


      template<uint64_t in_speed>
      constexpr uint64_t calculate_time_seconds_clear() {
        return in_speed / 60 * 60;
      }

    };


};


