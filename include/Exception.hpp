#ifndef LINEARSYSTEMSOLVE_EXCEPTION_HPP
#define LINEARSYSTEMSOLVE_EXCEPTION_HPP

#include <iostream>
#include <exception>

using namespace std;

class Exception: public exception{
    char* err_msg;
public:
    Exception(const char*);
    Exception(const Exception&);
    ~Exception();

    const char* what() const noexcept override;
};


#endif //LINEARSYSTEMSOLVE_EXCEPTION_HPP
