#include "Tags.h"

#include <vector>

/*
const char* update_registries[] = {
    "minecraft:timeline"
};

void* tag_updates[] = {
    &tag_updates_timeline
};

//Throws an error
Tag_Update tag_updates_timeline[] = {
    Tag_Update("", 1, nullptr), //length of array
    Tag_Update("minecraft:overworld", 1, &tag_id_updates_overworld)
};*/

int PACKET_UPDATE_TAGS_LENGTH = 1; //for the packet id


Tag *__TAG_in_overworld = new Tag("minecraft:in_overworld", {0});


std::vector<Tags> GlobalTags = std::vector<Tags>([]()->std::vector<Tags> {
    //create list of all tags in the vector tags of type Tags
    std::vector<Tags> tags = std::vector<Tags>();

    //maybe for better performance?
    tags.reserve(1);

    //vector for timeline tags
    std::vector<Tag> timeline_tags = std::vector<Tag>();
    timeline_tags.reserve(1);
    timeline_tags.push_back(*__TAG_in_overworld);

    //add all tags
    tags.push_back(Tags("minecraft:timeline", timeline_tags));

    PACKET_UPDATE_TAGS_LENGTH += tags.size();

    return tags;
}());
