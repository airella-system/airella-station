#!/usr/bin/env bash

find src include -type f \( -name "*.h" -o -name "*.c" -o -name "*.cpp" \) -exec clang-format -i -style=file {} \;