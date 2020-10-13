#ifndef CONNECTION_H
#define CONNECTION_H

#include "request.h"
#include <boost/asio.hpp>
#include <string>
#include <dirent.h> //struct dirent, opendir, readdir, closedir

class Connection
    : public std::enable_shared_from_this<Connection>
{
    public:
        Connection(boost::asio::io_service& io_service, std::string& work_dir);

        void start();
        boost::asio::ip::tcp::socket& socket();

    private:
        size_t read_complete(const boost::system::error_code& error, size_t bytes);
        std::vector<char> string_to_vector_char(std::string str);
        std::vector<char> make_index(DIR *dir, const std::string& path);
        std::vector<char> make_response(const Request& request, std::string& work_dir_);
        void handle_read(const boost::system::error_code& error, size_t bytes);
        void handle_write(const boost::system::error_code& error, size_t /*bytes_transferred*/);

        boost::asio::ip::tcp::socket socket_;
        char buff_[1024];
        std::string message_;
        std::string work_dir_;
};

#endif