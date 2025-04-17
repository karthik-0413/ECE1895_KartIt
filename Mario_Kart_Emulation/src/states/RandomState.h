#pragma once

#include "states/statebase.h"
#include "game.h"
#include "gui/textutils.h"
#include <vector>
#include <string>
#include "states/racepause.h"

class RandomState : public State {
public:
    RandomState(Game &game);

    void handleEvent(const sf::Event &event) override;
    bool update(const sf::Time &deltaTime) override;
    void draw(sf::RenderTarget &window) override;
    std::string string() const override { return "RandomState"; }
    void sendSerialMessage(std::string msg); // Send a message to the serial port.
    void sendSerialMap(); // Send a map to the serial port.
    sf::Time setNewRandomDelay(float &minDelay, float &maxDelay); // Get a new random delay.

private:
    std::vector<std::string> randomMessages;
    int randomMessageCounter; // Counts how many times the message has been randomized.
    bool winMessageDisplayed;
    bool randomizeMessages;   // Not used here since we no longer use a timer.
    std::string currentMessage;

    bool newMessage;
    bool game_over;
    bool game_start;
    bool pop_request;
    sf::Time elapsedTime;
    sf::Time popDelay;
    sf::Time changeDelay;
    float maxRandDelay;
    float minRandDelay;

};
