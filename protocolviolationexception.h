// ------------------------------------------------------------------
//
//                                ProtocolViolationException header file
//
// A ProtocolViolationException is thrown when protocol rules are 
// not followed
//
// Change log
//      020102  PH      Initial version
//  130515  PH  Removed namespace
//
// ------------------------------------------------------------------

#ifndef PROTOCOL_VIOLATION_EXCEPTION_H
#define PROTOCOL_VIOLATION_EXCEPTION_H

#include <string>

struct ProtocolViolationException {
public:
    ProtocolViolationException(std::string s, int p, int c) : 
        error_message{s}, protocol_num{p}, code{c} {}
    ProtocolViolationException(std::string s) : error_message{s} {}

    std::string error_message;
    int protocol_num;
    int code;


};

#endif
