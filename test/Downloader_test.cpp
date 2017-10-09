#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <boost/asio/io_service.hpp>

#include "Downloader.hpp"

TEST(DownloaderTests, constructorDoesntThrow){
  boost::asio::io_service ioservice;
  EXPECT_NO_THROW(Downloader d{ioservice});
}
