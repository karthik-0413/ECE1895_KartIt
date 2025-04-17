#include "RandomState.h"
#include <cstdlib>    // for rand()
#include <ctime>      // for time()
#include <iostream>

// Constructor: Initialize the random messages and seed the random generator.
RandomState::RandomState(Game &game)
    : State(game),
    randomizeMessages(false), // now messages are triggered by key X
    randomMessageCounter(0),
    winMessageDisplayed(false),
    currentMessage("press select to start"), // initial message
    game_over(false),
    game_start(false),
    pop_request(false),
    newMessage(false),
    elapsedTime(sf::Time::Zero),
    maxRandDelay(4.1f), // maximum delay between messages
    minRandDelay(2.6f), // minimum delay between messages
    popDelay(sf::seconds(4))// delay before popping the state
    
{
    // Fill the list of random messages.
    randomMessages.push_back("gas it");
    randomMessages.push_back("brake it");
    randomMessages.push_back("turn left");
    randomMessages.push_back("turn right");
    randomMessages.push_back("shift up");
    randomMessages.push_back("shift down");

    sendSerialMap(); // Send the map to the serial port.
    
    // Seed the random generator (if not already seeded elsewhere)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

sf::Time RandomState::setNewRandomDelay(float &minDelay, float &maxDelay) {
    
    // Decrease min and max delay
    if (minDelay > 1){
        minDelay -= 0.1f;
    }
    if (maxDelay > 2){
        maxDelay -= 0.1f;
    }

    // Set random change delay between minDelay and maxDelay.
    return sf::seconds(minDelay + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (maxDelay - minDelay))));
}

void RandomState::sendSerialMap(){
    
    int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_GR.exe");
}

void RandomState::sendSerialMessage(std::string msg){

    if (msg == "gas it"){
        int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_gas.exe");
    } else if (msg == "brake it"){
        int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_brake.exe");
    } else if (msg == "turn left"){
        int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_left.exe");
    } else if (msg == "turn right"){
        int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_right.exe");
    } else if (msg == "shift up"){
        int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_up.exe");
    } else if (msg == "shift down"){
        int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_down.exe");
    }
}



void RandomState::handleEvent(const sf::Event &event) {
    // Check for key press events.
    if (event.type == sf::Event::KeyPressed) {
        // Pressing the 'X' key triggers a new random message.
        if (event.key.code == sf::Keyboard::X) {
            // Only change the message if we haven't already "won" or ended the message sequence.
            if (!winMessageDisplayed && game_start) {
                // Select a random message from our list.
                // Increase the counter.
                randomMessageCounter++;
                
                // If the counter reaches 99, display a winning message.
                if (randomMessageCounter >= 99) {
                    currentMessage = "you win";
                    winMessageDisplayed = true;
                    pop_request = true; // Request to pop the state after a delay.
                    elapsedTime = sf::Time::Zero; // Reset elapsed time.
                } else {
                    // Find new random delay
                    changeDelay = setNewRandomDelay(minRandDelay, maxRandDelay); // Set a new random delay.
                    elapsedTime = sf::Time::Zero; // Reset elapsed time.
                    newMessage = true;
                }
            }
        }
        // (Optional) Other key handling:
        // For example, pressing Enter may start something:
        else if (event.key.code == sf::Keyboard::Enter) {
            // You can set a flag here to begin randomization if needed.
            game_start = true; // Start the game or randomization process.
            // Select a random message from our list.
            int index = std::rand() % randomMessages.size();
            
            // Set new random message
            currentMessage = randomMessages[index];

            // Find new random delay
            changeDelay = setNewRandomDelay(minRandDelay, maxRandDelay); // Set a new random delay.
            elapsedTime = sf::Time::Zero; // Reset elapsed time.
            newMessage = true;
        }
        // Pressing Z could show "game over" etc.
        else if (event.key.code == sf::Keyboard::Z) {
            if (!winMessageDisplayed && game_start && !pop_request) {
                // Show game over message.
                currentMessage = "game over";
                randomizeMessages = false;
                // Optionally, reset the counter.
                randomMessageCounter = 0;

                elapsedTime = sf::Time::Zero; // Reset elapsed time.
                pop_request = true; // Request to pop the state after a delay.
                
            }
        }
        // Pause the game and show a pause menu when Escape is pressed.
        else if (event.key.code == sf::Keyboard::Escape) {
            // Create a render texture to capture the current game frame.
            sf::RenderTexture render;
            sf::Vector2u windowSize = game.getWindow().getSize();
            if (!render.create(windowSize.x, windowSize.y)) {
                std::cerr << "Failed to create render texture." << std::endl;
                return;
            }
            // Draw the current state onto the render texture.
            game.getCurrentState()->draw(render);
            // Push the normal pause state. (StateRacePause is your normal pause state.)
            game.pushState(StatePtr(new StateRacePause(game, render, "paused")));
        }
    }
}

bool RandomState::update(const sf::Time &deltaTime) {
    // You may have fade in/out logic in here.
    // For example, gradually change the opacity or similar.
    // This example leaves update empty aside from potential fade effects.
    // If pop is requested, increase the elapsed time.
    if (pop_request) {
        elapsedTime += deltaTime;
        // When elapsed time exceeds the delay, pop this state to resume.
        if (elapsedTime >= popDelay) {
            game.popStatesUntil(1);
        }
    } else if (newMessage) {
        elapsedTime += deltaTime;

        // Check if the elapsed time exceeds the change delay.
        if (elapsedTime >= changeDelay) {
            int index = std::rand() % randomMessages.size();
            while(currentMessage == randomMessages[index]) {
            index = std::rand() % randomMessages.size(); // Ensure we get a different message.
            }

            // Set new random message
            currentMessage = randomMessages[index];

            // Reset new message flag
            newMessage = false;

            // Send the new message to the serial port.
            sendSerialMessage(currentMessage);
        }
    }

    

    return true;
}

void RandomState::draw(sf::RenderTarget &window) {
    // Clear the window with a black background.
    window.clear(sf::Color::Black);

    // Determine a scaling factor based on window size.
    sf::Vector2u windowSize = window.getSize();
    float scale = windowSize.x / 256.0f;  // adjust base width as needed

    // Draw the current message in the center of the window.
    TextUtils::write(
        window,
        currentMessage,
        sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f),
        scale,
        sf::Color::White,
        true,
        TextUtils::TextAlign::CENTER,
        TextUtils::TextVerticalAlign::MIDDLE);
}
