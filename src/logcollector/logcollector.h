// July 26, 2019

#ifndef LOGCOLLECTOR_H
#define LOGCOLLECTOR_H

class Logcollector : public Module {
public:
    Logcollector();
    ~Logcollector();

    void load(Config & config);
    void run();

private:
    int x;
};

#endif
