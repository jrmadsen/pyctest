#!/bin/bash

: ${CLANG_FORMAT_COMMAND:=clang-format}

if [ -n "${1}" ]; then CLANG_FORMAT_COMMAND=$1; fi

for i in $(find source | egrep '\.h$|\.hh$|\.hpp$|\.icc$|\.c$|\.cc$|\.cpp$|\.cxx$' | egrep -v 'build|/Checks/|/Tests/|/kwsys/|/QtIFW/|/Utilities/|/Modules/|/QtDialog/|/CursesDialog/|/LexerParser/|/pybind11/')
do
    echo -e "Formatting '${i}'..."
    ${CLANG_FORMAT_COMMAND} -i ${i}
done
