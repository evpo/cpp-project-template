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
  str << "[" << pt.m_x << ":" << pt.m_y << ":" << pt.m_z << "]";
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

class edge
{
private:
  point m_begin;
  point m_end;

public:
  edge(const point& begin = point(), const point& end = point()) : m_begin(begin), m_end(end) {}
  ~edge(void) {}
  edge (const edge& e) : m_begin(e.m_begin), m_end(e.m_end) {}
  const edge& operator=(const edge& e) {m_begin = e.m_begin; m_end = e.m_end; return *this;}

  bool operator==(const edge& e) const {return (m_begin == e.m_begin) && (m_end == e.m_end);}

  friend std::ostream& operator<< (std::ostream& str, const edge& e);

  const point& begin(void) const {return m_begin;}
  const point& end(void) const {return m_end;}

  void set_begin(const point& begin) {m_begin = begin;}
  void set_end(const point& end) {m_end = end;}

  void set(const point& begin = 0, const point& end = 0) {m_begin = begin; m_end = end;}

  friend void dump_edge(stlplus::dump_context& context, const edge& e);
  friend void restore_edge(stlplus::restore_context& context, edge& e);
};

std::ostream& operator<< (std::ostream& str, const edge& e)
{
  str << e.m_begin << "-" << e.m_end;
  return str;
}

void dump_edge(stlplus::dump_context& context, const edge& e)
{
  dump_point(context,e.m_begin);
  dump_point(context,e.m_end);
}

void restore_edge(stlplus::restore_context& context, edge& e)
{
  restore_point(context,e.m_begin);
  restore_point(context,e.m_end);
}

////////////////////////////////////////////////////////////////////////////////

#define DUMP "edge_test.tmp"
#define MASTER "edge_test.dump"

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing edge" << std::endl;

  try
  {
    // create an edge
    edge data(point(0,0,0),point(1,1,1));
    std::cerr << data << std::endl;

    // now dump it
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DUMP,dump_edge,0);

    // now restore a copy and compare it
    std::cerr << "restoring" << std::endl;
    edge restored;
    stlplus::restore_from_file(DUMP,restored,restore_edge,0);
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
      edge master;
      stlplus::restore_from_file(MASTER,master,restore_edge,0);
      if (data == master)
        std::cerr << "Edge is same as master" << std::endl;
      else
      {
        std::cerr << "Error: edge is different to master" << std::endl;
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
