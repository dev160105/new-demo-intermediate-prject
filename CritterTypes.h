#include "Critter.h"

class BasicCritter : public Critter {
public:
    BasicCritter(int hp, int speed, int reward, int strength, const std::string& textureFile, sf::Vector2f startPos, float tileSize)
        : Critter(hp, speed, reward, strength, textureFile, startPos, tileSize) {
    }
};

class ToughCritter : public Critter {
public:
    ToughCritter(int hp, float speed, int reward, int strength, const std::string& textureFile, sf::Vector2f startPos, float tileSize)
        : Critter(hp, speed, reward, strength, textureFile, startPos, tileSize) {
        this->hitPoints *= 4; // 4x more HP
        this->speed *= 0.5; // 2x slower
        this->sprite.setScale(4, 4); //4x bigger
        this->reward += 20;
    }
};

class FastCritter : public Critter {
public:
    FastCritter(int hp, int speed, int reward, int strength, const std::string& textureFile, sf::Vector2f startPos, float tileSize)
        : Critter(hp, speed, reward, strength, textureFile, startPos, tileSize) {
        this->speed *= 2; // 2x faster
        this->reward += 10;
    }
};

