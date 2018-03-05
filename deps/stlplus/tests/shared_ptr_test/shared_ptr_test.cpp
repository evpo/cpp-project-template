#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "persistence.hpp"
#include "strings.hpp"
#include "build.hpp"
#include "file_system.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

#ifdef STLPLUS_HAS_CXX11

typedef shared_ptr<string> string_ptr;

void print(const string& label, const string_ptr& value)
{
  cout << label;
  // use two ways of printing to check both
  cout << " = " << stlplus::shared_ptr_to_string(value, stlplus::string_to_string) << " = ";
  stlplus::print_shared_ptr(cout, value, stlplus::print_string);
  cout << " pointer = " << value.get();
  cout << " count = " << value.use_count();
  cout << endl;
}

void dump_string_ptr(stlplus::dump_context& context, const string_ptr& data)
  
{
  stlplus::dump_shared_ptr(context, data, stlplus::dump_string);
}

void restore_string_ptr(stlplus::restore_context& context, string_ptr& data)
  
{
  stlplus::restore_shared_ptr(context, data, stlplus::restore_string);
}

////////////////////////////////////////////////////////////////////////////////

typedef pair<string_ptr,string_ptr> string_ptr_pair;

void print(const string& label, const string_ptr_pair& values)
{
  cout << "pair " << label << endl;
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

#endif

////////////////////////////////////////////////////////////////////////////////

static const char* DATA1 = "data1.tmp";
static const char* MASTER1 = "shared_ptr_test1.dump";

static const char* DATA2 = "data2.tmp";
static const char* MASTER2 = "shared_ptr_test2.dump";

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{
  cerr << stlplus::build() << endl;
  try
  {
    unsigned errors = 0;

#ifdef STLPLUS_HAS_CXX11

    // shared_ptr()
    string_ptr s0;
    print("created string_ptr s0", s0);

    // shared_ptr(T*)
    string_ptr s1(new string("s1"));
    print("created string_ptr s1(new string)", s1);

    // shared_ptr(shared_ptr<T>)
    string_ptr s2(s1);
    print("created string_ptr s2(s1)", s2);
    if (s1.get() != s2.get())
    {
      cerr << "error: s2 not an alias of s1" << endl;
      errors++;
    }

    // shared_ptr(T*)
    string_ptr s3(new string("s3"));
    print("created string_ptr s3(new string)", s3);

    // operator=(shared_ptr<T>)
    s3 = s2;
    print("s3 = s2", s3);
    if (s3.get() != s2.get())
    {
      cerr << "error: s3 not an alias of s2" << endl;
      errors++;
    }

    // reset(T*)
    s3.reset(new string("s3 reset"));
    print("s3.reset(new string)", s3);
    print("s2", s2);
    print("s1", s1);
    if (s3.get() == s2.get())
    {
      cerr << "error: s3 an alias of s2" << endl;
      errors++;
    }

    // operator=(string_ptr)
    s3 = string_ptr(new string("s3 copy"));
    print("s3 = string_ptr(new string)", s3);
    print("s2", s2);
    print("s1", s1);
    if (s3.get() == s2.get())
    {
      cerr << "error: s3 an alias of s2" << endl;
      errors++;
    }

    // create a pair containing two aliases
    string_ptr_pair p1 = make_pair(s1,s1);
    print("make_pair(s1,s1)",p1);

    // dump/restore
    stlplus::dump_to_file(p1, DATA1, dump_string_ptr_pair, 0);
    string_ptr_pair p2;
    stlplus::restore_from_file(DATA1, p2, restore_string_ptr_pair, 0);
    print("p2 = dump/restore(p1)", p2);
    if (p2.first.get() != p2.second.get())
    {
      cerr << "error: first not an alias of second" << endl;
      errors++;
    }

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER1))
    {
      cerr << "creating master 1" << endl;
      stlplus::file_copy(DATA1,MASTER1);
    }
    else
    {
      cerr << "restoring master 1" << endl;
      string_ptr_pair master;
      stlplus::restore_from_file(MASTER1,master,restore_string_ptr_pair,0);
      if (!compare(p1,master))
        errors++;
    }

    // dealias one of these
    p1.second.reset(new string("p1"));
    print("p1.second.reset(new ...)", p1);
    if (p1.first.get() == p1.second.get())
    {
      cerr << "error: first still an alias of second" << endl;
      errors++;
    }

    // dump/restore
    stlplus::dump_to_file(p1, DATA2, dump_string_ptr_pair, 0);
    string_ptr_pair p3;
    stlplus::restore_from_file(DATA2, p3, restore_string_ptr_pair, 0);
    print("p3 = dump/restore(p1)", p3);
    if (p3.first.get() == p3.second.get())
    {
      cerr << "error: first still an alias of second" << endl;
      errors++;
    }

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER2))
    {
      cerr << "creating master 2" << endl;
      stlplus::file_copy(DATA2,MASTER2);
    }
    else
    {
      cerr << "restoring master 2" << endl;
      string_ptr_pair master;
      stlplus::restore_from_file(MASTER2,master,restore_string_ptr_pair,0);
      if (!compare(p1,master))
        errors++;
    }

#else

    cerr << "Note: This compiler does not support shared_ptr" << endl;
    cerr << "      This is not considered an error because it is the expected outcome for this compiler" << endl;

#endif

    if (errors == 0)
      cerr << "No errors were found - test SUCCEEDED" << endl;
    else
      cerr << "ERRORS were found - test FAILED" << endl;
    return errors;
  }
  catch(exception& exception)
  {
    cerr << "exception: failed with " << exception.what() << endl;
    return -1;
  }
  return -2;
}
