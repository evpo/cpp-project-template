#include "build.hpp"
#include "udp_sockets.hpp"
#include "string_utilities.hpp"
#include "subprocesses.hpp"
#include <vector>
#include <string>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// UDP test: this program is designed to be run twice - once without an
// argument as the server, which itself runs a second copy with an argument as
// the client. The two instances then swap information using UDP
////////////////////////////////////////////////////////////////////////////////

unsigned timeout = 10000000;

////////////////////////////////////////////////////////////////////////////////
// Server code
// sets up a UDP server then spawns this program again in client mode

int server(std::vector<std::string> values, int port, const std::string& command)
{
  int errors = 0;
  std::cerr << "server: " << stlplus::build() << std::endl;
  // this is the first-run instance and acts as the server
  // setup a server on the shared port
  std::cerr << "server: started" << std::endl;
  stlplus::UDP_server connection(port);
  std::cerr << "server: set up UDP server on port " << port << std::endl;
  // now spawn a second copy of this program
  stlplus::async_subprocess client;
  client.spawn(command);
  std::cerr << "server: spawned client with command: " << command << std::endl;
  // UDP servers just respond immediately to incoming datagrams
  std::cerr << "server: waiting for incoming connection" << std::endl;
  for (unsigned request = 0; request < values.size(); request++)
  {
    std::cerr << "server: checking for receive ready" << std::endl;
    // check connection for incoming request
    if (!connection.receive_ready(timeout))
    {
      std::cerr << "server: ERROR: wait for receive timed out" << std::endl;
      ++errors;
    }
    else
    {
      // receive the request
      std::string data;
      unsigned long remote_address = 0;
      unsigned short remote_port = 0;
      if (!connection.receive(data, remote_address, remote_port))
      {
        std::cerr << "server: ERROR: connection receive failed after receive_ready was true" << std::endl;
        ++errors;
      }
      else
      {
        // report the value and check it's correct
        std::cerr << "server: received \"" << data << "\" from " << remote_address << ":" << remote_port << std::endl;
        std::string value = std::string("error");
        if (request < values.size())
          value = values[request];
        if (data != value)
        {
          std::cerr << "server: ERROR: received data \"" << data << "\" != \"" << value << "\"" << std::endl;
          ++errors;
        }
        // now throw the same data back
        std::cerr << "server: testing for send ready" << std::endl;
        if (!connection.send_ready(timeout))
        {
          std::cerr << "server: ERROR: wait for send timed out" << std::endl;
          ++errors;
        }
        else
        {
          std::cerr << "server: sending \"" << data << "\"" << std::endl;
          if (!connection.send(data, remote_address, remote_port))
          {
            std::cerr << "server: ERROR: connection send failed after send_ready was true" << std::endl;
            ++errors;
          }
        }
      }            
    }
  }
  std::cerr << "server: exited server loop" << std::endl;
  // wait for client to exit
  while (client.tick())
  {
    std::cerr << "server: client has not exited - waiting" << std::endl;
    sleep(1);
  }
  if (client.exit_status() != 0)
  {
    std::cerr << "server: ERROR: client has exited with status " << client.exit_status() << std::endl;
    errors += client.exit_status();
  }
  std::cerr << "server: " << (errors ? "ERROR:" : "SUCCESS:") << " exiting with " << errors << " errors" << std::endl;
  return errors;
}

////////////////////////////////////////////////////////////////////////////////
// client

int client(std::vector<std::string> values, int port)
{
  int errors = 0;
  // this is the second-run instance and acts as the client
  std::cerr << "client: " << stlplus::build() << std::endl;
  std::cerr << "client: creating connection" << std::endl;
  stlplus::UDP_client connection("localhost", port, timeout);
  for(unsigned request = 0; request < values.size(); request++)
  {
    std::string value = values[request];
    // send this value
    std::cerr << "client: testing for send ready" << std::endl;
    if (!connection.send_ready(timeout))
    {
      std::cerr << "client: ERROR: wait for send timed out" << std::endl;
      ++errors;
    }
    else
    {
      // avoid the data being consumed by the send
      std::string data = value;
      std::cerr << "client: sending \"" << data << "\"" << std::endl;
      if (!connection.send(data))
      {
        std::cerr << "client: ERROR: connection send failed after send_ready was true" << std::endl;
        ++errors;
      }
    }
    // check connection for incoming request
    std::cerr << "client: testing for receive ready" << std::endl;
    if (!connection.receive_ready(timeout))
    {
      std::cerr << "client: ERROR: wait for receive timed out" << std::endl;
      ++errors;
    }
    else
    {
      // receive the request
      std::string data;
      if (!connection.receive(data))
      {
        std::cerr << "client: ERROR: connection receive failed after receive_ready was true" << std::endl;
        ++errors;
      }
      else
      {
        // report the value and check it's correct
        std::cerr << "client: received \"" << data << "\"" << std::endl;
        if (data != value)
        {
          std::cerr << "client: ERROR: received data \"" << data << "\" != \"" << value << "\"" << std::endl;
          ++errors;
        }
      }            
    }
  }
  std::cerr << "client: exited communication loop" << std::endl;
  std::cerr << "client: " << (errors ? "ERROR:" : "SUCCESS:") << " exiting with " << errors << " errors" << std::endl;
  return errors;
}

////////////////////////////////////////////////////////////////////////////////
// main program

int main (int argc, char* argv[])
{
  // code common to both instances to ensure both have the same information
  std::vector<std::string> values = stlplus::split("one:two:three", ":");
  int port = 3000;

  // now branch for the two instances
  int errors = 0;
  if (argc == 1)
  {
    std::string command = std::string(argv[0]) + std::string(" client");
    errors = server(values, port, command);
  }
  else
  {
    errors = client(values, port);
  }
  return errors;
}
