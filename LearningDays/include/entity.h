#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED
#include <string>
#include <iostream>
using namespace std;

class Entity {
    protected:
    int id;
    string name;
    string type; //player, enemy, items,..
    string tags; //for filtering: enemy, collectibles,...
    long long health;
    string power;

    public:
    virtual void setHealth(long long health) = 0; //any entity must have a setHealth to have different health
    virtual void setName(const string& name) = 0; //any entity must have a setName to have a name
    virtual void setTags(const string& tags) = 0; //any entity must have a setTag to have a tag
    virtual void setType(const string& type) = 0; //for indicating entity
    virtual void setPower(const string& power) = 0; //set power for entity

    virtual long long getHealth() const = 0;
    virtual string getName() const = 0;
    virtual string getTags() const = 0;
    virtual string getType() const = 0;
    virtual string getPower() const = 0;
    //TODO: sound effect of it, interactions

};

class Dot_Entity : public Entity {
    public:
    Dot_Entity() {
        id = 1; // Example default ID
        name = "Dot_Entity";
        type = "Player";
        tags = "Player";
        health = 100;
        power = "None";
    }

    void setHealth(long long health) override {
        this->health = health;
    }

    void setName(const string& name) override {
        this->name = name;
    }

    void setTags(const string& tags) override {
        this->tags = tags;
    }

    void setType(const string& type) override {
        this->type = type;
    }

    void setPower(const string& power) override {
        this->power = power;
    }

    long long getHealth() const override {
        return health;
    }

    string getName() const override {
        return name;
    }

    string getTags() const override {
        return tags;
    }

    string getType() const override {
        return type;
    }

    string getPower() const override {
        return power;
    }
};




#endif 