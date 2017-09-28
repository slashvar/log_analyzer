/*
 * log_analyzer.cc : entry point and cmd line parsing
 * cmd line and cmd parsing are volontary kept simple.
 *
 * The program accept a file name as input and can read query on stdin working
 * on the same file for each query.
 *
 * You can also pass a single query on the command line.
 */

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#ifndef NO_ISATTY
#include <unistd.h>
#endif /* NO_ISATTY */

#include "reader.hh"
#include "time_map.hh"

/*
 * query operations helper functions
 */

static
void top_freq(time_tree<std::string>& tree, date start, date stop, size_t n)
{
  using hdata = std::pair<std::string, size_t>;
  static auto cmp = [] (const hdata& p1, const hdata& p2) -> bool {
		      return p1.second < p2.second;
		    };

  auto hist = tree.compute_freq(start, stop);

  std::vector<hdata> data;
  for (auto p : hist)
    data.push_back(p);
  std::make_heap(begin(data), end(data), cmp);

  for (size_t i = 0; i < std::min(n, data.size()); i++) {
    std::pop_heap(begin(data), end(data), cmp);
    auto p = data.back();
    data.pop_back();
    std::cout << p.first << "\t" << p.second << std::endl;
  }
}

static
void count(time_tree<std::string>& tree, date begin, date end)
{
  std::cout << tree.count(begin, end) << std::endl;
}

/*
 * query forms:
 *
 * In both forms, elements must be separated by space chars on a single line.
 *
 * count begin end
 * counts the number of query between begin and end
 *
 * top begin end nb
 * display the nb most frequent query between begin and end
 *
 * begin and end have the same format as the input dates: yyyy-mm-dd hh:min:sec
 *
 * *TODO*:
 *	* support partial date format
 *	* better command parsing
 */

static
bool parse_range(const char *line, date& start, date& stop, int& pos)
{
  std::array<unsigned, 6> dbegin, dend;
  int oldpos = pos;
  if (!read_date(line + pos, dbegin, pos)) {
    return false;
  }
  pos += oldpos;
  oldpos = pos;
  if (!read_date(line + pos, dend, pos)) {
    return false;
  }
  pos += oldpos;
  start = date(dbegin);
  stop = date(dend);
  return start.value <= stop.value;
}
/*
 * TODO: check for content after last expected token
 */
static
bool parse_query(time_tree<std::string>& tree, const char *line)
{
  int n, pos;
  char kwd[6];
  n = std::sscanf(line, "%5s%n", kwd, &pos);
  if (n < 1)
    return false;
  date dbegin, dend;
  if (!parse_range(line, dbegin, dend, pos))
    return false;
  if (std::string("count") == kwd) {
    count(tree, dbegin, dend);
    return true;
  }
  if (std::string("top") == kwd) {
    size_t nth;
    char *stop;
    nth = std::strtoul(line + pos, &stop, 10);
    if (line + pos == stop || nth == 0)
      return false;
    top_freq(tree, dbegin, dend, nth);
    return true;
  }
  return false;
}

static
size_t read_data(std::istream& in, time_tree<std::string>& tree)
{
  auto consume = [&tree] (std::array<unsigned, 6> d, std::string cont) {
    tree.insert(date(d), cont);
  };

  auto count = read_log(in, std::clog, consume);
  return count;
}

static
time_tree<std::string> read_data_from_file(const char* fname)
{
  std::ifstream infile(fname);
  if (!infile) {
    std::clog << "Can't open file " << fname << std::endl;
    std::exit(1);
  }

  time_tree<std::string> tree;
  auto count = read_data(infile, tree);

  if (count == 0) {
    std::clog << "Empty content in input file " << fname << std::endl;
    std::exit(1);
  }

  return tree;
}

static
time_tree<std::string> read_data_from_stdin(void)
{
  time_tree<std::string> tree;
  auto count = read_data(std::cin, tree);

  if (count == 0) {
    std::clog << "Empty content in input\n";
    std::exit(1);
  }

  return tree;
}

static
void usage(const char *cmdname)
{
  std::clog << cmdname << " [-e query] [filename]\n";
  std::clog << "  -e query\tperform a single query\n\n";

  std::clog << "if [filename] is \"-\" use stdin as input, in that case \"-e\" option is mandatory.\n";

}

static
void prompt(void)
{
# ifndef NO_ISATTY
  if (isatty(STDIN_FILENO))
# endif
    std::cout << "> ";
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::clog << "Missing file name or option\n";
    usage(argv[0]);
    return 1;
  }
  bool single = false;
  int pos = 1;
  if (std::string("-e") == argv[1]) {
    single = true;
    if (argc < 3) {
      std::clog << "Missing argument after -e\n";
      usage(argv[0]);
      return 1;
    }
    pos += 2;
  }

  bool usestdin = false;

  if (argc < pos + 1 || std::string("-") == argv[pos]) {
    if (!single) {
      std::clog << "Usage of -e option is mandatory with stdin input.\n";
      return 1;
    }
    usestdin = true;
  }

  auto tree = usestdin ? read_data_from_stdin() : read_data_from_file(argv[pos]);

  if (single) {
    if (!parse_query(tree, argv[2])) {
      std::clog << "Query parse error.\n";
      return 1;
    }
  } else {
    char line[256];
    prompt();
    while (std::cin.getline(line, 256)) {
      if (!parse_query(tree, line)) {
	std::clog << "Query parse error.\n";
      }
      prompt();
    }
  }

  return 0;
}
