#include "tcpserver.h"
#include "tcpconnection.h"
#include "request.h"
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <ctime>

using boost::asio::ip::tcp;
using boost::system::error_code;
typedef boost::shared_ptr<Tcpconnection> connection_ptr;

Tcpserver::Tcpserver(boost::asio::io_service& io_service, const std::string& host, const std::string& port, std::string& outfile)
      : io_service_(io_service),
        resolver_(io_service),
        acceptor_(io_service),
        outfile_(outfile)
{
    namespace pls = std::placeholders;

    resolver_.async_resolve(tcp::resolver::query(host, port), bind(&Tcpserver::handle_resolve, this, pls::_1, pls::_2));
}

std::string Tcpserver::make_daytime_string()
{
    char buffer[80];
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

std::string Tcpserver::make_log_line(const std::string& message)
{
    return make_daytime_string() + " " + message + "\n";
}

void Tcpserver::write_log(const std::string& message)
{
    if (!outfile_.empty())
        std::ofstream(outfile_, std::ios::app) << make_log_line(message);
    else
        std::cout << make_log_line(message);
}

void Tcpserver::handle_accept(connection_ptr connection, const error_code& error)
{
    if (!error)
    {
        write_log(connection->socket().remote_endpoint().address().to_string());

        connection->start();

        start_accept();
    }
    else
    {
        std::cout << "Error async_accept: " << error.message() << "\n";
        return;
    }
}

void Tcpserver::start_accept()
{
     connection_ptr connection(new Tcpconnection(io_service_));
     acceptor_.async_accept(connection->socket(), bind(&Tcpserver::handle_accept, this, connection, boost::asio::placeholders::error));
}

void Tcpserver::handle_resolve(const error_code& err, tcp::resolver::iterator endpoint_iterator)
{
    if (!err)
    {
        try
        {
            tcp::resolver::iterator end;
            while (endpoint_iterator != end)
            {
                tcp::endpoint ep = *endpoint_iterator;
                try
                {
                    acceptor_.open(ep.protocol());
                    acceptor_.bind(ep);
                    acceptor_.listen();

                    start_accept();

                    break;
                }
                catch (const std::exception& e)
                {
                    acceptor_.close();
                    std::cout << "Exception: " << std::string(e.what()) << "\n";
                }
                endpoint_iterator++;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Exception: " << std::string(e.what()) << "\n";
        }
    }
    else
    {
        std::cout << "Error: " << err.message() << "\n";
    }
}