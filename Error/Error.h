//
// Created by user on 14/02/2026.
//

#ifndef GAME_ERROR_H
#define GAME_ERROR_H

#include <string>

using namespace std;

enum Status {
    success,
    failure
};


class Error {
    public:
    enum Type {
        no,
        general,
        duplicate,
    };
    static Error New(const string& message);
    static Error New(const string& message, Type type);
        static Error Success();
        Error();
        explicit Error(const string& message);
        explicit Error(const char* message);
        ~Error();
        Error& operator=(const string& msg);
        Error& operator=(const char* msg);
        string message;
        Status status;
        Type type;
};


#endif //GAME_ERROR_H
