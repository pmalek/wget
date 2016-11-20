#include <string>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <beast/core/streambuf.hpp>
#include <beast/http/message.hpp>
#include <beast/http/string_body.hpp>

class Downloader {
public:
  Downloader(const std::vector<std::string> &urls);
  void go();

private:
  struct Endpoint {
    Endpoint(const std::string &h, const std::string &u) : host(h), url(u) {}
    std::string host, url;
  };

  void read_handler(const boost::system::error_code &ec);
  void connect_handler(const boost::system::error_code &ec, const Endpoint &endpoint);
  void queue_read(const boost::system::error_code &ec);
  void resolve_handler(const boost::system::error_code &ec,
                       boost::asio::ip::tcp::resolver::iterator it, const Endpoint &endpoint);

  std::vector<Endpoint> endpoints;

  beast::http::response<beast::http::string_body> resp;
  beast::streambuf response_streambuf;

  boost::asio::io_service ioservice;
  boost::asio::ip::tcp::socket tcp_socket;
  boost::asio::ip::tcp::resolver resolv;
};
