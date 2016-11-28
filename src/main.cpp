#include "ArgParser.hpp"
#include "Downloader.hpp"

int main(int argc, const char** argv) {
  ArgParser a(argc, argv);
  if(!a.parse()) return 1;

  auto urls = a.get("files");
  Downloader d(urls);
  d.go();
}

