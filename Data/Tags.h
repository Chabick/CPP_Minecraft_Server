#ifndef MCSERVER_TAGS_H
#define MCSERVER_TAGS_H

#define TAG_COUNT
#include <string>
#include <utility>
#include <vector>
#include "Types/TVarInt.h"
#include "Types/TString.h"

/*
struct Tag_Update {
    std::string tag;
    int id_length;
    void* ids[];

    Tag_Update() = default;

    Tag_Update(std::string str, int i, void* ids)
        : tag(str), id_length(i), ids(ids) {};
};

#define TAG_UPDATE_COUNT 1

extern const char* update_registries[];
extern void* tag_updates[];
extern Tag_Update tag_updates_timeline[];
extern int tag_id_updates_overworld[];*/

extern int PACKET_UPDATE_TAGS_LENGTH;

struct Tag {
    std::string identifier;
    std::vector<int> binding_ids;

    Tag(std::string identifier, std::vector<int> binding_ids) {
        this->identifier = std::move(identifier);
        this->binding_ids = std::move(binding_ids);

        PACKET_UPDATE_TAGS_LENGTH += sizeStringRaw(this->identifier) + sizeVarInt(this->binding_ids.size());

        for (auto binding_id : this->binding_ids) {
            PACKET_UPDATE_TAGS_LENGTH += sizeVarInt(binding_id);
        }
    }
};

struct Tags {
    std::string registry_id;
    std::vector<Tag> tags;
    Tags(std::string registry_id, std::vector<Tag> tags) {
        this->registry_id = std::move(registry_id);
        this->tags = std::move(tags);

        PACKET_UPDATE_TAGS_LENGTH += sizeStringRaw(this->registry_id) + sizeVarInt(this->tags.size());
    }
};

extern std::vector<Tags> GlobalTags;


#endif //MCSERVER_TAGS_H