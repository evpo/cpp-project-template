/*------------------------------------------------------------------------------

  Author:    Andy Rushton

  Test the ini_manager subsystem

------------------------------------------------------------------------------*/
#include "debug.hpp"
#include "build.hpp"
#include "ini_manager.hpp"
#include "file_system.hpp"
#include "print_vector.hpp"
#include "print_string.hpp"
#include "string_utilities.hpp"

////////////////////////////////////////////////////////////////////////////////
// utility for printing a vector of strings, used a lot in this program

std::ostream& operator<<(std::ostream& device, const std::vector<std::string>& strings)
{
  stlplus::print_vector(device, strings, stlplus::print_string, ":");
  return device;
}

////////////////////////////////////////////////////////////////////////////////

bool test_ini_manager(stlplus::ini_manager& manager, const std::string& name)
{
  bool success = true;
  std::cerr << "======== Testing " << name << std::endl;
  for (unsigned i = 0; i < manager.size(); i++)
  {
    std::cerr << "ini file: " << manager.filename(i);
    std::cerr << (manager.writable(i) ? " writable" : " not writable");
    std::cerr << (manager.empty(i) ? " empty" : " not empty");
    std::cerr << std::endl;
    // print sections for each file
    for (unsigned i = 0; i < manager.size(); i++)
    {
      std::cerr << "section names for level " << i << " = " << manager.filename(i) << std::endl;
      std::vector<std::string> sections = manager.section_names(i);
      for (unsigned j = 0; j < sections.size(); j++)
        std::cerr << "  " << j << ": \"" << sections[j] << "\"" << std::endl;
    }
    // print list of sections in all files
    std::cerr << "section names for all levels" << std::endl;
    std::vector<std::string> sections = manager.section_names();
    for (unsigned j = 0; j < sections.size(); j++)
      std::cerr << "  " << j << ": \"" << sections[j] << "\"" << std::endl;
  }

  if (manager.variable_exists("section","test"))
  {
    std::cerr << "variable \"test\" found in section \"section\" = " 
              << manager.variable_value("section","test") << std::endl;
    std::cerr << "filename = " << manager.variable_filename("section","test") 
              << ", line = " << manager.variable_linenumber("section","test") << std::endl;
  }
  else
  {
    std::cerr << "variable \"test\" not found in section \"section\"" << std::endl;
    success = false;
  }
  return success;
}

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{
  DEBUG_TRACE;
  std::cerr << stlplus::build() << std::endl;

  try
  {
    unsigned errors = 0;

    std::vector<std::string> ini_files;
    ini_files.push_back("test.ini");
    ini_files.push_back("test2.ini");
    ini_files.push_back("test3.ini");
    std::cerr << "ini_files = " << ini_files << std::endl;

    // set up an Ini file manager
    stlplus::ini_manager manager(ini_files);
    std::cerr << "manager = " << manager << std::endl;
    if (!test_ini_manager(manager, "original"))
      errors++;

    stlplus::ini_manager manager2(manager);
    if (!test_ini_manager(manager2, "alias"))
      errors++;

    return errors;
  }
  catch(std::exception& exception)
  {
    std::cerr << "exception: failed with " << exception.what() << std::endl;
    return -1;
  }
  return -2;
}

////////////////////////////////////////////////////////////////////////////////
