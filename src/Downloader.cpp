#include "Downloader.hpp"

#include <iostream>

#include <beast/http/empty_body.hpp>
#include <beast/http/read.hpp>
#include <beast/http/write.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <network/uri/uri.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

Downloader::Downloader(boost::asio::io_service& ioservice)
    : ioservice(ioservice), resolv(ioservice) {}

Downloader::future_type Downloader::download_async(const std::string& url) {
  std::promise<response_type> promise;
  auto future = promise.get_future();

  download_async(url, std::move(promise));

  return future;
}

void Downloader::download_async(const std::string& url,
                                std::promise<response_type>&& promise) {
  auto state = std::make_shared<State>(std::move(promise),
                                       boost::asio::ip::tcp::socket{ioservice});
  try {
    state->uri = network::uri{url};
  } catch(...) {
    state->promise.set_exception(std::current_exception());
  }

  download_async(state);
}

void Downloader::download_async(state_ptr state) {
  ip::tcp::resolver::query query(state->uri.host().to_string(),
                                 state->uri.scheme().to_string());

  resolv.async_resolve(
      query,
      [this, state](const boost::system::error_code& ec,
                    ip::tcp::resolver::iterator it) { on_resolve(state, ec, it); });
}

void Downloader::on_resolve(state_ptr state,
                            const boost::system::error_code& ec,
                            tcp::resolver::iterator it) {
  if(ec) {
    state->promise.set_exception(
        std::make_exception_ptr(boost::system::system_error(ec)));
    return;
  }

  state->socket.async_connect(*it,
                              [this, state](const boost::system::error_code& ec) {
                                this->on_connect(state, ec);
                              });
}

void Downloader::on_connect(state_ptr state, const boost::system::error_code& ec) {
  if(ec) {
    state->promise.set_exception(
        std::make_exception_ptr(boost::system::system_error(ec)));
    return;
  }

  beast::http::request<beast::http::empty_body> req;
  req.method = "GET";
  req.url = state->uri.path().empty() ? "/" : state->uri.path().to_string();
  req.version = 11;
  req.fields.insert("Host", state->uri.host().to_string());
  req.fields.insert("User-Agent", "Beast");
  beast::http::prepare(req);

  if(state->uri.scheme().to_string() == "https") {
    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
    state->ssl_stream =
        std::make_unique<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>>(
            state->socket, ctx);
    state->ssl_stream->set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert);
    try {
      state->ssl_stream->handshake(boost::asio::ssl::stream_base::client);
    } catch(...) {
      state->promise.set_exception(std::current_exception());
      return;
    }

    beast::http::async_write(*state->ssl_stream,
                             std::move(req),
                             [this, state](const boost::system::error_code& ec) {
                               this->on_request_sent(state, ec);
                             });
  } else {
    beast::http::async_write(state->socket,
                             std::move(req),
                             [this, state](const boost::system::error_code& ec) {
                               this->on_request_sent(state, ec);
                             });
  }
}

void Downloader::on_request_sent(state_ptr state,
                                 const boost::system::error_code& ec) {
  if(ec) {
    state->promise.set_exception(
        std::make_exception_ptr(boost::system::system_error(ec)));
    return;
  }

  state->response =
      std::make_unique<beast::http::response<beast::http::string_body>>();
  state->streambuf = std::make_unique<beast::streambuf>();

  if(state->ssl_stream) {
    beast::http::async_read(*state->ssl_stream,
                            *state->streambuf,
                            *state->response,
                            [this, state](const boost::system::error_code& ec) {
                              this->on_read(state, ec);
                            });

  } else {
    beast::http::async_read(state->socket,
                            *state->streambuf,
                            *state->response,
                            [this, state](const boost::system::error_code& ec) {
                              this->on_read(state, ec);
                            });
  }
}

void Downloader::on_read(state_ptr state, const boost::system::error_code& ec) {
  if(ec) {
    state->promise.set_exception(
        std::make_exception_ptr(boost::system::system_error(ec)));
    return;
  }

  if((state->response->status == 301 || state->response->status == 302)
     && state->response->fields.exists("Location")) {
    download_async(state->response->fields["Location"].to_string(),
                   std::move(state->promise));
    return;
  }

  state->promise.set_value(std::move(*state->response));
}
