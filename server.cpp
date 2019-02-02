#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <fstream>
#include <sstream>

#define PORT 3000
#define DATABASE_NAME "databaze.db"

using std::cout;
using std::endl;
using std::string;
using std::ofstream;
using boost::asio::ip::tcp;


class Message {
  private: 
    string read_(tcp::socket & socket);
    string dateNow();
    string timeNow();
  public:
    void clear() {
      std::stringstream message;
      message.str("");
    }
    string databaseLog(int id, tcp::socket & socket){
      //complete the log into one string
      std::stringstream message;
      message << id << "  " << dateNow() << "  " << timeNow() << "  " << read_(socket);
      return message.str();
    }
};

//read message from client
string Message::read_(tcp::socket & socket) {
       boost::asio::streambuf buf;
       boost::asio::read_until( socket, buf, "\n" );
       string data = boost::asio::buffer_cast<const char*>(buf.data());
       return data;
}

string Message::dateNow(){
  //what is the date today :)
  std::time_t now = time(0);
  struct tm* timeinfo;
  char buffer [80];

  timeinfo = localtime(&now);
  strftime (buffer,80,"%d.%m.%Y",timeinfo);
  return buffer;
}

string Message::timeNow(){
  //what is the time now :)
  std::time_t now = time(0);
  struct tm* timeinfo;
  char buffer [80];

  timeinfo = localtime(&now);
  strftime (buffer,80,"%H.%M.%S",timeinfo);
  return buffer;
}

class Database{
  //lame class with only one method, that writes log into the predefined txt file
  public:
    void write(string log, ofstream& db){
      db.open(DATABASE_NAME,std::ios::app);
      db << log;
      db.close();
    }
};


int main()
{
  short port = 3000;
  int ID = 1;
  Database dtb;
  Message mess;

  try
  {
    ofstream fileHandler;

    while (1){
//start service     
    boost::asio::io_service io_service;
//listen for new connection
      tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), PORT ));
//socket creation 
      tcp::socket socket_(io_service);
//waiting for connection
      acceptor_.accept(socket_);
//read message and write log to "database"
      dtb.write(mess.databaseLog(ID,socket_), fileHandler);
      ID++;
      mess.clear();
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

