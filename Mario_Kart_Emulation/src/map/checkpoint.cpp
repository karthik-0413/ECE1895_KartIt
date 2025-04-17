#include "checkpoint.h"
#include <iostream>
#include "checkpointpausestate.h"

// Make sure NUM_CHECKPOINT_ASSETS is exactly the same value here.
//sf::Image Checkpoint::assets[4];
sf::Image Checkpoint::assetsActive[];
sf::Image Checkpoint::assetInactive;


FloorObjectState Checkpoint::getInitialState() const {
    return FloorObjectState::ACTIVE;
}

void Checkpoint::loadAssets(const std::string &assetName, sf::IntRect roi16x16,
                            sf::IntRect roi64x8, sf::IntRect roi8x64,
                            sf::IntRect roi96x8, sf::IntRect roi8x96,
                            sf::IntRect roi128x8, sf::IntRect roi8x128)
{
    sf::Image rawImage;
    if (!rawImage.loadFromFile(assetName)) {
        std::cerr << "Error loading checkpoint asset: " << assetName << std::endl;
        return;
    }
    
    assetsActive[0].create(roi64x8.width, roi64x8.height);
    assetsActive[0].copy(rawImage, 0, 0, roi64x8);

    assetsActive[1].create(roi8x64.width, roi8x64.height);
    assetsActive[1].copy(rawImage, 0, 0, roi8x64);

    assetsActive[2].create(roi96x8.width, roi96x8.height);
    assetsActive[2].copy(rawImage, 0, 0, roi96x8);

    assetsActive[3].create(roi8x96.width, roi8x96.height);
    assetsActive[3].copy(rawImage, 0, 0, roi8x96);

    assetsActive[4].create(roi128x8.width, roi128x8.height);
    assetsActive[4].copy(rawImage, 0, 0, roi128x8);

    assetsActive[5].create(roi8x128.width, roi8x128.height);
    assetsActive[5].copy(rawImage, 0, 0, roi8x128);

    assetsActive[6].create(roi16x16.width, roi16x16.height);
}

Checkpoint::Checkpoint(const sf::Vector2f &topLeftPixels,
                       const FloorObjectOrientation _orientation, CheckpointType type, int assetIndex)
    : FloorObject(topLeftPixels,
                  sf::Vector2f(assetsActive[assetIndex].getSize()),
                  MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, _orientation)
                  {
                    // Based on the checkpoint type, store the corresponding string.
                    switch (type) {
                        case CheckpointType::TURN_LEFT:
                            cpText = "turn left";
                            break;
                        case CheckpointType::TURN_RIGHT:
                            cpText = "turn right";
                            break;
                        case CheckpointType::SHIFT_UP:
                            cpText = "shift up";
                            break;
                        case CheckpointType::SHIFT_DOWN:
                            cpText = "shift down";
                            break;
                        case CheckpointType::GAS_PEDAL:
                            cpText = "gas it";
                            break;
                        case CheckpointType::BRAKE_PEDAL:
                            cpText = "brake it";
                            break;
                        default:
                            cpText = "checkpoint";
                            break;
                    }

                    // Set the checkpoint type
                    cpType = type;
                    
                    // Set asset size
                    completeAsset = assetsActive[assetIndex];
                    setImageOpacity(completeAsset, 0); // make transparent
                    
                }

void Checkpoint::setImageOpacity(sf::Image &image, sf::Uint8 newAlpha)
{
    sf::Vector2u size = image.getSize();
    for (unsigned int x = 0; x < size.x; ++x)
    {
        for (unsigned int y = 0; y < size.y; ++y)
        {
            sf::Color color = image.getPixel(x, y);
            // Option: optionally you can preserve the current alpha relative to the new value
            // color.a = static_cast<sf::Uint8>(color.a * (newAlpha / 255.0f));
            color.a = newAlpha;
            image.setPixel(x, y, color);
        }
    }
}

void Checkpoint::interactWith(const DriverPtr &driver) {
    if (getState() == FloorObjectState::ACTIVE && driver->getPlayerSelection() && !(driver->getState() == 16) && driver->getLap() < 6) {
        std::cout << "Driver 0 reached checkpoint at ("
                  << topLeftPixel.x << ", " << topLeftPixel.y 
                  << "). Pausing game." << std::endl;

        // Create a render texture to capture the current game frame.
        sf::RenderTexture render;
        sf::Vector2u windowSize = gameRef->getWindow().getSize();
        if (!render.create(windowSize.x, windowSize.y)) {
            std::cerr << "Failed to create render texture." << std::endl;
            return;
        }

        // Capture the current game state by drawing the current state into the render texture.
        gameRef->getCurrentState()->draw(render);

        // Push the checkpoint pause state, passing the render texture.
        gameRef->pushState(StatePtr(new StateRacePause(*gameRef, render, cpText)));

        // If you want the checkpoint to remain always active, do not change its state.
        // Otherwise, you could set it inactive here.
        setState(FloorObjectState::INACTIVE);


        // Call the external serial sender program.
        int ret;
        switch(cpType){
            case CheckpointType::TURN_LEFT:
                ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_left.exe");
                break;
            case CheckpointType::TURN_RIGHT:
                ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_right.exe");
                break;
            case CheckpointType::SHIFT_UP:
                ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_up.exe");
                break;
            case CheckpointType::SHIFT_DOWN:
                ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_down.exe");
                break;
            case CheckpointType::GAS_PEDAL:
                ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_gas.exe");
                break;
            case CheckpointType::BRAKE_PEDAL:
                ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_brake.exe");
                break;
            default:
                std::cerr << "Unknown checkpoint type." << std::endl;
                break;
        }

        // Set Gas Checkpoint Flag
        if (cpText == "gas it") {
            setGasCheckpoint(true);
        }
    }
}

void Checkpoint::applyChanges() const {
    FloorObject::defaultApplyChanges(this);
}

const sf::Image &Checkpoint::getCurrentImage() const {
    return completeAsset;
}

MapLand Checkpoint::getCurrentLand() const {
    return MapLand::OTHER;
}

std::string Checkpoint::name() const {
    return "Checkpoint";
}
