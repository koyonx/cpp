#ifndef HARL_HPP
#define HARL_HPP

#include <string>
#include <iostream>

class Harl
{
private:
    void debug();
    void info();
    void warning();
    void error();

    typedef void (Harl::*HarlMemFn)();

    struct LevelMap
    {
        std::string name;
        HarlMemFn  func;
    };

    static const LevelMap _map[4];

public:
    Harl();
    ~Harl();
    void complain(std::string level);
};

#endif
