A Simple Log Analyzer
=====================

*This project is designed to solve a coding challenge, it's not meant to be
used in production.*

## Building the program ##

You only need the clang compiler and GNU Make. Inside the repository just type
`make`. If the POSIX function `isatty(3)` is not available on your system, you
can define the environment variable `NO_POSIX_ISATTY` (to any value).

## Using the program ##

The command line program reads log in the following format:

    yyyy-mm-dd h:m:s query

The date is described using the usual numerical format and is separated from the
query by a tab character.

Once parsed, you can perform two kinds of operations: count the number of
queries in a given time range or list the most frequent queries in a given time
range. The operations are described using a small commands:

    count start end
    top start end nth

Where `start` and `end` are date in the same format as the log and `nth` the
number of most frequent queries.

In the most basic form, you can run the program this way:

    ./log_analyzer -e command file_name

Where `command` is a command in the previously described format and `file_name`
is the log file. The log file name can be `-` (or absent), in this case the
program reads the log from its standard input.

If the `-e` flag (and its command) is not provided, the program will read
commands from its standard input until it receives a end-of-file. Here is a
simple session based on the challenge log file:

    ./log_analyzer hn_logs.tsv
    > count 2015-08-01 00:00:00 2015-09-01 00:00:00
    1623420
    > top 2015-08-01 00:00:00 2015-09-01 00:00:00 5
    http%3A%2F%2Fwww.getsidekick.com%2Fblog%2Fbody-language-advice	6675
    http%3A%2F%2Fwebboard.yenta4.com%2Ftopic%2F568045	4652
    http%3A%2F%2Fwebboard.yenta4.com%2Ftopic%2F379035%3Fsort%3D1	3100
    http%3A%2F%2Fjamonkey.com%2F50-organizing-ideas-for-every-room-in-your-house%2F	2800
    chrome%3A%2F%2Fnewtab%2F	2535
    >

The program display a little prompt `> ` when ready (after reading the log file which may take few seconds). The prompt is only displayed in interactive shell (unless `NO_POSIX_ISATTY` has been defined at compile time).

When using the standard input for the log, the `-e` flag is mandatory.

## Algorithms and complexity ##

The program is based on a tree data structure to attach data on time points.

This tree is an hybrid between an m-way search tree (in fact more a B+ tree) and
a prefix tree. Only leaf contains association between key (date) and values, all
leaves are linked together, in increase order of date (like a B+ tree). Like in
B+ trees, keys may appear in any internal nodes (as well as in the leaves),
permitting upper bound queries and thus range queries. The main difference
with B+ tree is in the way keys are inserted: the tree has a fixed height
based on date decomposition (year, month, day, hour, minute and second), each
level represents a part of date (even if the key stored is the complete date),
this structure guarantees a constant time complexity for upper bound queries.
The tree has to sentinel (lower and upper bounds).

Counting queries on a given time range is computed by finding lower and upper
bounds and then sum the size of the leaves storage in the range, thus the
complexity is linear in the number of time points in the range (which is bounded
by the number of seconds in the range).

Most frequent queries on a given time range is computed by finding the range
(like counting queries), an histogram is then built from elements stored in the
range. The histogram is transformed into a maximum heap, and the `n` most
frequent queries are extracted using heap pop operation. Building the histogram
and transforming it in a heap is linear in the number of queries in the range,
then each pop take logarithmic time, thus, with `M` the number of elements in
the range and `N` the requested number of most frequent queries, the complexity
is in `O(M + N log M)`.

