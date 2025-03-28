#include "Tower.h"
#include "Bullet.h"
#include <cmath>
#include <iostream>

Tower::Tower(std::string n, int c, int r, int p, float rof, const std::string& textureFile, sf::Vector2f pos)
    : name(n), level(1), cost(c), refund_value(r), range(0), power(p), rate_of_fire(rof), position(pos),
      isFiring(false), firingAnimationDuration(0.2f) {
    if (!texture.loadFromFile(textureFile)) {
        std::cerr << "Failed to load texture: " << textureFile << "\n";
    }
    sprite.setTexture(texture);
    sprite.setPosition(position);
}

void Tower::upgrade(int& playerCoins) {
    int upgradeCost = 50;
    if (playerCoins >= upgradeCost) {
        playerCoins -= upgradeCost;
        level++;
        cost += upgradeCost;
        refund_value = cost / 2;
        range += 20;
        power += 5;
        rate_of_fire += 0.5f; // Increase by 0.5 shots per second per upgrade
        notify(TowerEventType::TowerUpgraded);
    }
}

void Tower::sell(int& playerCoins) {
    playerCoins += refund_value;
    notify(TowerEventType::TowerSold);
}

void Tower::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

void Tower::setScaleToTileSize(float tileSize) {
    sf::Vector2u textureSize = texture.getSize();
    float scaleX = tileSize / textureSize.x;
    float scaleY = tileSize / textureSize.y;
    sprite.setScale(scaleX, scaleY);
}

void Tower::updateAnimation() {
    if (isFiring) {
        float elapsed = firingTimer.getElapsedTime().asSeconds();
        if (elapsed >= firingAnimationDuration) {
            isFiring = false;
        }
    }
}

void Tower::attach(TowerObserver* observer) {
    if (observer && std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

void Tower::detach(TowerObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Tower::notify(TowerEventType event) {
    for (auto* observer : observers) {
        observer->update();
    }
}

sf::Vector2f Tower::getTileSize() const {
    sf::Vector2u textureSize = texture.getSize();
    return sf::Vector2f(textureSize.x * sprite.getScale().x, textureSize.y * sprite.getScale().y);
}

void Tower::attack(std::vector<Critter*>& critters, std::vector<Bullet>& bullets) {
    if (firingTimer.getElapsedTime().asSeconds() < 1.0f) {
        // Cooldown: Only fire one bullet per second
        return;
    }

    for (auto* critter : critters) {
        if (!critter->isDead()) {
            // Check if the critter is within range
            sf::Vector2f direction = critter->getPosition() - position;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (distance <= range) {
                // Fire a bullet toward the critter's current position
                bullets.emplace_back(position, critter->getPosition()); // Pass target position
                isFiring = true;
                firingTimer.restart();
                std::cout << "[Tower] Fired a bullet at critter at position (" 
                          << critter->getPosition().x << ", " 
                          << critter->getPosition().y << ")\n";
                break; // Fire only one bullet
            }
        }
    }
}