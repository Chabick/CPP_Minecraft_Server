#include "Player.h"
#include "World.h"
#include "Systems/PlayerManagement.h"

MC::Player::~Player() {
    MC::DynamicConfig.playercount--;
    delete this->death_location;
    delete this->location;

    System::PlayerManagement::unregisterPlayer(this);
}

MC::Player::Player(uint8_t uuid[]) {
    //TODO: load player info from files using uuid
    for (int i = 0; i < 16; i++) {
        this->uuid[i] = uuid[i];
    }

    this->EID = MC::DynamicConfig.playercount;
    MC::DynamicConfig.playercount++;

    this->game_mode = MC::SWorldConfig::standard_game_mode;
    this->prev_game_mode = MC::GameMode::UNKNOWN;

    this->location = new EntityLocation(0, 0, 0, 0, 0, 0, 0, 0);

    System::PlayerManagement::registerPlayer(this);
}