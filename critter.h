#ifndef CRITTER_H
#define CRITTER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

class CritterObserver;

enum class CritterEventType {
    CritterAdded,
    CritterKilled,
    CritterReachedEnd,
    CritterDamaged,
    CritterRemoved
};

class Critter {
public:
    Critter(int hp, int speed, int reward, int strength, const std::string& textureFile, sf::Vector2f startPos, float tileSize);
    virtual ~Critter() = default; // Destructor

    virtual void move(const std::vector<sf::Vector2i>& path, float tileSize, float deltaTime);
    virtual void takeDamage(int damage); // Declaration only
    virtual void slow(float effect);
    virtual bool isDead() const;
    virtual bool hasReachedExit(const sf::Vector2f& endTileCenter) const;
    virtual sf::Vector2f getPosition() const;
    virtual void draw(sf::RenderWindow& window); // Declaration only
    int getReward() const;
    int getStrength() const;
    void setSpeed(int newSpeed);
    int getSpeed() const;
    void setPosition(float x, float y);
    sf::Sprite getSprite() const;

    // Observer Pattern Methods
    void addObserver(CritterObserver* observer);
    void removeObserver(CritterObserver* observer);
    void notifyObservers(CritterEventType event);

protected:
    int hitPoints;
    float speed;
    int reward;
    int strength;
    sf::Sprite sprite;
    sf::Texture texture;
    size_t currentPathIndex;
    float distanceTraveled;
    std::vector<CritterObserver*> observers;

private:
    int maxHealth;
    int currentHealth;
    sf::RectangleShape healthBarBackground;
    sf::RectangleShape healthBar;
};

#endif // CRITTER_H