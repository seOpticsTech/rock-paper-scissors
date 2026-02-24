//
// Created by user on 14/02/2026.
//

#include "Error.h"
#include <string.h>

Error::Error() : message(), status(success), type() {};

Error::Error(const char *message)
    : message(message), status(strlen(message) == 0 ? success : failure), type(strlen(message) == 0 ? no : general) {}

Error::Error(const string &message)
    : message(message), status(message.empty() ? success : failure), type(message.empty() ? no : general) {}

Error::~Error() = default;

Error& Error::operator=(const string& msg) {
    this->message = msg;
    this->status = msg.empty() ? success : failure;
    this->type = status == success ? no : general;
    return *this;
}

Error& Error::operator=(const char* msg) {
    this->message = msg;
    this->status = strlen(msg) == 0 ? success : failure;
    this->type = status == success ? no : general;
    return *this;
}

Error Error::New(const string& message) {
    Error error(message);
    error.status = failure;
    error.type = general;
    return error;
}

Error Error::New(const string& message, Type type) {
    Error error(message);
    error.status = failure;
    error.type = type;
    return error;
}

Error Error::Success() {
    Error error;
    error.status = success;
    error.type = no;
    return error;
}
