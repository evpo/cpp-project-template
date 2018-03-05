#include "smart_ptr.hpp"
#include "persistent_smart_ptr.hpp"
#include "persistent_string.hpp"
#include "persistent_pair.hpp"
#include "persistent_shortcuts.hpp"
#include "string_utilities.hpp"
#include "string_string.hpp"
#include "string_pair.hpp"
#include "string_smart_ptr.hpp"
#include "build.hpp"
#include <iostream>
#include <vector>
#include <string>
////////////////////////////////////////////////////////////////////////////////

typedef stlplus::smart_ptr<std::string> string_ptr;

void print(const std::string& label, const string_ptr& value)
{
  std::cout << label;
  std::cout << " = " << stlplus::smart_ptr_to_string(value,stlplus::string_to_string,"<null>","(",")");
  std::cout << " handle = " << value._handle();
  std::cout << " aliases = " << value.alias_count();
  std::cout << std::endl;
}

void dump_string_ptr(stlplus::dump_context& context, const string_ptr& data)
  
{
  stlplus::dump_smart_ptr(context, data, stlplus::dump_string);
}

void restore_string_ptr(stlplus::restore_context& context, string_ptr& data)
  
{
  stlplus::restore_smart_ptr(context, data, stlplus::restore_string);
}

////////////////////////////////////////////////////////////////////////////////

typedef std::pair<string_ptr,string_ptr> string_ptr_pair;

void print(const std::string& label, const string_ptr_pair& values)
{
  std::cout << "pair " << label << std::endl;
  print("first:", values.first);
  print("second:", values.second);
}

void dump_string_ptr_pair(stlplus::dump_context& context, const string_ptr_pair& data)
  
{
  stlplus::dump_pair(context, data, dump_string_ptr, dump_string_ptr);
}

void restore_string_ptr_pair(stlplus::restore_context& context, string_ptr_pair& data)
  
{
  stlplus::restore_pair(context, data, restore_string_ptr, restore_string_ptr);
}

////////////////////////////////////////////////////////////////////////////////

static const char* DATA = "data.tmp";

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{
  std::cerr << stlplus::build() << std::endl;
  try
  {
    std::vector<std::string> data = stlplus::split("one:two:three",":");
    unsigned errors = 0;
    // smart_ptr()
    string_ptr s0;
    print("created s0", s0);
    for (unsigned i = 0; i < data.size(); i++)
    {
      std::string value = data[i];
      // smart_ptr(T)
      string_ptr s1(value);
      print("created s1(value)", s1);
      // smart_ptr(smart_ptr<T>)
      string_ptr s2(s1);
      print("created s2(s1)", s2);
      if (!s1.aliases(s2))
      {
        std::cerr << "error: s2 not an alias of s1" << std::endl;
        errors++;
      }
      // smart_ptr(T*)
      string_ptr s3(new std::string(value));
      print("created s3(new string(value))", s3);
      // operator=(smart_ptr<T>)
      s3 = s2;
      print("s3 = s2", s3);
      if (!s3.aliases(s2))
      {
        std::cerr << "error: s3 not an alias of s2" << std::endl;
        errors++;
      }
      // operator=(T)
      s3 = value;
      print("s3 = value", s3);
      print("s2", s2);
      print("s1", s1);
      if (!s3.aliases(s2))
      {
        std::cerr << "error: s3 not an alias of s2" << std::endl;
        errors++;
      }
      // operator=(T*)
      s3 = new std::string(value);
      print("s3 = value", s3);
      print("s2", s2);
      print("s1", s1);
      if (!s3.aliases(s2))
      {
        std::cerr << "error: s3 not an alias of s2" << std::endl;
        errors++;
      }
      // create a pair containing two aliases
      string_ptr_pair p1 = std::make_pair(s1,s1);
      print("make_pair(s1,s1)",p1);
      // dump/restore
      stlplus::dump_to_file(p1, DATA, dump_string_ptr_pair, 0);
      string_ptr_pair p2;
      stlplus::restore_from_file(DATA, p2, restore_string_ptr_pair, 0);
      print("p2 = dump/restore(p1)", p2);
      if (!p2.first.aliases(p2.second))
      {
        std::cerr << "error: first not an alias of second" << std::endl;
        errors++;
      }
      // dealias one of these
      p1.second.make_unique();
      print("p1.second.make_unique()", p1);
      if (p1.first.aliases(p1.second))
      {
        std::cerr << "error: first still an alias of second" << std::endl;
        errors++;
      }
      // dump/restore
      stlplus::dump_to_file(p1, DATA, dump_string_ptr_pair, 0);
      string_ptr_pair p3;
      stlplus::restore_from_file(DATA, p3, restore_string_ptr_pair, 0);
      print("p3 = dump/restore(p1)", p3);
      if (p3.first.aliases(p3.second))
      {
        std::cerr << "error: first still an alias of second" << std::endl;
        errors++;
      }
    }
    if (errors == 0)
      std::cerr << "No errors were found - test SUCCEEDED" << std::endl;
    else
      std::cerr << "ERRORS were found - test FAILED" << std::endl;
    return errors;
  }
  catch(std::exception& exception)
  {
    std::cerr << "exception: failed with " << exception.what() << std::endl;
    return -1;
  }
  return -2;
}
