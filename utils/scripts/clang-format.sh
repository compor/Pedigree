#!/usr/bin/env bash

find . -print0 \
  \( -iname "*.hpp" -o -iname "*.cpp" \) -a -not -path "*third_party*" \
  | xargs -0 clang-format -i -style=file

