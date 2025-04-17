#pragma once
#include "states/statebase.h"
#include "game.h"
#include <SFML/Graphics.hpp>
#include "gui/textutils.h"

class CheckpointPauseState : public State {
private:
    // Store the snapshot as a sprite.
    sf::Sprite backgroundSprite;
public:
    // Modified constructor now takes a render texture.
    CheckpointPauseState(Game &game, const sf::RenderTexture &render)
        : State(game) {
        // Initialize the sprite with the render texture's texture.
        backgroundSprite.setTexture(render.getTexture());
    }

    void handleEvent(const sf::Event &event) override {
        if (event.type == sf::Event::KeyPressed) {
            // Resume game by popping this state.
            game.popState();
        }
    }

    bool update(const sf::Time &deltaTime) override {
        return true;
    }

    void draw(sf::RenderTarget &window) override {
        // Draw the snapshot as the background.
        window.draw(backgroundSprite);
        // Draw overlay text.
        
    }

    std::string string() const override {
        return "CheckpointPauseState";
    }
};
