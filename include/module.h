// July 26, 2019

#ifndef MODULE_H
#define MODULE_H

class Module {
public:
    Module(const string & name) : name(name) { }
    virtual ~Module() { }

    virtual void load(Config & config) = 0;
    virtual void run() = 0;

protected:
    string name;
};

#endif
