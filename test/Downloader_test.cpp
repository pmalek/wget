#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <boost/asio/io_service.hpp>

#include "Downloader.hpp"

TEST(DownloaderTests, constructorDoesntThrow){
  boost::asio::io_service ioservice;
  EXPECT_NO_THROW(Downloader d{ioservice});
}

TEST(DownloaderTests, passingIncorrectURLThrows){
  boost::asio::io_service ioservice;
  Downloader d{ioservice};
  auto fut = d.download_async("url");
  EXPECT_THROW(fut.get(), std::system_error);
}
