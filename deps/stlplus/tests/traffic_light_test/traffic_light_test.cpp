#include "build.hpp"
#include "file_system.hpp"
#include "persistent_contexts.hpp"
#include "persistent_enum.hpp"
#include "persistent_shortcuts.hpp"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

enum traffic_light {red, red_amber, green, amber};

std::ostream& operator<< (std::ostream& str, const traffic_light& light)
{
  switch(light)
  {
  case red:
    str << "red";
    break;
  case red_amber:
    str << "red_amber";
    break;
  case green:
    str << "green";
    break;
  case amber:
    str << "amber";
    break;
  default:
    str << "Error:" << ((int)light);
    break;
  }
  return str;
}

void dump_traffic_light(stlplus::dump_context& context, const traffic_light& lights)
{
  stlplus::dump_enum(context, lights);
}

void restore_traffic_light(stlplus::restore_context& context, traffic_light& lights)
{
  stlplus::restore_enum(context, lights);
}

////////////////////////////////////////////////////////////////////////////////

#define DUMP "traffic_light_test.tmp"
#define MASTER "traffic_light_test.dump"

int main(int argc, char* argv[])
{
  bool result = true;
  std::cerr << stlplus::build() << " testing traffic_light" << std::endl;

  try
  {
    // create a traffic_light
    traffic_light data = green;
    std::cerr << data << std::endl;

    // now dump it
    std::cerr << "dumping" << std::endl;
    stlplus::dump_to_file(data,DUMP,dump_traffic_light,0);

    // now restore a copy and compare it
    std::cerr << "restoring" << std::endl;
    traffic_light restored;
    stlplus::restore_from_file(DUMP,restored,restore_traffic_light,0);
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
      traffic_light master;
      stlplus::restore_from_file(MASTER,master,restore_traffic_light,0);
      if (data == master)
        std::cerr << "Traffic_Light is same as master" << std::endl;
      else
      {
        std::cerr << "Error: traffic_light is different to master" << std::endl;
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
