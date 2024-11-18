#include "Router.h"
#include "SvSession.h"

Net::Routing::RoutTransact::RoutTransact()
{
}


Net::Routing::RoutTransact::RoutTransact(const std::string& path)
  :
  m_path(path)
{
}


void Net::Routing::RoutHandle::forward_transact(const std::shared_ptr<Transact>& transact)
{
  this->m_transact = transact;
}


void Net::Routing::RoutTransact::next_path() noexcept(false)
{
  size_t size = current_path().length();
  m_path.erase(0, size);
}


std::string Net::Routing::RoutTransact::current_path() const noexcept(false)
{
  const std::regex pattern("(^/[^/]+)");
  std::smatch pmatch;

  if (!std::regex_search(m_path, pmatch, pattern))
    throw std::runtime_error("No correct patch : " + m_path);

  return pmatch[1];
}


Net::Routing::Router::Router(const Router& router) : m_router_map(router.m_router_map)
{
}

Net::Routing::Router::Router(Router&& router) noexcept(true) : m_router_map(std::move(router.m_router_map))
{
}

Net::Routing::Router::FuncHandle::FuncHandle(const Function& callback, const std::shared_ptr<RoutHandle>& handle) :
  m_callback(callback),
  m_next_handle(handle)
{
}

void Net::Routing::Router::FuncHandle::route(const std::shared_ptr<Session>& session, const std::shared_ptr<Request>& message)
{
  m_next_handle->forward_transact(m_transact);
  m_callback(session, message, m_next_handle);
}

void Net::Routing::Router::EndHandle::route(const std::shared_ptr<Session>& session, const std::shared_ptr<Request>& message)
{
  throw std::runtime_error("End Handle");
}

void Net::Routing::Router::route(const std::shared_ptr<Session>& session, 
  const std::shared_ptr<Request>& request,
  const std::shared_ptr<Transact>& transact) noexcept(false)
{
  forward_transact(transact);
  route(session, request);
}

void Net::Routing::Router::remove(const std::string& key) noexcept(true)
{
  std::unique_lock<std::mutex> ul(m_mutex);
  m_router_map.erase(key);
  ul.unlock();
}

void Net::Routing::Router::route(const std::shared_ptr<Session>& session, const std::shared_ptr<Request>& request) noexcept(false)
{
  std::unique_lock<std::mutex> ul(m_mutex);
  std::string current_path;
  try {
    current_path = m_transact->current_path();
    m_transact->next_path();
  }
  catch (const std::runtime_error& err) {
    throw err;
  }

  const auto it = m_router_map.find(current_path);

  if (it != m_router_map.end())
  {
    it->second->forward_transact(m_transact);
    it->second->route(session, request);
  }

  ul.unlock();
}

std::shared_ptr<Net::Routing::RoutHandle> Net::Routing::Router::create_handle(const std::shared_ptr<Router>& router) {
  return router;
}


