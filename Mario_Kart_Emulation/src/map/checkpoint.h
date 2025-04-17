#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "entities/driver.h"
#include "map/floorobject.h"
#include "map/map.h"
#include "game.h"
#include "states/racepause.h"
#include "game_instance.h"



enum class CheckpointType {
    TURN_LEFT,
    TURN_RIGHT,
    SHIFT_UP,
    SHIFT_DOWN,
    GAS_PEDAL,
    BRAKE_PEDAL
};

static const unsigned int NUM_CHECKPOINT_ASSETS = 4;

class Checkpoint : public FloorObject {
public:



    // Load the checkpoint asset.
    // The asset is expected to be a horizontal strip of 16x16 images for each orientation.
    static void loadAssets(const std::string &assetName, sf::IntRect roi16x16,
                            // Bowser's Castle
                            sf::IntRect roi64x8, sf::IntRect roi8x64,
                            sf::IntRect roi96x8, sf::IntRect roi8x96,
                            sf::IntRect roi128x8, sf::IntRect roi8x128);

    // Constructor: topLeftPixels is given in raw pixel coordinates,
    // _orientation defines which slice to use.
    Checkpoint(const sf::Vector2f &topLeftPixels,
               const FloorObjectOrientation _orientation, 
               CheckpointType type, int assetIndex);

    // Called when a driver collides with the checkpoint.
    virtual void interactWith(const DriverPtr &driver) override;

    // Applies the checkpoint appearance to the map.
    virtual void applyChanges() const override;

    // Returns the appropriate texture for rendering.
    virtual const sf::Image &getCurrentImage() const override;

    // Returns the map land type for the checkpoint (we use OTHER).
    virtual MapLand getCurrentLand() const override;

    // Returns a string name for debugging.
    virtual std::string name() const override;

    void setImageOpacity(sf::Image &image, sf::Uint8 newAlpha);

private:

    // Assets
    //static sf::Image assets[4];
    sf::Image completeAsset;

    // Checkpoint type string
    std::string cpText;
    CheckpointType cpType;
    
    // Static textures for the active checkpoint for each orientation.
    static sf::Image assetsActive[7];
    // Texture for the inactive checkpoint.
    static sf::Image assetInactive;

    // Returns the initial state for a checkpoint.
    virtual FloorObjectState getInitialState() const override;
};

