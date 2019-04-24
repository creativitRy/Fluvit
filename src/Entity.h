//
// Created by gahwon on 4/16/19.
//

#ifndef GLSL_ENTITY_H
#define GLSL_ENTITY_H

class IUpdatable {
public:
    virtual void update() {};
};

class IRenderable {
public:
    virtual void render() {};
};


class Entity : public IUpdatable, public IRenderable {
public:
    bool enabled = true;

    virtual ~Entity() = default;

    virtual void start() {};
};


#endif //GLSL_ENTITY_H
