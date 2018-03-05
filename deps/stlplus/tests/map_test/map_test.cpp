#include <string>
#include <map>
#include "persistent_contexts.hpp"
#include "persistent_map.hpp"
#include "persistent_string.hpp"
#include "persistent_int.hpp"
#include "persistent_shortcuts.hpp"
#include "strings.hpp"
#include "dprintf.hpp"
#include "file_system.hpp"
#include "build.hpp"

////////////////////////////////////////////////////////////////////////////////

#define NUMBER 100
#define DATA "map_test.tmp"
#define MASTER "map_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef std::map<int,std::string> int_string_map;

void dump_int_string_map(stlplus::dump_context& context, const int_string_map& data)
{
  stlplus::dump_map(context, data, stlplus::dump_int, stlplus::dump_string);
}

void restore_int_string_map(stlplus::restore_context& context, int_string_map& data)
{
  stlplus::restore_map(context, data, stlplus::restore_int, stlplus::restore_string);
}

bool compare(const int_string_map& left, const int_string_map& right)
{
  bool result = true;
  if (left.size() != right.size())
  {
    std::cerr << "different size - left = " << left.size() << " right = " << right.size() << std::endl;
    result = false;
  }
  int_string_map::const_iterator j, k;
  for (j = left.begin(), k = right.begin(); j != left.end() && k != right.end(); j++, k++)
  {
    if (*j != *k)
    {
      std::cerr << "left = \"" << j->first << "\" is different from right = \"" << k->first << "\"" << std::endl;
      result = false;
    }
  }
  return result;
}

void print_int(std::ostream& str, int data)
{
  return stlplus::print_int(str, data);
}

std::ostream& operator<< (std::ostream& str, const int_string_map& data)
{
  stlplus::print_map(str, data, print_int, stlplus::print_string, ":", ",");
  return str;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing " << NUMBER << " mappings" << std::endl;

  try
  {
    // build the sample data structure
    std::cerr << "creating" << std::endl;
    int_string_map data;
    for (unsigned i = 0; i < NUMBER; i++)
      data[i] = stlplus::dformat("%d",i);
    std::cerr << data << std::endl;
    // now dump to the file
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DATA,dump_int_string_map,0);

    // now restore the same file and compare
    std::cerr << "restoring" << std::endl;
    int_string_map restored;
    stlplus::restore_from_file(DATA,restored,restore_int_string_map,0);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      int_string_map master;
      stlplus::restore_from_file(MASTER,master,restore_int_string_map,0);
      result &= compare(data,master);
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

  if (!result)
    std::cerr << "test failed" << std::endl;
  else
    std::cerr << "test passed" << std::endl;
  return result ? 0 : 1;
}
