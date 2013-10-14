#!/bin/sh
cat $(find ./ -iregex ".*\.\(c\|h\)") | wc
