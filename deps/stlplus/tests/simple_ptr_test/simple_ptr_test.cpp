#include "simple_ptr.hpp"
#include "persistent_simple_ptr.hpp"
#include "persistent_string.hpp"
#include "persistent_pair.hpp"
#include "persistent_shortcuts.hpp"
#include "string_utilities.hpp"
#include "string_string.hpp"
#include "string_pair.hpp"
#include "string_simple_ptr.hpp"
#include "build.hpp"
#include "file_system.hpp"
#include <iostream>
#include <vector>
#include <string>
////////////////////////////////////////////////////////////////////////////////

typedef stlplus::simple_ptr<std::string> string_ptr;

void print(const std::string& label, const string_ptr& value)
{
  std::cout << label;
  std::cout << " = " << stlplus::simple_ptr_to_string(value,stlplus::string_to_string,"<null>","(",")");
  std::cout << " pointer = " << value._pointer();
  std::cout << " count = " << value._count();
  std::cout << " aliases = " << value.alias_count();
  std::cout << std::endl;
}

void dump_string_ptr(stlplus::dump_context& context, const string_ptr& data)
  
{
  stlplus::dump_simple_ptr(context, data, stlplus::dump_string);
}

void restore_string_ptr(stlplus::restore_context& context, string_ptr& data)
  
{
  stlplus::restore_simple_ptr(context, data, stlplus::restore_string);
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

bool compare (string_ptr_pair& left, string_ptr_pair& right)
{
  return ((*left.first) == (*right.first)) && ((*left.second) == (*right.second));
}

////////////////////////////////////////////////////////////////////////////////

static const char* DATA1 = "data1.tmp";
static const char* MASTER1 = "simple_ptr_test1.dump";

static const char* DATA2 = "data2.tmp";
static const char* MASTER2 = "simple_ptr_test2.dump";

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{
  std::cerr << stlplus::build() << std::endl;
  try
  {
    std::string data = "one";
    unsigned errors = 0;
    // simple_ptr()
    string_ptr s0;
    print("created s0", s0);

    std::string value = data;
    // simple_ptr(T)
    string_ptr s1(value);
    print("created s1(value)", s1);
    // simple_ptr(simple_ptr<T>)
    string_ptr s2(s1);
    print("created s2(s1)", s2);
    if (!s1.aliases(s2))
    {
      std::cerr << "error: s2 not an alias of s1" << std::endl;
      errors++;
    }
    // simple_ptr(T*)
    string_ptr s3(new std::string(value));
    print("created s3(new string(value))", s3);
    // operator=(simple_ptr<T>)
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
    if (s3.aliases(s2))
    {
      std::cerr << "error: s3 an alias of s2" << std::endl;
      errors++;
    }
    // operator=(T*)
    s3 = new std::string(value);
    print("s3 = value", s3);
    print("s2", s2);
    print("s1", s1);
    if (s3.aliases(s2))
    {
      std::cerr << "error: s3 an alias of s2" << std::endl;
      errors++;
    }
    // create a pair containing two aliases
    string_ptr_pair p1 = std::make_pair(s1,s1);
    print("make_pair(s1,s1)",p1);
    // dump/restore
    stlplus::dump_to_file(p1, DATA1, dump_string_ptr_pair, 0);
    string_ptr_pair p2;
    stlplus::restore_from_file(DATA1, p2, restore_string_ptr_pair, 0);
    print("p2 = dump/restore(p1)", p2);
    if (!p2.first.aliases(p2.second))
    {
      std::cerr << "error: first not an alias of second" << std::endl;
      errors++;
    }
    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER1))
      stlplus::file_copy(DATA1,MASTER1);
    else
    {
      std::cerr << "restoring master 1" << std::endl;
      string_ptr_pair master;
      stlplus::restore_from_file(MASTER1,master,restore_string_ptr_pair,0);
      if (!compare(p1,master))
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
    stlplus::dump_to_file(p1, DATA2, dump_string_ptr_pair, 0);
    string_ptr_pair p3;
    stlplus::restore_from_file(DATA2, p3, restore_string_ptr_pair, 0);
    print("p3 = dump/restore(p1)", p3);
    if (p3.first.aliases(p3.second))
    {
      std::cerr << "error: first still an alias of second" << std::endl;
      errors++;
    }
    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER2))
      stlplus::file_copy(DATA2,MASTER2);
    else
    {
      std::cerr << "restoring master 2" << std::endl;
      string_ptr_pair master;
      stlplus::restore_from_file(MASTER2,master,restore_string_ptr_pair,0);
      if (!compare(p1,master))
        errors++;
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
