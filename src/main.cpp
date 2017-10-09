#include "ArgParser.hpp"
#include "Downloader.hpp"

int main(int argc, const char** argv) {
  ArgParser a(argc, argv);
  if(!a.parse()) return 1;

  boost::asio::io_service ioservice;

  auto urls = a.get("files");
  Downloader d(ioservice);
  d.go();
}

