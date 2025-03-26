#include <SFML/Graphics.hpp>
#include "Tower.h"
#include "Map.h"
#include "ConcreteMapObserver.h"
#include "CritterView.h"
#include "CritterGroupGenerator.h"
#include "Bullet.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include "Logger.h" // Include logger

using namespace std;

ofstream logFile; // Define global logFile

void logObserver(const string& message) {
    if (logFile.is_open()) {
        logFile << message << endl;
    } else {
        // cerr << "Error: Log file is not open!" << endl;
    }
}

TowerType selectedTowerType = TowerType::DirectDamage;

void drawTowerSelection(sf::RenderWindow& window, sf::Font& font) {
    sf::RectangleShape directDamageButton(sf::Vector2f(200, 50));
    directDamageButton.setFillColor(sf::Color(100, 100, 100));
    directDamageButton.setOutlineColor(sf::Color::White);
    directDamageButton.setOutlineThickness(2);
    directDamageButton.setPosition(50, 820);

    sf::Text directDamageText;
    directDamageText.setFont(font);
    directDamageText.setCharacterSize(20);
    directDamageText.setFillColor(sf::Color::White);
    directDamageText.setString("Direct Damage");
    sf::FloatRect directDamageTextBounds = directDamageText.getLocalBounds();
    directDamageText.setPosition(50 + (200 - directDamageTextBounds.width) / 2 - directDamageTextBounds.left, 
                                 820 + (50 - directDamageTextBounds.height) / 2 - directDamageTextBounds.top);

    sf::RectangleShape areaDamageButton(sf::Vector2f(200, 50));
    areaDamageButton.setFillColor(sf::Color(100, 100, 100));
    areaDamageButton.setOutlineColor(sf::Color::White);
    areaDamageButton.setOutlineThickness(2);
    areaDamageButton.setPosition(300, 820);

    sf::Text areaDamageText;
    areaDamageText.setFont(font);
    areaDamageText.setCharacterSize(20);
    areaDamageText.setFillColor(sf::Color::White);
    areaDamageText.setString("Area Damage");
    sf::FloatRect areaDamageTextBounds = areaDamageText.getLocalBounds();
    areaDamageText.setPosition(300 + (200 - areaDamageTextBounds.width) / 2 - areaDamageTextBounds.left, 
                               820 + (50 - areaDamageTextBounds.height) / 2 - areaDamageTextBounds.top);

    sf::RectangleShape slowDamageButton(sf::Vector2f(200, 50));
    slowDamageButton.setFillColor(sf::Color(100, 100, 100));
    slowDamageButton.setOutlineColor(sf::Color::White);
    slowDamageButton.setOutlineThickness(2);
    slowDamageButton.setPosition(550, 820);

    sf::Text slowDamageText;
    slowDamageText.setFont(font);
    slowDamageText.setCharacterSize(20);
    slowDamageText.setFillColor(sf::Color::White);
    slowDamageText.setString("Slow Damage");
    sf::FloatRect slowDamageTextBounds = slowDamageText.getLocalBounds();
    slowDamageText.setPosition(550 + (200 - slowDamageTextBounds.width) / 2 - slowDamageTextBounds.left, 
                               820 + (50 - slowDamageTextBounds.height) / 2 - slowDamageTextBounds.top);

    window.draw(directDamageButton);
    window.draw(directDamageText);
    window.draw(areaDamageButton);
    window.draw(areaDamageText);
    window.draw(slowDamageButton);
    window.draw(slowDamageText);
}

void handleTowerSelection(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (mousePos.y >= 820 && mousePos.y <= 870) {
            if (mousePos.x >= 50 && mousePos.x <= 250) {
                selectedTowerType = TowerType::DirectDamage;
                cout << "Tower selection: Direct Damage" << endl;
            } else if (mousePos.x >= 300 && mousePos.x <= 500) {
                selectedTowerType = TowerType::AreaDamage;
                cout << "Tower selection: Area Damage" << endl;
            } else if (mousePos.x >= 550 && mousePos.x <= 750) {
                selectedTowerType = TowerType::SlowDamage;
                cout << "Tower selection: Slow Damage" << endl;
            }
        }
    }
}

void promptUserToPlaceTowers(Map& map, sf::RenderWindow& window, sf::Font& font, bool& towersPlaced, bool forcePlacement = false) {
    if (towersPlaced && !forcePlacement) return;
    int targetCount = forcePlacement ? map.getTowerCount() + 1 : 6;
    while (map.getTowerCount() < targetCount) {
        bool towerPlaced = false;
        while (!towerPlaced) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return;
                }
                handleTowerSelection(window, event);
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (mousePos.y < 800) {
                        towerPlaced = map.placeTower(mousePos.x, mousePos.y, selectedTowerType);
                    }
                }
            }
            window.clear();
            map.draw(window);
            drawTowerSelection(window, font);
            window.display();
        }
    }
    if (!forcePlacement) towersPlaced = true;
}

bool showQuitConfirmation(sf::RenderWindow& window, sf::Font& font) {
    sf::RectangleShape dialog(sf::Vector2f(300, 150));
    dialog.setFillColor(sf::Color(50, 50, 50, 200));
    dialog.setPosition(250, 325);

    sf::Text promptText;
    promptText.setFont(font);
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::White);
    promptText.setString("Quit Game?\n1. Quit\n2. Resume");
    sf::FloatRect textBounds = promptText.getLocalBounds();
    promptText.setPosition(250 + (300 - textBounds.width) / 2 - textBounds.left, 
                           325 + (150 - textBounds.height) / 2 - textBounds.top);

    sf::RenderWindow confirmWindow(sf::VideoMode(800, 800), "Confirm Quit", sf::Style::None);
    confirmWindow.setPosition(window.getPosition());

    while (confirmWindow.isOpen()) {
        sf::Event event;
        while (confirmWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) confirmWindow.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    confirmWindow.close();
                    cout << "User chose to quit the game" << endl;
                    return true;
                } else if (event.key.code == sf::Keyboard::Num2) {
                    confirmWindow.close();
                    cout << "User chose to resume the game" << endl;
                    return false;
                }
            }
        }
        confirmWindow.clear(sf::Color::Transparent);
        confirmWindow.draw(dialog);
        confirmWindow.draw(promptText);
        confirmWindow.display();
    }
    return false;
}

int main() {
    logFile.open("observer_log.txt", ios::out | ios::trunc);
    if (!logFile.is_open()) {
        cerr << "Error: Could not open log file!" << endl;
        return 1;
    }
    logObserver("Program started");

    sf::RenderWindow window(sf::VideoMode(800, 900), "Tower Defense", sf::Style::Titlebar | sf::Style::Close);
    ConcreteMapObserver observer;
    int playerCoins = 1000;
    Map map(observer, playerCoins);
    bool towersPlaced = false;

    sf::Font font;
    if (!font.loadFromFile("./assets/Arial.ttf")) cout << ("Failed to load font") << endl;
    sf::Text coinText;
    coinText.setFont(font);
    coinText.setCharacterSize(20);
    coinText.setFillColor(sf::Color::White);
    coinText.setPosition(10, 10);

    sf::Text messageText;
    messageText.setFont(font);
    messageText.setCharacterSize(20);
    messageText.setFillColor(sf::Color::Yellow);
    messageText.setPosition(10, 40);
    sf::Clock messageTimer;

    sf::RectangleShape quitButton(sf::Vector2f(100, 40));
    quitButton.setFillColor(sf::Color(200, 50, 50));
    quitButton.setOutlineColor(sf::Color::White);
    quitButton.setOutlineThickness(2);
    quitButton.setPosition(690, 10);

    sf::Text quitText;
    quitText.setFont(font);
    quitText.setCharacterSize(20);
    quitText.setFillColor(sf::Color::White);
    quitText.setString("Quit");
    sf::FloatRect quitTextBounds = quitText.getLocalBounds();
    quitText.setPosition(690 + (100 - quitTextBounds.width) / 2 - quitTextBounds.left, 
                         10 + (40 - quitTextBounds.height) / 2 - quitTextBounds.top);

    map.loadGrassTexture("./assets/grass.png");
    map.updateGridDimensions(window);

    CritterGroupGenerator critterGen;
    std::vector<Critter*> critters;
    std::vector<Bullet> bullets;
    CritterView critterView(window);
    int waveNumber = 1;
    sf::Clock waveTimer;
    sf::Clock frameClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (map.isOverlayActive()) {
                map.handleEvent(event, window);
            } else {
                map.handleMouseDrag(event, window);
                handleTowerSelection(window, event);
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        int prevCoins = playerCoins;
                        int prevTowerCount = map.getTowerCount();
                        map.handleRightClick(mousePos.x, mousePos.y);
                        if (playerCoins > prevCoins) {
                            const auto& towers = map.getTowers();
                            for (int y = 0; y < towers.size(); ++y) {
                                for (int x = 0; x < towers[y].size(); ++x) {
                                    if (!towers[y][x] && prevTowerCount > map.getTowerCount()) {
                                        messageText.setString("Tower sold");
                                        messageTimer.restart();
                                        promptUserToPlaceTowers(map, window, font, towersPlaced, true);
                                        break;
                                    }
                                }
                            }
                        } else if (playerCoins < prevCoins) {
                            int gridX = mousePos.x / map.getTileSize().x;
                            int gridY = mousePos.y / map.getTileSize().y;
                            if (gridX >= 0 && gridX < map.getTowers()[0].size() && gridY >= 0 && gridY < map.getTowers().size() && map.getTowers()[gridY][gridX]) {
                                messageText.setString(map.getTowers()[gridY][gridX]->getName() + " upgraded to level " + std::to_string(map.getTowers()[gridY][gridX]->getLevel()));
                                messageTimer.restart();
                            }
                        }
                    } else if (event.mouseButton.button == sf::Mouse::Left && quitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        if (showQuitConfirmation(window, font)) {
                            window.close();
                        }
                    }
                }
            }
        }

        if (!map.isOverlayActive() && map.isPathCreated()) {
            if (critters.empty() && waveTimer.getElapsedTime().asSeconds() > 10) {
                for (auto* critter : critters) delete critter;
                critters.clear();
                critters = critterGen.generateWave(waveNumber++, map.getStartPosition(), map.getTileSize().x);
                cout << "Wave " + std::to_string(waveNumber) + " generated with " + std::to_string(critters.size()) + " critters" << endl;
                for (auto* critter : critters) {
                    critterView.addCritter(critter);
                }
                waveTimer.restart();
            }

            float deltaTime = frameClock.restart().asSeconds();
            sf::Vector2f endTileCenter(
                map.getEndPosition().x + map.getTileSize().x / 2.0f,
                map.getEndPosition().y + map.getTileSize().y / 2.0f
            );

            for (auto it = critters.begin(); it != critters.end();) {
                (*it)->move(map.getPathPositions(), map.getTileSize().x, deltaTime);
                if ((*it)->isDead()) {
                    playerCoins += (*it)->getReward();
                    critterView.removeCritter(*it);
                    delete *it;
                    it = critters.erase(it);
                } else if ((*it)->hasReachedExit(endTileCenter)) {
                    playerCoins -= (*it)->getStrength();
                    cout << "Critter reached exit, player lost " << std::to_string((*it)->getStrength()) << " coins" << endl;
                    critterView.removeCritter(*it);
                    delete *it;
                    it = critters.erase(it);
                } else {
                    ++it;
                }
            }

            for (auto it = bullets.begin(); it != bullets.end();) {
                it->update(deltaTime);
                if (!it->isActive()) {
                    it = bullets.erase(it);
                } else {
                    ++it;
                }
            }

            const auto& towers = map.getTowers();
            for (int y = 0; y < towers.size(); ++y) {
                for (int x = 0; x < towers[y].size(); ++x) {
                    if (towers[y][x] && towers[y][x]->getLevel() > 0) {
                        towers[y][x]->attack(critters, bullets);
                        towers[y][x]->updateAnimation();
                    }
                }
            }

            promptUserToPlaceTowers(map, window, font, towersPlaced);
        }

        coinText.setString("Coins: " + std::to_string(playerCoins));
        if (messageTimer.getElapsedTime().asSeconds() > 2.0f) {
            messageText.setString("");
        }

        window.clear();
        map.draw(window);
        for (auto* critter : critters) {
            if (critter) critter->draw(window);
        }
        for (const auto& bullet : bullets) {
            bullet.draw(window);
        }
        window.draw(coinText);
        window.draw(messageText);
        window.draw(quitButton);
        window.draw(quitText);
        drawTowerSelection(window, font);
        window.display();
    }

    for (auto* critter : critters) delete critter;
    critters.clear();

    logObserver("Program ended");
    logFile.close();

    return 0;
}