#ifndef MCSERVER_UPDATE_H
#define MCSERVER_UPDATE_H
#include <string>
#include <utility>
#include <variant>

namespace Update {
    namespace Data {
        struct _GlobalAddPlayer {
            const char * name;
        };
    }

    enum Type {
        ADDPLAYER, KEEPALIVE,
    };

    struct TypedUpdate {
        Update::Type type;

        union {
            bool nothing;
            Data::_GlobalAddPlayer addPlayer;
        };
    };
}


#endif //MCSERVER_UPDATE_H