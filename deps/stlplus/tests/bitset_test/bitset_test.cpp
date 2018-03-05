#include "persistent_bitset.hpp"
#include "persistent_vector.hpp"
#include "persistent_shortcuts.hpp"
#include "file_system.hpp"
#include "strings.hpp"
#include "build.hpp"
#include <string>
#include <bitset>
#include <vector>

////////////////////////////////////////////////////////////////////////////////

#define SIZE 50
#define COUNT 100
#define DATA "bitset_test.tmp"
#define MASTER "bitset_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef std::bitset<SIZE> bitset_N;

void print_bitset_N(std::ostream& device, const bitset_N& values)
{
  stlplus::print_bitset(device, values);
}

std::ostream& operator<<(std::ostream& device, const bitset_N& values)
{
  print_bitset_N(device, values);
  return device;
}

void dump_bitset_N(stlplus::dump_context& device, const bitset_N& values)
{
  stlplus::dump_bitset(device, values);
}

void restore_bitset_N(stlplus::restore_context& device, bitset_N& values)
{
  stlplus::restore_bitset(device, values);
}

////////////////////////////////////////////////////////////////////////////////

typedef std::vector<bitset_N> bitset_vector;

void print_bitset_vector(std::ostream& device, const bitset_vector& values)
{
  stlplus::print_vector(device, values, print_bitset_N, ",");
}

std::ostream& operator<<(std::ostream& device, const bitset_vector& values)
{
  print_bitset_vector(device, values);
  return device;
}

void dump_bitset_vector(stlplus::dump_context& device, const bitset_vector& values)
{
  stlplus::dump_vector(device, values, stlplus::dump_bitset<SIZE>);
}

void restore_bitset_vector(stlplus::restore_context& device, bitset_vector& values)
{
  stlplus::restore_vector(device, values, restore_bitset_N);
}

bool compare (const bitset_vector& left, const bitset_vector& right)
{
  bool result = true;
  if (left.size() != right.size())
  {
    std::cerr << "different size - left = " << left.size() << " right = " << right.size() << std::endl;
    result = false;
  }
  else
  {
  for (unsigned j = 0; j < left.size(); j++)
  {
    if (left[j] != right[j])
    {
      std::cerr << "bitset[" << j << "] is different: left = ";
	  stlplus::print_bitset(std::cerr, left[j]);
	  std::cerr << " right = ";
	  stlplus::print_bitset(std::cerr, right[j]);
	  std::cerr << std::endl;
      result = false;
    }
  }
  }
  return result;
}

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing " << COUNT << " bitset<" << SIZE << ">" << std::endl;

  try
  {
    // first build a vector of bit-sets and dump it
    std::cerr << "building" << std::endl;
    bitset_vector data;
    for (int i = 0; i < COUNT; i++)
      data.push_back(bitset_N(i));
    std::cerr << data << std::endl;
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DATA,dump_bitset_vector,0);

    // now restore a copy and compare it
    std::cerr << "restoring" << std::endl;
    bitset_vector restored;
    stlplus::restore_from_file(DATA,restored,restore_bitset_vector,0);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      bitset_vector master;
      stlplus::restore_from_file(MASTER,master,restore_bitset_vector,0);
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
