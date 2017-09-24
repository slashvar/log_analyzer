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
