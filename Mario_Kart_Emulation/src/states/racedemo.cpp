#include "racedemo.h"

const sf::Vector2f StateRaceDemo::ABS_CREDITS =
    sf::Vector2f(58.0f / BACKGROUND_WIDTH, 12.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateRaceDemo::ABS_NAME0 =
    sf::Vector2f(52.0f / BACKGROUND_WIDTH, 156.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateRaceDemo::REL_NAMEDY =
    sf::Vector2f(0.0f / BACKGROUND_WIDTH, 12.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateRaceDemo::ABS_COPY =
    sf::Vector2f(48.0f / BACKGROUND_WIDTH, 204.0f / BACKGROUND_HEIGHT);

const sf::Time StateRaceDemo::FADE_TIME = sf::seconds(0.5f);
const sf::Time StateRaceDemo::TIME_BETWEEN_CAMERA_SWITCHES = sf::seconds(8.0f),
               StateRaceDemo::TIME_BETWEEN_QP_REFRESHES = sf::seconds(2.0f);

const sf::Time StateRaceDemo::TIME_BETWEEN_ITEM_CHECKS =
    sf::seconds(1.0f) / (float)Item::UPDATES_PER_SECOND;

const sf::Time StateRaceDemo::SHOW_MESSAGE_TIME = sf::seconds(2.0f);
const sf::Time StateRaceDemo::SHOW_MESSAGE_FADE_TIME = sf::seconds(2.0f);

void StateRaceDemo::selectRandomTarget() {
    targetDirection = (nextSwitchTime.asMilliseconds() % 360) * M_PI / 180.0f;
    unsigned int lastTarget = currentTarget;
    while (currentTarget == lastTarget) {
        currentTarget = rand() % drivers.size();
    }
    nextSwitchTime = StateRace::currentTime + TIME_BETWEEN_CAMERA_SWITCHES;
}


void StateRaceDemo::sendSerialMessage(std::string msg) {

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

void StateRaceDemo::sendSerialMap() {

    int ret = std::system("start /B F:\\home\\kart-it\\serial_sender\\serial_GR.exe");

}

sf::Time StateRaceDemo::setNewRandomDelay(float &minDelay, float &maxDelay) {
    
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



void StateRaceDemo::init() {
    StateRace::currentTime = sf::Time::Zero;
    nextItemCheck = sf::Time::Zero;
    fadeTime = sf::Time::Zero;
    autoUseItems = true;
    fixCamera = false;
    firstPersonCamera = false;
    showMessageTime = sf::Time::Zero;

    ///////////////////////////////////
    //         Freestyle
    ///////////////////////////////////

    randomizeMessages = false; // now messages are triggered by key X
    randomMessageCounter = 0;
    winMessageDisplayed =false;
    currentMessage = "press select to start"; // initial message
    game_over = false;
    game_start = false;
    pop_request = false;
    newMessage = false;
    countdownFinished = false;
    elapsedTime = sf::Time::Zero;
    maxRandDelay = 4.1f; // maximum delay between messages
    minRandDelay = 2.6f; // minimum delay between messages
    //popDelay = sf::seconds(4);// delay before popping the state

    randomMessages.push_back("gas it");
    randomMessages.push_back("brake it");
    randomMessages.push_back("turn left");
    randomMessages.push_back("turn right");
    randomMessages.push_back("shift up");
    randomMessages.push_back("shift down");

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


    ///////////////////////////////////
    //         Freestyle
    ///////////////////////////////////

    VehicleProperties::setScaleFactor(2.0f, 1.0f);

    // init pseudoplayer
    pseudoPlayer = DriverPtr(
        new Driver("assets/drivers/invisible.png", sf::Vector2f(0.0f, 0.0f),
                   M_PI_2 * -1.0f, MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT,
                   DriverControlType::DISABLED, VehicleProperties::GODMODE,
                   MenuPlayer(0)));

    // init players
    sf::Vector2f pos = Map::getPlayerInitialPosition(1);
    pos = sf::Vector2f(pos.x / MAP_ASSETS_WIDTH, pos.y / MAP_ASSETS_HEIGHT);
    for (unsigned int i = 0; i < drivers.size(); i++) {
        while (rand() % 7 != 0) {
            pos += AIGradientDescent::getNextDirection(pos);
        }
        sf::Vector2f dir = AIGradientDescent::getNextDirection(pos);
        float angle = atan2(dir.y, dir.x);

        DriverPtr driver(new Driver(
            DRIVER_ASSET_NAMES[i].c_str(), sf::Vector2f(0.0f, 0.0f), 0.0f,
            MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::AI_GRADIENT,
            *DRIVER_PROPERTIES[i], MenuPlayer(i), positions));
        driver->setPositionAndReset(pos, angle);
        drivers[i] = driver;
        miniDrivers[i] = driver;
        positions[i] = driver.get();
    }
}

void StateRaceDemo::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed &&
        event.key.code >= sf::Keyboard::Num1 &&
        event.key.code <= sf::Keyboard::Num7) {
        PowerUps item = PowerUps(event.key.code - sf::Keyboard::Num0);
        for (const DriverPtr& driver : drivers) {
            driver->pickUpPowerUp(item);
            if (autoUseItems) {
                Item::useItem(driver, positions, true, true);
            }
        }

    ///////////////////////////////////
    //         Freestyle
    ///////////////////////////////////

    } else if (event.type == sf::Event::KeyPressed &&
               event.key.code == sf::Keyboard::Enter) {
        
        if (!game_start) {

            // You can set a flag here to begin randomization if needed.
            game_start = true; // Start the game or randomization process.
            // Select a random message from our list.
            int index = std::rand() % randomMessages.size();
            
            // Set new random message
            currentMessage = randomMessages[index];

            // Find new random delay
            changeDelay = setNewRandomDelay(minRandDelay, maxRandDelay); // Set a new random delay.
            elapsedTime = sf::Time::Zero; // Reset elapsed time.
        }
        
    } else if ((event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::X)) {
            
        // Only change the message if we haven't already "won" or ended the message sequence.
        if (!winMessageDisplayed && game_start && !pop_request) {
            // Select a random message from our list.
            // Increase the counter.
            randomMessageCounter += 1;

            currentMessage = "score!";
            
            // If the counter reaches 99, display a winning message.
            if (randomMessageCounter >= 99) {
                currentMessage = "you win";
                winMessageDisplayed = true;
                //pop_request = true; // Request to pop the state after a delay.
                elapsedTime = sf::Time::Zero; // Reset elapsed time.
            } else {
                // Find new random delay
                changeDelay = setNewRandomDelay(minRandDelay, maxRandDelay); // Set a new random delay.
                elapsedTime = sf::Time::Zero; // Reset elapsed time.
                newMessage = true;
            }
        }
    } else if ((event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Z)) {

        if (!winMessageDisplayed && game_start && !pop_request) {
            // Show game over message.
            currentMessage = "game over";
            randomizeMessages = false;

            elapsedTime = sf::Time::Zero; // Reset elapsed time.
            pop_request = true; // Request to pop the state after a delay.
            
        }
    }
    

    ///////////////////////////////////
    //         Freestyle
    ///////////////////////////////////
    
    else if (event.type == sf::Event::KeyPressed &&
               event.key.code == sf::Keyboard::Tab) {
        selectRandomTarget();
    } else if (event.type == sf::Event::KeyPressed &&
               event.key.code == sf::Keyboard::LAlt) {
        autoUseItems = !autoUseItems;
        showMessage = "auto use items > ";
        showMessage += (autoUseItems ? "on" : "off");
        showMessageTime = StateRace::currentTime + SHOW_MESSAGE_TIME;
    } else if (event.type == sf::Event::KeyPressed &&
               event.key.code == sf::Keyboard::LControl) {
        fixCamera = !fixCamera;
        showMessage = "fix camera > ";
        showMessage += (fixCamera ? "on" : "off");
        showMessageTime = StateRace::currentTime + SHOW_MESSAGE_TIME;
    } else if (event.type == sf::Event::KeyPressed &&
               event.key.code == sf::Keyboard::Space) {
        firstPersonCamera = !firstPersonCamera;
        showMessage = "first person camera > ";
        showMessage += (firstPersonCamera ? "on" : "off");
        showMessageTime = StateRace::currentTime + SHOW_MESSAGE_TIME;
    } else if (!raceFinished && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        raceFinished = true;
        fadeTime = sf::Time::Zero;
    }
}

bool StateRaceDemo::fixedUpdate(const sf::Time& deltaTime) {

    ///////////////////////////////////
    //         Freestyle
    ///////////////////////////////////
    if (!countdownFinished && game_start) {
        elapsedTime += deltaTime;
        if (elapsedTime >= sf::seconds(3)) {

            // Finsih countdown
            countdownFinished = true;
            
            // Set new random message
            currentMessage = randomMessages[std::rand() % randomMessages.size()];

            // Send the new message to the serial port.
            sendSerialMessage(currentMessage);

            // First message is displayed
            firstMessage = true;


        } else if (elapsedTime >= sf::seconds(2) && elapsedTime < sf::seconds(3)) {
            currentMessage = "          1          ";
        } else if (elapsedTime >= sf::seconds(1) && elapsedTime < sf::seconds(2)) {
            currentMessage = "          2          ";
        } else if (elapsedTime < sf::seconds(1)) {
            currentMessage = "          3          ";
        }
    }
    else if (newMessage && !pop_request) {
        elapsedTime += deltaTime;

        // Check if the elapsed time exceeds the change delay.
        if (elapsedTime >= changeDelay) {
            int index = std::rand() % randomMessages.size();
            while(currentMessage == randomMessages[index]) {
            index = std::rand() % randomMessages.size(); // Ensure we get a different message.
            }

            // Set new random message
            currentMessage = randomMessages[index];

            // Send the new message to the serial port.
            sendSerialMessage(currentMessage);

            // Reset new message flag
            newMessage = false;
            firstMessage = false;
        }
    }

    ///////////////////////////////////
    //         Freestyle
    ///////////////////////////////////



    // update global time
    StateRace::currentTime += deltaTime;
    fadeTime += deltaTime;
    if (raceFinished && fadeTime >= FADE_TIME) {
        if (!fadeFinished) {
            fadeFinished = true;
            CollisionHashMap::resetStatic();
            CollisionHashMap::resetDynamic();
            game.popState();
            game.popState();  // return to initLoad to push another start state
        }
        return true;
    }

    // Map object updates
    for (unsigned int i = 0; i < drivers.size(); i++) {
        // Player position updates
        drivers[i]->update(deltaTime);
    }
    // check if AI should use its items
    if (StateRace::currentTime > nextItemCheck) {
        nextItemCheck = StateRace::currentTime + TIME_BETWEEN_ITEM_CHECKS;
        for (unsigned int i = 0; i < drivers.size(); i++) {
            if (i == currentTarget &&
                drivers[i]->getPowerUp() != PowerUps::NONE) {
                float r = rand() / (float)RAND_MAX;
                AIItemProb prob =
                    Item::getUseProbability(drivers[i], positions);
                if (r < std::get<0>(prob)) {
                    Item::useItem(drivers[i], positions, std::get<1>(prob));
                }
            }
        }
    }
    Map::updateObjects(deltaTime);

    // only for the demo - reactivate them every few seconds
    if (StateRace::currentTime > nextQPTime) {
        nextQPTime = StateRace::currentTime + TIME_BETWEEN_QP_REFRESHES;
        Map::reactivateQuestionPanels();
    }

    // Pseudo-player (camera) update
    if (!fixCamera && StateRace::currentTime > nextSwitchTime) {
        // target switch
        selectRandomTarget();
    }
    // camera movement
    if (firstPersonCamera) {
        pseudoPlayer->position = drivers[currentTarget]->position;
        pseudoPlayer->posAngle = drivers[currentTarget]->posAngle;
    } else {
        sf::Vector2f targetPosition =
            drivers[currentTarget]->position +
            sf::Vector2f(cosf(targetDirection), sinf(targetDirection)) * 0.05f;
        pseudoPlayer->position +=
            (targetPosition - pseudoPlayer->position) * POS_LERP_PCT;
        sf::Vector2f d =
            drivers[currentTarget]->position - pseudoPlayer->position;
        float targetAngle = atan2(d.y, d.x);
        // more smooth angle transition (don't do 0-2pi range)
        while (fabsf(targetAngle - pseudoPlayer->posAngle) >
               fabsf(targetAngle + 2.0f * M_PI - pseudoPlayer->posAngle)) {
            targetAngle += 2.0f * M_PI;
        }
        while (fabsf(targetAngle - pseudoPlayer->posAngle) >
               fabsf(targetAngle - pseudoPlayer->posAngle - 2.0f * M_PI)) {
            targetAngle -= 2.0f * M_PI;
        }
        pseudoPlayer->posAngle +=
            (targetAngle - pseudoPlayer->posAngle) * ANGLE_LERP_PCT;
    }

    // Ranking updates - last gradient contains
    std::sort(positions.begin(), positions.end(),
              [](const Driver* lhs, const Driver* rhs) {
                  // returns true if player A is ahead of B
                  if (lhs->getLaps() == rhs->getLaps()) {
                      return lhs->getLastGradient() < rhs->getLastGradient();
                  } else {
                      return lhs->getLaps() > rhs->getLaps();
                  }
              });
    for (unsigned int i = 0; i < positions.size(); i++) {
        positions[i]->rank = i;
    }

    // Collision updates
    // Register all objects for fast detection
    CollisionHashMap::resetDynamic();
    Map::registerItemObjects();
    for (const DriverPtr& driver : drivers) {
        CollisionHashMap::registerDynamic(driver);
    }

    // Detect collisions with players
    CollisionData data;
    for (const DriverPtr& driver : drivers) {
        if (CollisionHashMap::collide(driver, data)) {
            driver->collisionMomentum = data.momentum;
            driver->speedForward *= data.speedFactor;
            driver->speedTurn *= data.speedFactor;
            switch (data.type) {
                case CollisionType::HIT:
                    driver->applyHit();
                    break;
                case CollisionType::SMASH:
                    driver->applySmash();
                    break;
                default:
                    if (!driver->isImmune()) {
                        driver->addCoin(-1);
                    }
                    Map::addEffectSparkles(driver->position);
                    break;
            }
        }
    }

    // UI updates

    bool hasChanged = FloorObject::applyAllChanges();
    if (hasChanged) {
        Map::updateMinimap();
    }

    return true;
}

void StateRaceDemo::draw(sf::RenderTarget& window) {
    // scale
    static constexpr const float NORMAL_WIDTH = 512.0f;
    const float scale = window.getSize().x / NORMAL_WIDTH;
    sf::Vector2u windowSize = game.getWindow().getSize();
    const float PAD_TOP = Map::SKY_HEIGHT_PCT * 1.5f * windowSize.y;

    window.clear(sf::Color::Black);

    // Get textures from map
    sf::Texture tSkyBack, tSkyFront, tCircuit;
    Map::skyTextures(pseudoPlayer, tSkyBack, tSkyFront);
    Map::circuitTexture(pseudoPlayer, tCircuit);

    // Create sprites and scale them accordingly
    sf::Sprite skyBack(tSkyBack), skyFront(tSkyFront), circuit(tCircuit);
    float backFactor = windowSize.x / (float)tSkyBack.getSize().x;
    float frontFactor = windowSize.x / (float)tSkyFront.getSize().x;
    skyBack.setScale(backFactor, backFactor);
    skyFront.setScale(frontFactor, frontFactor);

    // Sort them correctly in Y position and draw
    float currentHeight = PAD_TOP;
    skyBack.setPosition(0.0f, currentHeight);
    skyFront.setPosition(0.0f, currentHeight);
    window.draw(skyBack);
    window.draw(skyFront);
    currentHeight += windowSize.y * Map::SKY_HEIGHT_PCT;
    circuit.setPosition(0.0f, currentHeight);
    window.draw(circuit);
    currentHeight += windowSize.y * Map::CIRCUIT_HEIGHT_PCT;

    // Circuit objects (must be before minimap)
    std::vector<std::pair<float, sf::Sprite*>> wallObjects;
    Map::getWallDrawables(window, pseudoPlayer, scale, wallObjects);
    Map::getItemDrawables(window, pseudoPlayer, scale, wallObjects);
    Map::getDriverDrawables(
        window, firstPersonCamera ? drivers[currentTarget] : pseudoPlayer,
        drivers, scale, wallObjects);
    if (firstPersonCamera) {
        drivers[currentTarget]->getDrawables(window, scale, wallObjects);
    }
    std::sort(wallObjects.begin(), wallObjects.end(),
              [](const std::pair<float, sf::Sprite*>& lhs,
                 const std::pair<float, sf::Sprite*>& rhs) {
                  return lhs.first > rhs.first;
              });
    for (const auto& pair : wallObjects) {
        pair.second->move(0.0f, PAD_TOP);
        window.draw(*pair.second);
    }

    sf::Image black;
    black.create(windowSize.x, windowSize.y, sf::Color::Black);
    sf::Texture blackTex;
    blackTex.loadFromImage(black);

    // credits below
    sf::Sprite blackOverlay(blackTex);
    blackOverlay.setPosition(0.0f, currentHeight);
    window.draw(blackOverlay);
    blackOverlay.scale(1.0f, PAD_TOP / windowSize.y);
    blackOverlay.setPosition(0.0f, 0.0f);
    window.draw(blackOverlay);

    sf::Vector2f textPos = ABS_CREDITS;

    TextUtils::write(
        window, "super mario kart",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    
    textPos = ABS_NAME0;

    std::string score;
    score = "score              " + std::to_string(randomMessageCounter);

    // At load time
    if (!game_start)
    {
        TextUtils::write(
            window, "---------------------",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, "press select to start",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, "---------------------",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos = ABS_COPY;
        TextUtils::write(
            window, "score              0",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
    } else if (pop_request) {
        TextUtils::write(
            window, "---------------------",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, "      game over      ",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, "---------------------",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos = ABS_COPY;
        TextUtils::write(
            window, score /*"score              " + (randomMessageCounter < 10) ? "0" : "" + std::to_string(randomMessageCounter)*/,
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
    } else if (winMessageDisplayed) {
        TextUtils::write(
            window, "---------------------",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, "       you win       ",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, "---------------------",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos = ABS_COPY;
        TextUtils::write(
            window, score /*"score              " + (randomMessageCounter < 10) ? "0" : "" + std::to_string(randomMessageCounter)*/,
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
    } else {
        TextUtils::write(
            window, "",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, currentMessage,
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos += REL_NAMEDY;
        TextUtils::write(
            window, "",
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
        textPos = ABS_COPY;
        TextUtils::write(
            window, score /*"score              " + (randomMessageCounter < 10) ? "0" : "" + std::to_string(randomMessageCounter)*/,
            sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
            scale * 2.0f);
    }
       
    /* TextUtils::write(
        window, "javier gimenez",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    textPos += REL_NAMEDY;
    TextUtils::write(
        window, "victor martinez",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    textPos += REL_NAMEDY;
    TextUtils::write(
        window, "diego royo",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    textPos = ABS_COPY;
    TextUtils::write(
        window, "Score      " + std::to_string(randomMessageCounter),
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f); */

    // text message
    /* float pct = 0.0f;
    if (showMessageTime > StateRace::currentTime) {
        pct = 1.0f;
    } else if (showMessageTime >
               StateRace::currentTime + SHOW_MESSAGE_FADE_TIME) {
        pct = 1.0f - (StateRace::currentTime - showMessageTime) /
                         SHOW_MESSAGE_FADE_TIME;
    }
    if (pct > 0.0f) {
        sf::Color textColor(Color::MenuPrimary);
        textColor.a = std::min(pct * 255.0f, 255.0f);
        TextUtils::write(
            window, showMessage,
            sf::Vector2f(0.5f * windowSize.x, 0.25f * windowSize.y),
            scale * 2.0f, textColor, true, TextUtils::TextAlign::CENTER,
            TextUtils::TextVerticalAlign::MIDDLE);
    } */

    // black fade
    if (fadeTime < FADE_TIME || fadeFinished) {
        float pct = fadeTime / FADE_TIME;
        if (!raceFinished) {
            pct = 1.0f - pct;
        }
        if (fadeFinished) {
            pct = 1.0f;
        }
        int alpha = std::min(pct * 255.0f, 255.0f);
        sf::Sprite blackSprite(blackTex);
        blackSprite.setPosition(0.0f, 0.0f);
        blackSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(blackSprite);
    }
}