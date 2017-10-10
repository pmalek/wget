#include <iostream>
#include <vector>
#include <tuple>
#include <string>

#include "ArgParser.hpp"
#include "Downloader.hpp"

int main(int argc, const char** argv) {
  ArgParser a(argc, argv);
  if(!a.parse()) return 1;

  boost::asio::io_service io_service;

  auto urls = a.get("files");
  Downloader d(io_service);

  std::vector<std::tuple<std::string, Downloader::future_type>> downloads(urls.size());

  std::transform(std::begin(urls), std::end(urls), std::begin(downloads), [&](auto& url) {
    return std::make_tuple(url, std::move(d.download_async(url)));
  });

  std::thread asio_thread{[&]() { io_service.run(); }};

  for(auto& tuple : downloads) {
    auto& url = std::get<0>(tuple);
    auto& download = std::get<1>(tuple);

    std::cout << url << "\n===\n";
    try {
      auto response = download.get();

      std::cout << "Received status code: " << response.status << '\n';

      for(auto pair : response.fields) {
        std::cout << pair.first << " : " << pair.second << '\n';
      }
    } catch(boost::system::system_error& e) {
      std::cout << "Error (" << e.code() << "): " << e.what() << "\n";
    } catch(std::exception& e) {
      std::cout << "Error: " << e.what() << "\n";
    }
    std::cout << "\n";
  }

  asio_thread.join();
}
