#include "logger_init.h"
#include "plog/Log.h"
#include "plog/Appenders/ColorConsoleAppender.h"

namespace CppProject
{

void InitLogger(const std::string &file_name)
{
    if(file_name == "-")
    {
        plog::init(plog::debug, new plog::ColorConsoleAppender<plog::TxtFormatter>());
    }
    else
    {
        plog::init(plog::debug, file_name.data());
        LOG_INFO << "Log instance started";
    }
}

}
