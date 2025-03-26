#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Tower.h"
#include "MapEvents.h" // Ensure EventType is defined
#include "MapObserver.h" // Include after MapEvents.h
#include <fstream>
#include <iostream>

using namespace std;

// Forward declarations for derived tower classes
class DirectDamageTower;
class AreaDamageTower;
class SlowDamageTower;

enum class TowerType { 
    DirectDamage, 
    AreaDamage, 
    SlowDamage 
};

class Map {
private:
    int width, height;
    std::vector<std::vector<sf::RectangleShape>> grid;
    std::vector<std::vector<Tower*>> towers;
    sf::RectangleShape overlay;
    sf::RectangleShape enterButton;
    sf::Text enterButtonText;
    sf::RectangleShape widthInputBox, heightInputBox;
    sf::Text widthInputText, heightInputText;
    sf::Text widthLabel, heightLabel;
    sf::Text enterSizeLabel;
    sf::Font font;
    std::string widthInput, heightInput;
    bool overlayActive;
    sf::RectangleShape startTile, endTile;
    sf::Texture grassTexture;
    sf::Texture pathTexture;
    sf::Texture directDamageTowerTexture, areaDamageTowerTexture, slowDamageTowerTexture;
    std::vector<sf::Vector2i> pathPositions;
    bool isDragging;
    sf::Vector2i startDragPos;
    int towerCount;
    const int maxTowers = 6;
    std::vector<MapObserver*> observers;
    int& playerCoins;

public:
    Map(MapObserver& observer, int& coins);
    ~Map();
    void draw(sf::RenderWindow& window);
    void drawGrid(sf::RenderWindow& window);
    void drawOverlay(sf::RenderWindow& window);
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void handleMouseDrag(sf::Event& event, sf::RenderWindow& window);
    void createGrid();
    void loadGrassTexture(const std::string& filePath);
    void loadPathTexture(const std::string& filePath);
    void loadTowerTextures();
    bool placeTower(int x, int y, TowerType type);
    bool canPlaceMoreTowers() const;
    bool isOverlayActive() const;
    bool isPathCreated() const;
    void handleRightClick(int x, int y);
    void upgradeTower(int gridX, int gridY);
    void sellTower(int gridX, int gridY);
    int getTowerCount() const;
    sf::Vector2f getStartPosition() const;
    const std::vector<sf::Vector2i>& getPathPositions() const;
    sf::Vector2f getTileSize() const;
    sf::Vector2f getEndPosition() const;
    const std::vector<std::vector<Tower*>>& getTowers() const;
    void updateGridDimensions(sf::RenderWindow& window);
    void attach(MapObserver* observer);
    void detach(MapObserver* observer);
    void notify(EventType event);
    void setPlayerCoins(int& coins);
};

#endif