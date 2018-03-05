#include "build.hpp"
#include "tcp_sockets.hpp"
#include "string_utilities.hpp"
#include "subprocesses.hpp"
#include "file_system.hpp"
#include <vector>
#include <string>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Subprocess test: this program is designed to be run twice - once without an
// argument as the server, which itself runs a second copy with an argument as
// the client. The two instances then swap information using pipes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Server code
// spawns this program again in client mode

int server(const std::string& basic_text, const std::string& command)
{
  int errors = 0;
  std::cerr << "server: " << stlplus::build() << std::endl;

  ////////////////////////////////////////////////////////////////////////////////
  // environment

  stlplus::env_vector environment;
  if (environment.size() == 0)
  {
    std::cerr << "server: ERROR: no environment" << std::endl;
    errors++;
  }
  // else if (!environment.present("PWD"))
  // {
  //   std::cerr << "server: ERROR: no PWD in environment" << std::endl;
  //   errors++;
  // }
  // else
  // {
  //   std::cerr << "server: PWD=" << environment["PWD"] << std::endl;
  //   if (environment["PWD"] != stlplus::folder_current_full())
  //   {
  //     std::cerr << "server: ERROR: PWD is wrong, should be " << stlplus::folder_current_full() << std::endl;
  //     errors++;
  //   }
  // }

  std::cerr << "server: environment:" << std::endl;
  for (unsigned i = 0; i < environment.size(); i++)
  {
    std::pair<std::string,std::string> value = environment[i];
    std::cerr << i << ": " << value.first << "=" << value.second << std::endl;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Asynchronous subprocess test
  // spawn a client copy of this program and connect only the stdout pipe
  std::cerr << "server: **** Asynchronous subprocess test" << std::endl;
  stlplus::async_subprocess client;
  client.spawn(command, false, true, false);
  std::cerr << "server: spawned client with command: " << command << std::endl;
  // now wait for client text
  std::string async_text;
  for(;;)
  {
    // pause to let the client send some text
    sleep(1);
    std::cerr << "server: checking client for text" << std::endl;
    client.tick();
    // now get some text
    int read = client.read_stdout(async_text);
    if (read != -1)
    {
      std::cerr << "server: got " <<  read << " bytes" << std::endl;
    }
    else
    {
      client.close_stdout();
      std::cerr << "server: client end of text" << std::endl;
      break;
    }
  }
  // check the text received
  std::string expected_text = basic_text + basic_text + basic_text;
  if (async_text == expected_text)
    std::cerr << "server: SUCCESS: client sent correct text: " << async_text << std::endl;
  else
  {
    std::cerr << "server: ERROR: client sent wrong text: " << async_text << std::endl;
    std::cerr << "server:      :           correct text: " << expected_text << std::endl;
    ++errors;
  }
  if (client.exit_status() != 0)
  {
    std::cerr << "server: ERROR: client has exited with status " << client.exit_status() << std::endl;
    errors += client.exit_status();
  }
  ////////////////////////////////////////////////////////////////////////////////
  // Backtick subprocess test
  std::cerr << "server: **** Backtick subprocess test" << std::endl;
  std::vector<std::string> reply = stlplus::backtick(command);
  if ((reply.size() == 1) && (reply[0] == expected_text))
    std::cerr << "server: SUCCESS: backtick client sent correct text: " << async_text << std::endl;
  else
  {
    std::cerr << "server: ERROR: backtick client sent wrong text: " << stlplus::join(reply,"") << std::endl;
    std::cerr << "server:      :                    correct text: " << expected_text << std::endl;
    ++errors;
  }

  std::cerr << "server: " << (errors ? "ERROR:" : "SUCCESS:") << " exiting with " << errors << " errors" << std::endl;
  return errors;
}

////////////////////////////////////////////////////////////////////////////////
// client

int client(const std::string& text)
{
  int errors = 0;
  // this is the second-run instance and acts as the client
  std::cerr << "client: " << stlplus::build() << std::endl;
  // just send some text and exit
  // do this three times with pauses
  std::cout << text << std::flush;
  sleep(2);
  std::cout << text << std::flush;
  sleep(1);
  std::cout << text << std::flush;
  sleep(1);
  std::cerr << "client: " << (errors ? "ERROR:" : "SUCCESS:") << " exiting with " << errors << " errors" << std::endl;
  return errors;
}

////////////////////////////////////////////////////////////////////////////////
// main program

int main (int argc, char* argv[])
{
  // code common to both instances to ensure both have the same information
  std::string text = "Hello World";

  // now branch for the two instances
  int errors = 0;
  if (argc == 1)
  {
    std::string command = std::string(argv[0]) + std::string(" client");
    errors = server(text, command);
  }
  else
  {
    errors = client(text);
  }
  return errors;
}
