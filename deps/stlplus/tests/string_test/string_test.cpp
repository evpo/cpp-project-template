#include "persistence.hpp"
#include "file_system.hpp"
#include "build.hpp"
#include "string_utilities.hpp"
#include "string_float.hpp"
#include "string_int.hpp"
#include "version.hpp"

#define DATA "string_test.tmp"
#define MASTER "string_test.dump"

static std::string print_bytes (const std::string& val)
{
  std::string result;
  for (unsigned i = 0; i < val.size(); i++)
  {
    if (i > 0) result += ":";
    result += stlplus::int_to_string((int)val[i]);
  }
  return result;
}

int main(int argc, char* argv[])
{
  std::cerr << stlplus::build() << std::endl;

  bool result = true;
  try
  {
    // just dump a std::string
    std::cerr << "dumping" << std::endl;
    std::string original = "Hello\r\nWorld";
    stlplus::dump_to_file(original, DATA, stlplus::dump_string, 0);

    // now restore a copy and compare it
    std::cerr << "restoring" << std::endl;
    std::string restored;
    stlplus::restore_from_file(DATA, restored, stlplus::restore_string, 0);

    std::cerr << "original=\"" << original << "\", restored = \"" << restored << "\"" << std::endl;
    if (original != restored)
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
      std::string master;
      stlplus::restore_from_file(MASTER, master, stlplus::restore_string, 0);

      if (restored != master)
      {
        std::cerr << "ERROR: strings are different" << std::endl;
        std::cerr << "  restored: " << print_bytes(restored) << std::endl;
        std::cerr << "  master:   " << print_bytes(master) << std::endl;
        result = false;
      }
    }

    // short-term tests just to check new features

    // conversions to/from double
    std::string version = stlplus::version();
    double double_version = stlplus::string_to_double(version);
    std::string fixed_version = stlplus::double_to_string(double_version, stlplus::display_fixed);
    std::string float_version = stlplus::double_to_string(double_version, stlplus::display_floating);
    std::string default_version = stlplus::double_to_string(double_version, stlplus::display_mixed);
    std::cerr << "version = " << version
              << ", to_double = " << double_version
              << ", fixed = " << fixed_version
              << ", scientific = " << float_version
              << ", mixed = " << default_version
              << std::endl;

    // display of bytes and multiples
    for (long bytes = 1; bytes <= 1000000000; bytes *= 4)
      std::cerr << "bytes = " << bytes << ", display = " << stlplus::display_bytes(bytes) << std::endl;
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
