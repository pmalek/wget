#include <boost/program_options.hpp>

struct ArgParser {
  ArgParser(int argc, const char** argv);
  ArgParser(const ArgParser&) = delete;
  ArgParser& operator=(const ArgParser&) = delete;

  bool parse();

  std::vector<std::string> get(const std::string& arg_name) const;

private:
  int argc;
  const char** argv;
  boost::program_options::variables_map vm;
};
