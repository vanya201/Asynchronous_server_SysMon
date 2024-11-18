#include "SvServer.h"
#include "Router.h"
#include "SvSession.h"
#include "SystemService.h"
#include "ViewService.h"

class WebApplication
{
public:
  static void run() 
  {
    std::shared_ptr<Routing::Router> router = std::make_shared<Routing::Router>();
    router->add("/systeminfo", SystemService());
    router->add("/apisystem", ViewService());

    std::shared_ptr<Server> server = std::make_shared<SvServer>("127.0.0.1", 8080, router);
    server->init();

    std::thread server_runtime([server]() { server->runtime(); });
    server_runtime.join();
  }
};

int main () 
{
  WebApplication::run();
}




