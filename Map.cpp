#include "Map.h"
#include "Logger.h"
#include "MapObserver.h"
#include "DirectDamageTower.h"
#include "AreaDamageTower.h"
#include "SlowDamageTower.h"

using namespace std;

Map::Map(MapObserver& observer, int& coins) : width(10), height(10), overlayActive(true), widthInput("10"), heightInput("10"), isDragging(false), towerCount(0), playerCoins(coins) {
    observer.setSubject(this);
    attach(&observer);

    if (!font.loadFromFile("./assets/Arial.ttf")) {
        cout << ("Failed to load font") << endl;
    }

    overlay.setSize({800, 800});
    overlay.setFillColor(sf::Color(50, 50, 50, 200));
    overlay.setPosition(0, 0);

    enterButton.setSize({120, 60});
    enterButton.setFillColor(sf::Color(0, 150, 0));
    enterButton.setOutlineColor(sf::Color::White);
    enterButton.setOutlineThickness(2);
    enterButton.setPosition(340, 650);

    enterButtonText.setFont(font);
    enterButtonText.setCharacterSize(24);
    enterButtonText.setFillColor(sf::Color::White);
    enterButtonText.setString("Enter");
    sf::FloatRect enterBounds = enterButtonText.getLocalBounds();
    enterButtonText.setPosition(
        340 + (120 - enterBounds.width) / 2 - enterBounds.left,
        650 + (60 - enterBounds.height) / 2 - enterBounds.top
    );

    widthInputBox.setSize({150, 50});
    widthInputBox.setFillColor(sf::Color(220, 220, 220));
    widthInputBox.setOutlineColor(sf::Color::Black);
    widthInputBox.setOutlineThickness(1);
    widthInputBox.setPosition(200, 350);

    heightInputBox.setSize({150, 50});
    heightInputBox.setFillColor(sf::Color(220, 220, 220));
    heightInputBox.setOutlineColor(sf::Color::Black);
    heightInputBox.setOutlineThickness(1);
    heightInputBox.setPosition(450, 350);

    widthInputText.setFont(font);
    widthInputText.setCharacterSize(24);
    widthInputText.setFillColor(sf::Color::Black);
    widthInputText.setString(widthInput);
    sf::FloatRect widthTextBounds = widthInputText.getLocalBounds();
    widthInputText.setPosition(
        200 + (150 - widthTextBounds.width) / 2 - widthTextBounds.left,
        350 + (50 - widthTextBounds.height) / 2 - widthTextBounds.top
    );

    heightInputText.setFont(font);
    heightInputText.setCharacterSize(24);
    heightInputText.setFillColor(sf::Color::Black);
    heightInputText.setString(heightInput);
    sf::FloatRect heightTextBounds = heightInputText.getLocalBounds();
    heightInputText.setPosition(
        450 + (150 - heightTextBounds.width) / 2 - heightTextBounds.left,
        350 + (50 - heightTextBounds.height) / 2 - heightTextBounds.top
    );

    widthLabel.setFont(font);
    widthLabel.setCharacterSize(20);
    widthLabel.setFillColor(sf::Color::White);
    widthLabel.setString("Width:");
    sf::FloatRect widthLabelBounds = widthLabel.getLocalBounds();
    widthLabel.setPosition(
        200 + (150 - widthLabelBounds.width) / 2 - widthLabelBounds.left,
        310 - widthLabelBounds.top
    );

    heightLabel.setFont(font);
    heightLabel.setCharacterSize(20);
    heightLabel.setFillColor(sf::Color::White);
    heightLabel.setString("Height:");
    sf::FloatRect heightLabelBounds = heightLabel.getLocalBounds();
    heightLabel.setPosition(
        450 + (150 - heightLabelBounds.width) / 2 - heightLabelBounds.left,
        310 - heightLabelBounds.top
    );

    enterSizeLabel.setFont(font);
    enterSizeLabel.setCharacterSize(30);
    enterSizeLabel.setFillColor(sf::Color::White);
    enterSizeLabel.setString("Enter Map Size");
    sf::FloatRect titleBounds = enterSizeLabel.getLocalBounds();
    enterSizeLabel.setPosition(
        (800 - titleBounds.width) / 2 - titleBounds.left,
        200 - titleBounds.top
    );

    logObserver("[Map Observer]: Overlay loaded");
    notify(EventType::OverlayLoaded);

    loadGrassTexture("./assets/grass.png");
    loadPathTexture("./assets/path.png");
    loadTowerTextures();
    createGrid();

    logObserver("[Map Observer]: Map loaded");
    notify(EventType::MapLoaded);
}

Map::~Map() {
    for (auto& row : towers) {
        for (auto* tower : row) {
            delete tower;
        }
    }
}

void Map::drawOverlay(sf::RenderWindow& window) {
    window.draw(overlay);
    window.draw(enterButton);
    window.draw(enterButtonText);
    window.draw(widthInputBox);
    window.draw(heightInputBox);
    window.draw(widthInputText);
    window.draw(heightInputText);
    window.draw(widthLabel);
    window.draw(heightLabel);
    window.draw(enterSizeLabel);
}

void Map::handleEvent(sf::Event& event, sf::RenderWindow& window) {
    if (overlayActive) {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f clickPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            cout << ("Mouse click at: (" + std::to_string(clickPos.x) + ", " + std::to_string(clickPos.y) + ")") << endl;
            notify(EventType::OverlayClicked);
            if (enterButton.getGlobalBounds().contains(clickPos)) {
                cout << ("Enter button clicked") << endl;
                width = std::stoi(widthInput);
                height = std::stoi(heightInput);
                createGrid();
                overlayActive = false;
                cout << ("Map dimensions confirmed") << endl;
                notify(EventType::DimensionsConfirmed);
            } else if (widthInputBox.getGlobalBounds().contains(clickPos)) {
                widthInput.clear();
                widthInputText.setString(widthInput);
                sf::FloatRect widthTextBounds = widthInputText.getLocalBounds();
                widthInputText.setPosition(
                    200 + (150 - widthTextBounds.width) / 2 - widthTextBounds.left,
                    350 + (50 - widthTextBounds.height) / 2 - widthTextBounds.top
                );
            } else if (heightInputBox.getGlobalBounds().contains(clickPos)) {
                heightInput.clear();
                heightInputText.setString(heightInput);
                sf::FloatRect heightTextBounds = heightInputText.getLocalBounds();
                heightInputText.setPosition(
                    450 + (150 - heightTextBounds.width) / 2 - heightTextBounds.left,
                    350 + (50 - heightTextBounds.height) / 2 - heightTextBounds.top
                );
            }
        } else if (event.type == sf::Event::TextEntered) {
            cout << ("Text entered: " + std::string(1, static_cast<char>(event.text.unicode))) << endl;
            notify(EventType::OverlayClicked);
            if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                if (widthInputBox.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    widthInput += static_cast<char>(event.text.unicode);
                    widthInputText.setString(widthInput);
                    sf::FloatRect widthTextBounds = widthInputText.getLocalBounds();
                    widthInputText.setPosition(
                        200 + (150 - widthTextBounds.width) / 2 - widthTextBounds.left,
                        350 + (50 - widthTextBounds.height) / 2 - widthTextBounds.top
                    );
                } else if (heightInputBox.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    heightInput += static_cast<char>(event.text.unicode);
                    heightInputText.setString(heightInput);
                    sf::FloatRect heightTextBounds = heightInputText.getLocalBounds();
                    heightInputText.setPosition(
                        450 + (150 - heightTextBounds.width) / 2 - heightTextBounds.left,
                        350 + (50 - heightTextBounds.height) / 2 - heightTextBounds.top
                    );
                }
            } else if (event.text.unicode == 8) {
                if (widthInputBox.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (!widthInput.empty()) widthInput.pop_back();
                    widthInputText.setString(widthInput);
                    sf::FloatRect widthTextBounds = widthInputText.getLocalBounds();
                    widthInputText.setPosition(
                        200 + (150 - widthTextBounds.width) / 2 - widthTextBounds.left,
                        350 + (50 - widthTextBounds.height) / 2 - widthTextBounds.top
                    );
                } else if (heightInputBox.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                    if (!heightInput.empty()) heightInput.pop_back();
                    heightInputText.setString(heightInput);
                    sf::FloatRect heightTextBounds = heightInputText.getLocalBounds();
                    heightInputText.setPosition(
                        450 + (150 - heightTextBounds.width) / 2 - heightTextBounds.left,
                        350 + (50 - heightTextBounds.height) / 2 - heightTextBounds.top
                    );
                }
            }
        }
    } else {
        handleMouseDrag(event, window);
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            handleRightClick(mousePos.x, mousePos.y);
        }
    }
}

void Map::draw(sf::RenderWindow& window) {
    drawGrid(window);
    if (overlayActive) drawOverlay(window);
    for (const auto& row : towers) {
        for (const auto* tower : row) {
            if (tower) tower->draw(window);
        }
    }
}

bool Map::isOverlayActive() const { return overlayActive; }

void Map::createGrid() {
    cout << "Creating grid with dimensions: " << std::to_string(width) << "x" << std::to_string(height) << endl;
    grid.clear();
    grid.resize(height, std::vector<sf::RectangleShape>(width));
    towers.clear();
    towers.resize(height, std::vector<Tower*>(width, nullptr));
    sf::Vector2f overlaySize = overlay.getSize();
    sf::Vector2u windowSize(static_cast<unsigned int>(overlaySize.x), static_cast<unsigned int>(overlaySize.y));
    float tileSize = std::min(static_cast<float>(windowSize.x) / width, static_cast<float>(windowSize.y) / height);
    sf::Vector2u grassTextureSize = grassTexture.getSize();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grid[i][j].setSize({tileSize, tileSize});
            grid[i][j].setTexture(&grassTexture);
            grid[i][j].setTextureRect(sf::IntRect(0, 0, grassTextureSize.x, grassTextureSize.y));
            grid[i][j].setOutlineColor(sf::Color::Black);
            grid[i][j].setOutlineThickness(1);
            grid[i][j].setPosition(j * tileSize, i * tileSize);
        }
    }
    startTile.setSize({tileSize, tileSize});
    startTile.setFillColor(sf::Color::Green);
    startTile.setPosition(0, (height / 2 - 1) * tileSize);
    endTile.setSize({tileSize, tileSize});
    endTile.setFillColor(sf::Color::Red);
    endTile.setPosition((width - 1) * tileSize, (height / 2) * tileSize);
}

void Map::drawGrid(sf::RenderWindow& window) {
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            window.draw(cell);
        }
    }
    window.draw(startTile);
    window.draw(endTile);
}

void Map::loadGrassTexture(const std::string& filePath) {
    if (!grassTexture.loadFromFile(filePath)) logObserver("Failed to load grass texture");
}

void Map::loadPathTexture(const std::string& filePath) {
    if (!pathTexture.loadFromFile(filePath)) logObserver("Failed to load path texture");
}

void Map::loadTowerTextures() {
    if (!directDamageTowerTexture.loadFromFile("./assets/direct_tower.png")) logObserver("Failed to load direct damage tower texture");
    if (!areaDamageTowerTexture.loadFromFile("./assets/area_tower.png")) logObserver("Failed to load area damage tower texture");
    if (!slowDamageTowerTexture.loadFromFile("./assets/slow_tower.png")) logObserver("Failed to load slow damage tower texture");
}

void Map::handleMouseDrag(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Vector2i endTilePos(endTile.getPosition().x / grid[0][0].getSize().x, endTile.getPosition().y / grid[0][0].getSize().y);
        if (startTile.getGlobalBounds().contains(mousePos) ||
            (!pathPositions.empty() && grid[pathPositions.back().y][pathPositions.back().x].getGlobalBounds().contains(mousePos) &&
             pathPositions.back() != endTilePos)) {
            isDragging = true;
            startDragPos = sf::Vector2i(mousePos.x / grid[0][0].getSize().x, mousePos.y / grid[0][0].getSize().y);
            if (pathPositions.empty()) pathPositions.push_back(startDragPos);
        }
    } else if (event.type == sf::Event::MouseMoved && isDragging && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Vector2i currentPos(mousePos.x / grid[0][0].getSize().x, mousePos.y / grid[0][0].getSize().y);
        if (currentPos.x >= 0 && currentPos.x < width && currentPos.y >= 0 && currentPos.y < height) {
            if (currentPos != pathPositions.back()) {
                int dx = std::abs(currentPos.x - pathPositions.back().x);
                int dy = std::abs(currentPos.y - pathPositions.back().y);
                if (dx <= 1 && dy <= 1 && (dx + dy > 0)) {
                    int minX = std::min(pathPositions.back().x, currentPos.x);
                    int maxX = std::max(pathPositions.back().x, currentPos.x);
                    int minY = std::min(pathPositions.back().y, currentPos.y);
                    int maxY = std::max(pathPositions.back().y, currentPos.y);
                    for (int y = minY; y <= maxY; ++y) {
                        for (int x = minX; x <= maxX; ++x) {
                            if (x >= 0 && x < width && y >= 0 && y < height) {
                                grid[y][x].setTexture(&pathTexture);
                                grid[y][x].setTextureRect(sf::IntRect(0, 0, pathTexture.getSize().x, pathTexture.getSize().y));
                                pathPositions.push_back(sf::Vector2i(x, y));
                            }
                        }
                    }
                    pathPositions.push_back(currentPos);
                }
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased && isDragging) {
        isDragging = false;
        notify(EventType::PathDraw);
    }
}

void Map::updateGridDimensions(sf::RenderWindow& window) {
    createGrid();
}

bool Map::placeTower(int x, int y, TowerType type, float tileSize) {
    if (towerCount >= maxTowers) return false;
    int gridX = x / grid[0][0].getSize().x;
    int gridY = y / grid[0][0].getSize().y;
    if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
        for (const auto& pos : pathPositions) {
            if (pos.x == gridX && pos.y == gridY) return false;
        }
        sf::Vector2f position(gridX * getTileSize().x, gridY * getTileSize().y);
        delete towers[gridY][gridX];
        switch (type) {
            case TowerType::DirectDamage:
                towers[gridY][gridX] = new DirectDamageTower(position);
                break;
            case TowerType::AreaDamage:
                towers[gridY][gridX] = new AreaDamageTower(position);
                break;
            case TowerType::SlowDamage:
                towers[gridY][gridX] = new SlowDamageTower(position);
                break;
        }
        towers[gridY][gridX]->setScaleToTileSize(tileSize); // Scale tower to tile size
        towerCount++;
        cout << "Tower placed at (" << std::to_string(gridX) << ", " << std::to_string(gridY) << ")" << endl;
        notify(EventType::TowerPlaced);
        return true;
    }
    return false;
}

bool Map::canPlaceMoreTowers() const { return towerCount < maxTowers; }

bool Map::isPathCreated() const {
    return !pathPositions.empty() &&
           pathPositions.back() == sf::Vector2i(endTile.getPosition().x / grid[0][0].getSize().x,
                                                endTile.getPosition().y / grid[0][0].getSize().y);
}

void Map::handleRightClick(int x, int y) {
    int gridX = x / grid[0][0].getSize().x;
    int gridY = y / grid[0][0].getSize().y;
    if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
        if (towers[gridY][gridX]) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) upgradeTower(gridX, gridY);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) sellTower(gridX, gridY);
        }
    }
}

void Map::upgradeTower(int gridX, int gridY) {
    if (towers[gridY][gridX]) {
        towers[gridY][gridX]->upgrade(playerCoins);
        logObserver("[TowerObserver]: Tower upgraded to level " + std::to_string(towers[gridY][gridX]->getLevel()));
    }
}

void Map::sellTower(int gridX, int gridY) {
    if (towers[gridY][gridX]) {
        towers[gridY][gridX]->sell(playerCoins);
        delete towers[gridY][gridX];
        towers[gridY][gridX] = nullptr;
        towerCount--;
        logObserver("[TowerObserver]: Tower sold");
    }
}

int Map::getTowerCount() const { return towerCount; }

sf::Vector2f Map::getStartPosition() const { return startTile.getPosition(); }

const std::vector<sf::Vector2i>& Map::getPathPositions() const { return pathPositions; }

sf::Vector2f Map::getTileSize() const { return grid.empty() ? sf::Vector2f(0, 0) : grid[0][0].getSize(); }

sf::Vector2f Map::getEndPosition() const { return endTile.getPosition(); }

const std::vector<std::vector<Tower*>>& Map::getTowers() const { return towers; }

void Map::attach(MapObserver* observer) {
    if (observer && std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
        logObserver("[Map Observer]: Observer attached. Total observers: " + std::to_string(observers.size()));
    }
}

void Map::detach(MapObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    logObserver("[Map Observer]: Observer detached. Total observers: " + std::to_string(observers.size()));
}

void Map::notify(EventType event) {
    std::string eventStr;
    switch (event) {
        case EventType::MapLoaded: eventStr = "MapLoaded"; break;
        case EventType::OverlayLoaded: eventStr = "OverlayLoaded"; break;
        case EventType::OverlayClicked: eventStr = "OverlayClicked"; break;
        case EventType::DimensionsConfirmed: eventStr = "DimensionsConfirmed"; break;
        case EventType::PathDraw: eventStr = "PathDraw"; break;
        case EventType::TowerPlaced: eventStr = "TowerPlaced"; break;
    }
    logObserver("[Map Observer]: Notifying observers of event: " + eventStr);
    for (auto* observer : observers) {
        observer->update();
    }
}

void Map::setPlayerCoins(int& coins) {
    playerCoins = coins;
}

void Map::resetMap() {
    for (auto& row : towers) {
        for (auto* tower : row) {
            delete tower;
        }
        row.assign(width, nullptr);
    }
    towerCount = 0;
}

void Map::clearPath() {
    pathPositions.clear();
    // Reset grid textures to grass where path was
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grid[i][j].setTexture(&grassTexture);
            grid[i][j].setTextureRect(sf::IntRect(0, 0, grassTexture.getSize().x, grassTexture.getSize().y));
        }
    }
}

void Map::resetOverlay() {
    overlayActive = true;
    widthInput = "10";
    heightInput = "10";
    widthInputText.setString(widthInput);
    heightInputText.setString(heightInput);
    sf::FloatRect widthTextBounds = widthInputText.getLocalBounds();
    widthInputText.setPosition(
        200 + (150 - widthTextBounds.width) / 2 - widthTextBounds.left,
        350 + (50 - widthTextBounds.height) / 2 - widthTextBounds.top
    );
    sf::FloatRect heightTextBounds = heightInputText.getLocalBounds();
    heightInputText.setPosition(
        450 + (150 - heightTextBounds.width) / 2 - heightTextBounds.left,
        350 + (50 - heightTextBounds.height) / 2 - heightTextBounds.top
    );
}