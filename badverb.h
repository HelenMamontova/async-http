#ifndef BADVERB_H
#define BADVERB_H

#include <stdexcept>
#include <string>

class Badverb: public std::runtime_error
{
    public:
        using std::runtime_error::runtime_error;
};

#endif