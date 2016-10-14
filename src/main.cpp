#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "Downloader.h"

namespace po = boost::program_options;

struct ArgParser {
  ArgParser(int argc, const char** argv) : argc(argc), argv(argv) {}
  ArgParser(const ArgParser&) = delete;
  ArgParser& operator=(const ArgParser&) = delete;

  bool parse() {
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
        "files",
        po::value<std::vector<std::string>>()->required()->multitoken(),
        "list of files to be downloaded");

    po::positional_options_description pos;
    pos.add("files", -1);

    try {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(pos).run(), vm);
      po::notify(vm);
    } catch(const po::error& e) {
      std::cerr << "Couldn't parse command line arguments properly:\n";
      std::cerr << e.what() << '\n' << '\n';
      std::cerr << desc << '\n';
      return false;
    }

    if(vm.count("help") || !vm.count("files")) {
      std::cout << desc << "\n";
      return false;
    }

    return true;
  }

  std::vector<std::string> get(const std::string& arg_name) const {
    return vm[arg_name].as<std::vector<std::string>>();
  }

private:
  int argc;
  const char** argv;
  po::variables_map vm;
};

int main(int argc, const char** argv) {
  ArgParser a(argc, argv);
  if(!a.parse()) return 1;

  auto urls = a.get("files");
  Downloader d(urls);
  d.go();
}

