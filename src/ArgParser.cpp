#include "ArgParser.hpp"

#include <iostream>

namespace po = boost::program_options;

ArgParser::ArgParser(int argc, const char** argv) : argc(argc), argv(argv) {}

bool ArgParser::parse() {
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
  } catch(const po::required_option& e) {
    if(!vm.count("help")) std::cerr << e.what() << '\n' << '\n';
    std::cout << desc << "\n";
    return false;
  } catch(const po::error& e) {
    std::cerr << "Couldn't parse command line arguments properly:\n";
    std::cerr << e.what() << '\n' << '\n';
    std::cerr << desc << '\n';
    return false;
  }

  if(vm.count("help")) {
    std::cout << desc << "\n";
    return false;
  }

  return true;
}

std::vector<std::string> ArgParser::get(const std::string& arg_name) const {
  return vm[arg_name].as<std::vector<std::string>>();
}
