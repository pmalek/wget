#include "Downloader.h"

#include <iostream>

#include <boost/asio/ip/tcp.hpp>

#include <beast/http/empty_body.hpp>
#include <beast/http/read.hpp>
#include <beast/http/write.hpp>

using namespace std::placeholders;  // for _1, _2 etc.
using namespace boost::asio;
using namespace boost::asio::ip;

Downloader::Downloader(const std::vector<std::string> &urls)
    : tcp_socket(ioservice), resolv(ioservice) {
  endpoints.reserve(urls.size());
  // TODO create a class for exctracting host:url pair from string url
  for(const auto &url : urls) {
    std::string host = url;
    auto it = host.find("http://");
    if(it != std::string::npos) host.erase(it, 7);
    it = host.find("www.");
    if(it != std::string::npos) host.erase(it, 4);

    it = host.find_first_of('/');

    if(it == std::string::npos) {
      endpoints.emplace_back(host, "/");
    } else {
      endpoints.emplace_back(std::string(host, 0, it), std::string(host, it));
    }
  }
}

void Downloader::go() {
  for(const auto &endpoint : endpoints) {
    tcp::resolver::query q{endpoint.host, "80"};
    resolv.async_resolve(q, std::bind(&Downloader::resolve_handler, this, _1, _2, endpoint));
  }

  ioservice.run();
}

void Downloader::read_handler(const boost::system::error_code &ec) {
  if(ec) {
    ioservice.post(
        [=]() { std::cerr << "Problem reading the response: \"" << ec.message() << "\"\n"; });
    return;
  }

  std::cout << "\n*******************************" << '\n';
  std::cout << "Headers\n\n";
  for(auto i : resp.headers) std::cout << i.first << " : " << i.second << '\n';
  std::cout << "*******************************"
            << "\n\n";

  if(resp.status != 200) {
    std::cout << "\n\nReceived error code: " << resp.status << '\n';
    std::cout << "Unfortunately this is not yet supported :(" << '\n';
    return;
  }

  std::cout << "*******************************" << '\n';
  std::cout << "Response body\n\n" << resp.body;
}

void Downloader::connect_handler(const boost::system::error_code &ec, const Endpoint &endpoint) {
  if(ec) return;

  // Send HTTP request using beast
  beast::http::request_v1<beast::http::empty_body> req;
  req.method = "GET";
  req.url = endpoint.url;
  req.version = 11;
  req.headers.insert("Host", endpoint.host);
  req.headers.insert("User-Agent", "Beast");
  beast::http::prepare(req);

  std::cout << "*******************************" << '\n';
  std::cout << "Sending a request\n\n" << req;
  std::cout << "*******************************" << '\n';

  beast::http::async_write(
      tcp_socket, std::move(req), std::bind(&Downloader::queue_read, this, _1));
}

void Downloader::queue_read(const boost::system::error_code &ec) {
  if(ec) {
    ioservice.post(
        [=]() { std::cerr << "Problem reading the response: \"" << ec.message() << "\"\n"; });
    return;
  }

  beast::http::async_read(
      tcp_socket, response_streambuf, resp, std::bind(&Downloader::read_handler, this, _1));
};

void Downloader::resolve_handler(const boost::system::error_code &ec, tcp::resolver::iterator it,
                                 const Endpoint &endpoint) {
  std::cout << "*******************************" << '\n';
  std::cout << "Resolving " << endpoint.host << '\n';
  std::cout << "*******************************"
            << "\n\n";

  if(ec) {
    ioservice.post([=]() {
      std::cerr << "Problem resolving URL: \"" << endpoint.host << "\"\n";
      std::cerr << ec.message() << '\n';
    });
    return;
  }
  tcp_socket.async_connect(*it, std::bind(&Downloader::connect_handler, this, _1, endpoint));
};
