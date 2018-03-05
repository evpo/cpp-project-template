#include "build.hpp"
#include "tcp_sockets.hpp"
#include "string_utilities.hpp"
#include "subprocesses.hpp"
#include "file_system.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////
// TCP test: this program is designed to be run twice - once without an
// argument as the server, which itself runs a second copy with an argument as
// the client. The two instances then swap information using TCP
////////////////////////////////////////////////////////////////////////////////

unsigned timeout = 10000000;

////////////////////////////////////////////////////////////////////////////////
// Server code
// sets up a TCP server then spawns this program again in client mode

int server(std::vector<std::string> values, int port, std::string command, std::string logname)
{
  int errors = 0;
  std::cerr << "server: " << stlplus::build() << std::endl;
  // this is the first-run instance and acts as the server
  // setup a server on the shared port
  std::cerr << "server: started" << std::endl;
  stlplus::TCP_server server(port);
  std::cerr << "server: set up TCP server on port " << port << std::endl;
  // now spawn a second copy of this program
  stlplus::async_subprocess client;
  client.spawn(command);
  std::cerr << "server: spawned client with command: " << command << std::endl;
  // now wait for a client connection to come in
  std::cerr << "server: waiting for incoming connection" << std::endl;
  if (!server.accept_ready(timeout))
  {
    std::cerr << "server: ERROR: incoming connection failed, error code: " << server.message() << std::endl;
    ++errors;
  }
  else
  {
    std::cerr << "server: accepting incoming connection" << std::endl;
    stlplus::TCP_connection connection = server.accept();
    unsigned request = 0;
    while(connection.initialised())
    {
      std::cerr << "server: checking for receive request" << std::endl;
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
        if (!connection.receive(data))
        {
          std::cerr << "server: ERROR: connection receive failed after receive_ready was true" << std::endl;
          ++errors;
        }
        else if (data.size() == 0)
        {
          // legitimate (?) last receive of an empty string
          std::cerr << "server: received empty string \"" << data << "\"" << std::endl;
        }
        else
        {
          // report the value and check it's correct
          std::cerr << "server: received \"" << data << "\"" << std::endl;
          std::string value = std::string("error");
          if (request < values.size())
            value = values[request];
          if (data != value)
          {
            std::cerr << "server: ERROR: received data \"" << data << "\" != \"" << value << "\"" << std::endl;
            ++errors;
          }
          // now throw the same data back
          if (!connection.send_ready(timeout))
          {
            std::cerr << "server: ERROR: wait for send timed out" << std::endl;
            ++errors;
          }
          else
          {
            std::cerr << "server: sending \"" << data << "\"" << std::endl;
            if (!connection.send(data))
            {
              std::cerr << "server: ERROR: connection send failed after send_ready was true" << std::endl;
              ++errors;
            }
          }
          ++request;
        }            
      }
      // just check for client having exited
      if (!client.tick())
      {
        std::cerr << "server: client has exited - closing connection" << std::endl;
        connection.close();
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
    if (request < values.size())
    {
      std::cerr << "server: ERROR: client has not sent enough samples, received=" << request << ", expected=" << values.size() << std::endl;
      ++errors;
    }
  }
  if (errors > 0)
  {
    std::cerr << "server: ERROR: exiting with " << errors << " errors" << std::endl;
      if (stlplus::file_exists(logname))
      {
        std::ifstream input(logname.c_str());
        std::string line;
        while (std::getline(input,line))
          std::cerr << line << std::endl;
      }
  }
  else
    std::cerr << "server: SUCCESS: exiting with " << errors << " errors" << std::endl;
  return errors;
}

////////////////////////////////////////////////////////////////////////////////
// client

int client(std::vector<std::string> values, int port, std::string logname)
{
  int errors = 0;
  // this is the second-run instance and acts as the client
  // write to a log file to avoid clashes
  if (stlplus::file_exists(logname))
    stlplus::file_delete(logname);
  std::ofstream logfile("test.tmp");
  logfile << "client: " << stlplus::build() << std::endl;
  logfile << "client: creating connection" << std::endl;
  stlplus::TCP_client connection("localhost", port, timeout);
  unsigned request = 0;
  if (!connection.connected())
  {
      logfile << "client: ERROR: connection failed with code " << connection.error() << std::endl;
      ++errors;
  }
  else while(connection.connected())
  {
    std::string value = values[request];
    // send this value
    logfile << "client: preparing to send data" << std::endl;
    if (!connection.send_ready(timeout))
    {
      logfile << "client: ERROR: wait for send timed out" << std::endl;
      ++errors;
    }
    else
    {
      // avoid the data being consumed by the send
      std::string data = value;
      logfile << "client: sending \"" << data << "\"" << std::endl;
      if (!connection.send(data))
      {
        logfile << "client: ERROR: connection send failed after send_ready was true" << std::endl;
        ++errors;
      }
    }
    // check connection for incoming request
    if (!connection.receive_ready(timeout))
    {
      logfile << "client: ERROR: wait for receive timed out" << std::endl;
      ++errors;
    }
    else
    {
      // receive the request
      std::string data;
      if (!connection.receive(data))
      {
        logfile << "client: ERROR: connection receive failed after receive_ready was true" << std::endl;
        ++errors;
      }
      else
      {
        // report the value and check it's correct
        logfile << "client: received \"" << data << "\"" << std::endl;
        if (data != value)
        {
          logfile << "client: ERROR: received data \"" << data << "\" != \"" << value << "\"" << std::endl;
          ++errors;
        }
        request++;
      }            
    }
    if (request >= values.size())
    {
      logfile << "client: all values sent, closing connection" << std::endl;
      connection.close();
    }
  }
  logfile << "client: exited communication loop" << std::endl;
  logfile << "client: " << (errors ? "ERROR:" : "SUCCESS:") << " exiting with " << errors << " errors" << std::endl;
  return errors;
}

////////////////////////////////////////////////////////////////////////////////
// main program

int main (int argc, char* argv[])
{
  // code common to both instances to ensure both have the same information
  std::vector<std::string> values = stlplus::split("one:two:three", ":");
  int port = 3000;
  std::string logname = "test.tmp";

  // now branch for the two instances
  int errors = 0;
  if (argc == 1)
  {
    std::string command = std::string(argv[0]) + std::string(" client");
    errors = server(values, port, command, logname);
  }
  else
  {
    errors = client(values, port, logname);
  }
  return errors;
}
