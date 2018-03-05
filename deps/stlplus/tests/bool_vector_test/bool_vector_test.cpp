#include "persistent_vector.hpp"
#include "persistent_shortcuts.hpp"
#include "file_system.hpp"
#include "strings.hpp"
#include "build.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

////////////////////////////////////////////////////////////////////////////////

#define SIZE 27
#define COUNT 25
#define DATA "bool_vector_test.tmp"
#define MASTER "bool_vector_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef std::vector<bool> bool_vector;

std::string to_string(const bool_vector& values)
{
  return stlplus::bool_vector_to_string(values);
}

std::ostream& operator << (std::ostream& device, const bool_vector& data)
{
  stlplus::print_bool_vector(device, data);
  return device;
}

////////////////////////////////////////////////////////////////////////////////

typedef std::vector<bool_vector> bool_vector_vector;

void dump_bool_vector_vector(stlplus::dump_context& str, const bool_vector_vector& data)
{
  stlplus::dump_vector(str, data, stlplus::dump_vector_bool);
}

void restore_bool_vector_vector(stlplus::restore_context& str, bool_vector_vector& data)
{
  stlplus::restore_vector(str, data, stlplus::restore_vector_bool);
}

bool compare (bool_vector_vector& left, bool_vector_vector& right)
{
  bool result = true;
  if (left.size() != right.size())
  {
    std::cerr << "different size - left = " << left.size() << " right = " << right.size() << std::endl;
    result = false;
  }
  else
  {
    for(unsigned j = 0; j < left.size(); j++)
    {
      if (left[j] != right[j])
      {
        std::cerr << "vector[" << j << "] is different: left = " << left[j] << " right = " << right[j] << std::endl;
        result = false;
      }
    }
  }
  return result;
}

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing " << COUNT << " bool_vector size " << SIZE << std::endl;

  try
  {
    // first build a vector of bit-vectors and dump it
    std::cerr << "dumping" << std::endl;
    bool_vector_vector data;
    for (unsigned long i = 0; i < COUNT; i++)
    {
      bool_vector element;
      unsigned long sample = i * i * i;
      unsigned long mask = 1;
      for (unsigned j = 0; j < SIZE; j++)
      {
        element.push_back( ((sample & mask) != 0) );
        mask <<= 1;
      }
      std::cerr << "sample = " << sample << ", vector = " << element << std::endl;
      data.push_back(element);
    }
    stlplus::dump_to_file(data,DATA,dump_bool_vector_vector,0);

    // now restore a copy and compare it
    std::cerr << "restoring" << std::endl;
    bool_vector_vector restored;
    stlplus::restore_from_file(DATA,restored,restore_bool_vector_vector,0);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      bool_vector_vector master;
      stlplus::restore_from_file(MASTER,master,restore_bool_vector_vector,0);
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
