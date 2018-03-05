#include "build.hpp"
#include "file_system.hpp"
#include "persistent_contexts.hpp"
#include "persistent_int.hpp"
#include "persistent_shortcuts.hpp"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

class point
{
private:
  int m_x;
  int m_y;
  int m_z;
public:
  point(int x = 0, int y = 0, int z = 0) : m_x(x), m_y(y), m_z(z) {}
  ~point(void) {}
  point (const point& pt) : m_x(pt.m_x), m_y(pt.m_y), m_z(pt.m_z) {}
  const point& operator=(const point& pt) {m_x = pt.m_x; m_y = pt.m_y; m_z = pt.m_z; return *this;}

  bool operator==(const point& pt) const {return (m_x == pt.m_x) && (m_y == pt.m_y) && (m_z == pt.m_z);}

  friend std::ostream& operator<< (std::ostream& str, const point& pt);

  int x(void) const {return m_x;}
  int y(void) const {return m_y;}
  int z(void) const {return m_z;}

  void set_x(int x) {m_x = x;}
  void set_y(int y) {m_y = y;}
  void set_z(int z) {m_z = z;}

  void set(int x = 0, int y = 0, int z = 0) {m_x = x; m_y = y; m_z = z;}

  friend void dump_point(stlplus::dump_context& context, const point& pt);
  friend void restore_point(stlplus::restore_context& context, point& pt);
};

std::ostream& operator<< (std::ostream& str, const point& pt)
{
  str << pt.m_x << ":" << pt.m_y << ":" << pt.m_z;
  return str;
}

void dump_point(stlplus::dump_context& context, const point& pt)
{
  stlplus::dump_int(context, pt.m_x);
  stlplus::dump_int(context, pt.m_y);
  stlplus::dump_int(context, pt.m_z);
}

void restore_point(stlplus::restore_context& context, point& pt)
{
  stlplus::restore_int(context, pt.m_x);
  stlplus::restore_int(context, pt.m_y);
  stlplus::restore_int(context, pt.m_z);
}

////////////////////////////////////////////////////////////////////////////////

#define DUMP "point_test.tmp"
#define MASTER "point_test.dump"

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing point" << std::endl;

  try
  {
    // create a point
    point data(1,2,3);
    std::cerr << data << std::endl;

    // now dump it
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DUMP,dump_point,0);

    // now restore a copy and compare it
    std::cerr << "restoring" << std::endl;
    point restored;
    stlplus::restore_from_file(DUMP,restored,restore_point,0);
    std::cerr << restored << std::endl;
    if (data == restored)
      std::cerr << "Restored is same as dumped" << std::endl;
    else
    {
      std::cerr << "Error: Restored is different to dumped" << std::endl;
      result = false;
    }

    // compare with the master dump if present
    if (!stlplus::file_exists(MASTER))
      stlplus::file_copy(DUMP,MASTER);
    else
    {
      std::cerr << "restoring master" << std::endl;
      point master;
      stlplus::restore_from_file(MASTER,master,restore_point,0);
      if (data == master)
        std::cerr << "Point is same as master" << std::endl;
      else
      {
        std::cerr << "Error: point is different to master" << std::endl;
        result = false;
      }
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
