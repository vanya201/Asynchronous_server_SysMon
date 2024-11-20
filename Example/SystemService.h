#include "SvServer.h"
#include "Router.h"
#include "SvSession.h"
#include "SystemInfo.h"
#include "boost/thread.hpp"

using namespace Net;
using namespace Routing;

class SystemService : public std::enable_shared_from_this<SystemService>
{
public:
  SystemService() = default;

private:
  std::string GetSystemInfo()
  {
    const SystemInfo::Info systemInfo = m_sysInfo->get_system_usage();

    std::ostringstream systemInfoStream;
    systemInfoStream << "CPU Usage: " << systemInfo.cpuUsage << "%"
      << "; Memory Usage: " << systemInfo.memoryUsage << " MB"
      << "; Disk Usage:" << systemInfo.diskUsage;

    return systemInfoStream.str();
  }

public:
  void operator()(const std::shared_ptr<Session>& session,
    const std::shared_ptr<Request>& request,
    const std::shared_ptr<RoutHandle>& handle)
  {

    boost::thread([session, request, self = shared_from_this()]()
    {
      std::string sysinfo = self->GetSystemInfo();

      std::shared_ptr<Response> response = std::make_shared<Response>(
        http::status::ok, request->version());
      response->set(http::field::server, "Boost.Beast Server");
      response->set(http::field::content_type, "text/plain");
      response->body() = sysinfo;
      response->prepare_payload();

      session->send(response);
    }).detach();
  }

private:
  std::shared_ptr<SystemInfo> m_sysInfo = std::make_shared<SystemInfo>();
};
