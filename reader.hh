/*
 * reader.h: reading logs
 */

#ifndef _ALGOLIA_INTERVIEW_READER_HH_
#define _ALGOLIA_INTERVIEW_READER_HH_

#include <array>
#include <cstdio>
#include <functional>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <utility>

/*
 * read_date(line, date, pos): reads a date in line and update pos index
 */
bool read_date(const char *line, std::array<unsigned, 6>& date, int& pos);

/*
 * read_log(input, errors, consume): reads log line by line from input and call
 * consume on each parsed line. Errors are output on errors stream.
 */
using consume_t = std::function<void(std::array<unsigned,6>, std::string)>;
size_t read_log(std::istream& input, std::ostream& errors, consume_t consume);

#endif /* _ALGOLIA_INTERVIEW_READER_HH_ */

