#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Downloader.hpp"

TEST(DownloaderTests, XXX){
  std::vector<std::string> urls = {"http://www.google.com"};
  Downloader ds(urls);
}

TEST(DownloaderTests, THROW){
  std::vector<std::string> urls = {"htp://www.google.com"};
  Downloader ds(urls);
}
