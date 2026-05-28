#ifndef MCSERVER_UPDATE_H
#define MCSERVER_UPDATE_H
#include <string>

struct UpdateData {
    /*std::string str;
    int x1, x2, x3;
    bool b1, b2, b3;*/
};

struct UpdateDataADDPLAYER : public UpdateData {
    std::string name;
};


enum EGlobalUpdate {
    ADDPLAYER
};

enum EClientUpdate {};


struct GlobalUpdate {
    EGlobalUpdate type;
    UpdateData data;
};

struct ClientUpdate {
    EClientUpdate type;
    UpdateData data;
};



#endif //MCSERVER_UPDATE_H