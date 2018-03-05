#include "persistence.hpp"
#include "file_system.hpp"
#include "string_stl.hpp"
#include "string_basic.hpp"
#include "build.hpp"
#include <string>
#include <list>
#include <iostream>

#define SIZE 50
#define NUMBER 50
#define DATA "list_test.tmp"
#define MASTER "list_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef std::list<std::string> string_list;

std::ostream& operator<< (std::ostream& str, const string_list& data)
{
  return str << stlplus::list_to_string(data, stlplus::string_to_string, ",");
}

void dump_string_list(stlplus::dump_context& context, const string_list& data)
{
  stlplus::dump_list(context,data,stlplus::dump_string);
}
void restore_string_list(stlplus::restore_context& context, string_list& data)
{
  stlplus::restore_list(context,data,stlplus::restore_string);
}

bool compare(const string_list& left, const string_list& right)
{
  bool result = true;
  if (left.size() != right.size())
  {
    std::cerr << "different size - left = " << left.size() << " right = " << right.size() << std::endl;
    result = false;
  }
  string_list::const_iterator i, j;
  for (i = left.begin(), j = right.begin(); i != left.end() && j != right.end(); i++, j++)
  {
    if (*i != *j)
    {
      std::cerr << "left = \"" << *i << "\" is different from right = \"" << *j << "\"" << std::endl;
      result = false;
    }
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing " << NUMBER << " list elements of " << SIZE << "-byte strings" << std::endl;

  try
  {
    // build the sample data structure
    std::cerr << "building" << std::endl;
    string_list data;
    for (unsigned k = 0; k < NUMBER; k++)
    {
      std::string value;
      for (unsigned j = 0; j < SIZE; j++)
        value += (char)(k + j + 32);
      data.push_back(value);
    }
    std::cerr << data << std::endl;
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DATA,dump_string_list,0);

    // now restore the same file and compare
    std::cerr << "restoring" << std::endl;
    string_list restored;
    stlplus::restore_from_file(DATA,restored,restore_string_list,0);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      string_list master;
      stlplus::restore_from_file(MASTER,master,restore_string_list,0);
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
