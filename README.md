[![Build Status](https://travis-ci.org/evpo/cpp-project-template.svg?branch=master)](https://travis-ci.org/evpo/cpp-project-template)

# Project template

This is a template for a C++ project. It's a good starting point. Download and start hacking.

    wget -O - https://github.com/evpo/cpp-project-template/archive/master.tar.gz | tar xzvf -
    mv cpp-project-template-master hello_world
    cd hello_world
    git init .
    git add -A
    git commit -m "initial commit"

You can also use [this script](https://github.com/evpo/scriptocracy/blob/master/scripts/create-cpp-project.sh) to automate new projects.

## Demo

[![asciicast](https://asciinema.org/a/239383.png)](https://asciinema.org/a/239383?speed=2&theme=solarized-dark)

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
    Mingw
    C++11 g++ or clang


## Acknowledgments

1. [stlplus](http://stlplus.sourceforge.net/)
2. [gtest](https://github.com/google/googletest)
3. [plog](https://github.com/SergiusTheBest/plog)

## License

Creative Commons BSD License
