#include <string>
#include "persistent_matrix.hpp"
#include "persistent_string.hpp"
#include "persistent_shortcuts.hpp"
#include "dprintf.hpp"
#include "strings.hpp"
#include "file_system.hpp"
#include "build.hpp"

////////////////////////////////////////////////////////////////////////////////

#define R 10
#define C 20
#define DATA "matrix_test.tmp"
#define MASTER "matrix_test.dump"

////////////////////////////////////////////////////////////////////////////////

typedef stlplus::matrix<std::string> string_matrix;

std::ostream& operator<< (std::ostream& str, const string_matrix& data)
{
  stlplus::print_matrix(str, data, stlplus::print_string, ",", "\n");
  return str;
}

void dump_string_matrix(stlplus::dump_context& context, const string_matrix& data)
{
  stlplus::dump_matrix(context, data, stlplus::dump_string);
}

void restore_string_matrix(stlplus::restore_context& context, string_matrix& data)
{
  stlplus::restore_matrix(context, data, stlplus::restore_string);
}

bool compare(const string_matrix& left, const string_matrix& right)
{
  bool result = true;
  if (left.rows() != right.rows())
  {
    std::cerr << "different rows - left = " << left.rows() << " right = " << right.rows() << std::endl;
    result = false;
  }
  else if (left.columns() != right.columns())
  {
    std::cerr << "different columns - left = " << left.columns() << " right = " << right.columns() << std::endl;
    result = false;
  }
  else
  {
    for (unsigned r = 0; r < left.rows(); r++)
    {
      for (unsigned c = 0; c < left.columns(); c++)
      {
        if (left(r,c) != right(r,c))
        {
          std::cerr << "(" << r << "," << c << ")" 
                    << " left = \"" << left(r,c) << "\" different from right = \"" << right(r,c) << "\"" << std::endl;
          result = false;
        }
      }
    }
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing " << R << "*" << C << " mappings" << std::endl;

  try
  {
    // build the sample data structure
    std::cerr << "building" << std::endl;
    string_matrix data(R,C);
    for (unsigned r = 0; r < R; r++)
      for (unsigned c = 0; c < C; c++)
        data(r,c) = stlplus::dformat("%d:%d",r,c);
    std::cerr << data << std::endl;
    // now dump to the file
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DATA,dump_string_matrix,0);

    // now restore the same file and compare
    std::cerr << "restoring" << std::endl;
    string_matrix restored;
    stlplus::restore_from_file(DATA,restored,restore_string_matrix,0);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      string_matrix master;
      stlplus::restore_from_file(MASTER,master,restore_string_matrix,0);
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
