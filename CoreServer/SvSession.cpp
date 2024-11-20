#include "SvSession.h"
#include "Router.h"
#include <boost/endian/conversion.hpp>

Net::SvSession::SvSession(const std::shared_ptr<asio::io_context>& asioContext, 
  asio::ip::tcp::socket socket, 
  const std::shared_ptr<SharedQueue>& queue)
  :
  m_asioContext(asioContext),
  m_socket(std::move(socket)),
  m_queue(queue)
{
  
}

void Net::SvSession::send(std::shared_ptr<Response> response)
{
  if (isConnect()) 
  {
    http::async_write(
      m_socket, *response,
      [self = shared_from_this(), response](beast::error_code ec, std::size_t) {
      if (ec)
      {
        self->disconect();
      }
    });
  }
}

bool Net::SvSession::isConnect() const
{
  return m_socket.is_open();
}

void Net::SvSession::disconect()
{
  if (isConnect())
    asio::post(*m_asioContext, [self = shared_from_this()]() { self->m_socket.close(); });
}

void Net::SvSession::listen()
{
  if (isConnect())
    read_request();
}


void Net::SvSession::read_request()
{
  std::shared_ptr<Request> request_ = std::make_shared<Request>();
  http::async_read(
    m_socket, buffer_, *request_,
    [self = shared_from_this(), request_](beast::error_code ec, std::size_t a_size) {
    if (!ec) {
        transact_queue* pair_accept = new transact_queue(self, request_);
        while (!self->m_queue->push(pair_accept));
    }
    self->listen();
  });
}



