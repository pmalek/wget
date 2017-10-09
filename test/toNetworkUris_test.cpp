#include <string>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/utility/string_view.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <rapidcheck/gtest.h>

#include "toNetworkUris.hpp"

namespace {
const std::string HTTP_WWW = "http://www.";
const std::string HTTPS_WWW = "https://www.";
}

RC_GTEST_PROP(toNetworkUrisTest, DoesntThrowForCorrectURLs, (std::string url)) {
  // TODO
  //if(!url.empty()) {
    //if(boost::starts_with(url, HTTP_WWW) || boost::starts_with(url, HTTPS_WWW)) {
      //EXPECT_NO_THROW(utils::toNetworkUris({url}));
    //} else {
      //EXPECT_THROW(utils::toNetworkUris({url}), network::uri_syntax_error) << "Failed URL '" << url << "'"
                                                                       //<< std::endl;
    //}
  //}
}

// TEST(toNetworkUrisTests, XXX){
// std::vector<std::string> urls{"http://www.google.com"};
// EXPECT_NO_THROW(utils::toNetworkUris(urls));
//}

// TEST(toNetworkUrisTests, XXX2){
// std::vector<std::string> urls{"httpx://www.google.com"};
// EXPECT_THROW(utils::toNetworkUris(urls), std::logic_error);
//}

// TEST(toNetworkUrisTests, XXX3){
// std::vector<std::string> urls{"google.com"};
// EXPECT_THROW(utils::toNetworkUris(urls), std::logic_error);
//}
