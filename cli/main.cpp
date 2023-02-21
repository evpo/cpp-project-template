#include <iostream>
#include <sstream>
#include <string>
#include "version.h"
#include "cli_parser.hpp"
#include "hello_name_generator.h"
#include "logger_init.h"

using namespace std;
using namespace stlplus;
using namespace CppProject;

namespace CppProject
{

void PrintUsage()
{
    const char *usage =
        VER_PRODUCTNAME_STR " " VER_PRODUCTVERSION_STR "\n"
R"(
Usage: cpp-project --help | <user_name>
c++ project that outputs Hello World
Commands:
--help                  output this help message
Options:
--log <LOG_FILE>        the log file to output diagnostic messages
)";

    std::cout << usage << std::endl;
}

}

int main(int, char *argv[])
{
    cli_definitions cli_defs = {
        {
            "help",
            cli_kind_t::cli_switch_kind,
            cli_mode_t::cli_single_mode,
            "help",
        },
        {
            "log",
            cli_kind_t::cli_value_kind,
            cli_mode_t::cli_single_mode,
            "log",
        },
        {
            "",
            cli_kind_t::cli_value_kind,
            cli_mode_t::cli_single_mode,
            "name",
        },
    };

    message_handler messages(std::cerr);
    cli_parser parser(cli_defs, messages);
    if(!parser.parse(argv))
    {
        PrintUsage();
        exit(1);
    }

    string log_file;

    string name = "Sir/Madam";
    for(unsigned i = 0; i < parser.size(); i++)
    {
        if(parser.name(i) == "help")
        {
            PrintUsage();
            exit(0);
        }
        else if(parser.name(i) == "log")
        {
            log_file = parser.string_value(i);
        }
        else if(parser.name(i) == "")
        {
            name = parser.string_value(i);
        }
    }

    if(log_file.empty())
    {
        log_file = "debug.log";
    }

    InitLogger(log_file);
    cout << GenerateHelloName(name) << endl;
    return 0;
}
