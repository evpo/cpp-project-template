/* This test is intended to test the build process for creating shared libraries.
   There is no test program to run, but the shared library created by this test is used in the dynaload test. */
#include "version.hpp"
#include <string.h>

// Visual Studio requires the export directive on every function callable in the DLL
#ifdef _MSC_VER
#define DLLEXPORT _declspec(dllexport)
#else
#define DLLEXPORT
#endif

static char* copy_string (const char* str)
{
  char* result = new char[strlen(str)+1];
  strcpy(result,str);
  return result;
}

extern "C" DLLEXPORT char* stlplus_version()
{
  return copy_string(stlplus::version().c_str());
}
