#include "persistent_shortcuts.hpp"
#include "persistent_cstring.hpp"
#include "file_system.hpp"
#include "build.hpp"
#include <fstream>
#include <string.h>
#include <stdio.h>

#define DATA "cstring_test.tmp"
#define MASTER "cstring_test.dump"

static std::string to_string(int number)
{
  // use sprintf in a very controlled way that cannot overrun
  char* buffer = new char[50];
  sprintf(buffer, "%i", number);
  std::string result = buffer;
  delete[] buffer;
  return result;
}

static std::string print_bytes (const std::string& val)
{
  std::string result;
  for (unsigned i = 0; i < val.size(); i++)
  {
    if (i > 0) result += ":";
    result += to_string((int)val[i]);
  }
  return result;
}

int main(int argc, char* argv[])
{
  std::cerr << stlplus::build() << std::endl;

  bool result = true;
  try
  {
    // just dump some basic types
    std::cerr << "dumping" << std::endl;
    const char* original = "Hello\r\nWorld";
    stlplus::dump_to_file(original, DATA, stlplus::dump_cstring, 0);

    // now restore a copy and compare it
    std::cerr << "restoring" << std::endl;
    char* restored = 0;
    stlplus::restore_from_file(DATA, restored, stlplus::restore_cstring, 0);

    std::cerr << "original=\"" << original << "\", restored = \"" << restored << "\"" << std::endl;
    if (strcmp(original, restored) != 0)
    {
      std::cerr << "ERROR: strings are different" << std::endl;
      std::cerr << "  original: " << print_bytes(original) << std::endl;
      std::cerr << "  restored: " << print_bytes(restored) << std::endl;
      result = false;
    }

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      char* master = 0;
      stlplus::restore_from_file(MASTER, master, stlplus::restore_cstring, 0);

      if (strcmp(restored, master) != 0)
      {
        std::cerr << "ERROR: strings are different" << std::endl;
        std::cerr << "  restored: " << print_bytes(restored) << std::endl;
        std::cerr << "  master:   " << print_bytes(master) << std::endl;
        result = false;
      }
    }
  }
  catch(std::exception& except)
  {
    std::cerr << "caught standard exception " << except.what() << std::endl;
    result = false;
  }
  catch(...)
  {
    std::cerr << "caught unknown exception" << std::endl;
    result = false;
  }
  if (result)
    std::cerr << "test passed" << std::endl;
  else
    std::cerr << "test FAILED" << std::endl;
  return result ? 0 : 1;
}
