#pragma once

#if defined(_MSC_VER)
#define SERVER_COMPONENT __declspec(dllexport)
#define INTERFACE_COMPONENT __declspec(novtable)
#elif defined(__GNUC__)
#define SERVER_COMPONENT __attribute__((visibility("default")))
#define INTERFACE_COMPONENT
#else
#define SERVER_COMPONENT
#define INTERFACE_COMPONENT
#endif

#define POLITIC_REQUEST_SIZE 40 * 1000

#include <unordered_set>
#include <unordered_map>  
#include <functional>
#include <regex>
#include <mutex>
#include <list>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>


namespace Net {
  namespace beast = boost::beast;
  namespace http = beast::http;
  using Response = http::response<http::string_body>;
  using Request = http::request<http::string_body>;
};

using namespace boost;