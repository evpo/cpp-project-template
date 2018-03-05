#include <string>
#include <vector>
#include <iostream>
#include "smart_ptr.hpp"
#include "persistent_callback.hpp"
#include "persistent_string.hpp"
#include "persistent_vector.hpp"
#include "persistent_shortcuts.hpp"
#include "persistent_smart_ptr.hpp"
#include "file_system.hpp"
#include "strings.hpp"
#include "build.hpp"

#define NUMBER 1000
#define DATA "callback_test.tmp"
#define MASTER "callback_test.dump"

////////////////////////////////////////////////////////////////////////////////

static std::string to_string(int number)
{
  return stlplus::int_to_string(number);
}

////////////////////////////////////////////////////////////////////////////////

class base
{
public:
  int m_value;

  base(int value = 0) : m_value(value) {}
  virtual ~base(void) {}

  virtual int value (void) const {return m_value;}
  virtual void set(int value = 0) {m_value = value;}

  virtual base* clone(void) const {return new base(*this);}

  virtual std::ostream& print(std::ostream& str) const {return str << "base: " << m_value;}
};

std::ostream& operator << (std::ostream& str, const base& object)
{
  return object.print(str);
}

bool operator == (const base& left, const base& right)
{
  return typeid(left) == typeid(right) && left.value() == right.value();
}

void dump_base(stlplus::dump_context& context, const void* data)
{
  stlplus::dump_int(context,((const base*)data)->m_value);
}

void* create_base(void)
{
  return new base;
}

void restore_base(stlplus::restore_context& context, void* data)
{
  stlplus::restore_int(context,((base*)data)->m_value);
}

////////////////////////////////////////////////////////////////////////////////

class derived : public base
{
public:
  std::string m_image;

  derived(int value = 0) : base(value), m_image(to_string(value)) {}
  virtual ~derived(void) {}

  virtual void set(int value = 0) {m_image = to_string(value); base::set(value);}

  virtual base* clone(void) const {return new derived(*this);}

  virtual std::ostream& print(std::ostream& str) const {return str << "derived: " << m_image << "(" << value() << ")";}
};

void dump_derived(stlplus::dump_context& context, const void* data)
{
  dump_base(context,data);
  stlplus::dump_string(context,((const derived*)data)->m_image);
}

void* create_derived(void)
{
  return new derived;
}

void restore_derived(stlplus::restore_context& context, void* data)
{
  restore_base(context,data);
  stlplus::restore_string(context,((derived*)data)->m_image);
}

////////////////////////////////////////////////////////////////////////////////

typedef stlplus::smart_ptr_clone<base> base_ptr;

void dump_base_ptr(stlplus::dump_context& context, const base_ptr& ptr)
{
  stlplus::dump_smart_ptr_clone_callback(context,ptr);
}

void restore_base_ptr(stlplus::restore_context& context, base_ptr& ptr)
{
  stlplus::restore_smart_ptr_clone_callback(context,ptr);
}

void print_base(std::ostream& device, const base_ptr& ptr)
{
  if (!ptr)
    device << "<null>";
  else
    ptr->print(device);
}

std::ostream& operator<< (std::ostream& device, const base_ptr& ptr)
{
  print_base(device,ptr);
  return device;
}

////////////////////////////////////////////////////////////////////////////////

typedef std::vector<base_ptr> base_vector;

void dump_base_vector(stlplus::dump_context& context, const base_vector& vec)
{
  stlplus::dump_vector(context,vec,dump_base_ptr);
}

void restore_base_vector(stlplus::restore_context& context, base_vector& vec)
{
  stlplus::restore_vector(context,vec,restore_base_ptr);
}

void print_base_vector (std::ostream& device, const base_vector& data)
{
  stlplus::print_vector(device, data, print_base, "\n");
}

std::ostream& operator<< (std::ostream& device, const base_vector& data)
{
  print_base_vector(device,data);
  return device;
}

////////////////////////////////////////////////////////////////////////////////

void make_base_dump_persistent(stlplus::dump_context& context)
{
  context.register_callback(typeid(base),dump_base);
  context.register_callback(typeid(derived),dump_derived);
}

void make_base_restore_persistent(stlplus::restore_context& context)
{
  context.register_callback(create_base,restore_base);
  context.register_callback(create_derived,restore_derived);
}

bool compare(const base_vector& left, const base_vector& right)
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
    if (left[j].null() || right[j].null())
    {
      if (left[j].present() || right[j].present())
      {
        std::cerr << "base[" << j << "] left = " << left[j] << " is different from right = " << right[j] << std::endl;
        result = false;
      }
    }
    else if (!(*left[j] == *right[j]))
    {
      std::cerr << "base[" << j << "] left = \"" << *left[j] << "\" is different from right = \"" << *right[j] << "\"" << std::endl;
      result = false;
    }
  }
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing " << NUMBER << " objects" << std::endl;

  try
  {
    // build the data structure and dump it
    base_vector data;
    for (unsigned i = 0; i < NUMBER; i++)
    {
      switch(i % 3)
      {
      case 0:
        // add a null
        data.push_back(base_ptr());
        break;
      case 1:
        // add a base
        data.push_back(base_ptr(new base(i)));
        break;
      case 2:
        // add a derived
        data.push_back(base_ptr(new derived(i)));
        break;
      }
    }
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DATA,dump_base_vector,make_base_dump_persistent);

    // now restore the same file and compare it
    std::cerr << "restoring" << std::endl;
    base_vector restored;
    stlplus::restore_from_file(DATA,restored,restore_base_vector,make_base_restore_persistent);
    result &= compare(data,restored);

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DATA,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      base_vector master;
      stlplus:: restore_from_file(MASTER,master,restore_base_vector,make_base_restore_persistent);
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
