#include <future>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <beast/core/streambuf.hpp>
#include <beast/http/string_body.hpp>

#include <network/uri/uri.hpp>

class Downloader {
public:
  using response_type = beast::http::response<beast::http::string_body>;
  using future_type = std::future<response_type>;

  explicit Downloader(boost::asio::io_service &ioservice);
  future_type download_async(const std::string &url);

private:
  struct State {
    std::promise<response_type> promise;
    network::uri uri;
    boost::asio::ip::tcp::socket socket;
    std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket &>>
        ssl_stream;
    std::unique_ptr<beast::http::response<beast::http::string_body>> response;
    std::unique_ptr<beast::streambuf> streambuf;

    State(std::promise<response_type> &&promise,
          boost::asio::ip::tcp::socket &&socket)
        : promise{std::move(promise)}, socket(std::move(socket)) {}
  };
  using state_ptr = std::shared_ptr<State>;

  void download_async(const std::string &url, std::promise<response_type> &&promise);
  void download_async(state_ptr state);

  void on_resolve(state_ptr state,
                  const boost::system::error_code &ec,
                  boost::asio::ip::tcp::resolver::iterator iterator);
  void on_connect(state_ptr state, const boost::system::error_code &ec);
  void on_request_sent(state_ptr state, const boost::system::error_code &ec);
  void on_read(state_ptr state, const boost::system::error_code &ec);

  boost::asio::io_service &ioservice;
  boost::asio::ip::tcp::resolver resolv;
};
