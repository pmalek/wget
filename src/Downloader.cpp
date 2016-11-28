#include "Downloader.hpp"

#include <iostream>

#include <boost/asio/ip/tcp.hpp>

#include <beast/http/empty_body.hpp>
#include <beast/http/read.hpp>
#include <beast/http/write.hpp>

#include <network/uri/uri.hpp>

using namespace std::placeholders;  // for _1, _2 etc.
using namespace boost::asio;
using namespace boost::asio::ip;

Downloader::Downloader(const std::vector<std::string> &urls) : tcp_socket(ioservice), resolv(ioservice) {
  std::transform(urls.begin(), urls.end(), std::back_inserter(uris), [this](const std::string &url) {
    std::error_code ec;
    network::uri u(url, ec);
    if(ec) {
      ioservice.post([=] {
        std::cout << "Error parsing URL : " << url << '\n' << "Error code : " << ec.message() << '\n';
      });
    }
    return u;
  });
}

void Downloader::go() {
  for(const auto &uri : uris) {
    std::cout << "*******************************" << '\n'
              << "Resolving " << uri.host() << '\n'
              << "*******************************\n\n";

    resolv.async_resolve(
        tcp::resolver::query{uri.host().to_string(), (uri.scheme().to_string() == "https" ? "443" : "80")},
        std::bind(&Downloader::resolve_handler, this, _1, _2, uri));
  }

  ioservice.run();
}

void Downloader::read_handler(const boost::system::error_code &ec,
                              std::shared_ptr<beast::http::response<beast::http::string_body>> response,
                              std::shared_ptr<beast::streambuf>) {
  if(ec) {
    ioservice.post([=] {
      std::cerr << "Problem reading the response: \n"
                << "error : " << ec << '\n'
                << "error_core.value() : " << ec.value() << '\n'
                << "error_core.message() : " << ec.message() << '\n';
    });
    return;
  }

  std::cout << "\n*******************************" << '\n'
            << "Headers\n\n";
  for(auto i : response->fields) std::cout << i.first << " : " << i.second << '\n';
  std::cout << "*******************************"
            << "\n\n";

  std::cout << "Received status code: " << response->status << '\n';

  if((response->status == 301 || response->status == 302) && response->fields.exists("Location")) {
    network::uri u(response->fields["Location"].to_string());
    std::cout << "Added a new request for redirection to " << u.string() << '\n';
    resolv.async_resolve(
        tcp::resolver::query{u.host().to_string(), (u.scheme().to_string() == "https" ? "443" : "80")},
        std::bind(&Downloader::resolve_handler, this, _1, _2, u));
    return;
  }

  std::cout << "*******************************" << '\n'
            << "Response body\n\n" << response->body
            << "\n\n";
}

void Downloader::connect_handler(const boost::system::error_code &ec, const network::uri& uri) {
  if(ec) {
    ioservice.post([=] {
      std::cout << "error connecting : " << ec << '\n'
                << "error_core.value() : " << ec.value() << '\n'
                << "error_core.message() : " << ec.message() << '\n';
    });
    return;
  }

  // Send HTTP(S) request using beast
  beast::http::request<beast::http::empty_body> req;
  req.method = "GET";
  req.url = (uri.path().empty() ? "/" : uri.path().to_string());
  req.version = 11;
  req.fields.insert("Host", uri.host().to_string());
  req.fields.insert("User-Agent", "Beast");
  beast::http::prepare(req);

  const bool HTTPS = (uri.scheme().to_string() == "https");
  std::cout << "*******************************" << '\n'
            << "Sending a HTTP" << (HTTPS ? "S" : "") << " request\n\n" << req
            << "*******************************" << '\n';

  beast::http::async_write(tcp_socket, std::move(req), std::bind(&Downloader::queue_read, this, _1));
}

void Downloader::queue_read(const boost::system::error_code &ec) {
  if(ec) {
    ioservice.post([=] {
      std::cerr << "error : " << ec << '\n'
                << "error_core.value() : " << ec.value() << '\n'
                << "error_core.message() : " << ec.message() << '\n';
    });
    return;
  }

  auto response = std::make_shared<beast::http::response<beast::http::string_body>>();
  auto response_streambuf = std::make_shared<beast::streambuf>();

  beast::http::async_read(tcp_socket,
                          *response_streambuf,
                          *response,
                          std::bind(&Downloader::read_handler, this, _1, response, response_streambuf));
}

void Downloader::resolve_handler(const boost::system::error_code &ec,
                                 tcp::resolver::iterator it,
                                 const network::uri& uri) {
  if(ec) {
    ioservice.post([=] {
      std::cerr << "Problem resolving URL: \"" << uri.host() << "\"\n" << ec.message() << '\n';
    });
    return;
  }

  auto ep = it->endpoint();
  std::cout << "*******************************" << '\n'
            << "Resolved to " << ep.address() << ':' << ep.port() << '\n'
            << "*******************************"
            << "\n\n";

  tcp_socket.async_connect(*it, std::bind(&Downloader::connect_handler, this, _1, uri));
}
