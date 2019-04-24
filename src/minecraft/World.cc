#include <utility>

//
// Created by gahwon on 4/17/19.
//

#include "World.h"

World::World() {
}

void World::start() {
    auto pos = glm::ivec3(glm::round(camera_pos() / (float) Chunk::chunk_size));
    for (auto z = -render_distance; z <= render_distance; ++z) {
        for (auto x = -render_distance; x <= render_distance; ++x) {
            if (x * x + z * z > render_distance * render_distance)
                continue;
            auto p = glm::ivec2(pos.x + x, pos.z + z);
            auto pair = chunks.emplace(p, p);
            if (pair.second) {
                pair.first->second.start();
            }
        }
    }
}

void World::update() {
    std::unordered_set<glm::ivec2> to_remove{};
    for (auto &chunk : chunks) {
        to_remove.insert(chunk.first);
    }

    auto pos = glm::ivec3(glm::round(camera_pos() / (float) Chunk::chunk_size));
    for (auto z = -render_distance; z <= render_distance; ++z) {
        for (auto x = -render_distance; x <= render_distance; ++x) {
            if (x * x + z * z > render_distance * render_distance)
                continue;
            auto p = glm::ivec2(pos.x + x, pos.z + z);
            auto pair = chunks.emplace(p, p);

            if (pair.second) {
                pair.first->second.start();
            }

            to_remove.erase(p);
        }
    }

    for (auto &remove : to_remove) {
        chunks.erase(remove);
    }
}

void World::render() {
    auto cam = camera_pos();
    auto cam_chunk = glm::ivec2(glm::floor(glm::vec2(cam.x, cam.z) / (float) Chunk::chunk_size));
    auto yaww = yaw();
    auto yaw_dir = glm::vec2(glm::cos(-yaww - glm::half_pi<float>()), glm::sin(-yaww - glm::half_pi<float>()));

    for (auto &pair : chunks) {
        auto &chunk = pair.second;
        if (!chunk.enabled)
            continue;

        auto delta = cam_chunk - chunk.position;
        if (delta.x * delta.x + delta.y * delta.y <= 4) {
            chunk.render();
            continue;
        }

        // cull invisible regions
        auto pos = chunk.position * (int) Chunk::chunk_size;
        auto vector = glm::vec2(pos) - glm::vec2(cam.x, cam.z);

        if (glm::dot(vector, yaw_dir) < 0)
            continue;

        chunk.render();
    }
}

int World::get_max_height(float x, float z) {
    auto floor = glm::floor(glm::vec2(x, z));
    auto ifloor = glm::ivec2(floor);
    auto frac = glm::vec2(x, z) - floor;

    if (frac.x == 0 && frac.y == 0)
        return get_height(ifloor.x, ifloor.y);

    auto lower_left = get_height(ifloor.x, ifloor.y);
    auto upper_left = get_height(ifloor.x, ifloor.y + 1);
    auto upper_right = get_height(ifloor.x + 1, ifloor.y + 1);
    auto lower_right = get_height(ifloor.x + 1, ifloor.y);

    return glm::max(glm::max(lower_left, lower_right), glm::max(upper_left, upper_right));
}

int World::get_height(int x, int z) {
    auto chunk_pos = glm::ivec2(glm::floor(glm::vec2(x, z) / (float) Chunk::chunk_size));

    auto iterator = chunks.find(chunk_pos);
    if (iterator == chunks.end())
        return 0;

    auto &chunk =  iterator->second;
    auto local_pos = glm::ivec2(x, z) - (chunk_pos * (int) Chunk::chunk_size);

    return (int) chunk.get_height(local_pos.x, local_pos.y);
}
