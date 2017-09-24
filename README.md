A Simple Log Analyzer
=====================

*This project is designed to solve a coding challenge, it's not meant to be
used in production.*

## Building the program ##

You only need the clang compiler and GNU Make. Inside the repository just type
`make`.

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

The program display a little prompt `> ` when ready (after reading the log file which may take few seconds).
