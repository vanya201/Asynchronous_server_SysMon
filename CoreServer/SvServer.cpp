#include "SvServer.h"
#include "SvSession.h"

Net::SvServer::SvServer(std::string ip, uint16_t port, const std::shared_ptr<Routing::Router>& router)
  : 
  m_asioContext(std::make_shared<asio::io_context>()),
  m_asioAcceptor(*m_asioContext, 
    asio::ip::tcp::endpoint(asio::ip::make_address_v4(ip),  port)),
  m_queue(std::make_shared<BlockPopSharedQueue>(128)),
  m_router(router)
{
}


void Net::SvServer::init()
{
   accept();
   m_threadContext = std::thread([this]() { m_asioContext->run(); });
}


void Net::SvServer::accept()
{
  m_asioAcceptor.async_accept(
    [this](beast::error_code ec, asio::ip::tcp::socket socket) {
    if (!ec)
    {
      if (allow_connect(socket))
      {
        std::shared_ptr<Session> session = std::make_shared<SvSession>(m_asioContext, std::move(socket), m_queue);
        session->listen();
        sessions.emplace_back(session);
      }
    }
    accept();
   }
  );
}


bool Net::SvServer::allow_connect(const asio::ip::tcp::socket& socket)
{
  return true;
}


void Net::SvServer::runtime()
{
  
  asio::steady_timer timer(*m_asioContext, asio::chrono::seconds(time_seconds_clear));
  timer.async_wait(bind(&SvServer::clear_sessions, this, asio::placeholders::error, &timer));

  while (done)
  {
    std::pair<std::shared_ptr<Session>, std::shared_ptr<Request>>* pair_accepted = nullptr;
    if (m_queue->pop(pair_accepted))
    {
      if (pair_accepted != nullptr)
      {
        std::shared_ptr<Session> session = pair_accepted->first;
        std::shared_ptr<Request> request = pair_accepted->second;
        std::shared_ptr<Routing::Transact> transact = std::make_shared<Routing::RoutTransact>(request->target());

        try {
          m_router->route(session, request, transact);
        }
        catch(const std::runtime_error& err)
        {

        }

        delete pair_accepted;
      }
    }
  }

}


void Net::SvServer::set_router(const std::shared_ptr<Routing::Router>& router)
{
  this->m_router = router;
}


void Net::SvServer::stop()
{
  done = false;
  m_asioContext->stop();
  m_queue->notify_stop();
  if (m_threadContext.joinable())
    m_threadContext.join();
}


void Net::SvServer::clear_sessions(const system::error_code& err, asio::steady_timer* timer) 
{
  sessions.erase(std::remove_if(sessions.begin(), sessions.end(), [](const std::shared_ptr<Session>& session) {
    return !session->isConnect();
  }), sessions.end());

  timer->expires_at(timer->expiry() + asio::chrono::seconds(time_seconds_clear));
  timer->async_wait(bind(&SvServer::clear_sessions, this, asio::placeholders::error, timer));
}