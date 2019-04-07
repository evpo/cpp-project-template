[![Build Status](https://travis-ci.org/evpo/cpp-project-template.svg?branch=master)](https://travis-ci.org/evpo/cpp-project-template)

# Project template

This is a template for a C++ project. It's a good starting point. Download and start hacking.

    wget -O - https://github.com/evpo/cpp-project-template/archive/v20190407.tar.gz | tar xzvf -
    mv cpp-project-template-* hello_world
    cd hello_world
    git init .
    git add -A
    git commit -m "initial commit"

You can also use [this script](https://github.com/evpo/scriptocracy/blob/master/scripts/create-cpp-project.sh) to automate new projects.

CLI:

    ./bin/debug/cpp-project

Unit tests:

    ./bin/debug/cpp-project-tests

## System requirements

    Linux
    Mingw
    C++11 g++ or clang

## Compile

Build:

    make

Release:

    make RELEASE=on

Verbose:

    make VERBOSE=on

Clean:

    make clean

Tests:

    make tests
    
## Acknowledgments

1. [stlplus](http://stlplus.sourceforge.net/)
2. [gtest](https://github.com/google/googletest)
3. [plog](https://github.com/SergiusTheBest/plog)

## License

Creative Commons BSD License
