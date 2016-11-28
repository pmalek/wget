#include "toNetworkUris.hpp"

#include <string>
#include <vector>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace utils {
std::vector<network::uri> toNetworkUris(const std::vector<std::string>& urls) {
  using namespace boost::adaptors;
  std::vector<network::uri> v;
  boost::copy(urls | transformed([](const auto& url) { return network::uri{url}; }), std::back_inserter(v));
  return v;
}
}
