#include "dynaload.hpp"
#include "debug.hpp"
#include "build.hpp"
#include "version.hpp"
#include <iostream>

// parallel test directory containing the shared library
std::string library = "shared_library_test";

// subdirectory to find shared library
// build system should set this up automatically
std::string platform = TESTPLATFORM;

// name of function to callk in that library
std::string library_function = "stlplus_version";
// type of that function
typedef char* (*version_t)(void);

int main (int argc, char* argv[])
{
  DEBUG_TRACE;
  std::cerr << stlplus::build() << std::endl;

  int errors = 0;

  // test a non-existent library
  stlplus::dynaload nonexistent;
  if (nonexistent.load("nonexistent"))
  {
    std::cerr << "ERROR: nonexistent library LOADED!" << std::endl;
    errors++;
  }
  else
  {
    std::cerr << "success: nonexistent library load failed with code "
              << nonexistent.error_type() << ": "
              << nonexistent.error_text() << std::endl;
    nonexistent.clear_error();
  }

  // test load an existent library
  stlplus::dynaload existent(library, "../" + library + "/" + platform);
  if (!existent.loaded())
  {
    std::cerr << "ERROR: existent library " << library << " load FAILED with code "
              << existent.error_type() << ": "
              << existent.error_text() << std::endl;
    existent.clear_error();
    errors++;
  }
  else
  {
    std::cerr << "success: existent library " << library << " loaded OK" << std::endl;
    // now get the function from the shared library
    version_t version = (version_t)existent.symbol(library_function);
    if (!version)
    {
      std::cerr << "ERROR: existent function " + library_function + " load FAILED "
                << existent.error_type() << ": "
                << existent.error_text() << std::endl;
      existent.clear_error();
      errors++;
    }
    else
    {
      std::cerr << "success: existent function " << library_function << " loaded OK" << std::endl;
      if (std::string(version()) != stlplus::version())
      {
        std::cerr << "ERROR: version mismatch" << std::endl;
        errors++;
      }
      else
      {
        std::cerr << "success: STLplus version matches: " << version() << std::endl;
      }
    }
  }
  if (errors == 0)
    std::cerr << "test PASSED" << std::endl;
  else
    std::cerr << "test FAILED" << std::endl;

  return errors;
}
