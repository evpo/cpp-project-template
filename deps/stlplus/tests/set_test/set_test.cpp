#include "persistent_multiset.hpp"
#include "persistent_string.hpp"
#include "persistent_shortcuts.hpp"
#include "file_system.hpp"
#include "dprintf.hpp"
#include "build.hpp"
#include <string>
#include <set>

////////////////////////////////////////////////////////////////////////////////

#define NUMBER 1000
#define DATA "set_test.tmp"
#define MASTER "set_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef std::multiset<std::string> string_set;

void dump_string_set(stlplus::dump_context& context, const string_set& data)
{
  stlplus::dump_multiset(context, data, stlplus::dump_string);
}

void restore_string_set(stlplus::restore_context& context, string_set& data)
{
  stlplus::restore_multiset(context, data, stlplus::restore_string);
}

bool compare(const string_set& left, const string_set& right)
{
  bool result = true;
  if (left.size() != right.size())
  {
    std::cerr << "different size - left = " << left.size() << " right = " << right.size() << std::endl;
    result = false;
  }
  string_set::const_iterator j, k;
  for (j = left.begin(), k = right.begin(); j != left.end() && k != right.end(); j++, k++)
  {
    if (*j != *k)
    {
      std::cerr << "left = \"" << *j << "\" is different from right = \"" << *k << "\"" << std::endl;
      result = false;
    }
  }
  return result;
}

std::ostream& operator<< (std::ostream& device, const string_set& values)
{
  for (string_set::const_iterator i = values.begin(); i != values.end(); i++)
  {
    if (i != values.begin()) device << ",";
    device << *i;
  }
  return device;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing " << NUMBER << " set members" << std::endl;

  try
  {
    // build the sample data structure
    string_set data;
    for (unsigned i = 0; i < NUMBER; i++)
      data.insert(stlplus::dformat("%d",i));
    // now dump to the file
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DATA,dump_string_set,0);

    // now restore the same file and compare
    std::cerr << "restoring" << std::endl;
    string_set restored;
    stlplus::restore_from_file(DATA,restored,restore_string_set,0);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      string_set master;
      stlplus::restore_from_file(MASTER,master,restore_string_set,0);
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
