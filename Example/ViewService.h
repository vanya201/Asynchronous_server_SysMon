#include "SvServer.h"
#include "Router.h"
#include "SvSession.h"
#include <fstream>
#include <sstream>
#include <string>
using namespace Net;
using namespace Routing;

class ViewService
{
public:
  ViewService() = default;

  void operator()(const std::shared_ptr<Session>& session, const std::shared_ptr<Request>& request, 
    const std::shared_ptr<RoutHandle>& handle)
  {
    std::ifstream file(m_path_to_html);
    if (!file.is_open()) 
      return;

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    std::shared_ptr<Response> response = std::make_shared<Response>(
      http::status::ok, request->version());
    response->set(http::field::server, "Boost.Beast Server");
    response->set(http::field::content_type, "html");
    response->body() = content;
    response->prepare_payload();

    session->send(response);
  }

private:
  const std::string m_path_to_html = "D://c++/Clonium/Example/index.html" ;
};