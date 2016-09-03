#include "Exception.hpp"
#include <cstring>

Exception::Exception(const char * str){
    err_msg = new char[strlen(str) + 1];
    strcpy(err_msg, str);
}

Exception::Exception(const Exception & e) {
    err_msg = new char[strlen(e.what()) + 1];
    strcpy(err_msg, e.what());
}

Exception::~Exception() {
    delete []err_msg;
}

const char *Exception::what() const noexcept {
    return err_msg;
}







