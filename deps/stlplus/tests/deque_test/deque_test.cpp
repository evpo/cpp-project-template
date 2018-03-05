#include "persistent_deque.hpp"
#include "persistent_string.hpp"
#include "persistent_shortcuts.hpp"
#include "file_system.hpp"
#include "build.hpp"
#include <string>
#include <deque>

////////////////////////////////////////////////////////////////////////////////

#define SIZE 1000
#define NUMBER 1000
#define DATA "deque_test.tmp"
#define MASTER "deque_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef std::deque<std::string> string_deque;

void dump_string_deque(stlplus::dump_context& str, const string_deque& data)
{
  stlplus::dump_deque(str,data,stlplus::dump_string);
}

void restore_string_deque(stlplus::restore_context& str, string_deque& data)
{
  stlplus::restore_deque(str,data,stlplus::restore_string);
}

bool compare(const string_deque& left, string_deque& right)
{
  bool result = true;
  if (left.size() != right.size())
  {
    std::cerr << "different size - left = " << left.size() << " right = " << right.size() << std::endl;
    result = false;
  }
  string_deque::const_iterator i, j;
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

int main(int argc, char* argv[])
{
  std::cerr << stlplus::build() << " testing " << NUMBER << " deque elements of " << SIZE << "-byte strings" << std::endl;
  bool result = true;

  try
  {
    // create a data set and dump it to file
    std::cerr << "dumping" << std::endl;
    string_deque data;
    for (unsigned k = 0; k < NUMBER; k++)
    {
      std::string value;
      for  (unsigned j = 0; j < SIZE; j++)
        value += (char)(k * j);
      data.push_back(value);
    }
    stlplus::dump_to_file(data,DATA,dump_string_deque,0);

    // now restore the same file and compare the data sets
    std::cerr << "restoring" << std::endl;
    string_deque restored;
    stlplus::restore_from_file(DATA,restored,restore_string_deque,0);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      string_deque master;
      stlplus::restore_from_file(MASTER,master,restore_string_deque,0);
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
    std::cerr << "test succeeded" << std::endl;
  return result ? 0 : 1;
}
