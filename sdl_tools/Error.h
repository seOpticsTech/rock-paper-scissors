//
// Created by user on 14/02/2026.
//

#ifndef GAME_ERROR_H
#define GAME_ERROR_H

#include <string>

using namespace std;

constexpr bool success=true;
constexpr bool failure=false;


class Error {
    public:
        static Error New(const string& message);
        static Error Success(const string& message);

        Error();
        explicit Error(const string& message);
        explicit Error(const char* message);
        ~Error();
        Error& operator=(const string& msg);
        Error& operator=(const char* msg);
        string message;
        bool status;
};


#endif //GAME_ERROR_H