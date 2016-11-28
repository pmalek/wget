#include <string>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <beast/core/streambuf.hpp>
#include <beast/http/string_body.hpp>

#include <network/uri/uri.hpp>

class Downloader {
public:
  Downloader(const std::vector<std::string> &urls);
  void go();

private:
  void read_handler(const boost::system::error_code &ec,
                    std::shared_ptr<beast::http::response<beast::http::string_body>> response,
                    std::shared_ptr<beast::streambuf>);

  void connect_handler(const boost::system::error_code &ec, const network::uri &uri);

  void queue_read(const boost::system::error_code &ec);

  void resolve_handler(const boost::system::error_code &ec,
                       boost::asio::ip::tcp::resolver::iterator it,
                       const network::uri &uri);

  std::vector<network::uri> uris;

  boost::asio::io_service ioservice;
  boost::asio::ip::tcp::socket tcp_socket;

  boost::asio::ip::tcp::resolver resolv;
};
