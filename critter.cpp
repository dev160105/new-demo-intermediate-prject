#include "Critter.h"
#include <cmath>
#include <iostream>
#include "CritterObserver.h"
#include "Logger.h" // Include Logger.h for logObserver()

using namespace std;

Critter::Critter(int hp, int speed, int reward, int strength, const std::string& textureFile, sf::Vector2f startPos, float tileSize)
    : maxHealth(hp), currentHealth(hp), speed(speed), reward(reward), strength(strength), currentPathIndex(0), distanceTraveled(0.0f) {
    // Load texture
    if (!texture.loadFromFile(textureFile)) {
        std::cerr << "Failed to load texture: " << textureFile << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setPosition(startPos);
    sprite.setScale(tileSize / texture.getSize().x, tileSize / texture.getSize().y);

    // Initialize health bar background
    healthBarBackground.setSize(sf::Vector2f(30, 5)); // Width: 30, Height: 5
    healthBarBackground.setFillColor(sf::Color::Red);

    // Initialize health bar
    healthBar.setSize(sf::Vector2f(30, 5));
    healthBar.setFillColor(sf::Color::Green);
}

Critter::~Critter() {
    // Destructor logic if needed
}

void Critter::move(const std::vector<sf::Vector2i>& path, float tileSize, float deltaTime) {
    if (currentPathIndex >= path.size()) {
        cout << "[Critter] Reached end of path at position (" + std::to_string(sprite.getPosition().x) + ", " + std::to_string(sprite.getPosition().y) + ")" << endl;
        notifyObservers(CritterEventType::CritterReachedEnd);
        return;
    }

    sf::Vector2f currentPos = sprite.getPosition();
    sf::Vector2f targetPos(
        path[currentPathIndex].x * tileSize + tileSize / 2.0f,
        path[currentPathIndex].y * tileSize + tileSize / 2.0f
    );

    sf::Vector2f direction(targetPos.x - currentPos.x, targetPos.y - currentPos.y);
    float distanceToTarget = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float moveSpeed = speed * deltaTime * 60.0f;

    if (distanceToTarget <= moveSpeed) {
        sprite.setPosition(targetPos);
        currentPathIndex++;
        if (currentPathIndex < path.size()) {
            sf::Vector2f nextTarget(
                path[currentPathIndex].x * tileSize + tileSize / 2.0f,
                path[currentPathIndex].y * tileSize + tileSize / 2.0f
            );
            sf::Vector2f nextDirection(nextTarget.x - targetPos.x, nextTarget.y - targetPos.y);
            float remainingDistance = moveSpeed - distanceToTarget;
            if (remainingDistance > 0) {
                float nextDistance = std::sqrt(nextDirection.x * nextDirection.x + nextDirection.y * nextDirection.y);
                if (nextDistance > 0) {
                    sf::Vector2f moveVec = (nextDirection / nextDistance) * remainingDistance;
                    sprite.move(moveVec);
                }
            }
        }
    }
    else {
        sf::Vector2f moveVec = (direction / distanceToTarget) * moveSpeed;
        sprite.move(moveVec);
    }
}

void Critter::takeDamage(int damage) {
    currentHealth -= damage;
    if (currentHealth < 0) currentHealth = 0;

    // Update health bar size
    float healthPercentage = static_cast<float>(currentHealth) / maxHealth;
    healthBar.setSize(sf::Vector2f(30 * healthPercentage, 5));
}

void Critter::slow(float effect) {
    float oldSpeed = speed;
    speed *= effect;
    cout <<("[Critter] Slowed from " + std::to_string(oldSpeed) + " to " + std::to_string(speed)) << endl;
}

bool Critter::isDead() const {
    return currentHealth <= 0;
}

bool Critter::hasReachedExit(const sf::Vector2f& endTileCenter) const {
    sf::Vector2f pos = sprite.getPosition();
    float threshold = 5.0f;
    float dx = pos.x - endTileCenter.x;
    float dy = pos.y - endTileCenter.y;
    bool reached = std::sqrt(dx * dx + dy * dy) <= threshold;
    if (reached) {
        cout << "[Critter] Confirmed reached exit at (" << std::to_string(endTileCenter.x) << ", " + std::to_string(endTileCenter.y) << ")" << endl;
    }
    return reached;
}

void Critter::draw(sf::RenderWindow& window) {
    // Draw the critter
    window.draw(sprite);

    // Position the health bar above the critter
    sf::Vector2f critterPosition = sprite.getPosition();
    healthBarBackground.setPosition(critterPosition.x, critterPosition.y - 10); // Adjust Y offset
    healthBar.setPosition(critterPosition.x, critterPosition.y - 10);

    // Draw health bar
    window.draw(healthBarBackground);
    window.draw(healthBar);
}

sf::Vector2f Critter::getPosition() const {
    return sprite.getPosition();
}

int Critter::getStrength() const {
    return strength;
}

int Critter::getReward() const {
    return reward;
}

void Critter::setSpeed(int newSpeed) {
    speed = newSpeed;
    cout << "[Critter] Speed set to " + std::to_string(speed) << endl;
}

int Critter::getSpeed() const {
    return speed;
}

// Observer Pattern Methods 
void Critter::addObserver(CritterObserver* observer) {
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
        cout << "[Critter] Observer added. Total observers: " + std::to_string(observers.size()) << endl;
    }
}

void Critter::removeObserver(CritterObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    cout << "[Critter] Observer removed. Total observers: " + std::to_string(observers.size()) << endl;
}

void Critter::notifyObservers(CritterEventType event) {
    std::string eventStr;
    switch (event) {
        case CritterEventType::CritterAdded: eventStr = "CritterAdded"; break;
        case CritterEventType::CritterKilled: eventStr = "CritterKilled"; break;
        case CritterEventType::CritterReachedEnd: eventStr = "CritterReachedEnd"; break;
        case CritterEventType::CritterDamaged: eventStr = "CritterDamaged"; break;
        case CritterEventType::CritterRemoved: eventStr = "CritterRemoved"; break;
    }
    logObserver("[Critter Observer] Notifying event: " + eventStr + " at position (" + 
                std::to_string(sprite.getPosition().x) + ", " + 
                std::to_string(sprite.getPosition().y) + ")");
    for (CritterObserver* observer : observers) {
        observer->updateView(event); // Note: Changed to updateView to match your implementation
    }
}
void Critter::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

sf::Sprite Critter::getSprite() const {
    return sprite;
}