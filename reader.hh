/*
 * reader.h: reading logs
 */

#ifndef _ALGOLIA_INTERVIEW_READER_HH_
#define _ALGOLIA_INTERVIEW_READER_HH_

#include <array>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

#define MAXLINELEN 1024

static
unsigned days_in_month(unsigned y, unsigned m)
{
  unsigned leap = y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
  if (m == 2) return 28 + leap;
  return 30 + ((m + m / 8) % 2);
}

static
bool check_date(const std::array<unsigned, 6>& date)
{
  if (date[1] < 1 || date[1] > 12)
    return false;
  if (date[2] < 1 || date[2] > days_in_month(date[0], date[1]))
    return false;
  return date[3] < 24 && date[4] < 60 && date[5] < 60;
}

static
bool read_date(const char *line, std::array<unsigned, 6>& date, int& pos)
{
  int n;
  n = std::sscanf(line, "%4u-%2u-%2u %2u:%2u:%2u\t%n",
      &date[0], &date[1], &date[2], &date[3], &date[4], &date[5], &pos);
  if (n < 6)
    return false;
  return check_date(date);
}

static
bool read_line(const char *line, std::array<unsigned, 6>& date, std::string& query)
{
  int pos;
  if (!read_date(line, date, pos))
    return false;
  query = line + pos;
  return true;
}

template<typename STREAM, typename ERRSTREAM>
size_t read_log(STREAM& input, ERRSTREAM& errors, std::function<void(std::array<unsigned,6>, std::string)> consume)
{
  size_t count = 0, lcount = 0;
  while (input) {
    static char line[MAXLINELEN];
    std::array<unsigned, 6> date;
    std::string query;
    input.getline(line, MAXLINELEN);
    lcount++;
    if (line[0] == 0)
      break;
    if (read_line(line, date, query)) {
      count++;
      consume(date, query);
    } else {
      errors<< "Line " << lcount << ": bad format\n";
    }
  }
  return count;
}

#endif /* _ALGOLIA_INTERVIEW_READER_HH_ */

