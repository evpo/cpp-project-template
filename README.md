[![CircleCI](https://dl.circleci.com/status-badge/img/gh/evpo/cpp-project-template/tree/master.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/evpo/cpp-project-template/tree/master)
# C++ Project Template

This is a template for your new C++ project. It's a good starting point for a tool or proof of concept. Download and start hacking.

    wget -O - https://github.com/evpo/cpp-project-template/archive/master.tar.gz | tar xzvf -
    mv cpp-project-template-master hello_world
    cd hello_world
    git init .
    git add -A
    git commit -m "initial commit"

You can also use [this script](https://github.com/evpo/scriptocracy/blob/master/scripts/create-cpp-project.sh) to automate creation of new projects.

## Features

* Help and command line parameters
* Simple to understand makefile build
* CLI front-end and a back-end library
* Diagnostic logging
* Unit tests
* Multiplatform: Linux, Windows, macOS
* Almost no dependencies

## Demo

[![asciicast](https://asciinema.org/a/239383.svg)](https://asciinema.org/a/239383)

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
    
## Run

CLI:

    ./bin/debug/cpp-project

Unit tests:

    ./bin/debug/cpp-project-tests

## System requirements

    Linux
    macOS
    Mingw
    C++17 g++ or clang


## Acknowledgments

1. [stlplus](http://stlplus.sourceforge.net/)
2. [gtest](https://github.com/google/googletest)
3. [plog](https://github.com/SergiusTheBest/plog)

## License

Creative Commons BSD License
