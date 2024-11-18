#pragma once
#include "Common.h"
#include "SvServer.h"
#include "Queue.h"


namespace Net
{
  class INTERFACE_COMPONENT Session {
  public:
    virtual ~Session() = default;

  public:
    virtual void send(std::shared_ptr<Response> response) = 0;
    virtual bool isConnect() const = 0;
    virtual void disconect() = 0;
    virtual void listen() = 0;
  };




  class SERVER_COMPONENT SvSession : public Session,
    public std::enable_shared_from_this<SvSession> {

  public:
    SvSession(const std::shared_ptr<asio::io_context>& asioContext,
      asio::ip::tcp::socket socket,
      const std::shared_ptr<SharedQueue>& queue);

  public:
    void send(std::shared_ptr<Response> response) override;
    void listen() override;

  public:
    bool isConnect() const override;
    void disconect() override;

  private:
    void read_request();

  private:
    asio::ip::tcp::socket m_socket;
    std::shared_ptr<asio::io_context> m_asioContext;
    std::shared_ptr<SharedQueue> m_queue;
    beast::flat_buffer buffer_;
  };

};
