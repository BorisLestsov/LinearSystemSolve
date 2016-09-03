#ifndef LINEARSYSTEMSOLVE_EXCEPTION_H
#define LINEARSYSTEMSOLVE_EXCEPTION_H

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


#endif //LINEARSYSTEMSOLVE_EXCEPTION_H
