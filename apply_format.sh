#!/bin/bash

find . -type f -name "*.hpp" -exec clang-format -i {} +
