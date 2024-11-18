#include "SvServer.h"
#include "Router.h"
#include "SvSession.h"
#include "SystemInfo.h"

using namespace Net;
using namespace Routing;

class SystemService
{
public:
  SystemService() = default;

public:
  void operator()(const std::shared_ptr<Session>& session,
    const std::shared_ptr<Request>& request,
    const std::shared_ptr<RoutHandle>& handle)
  {
    const SystemInfo::Info systemInfo = m_sysInfo->get_system_usage();

    std::ostringstream systemInfoStream;
    systemInfoStream << "CPU Usage: " << systemInfo.cpuUsage << "%"
      << "; Memory Usage: " << systemInfo.memoryUsage << " MB"
      << "; Disk Usage:" << systemInfo.diskUsage;

    std::shared_ptr<Response> response = std::make_shared<Response>(
      http::status::ok, request->version());
    response->set(http::field::server, "Boost.Beast Server");
    response->set(http::field::content_type, "text/plain");
    response->body() = systemInfoStream.str();
    response->prepare_payload();

    session->send(response);
  }

private:
  std::shared_ptr<SystemInfo> m_sysInfo = std::make_shared<SystemInfo>();
};
