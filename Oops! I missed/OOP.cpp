# include <iostream>
# include <cstdlib> // For random numbers
# include <fstream>
# include <ctime>   // For seeding randomness
# include <vector>
# include "SFML/Graphics.hpp"
# include "SFML/Audio.hpp"
# include "SFML/Window.hpp"

using namespace std;
using namespace sf;

class Bird
{
protected:
    Texture birdTexture;
    Sprite birdSprite;
    Vector2u textureSize; // Total size of the texture
    int frameWidth, frameHeight; // Dimensions of a single frame
    int currentFrame; // Current frame index
    int totalFrames; // Total number of frames in the sprite sheet
    float frameDuration; // Time per frame (seconds)
    Clock animationClock; // Clock for animation timing

public:
    Bird(const string& filePath, int columns, int rows, float duration)
    {
        // Load the texture
        birdTexture.loadFromFile(filePath);

        // Set up texture properties
        textureSize = birdTexture.getSize();
        frameWidth = textureSize.x / columns;
        frameHeight = textureSize.y / rows;
        totalFrames = (columns * rows) - 1;
        frameDuration = duration;

        // Set up the sprite
        birdSprite.setTexture(birdTexture);
        birdSprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        birdSprite.setScale(0.5f, 0.5f);
        currentFrame = 0;
    }

    virtual void updateAnimation()
    {
        // Update animation frame based on elapsed time
        if (animationClock.getElapsedTime().asSeconds() > frameDuration)
        {
            currentFrame = (currentFrame + 1) % totalFrames; // Cycle through frames
            int frameX = (currentFrame % (textureSize.x / frameWidth)) * frameWidth;
            int frameY = (currentFrame / (textureSize.x / frameWidth)) * frameHeight;
            birdSprite.setTextureRect(IntRect(frameX, frameY, frameWidth, frameHeight));
            animationClock.restart();
        }
    }

    Sprite& getSprite() // Provide access to the sprite
    {
        return birdSprite;
    }

    void randomizeStart(const Vector2u& windowSize)
    {
        // Randomly choose a vertical position
        float startY = rand() % (windowSize.y / 3);
        birdSprite.setPosition(-birdSprite.getGlobalBounds().width, startY); // Start just off the left
    }
};

class WhiteBird : public Bird
{
public:
    WhiteBird(const string& filePath, int columns, int rows, float duration) : Bird(filePath, columns, rows, duration) {}
};

class BlueBird : public Bird
{
public:
    BlueBird(const string& filePath, int columns, int rows, float duration) : Bird(filePath, columns, rows, duration) {}
};

class TurboBird : public Bird
{
public:
    TurboBird(const string& filePath, int columns, int rows, float duration) : Bird(filePath, columns, rows, duration) {}
};

class Movement
{
    float speed;         // Speed of movement
    Vector2f direction;  // Direction of movement (positive or negative x-axis)
    bool goingRight;     // Indicates if the bird is moving right

public:
    Movement(float birdSpeed)
    {
        speed = birdSpeed;
        goingRight = true;
        srand(time(0)); // Seed random generator
    }

    void randomizeStart(Sprite& sprite, const Vector2u& windowSize)
    {
        // Randomly choose a vertical position
        float startY = rand() % (windowSize.y / 3);

        // Randomly choose direction (0 = left to right, 1 = right to left)
        goingRight = rand() % 2;

        // Set position and direction
        if (goingRight)
        {
            sprite.setPosition(-sprite.getGlobalBounds().width, startY); // Start just off the left
            direction = { 1.0f, 0.0f }; // Moving right
            sprite.setScale(0.5f, 0.5f); // Ensure correct orientation
        }
        else
        {
            sprite.setPosition(windowSize.x + 50, startY); // Start just off the right
            direction = { -1.0f, 0.0f }; // Moving left
            sprite.setScale(-0.5f, 0.5f); // Flip horizontally
        }
    }

    void update(Sprite& sprite, const Vector2u& windowSize)
    {
        // Move the sprite in the current direction
        sprite.move(direction.x * speed, direction.y * speed);

        // Reset the bird when it goes off-screen
        if ((goingRight && sprite.getPosition().x > windowSize.x) || (!goingRight && sprite.getPosition().x < -sprite.getGlobalBounds().width))
        {
            randomizeStart(sprite, windowSize);
        }
    }
};


class SinMovement
{
    float speed;         // Speed of movement along x-axis
    float amplitude;     // Amplitude of the sine wave
    float frequency;     // Frequency of the sine wave
    Vector2f direction;  // Horizontal direction
    bool goingRight;     // Indicates if the bird is moving right
    float elapsedTime; // Tracks elapsed time for sine wave
    bool issin;

public:
    SinMovement(float birdSpeed, float waveAmplitude, float waveFrequency)
    {
        speed = birdSpeed;
        amplitude = waveAmplitude;
        frequency = waveFrequency;
        elapsedTime = 0.0f;
        issin = true;
        srand(time(0)); // Seed the random number generator
    }

    void randomizeStart(Sprite& sprite, const Vector2u& windowSize)
    {
        // Random vertical starting position
        float startY = rand() % (windowSize.y / 4);

        // Random horizontal direction
        goingRight = rand() % 2;

        if (goingRight)
        {
            sprite.setPosition(-sprite.getGlobalBounds().width, startY); // Start off the left side
            direction = { 1.0f, 0.0f }; // Moving right
            sprite.setScale(0.5f, 0.5f); // Ensure correct orientation
        }
        else
        {
            sprite.setPosition(windowSize.x + 50, startY); // Start off the right side
            direction = { -1.0f, 0.0f }; // Moving left
            sprite.setScale(-0.5f, 0.5f); // Flip horizontally
        }

        elapsedTime = 0.0f; // Reset elapsed time for sine wave
    }

    void toggleMovementMode()
    {
        issin = !issin; // Toggle between sinusoidal and straight movement
    }

    void update(Sprite& sprite, const Vector2u& windowSize, float deltaTime)
    {
        if (issin)
        {
            elapsedTime += deltaTime;

            // Calculate new x and y positions
            float posX = sprite.getPosition().x + direction.x * speed * deltaTime;
            float posY = sprite.getPosition().y + amplitude * sin(frequency * elapsedTime);

            // Set new position
            sprite.setPosition(posX, posY);
        }
        else
        {
            // Straight-line movement
            sprite.move(direction.x * speed * deltaTime, 0);
        }

        // Reset the bird when it goes off-screen
        if ((goingRight && sprite.getPosition().x > windowSize.x) || (!goingRight && sprite.getPosition().x < -sprite.getGlobalBounds().width))
        {
            randomizeStart(sprite, windowSize);
        }
    }
};

class PistolSprite
{
    Texture pistolTexture;
    Sprite pistolSprite;
    Vector2u textureSize; // Total size of the texture
    int frameWidth, frameHeight; // Dimensions of a single frame
    int currentFrame; // Current frame index
    int totalFrames; // Total number of frames in the sprite sheet
    float frameDuration; // Time per frame (seconds)
    Clock animationClock; // Clock for animation timing
    bool isShooting; // Flag to indicate if shooting animation is active

    Clock cooldownClock; // Clock to manage cooldown
    float shootCooldown; // Cooldown time (seconds)

    SoundBuffer fireSoundBuffer; // Sound buffer for shotgun firing
    SoundBuffer reloadSoundBuffer; // Sound buffer for shotgun reloading
    Sound fireSound; // Sound object for shotgun firing
    Sound reloadSound; // Sound object for shotgun reloading


public:
    // Constructor
    PistolSprite(const string& filePath, int columns, int rows, float duration)
    {
        // Load the texture
        pistolTexture.loadFromFile(filePath);
        pistolSprite.setOrigin(400.f, 380.f);

        // Set up texture properties
        textureSize = pistolTexture.getSize();
        frameWidth = (textureSize.x / columns);  // Divide texture width by number of columns
        frameHeight = (textureSize.y / rows) - 10;    // Divide texture height by number of rows
        totalFrames = columns * rows;          // Total number of frames
        frameDuration = duration;              // Duration to display each frame

        // Set up the sprite
        pistolSprite.setTexture(pistolTexture);
        pistolSprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));  // Initial frame
        pistolSprite.setScale(0.8f, 0.8f);  // Scale it down to fit the screen
        currentFrame = 0;
        isShooting = false;
        shootCooldown = 0.74f; // Cooldown of 2 seconds between shots

        // Load sound audio effects
        fireSoundBuffer.loadFromFile("Sound Effects/shotgun firing.ogg");
        reloadSoundBuffer.loadFromFile("Sound Effects/shotgun reload.ogg");

        // Set up sounds
        fireSound.setBuffer(fireSoundBuffer);
        fireSound.setVolume(30); // Adjust volume as needed
        reloadSound.setBuffer(reloadSoundBuffer);
        reloadSound.setVolume(30); // Adjust volume as needed
    }

    void startShooting()
    {
        if (cooldownClock.getElapsedTime().asSeconds() >= shootCooldown)
        {
            isShooting = true;
            currentFrame = 0; // Reset animation to the first frame
            animationClock.restart();
            cooldownClock.restart();
            fireSound.play();
            reloadSound.play();
        }
    }

    void updateAnimation()
    {
        if (isShooting)
        {
            if (animationClock.getElapsedTime().asSeconds() > frameDuration)
            {
                currentFrame++;
                if (currentFrame >= totalFrames)
                {
                    isShooting = false; // Stop animation after the last frame
                    currentFrame = 0;
                }
                else
                {
                    int frameX = (currentFrame % (textureSize.x / frameWidth)) * frameWidth;
                    int frameY = (currentFrame / (textureSize.x / frameWidth)) * frameHeight;
                    pistolSprite.setTextureRect(IntRect(frameX, frameY, frameWidth, frameHeight));
                }
                animationClock.restart();
            }
        }
    }


    Sprite& getSprite()  // Provide access to the sprite
    {
        return pistolSprite;
    }

    void setPosition(float x, float y)
    {
        pistolSprite.setPosition(x, y);
    }

    void rotateToMouse(float mouseX, float mouseY)
    {
        // Get the sprite's current position
        float spriteX = pistolSprite.getPosition().x;
        float spriteY = pistolSprite.getPosition().y;

        // Calculate the difference in positions
        float dx = mouseX - spriteX - 8000; // Adjust the offset if needed
        float dy = mouseY - spriteY;

        // Use a simple ratio to determine rotation
        float angle = (dy / dx) * 90.0f; // Map to a rough [-45, 45] range

        // Clamp the angle to keep it between -45° and +45°
        if (angle < -45.0f) angle = -45.0f;
        if (angle > 45.0f) angle = 45.0f;

        // Apply rotation to the sprite
        pistolSprite.setRotation(angle);
    }

};

void drawCrosshair(RenderWindow& window)
{
    // Get the size of the window
    Vector2u windowSize = window.getSize();

    // Get the current mouse position
    Vector2i mousePos = Mouse::getPosition(window);

    // Create the horizontal line of the crosshair
    RectangleShape horizontalLine(Vector2f(windowSize.x / 15.f, 2.f)); // 10% of the screen width, 2px height
    horizontalLine.setPosition(mousePos.x - horizontalLine.getSize().x / 2.f, mousePos.y - horizontalLine.getSize().y / 2.f);
    horizontalLine.setFillColor(Color::White); // Set the color of the crosshair

    // Create the vertical line of the crosshair
    RectangleShape verticalLine(Vector2f(2.f, windowSize.y / 15.f)); // 10% of the screen height, 2px width
    verticalLine.setPosition(mousePos.x - verticalLine.getSize().x / 2.f, mousePos.y - verticalLine.getSize().y / 2.f);
    verticalLine.setFillColor(Color::White); // Set the color of the crosshair

    // Draw the crosshair lines at the mouse position
    window.draw(horizontalLine);
    window.draw(verticalLine);
}

void constrainCursor(RenderWindow& window)
{
    // Get the current position of the mouse relative to the window
    Vector2i mousePos = Mouse::getPosition(window);

    // Get the size of the window
    Vector2u windowSize = window.getSize();

    // Constrain the mouse position to stay within the window bounds (only on the top half)
    if (mousePos.x < 0) mousePos.x = 0;
    if (mousePos.x > (int)windowSize.x - 1) mousePos.x = windowSize.x - 1;

    // Constrain the vertical position to be within the top half of the screen
    if (mousePos.y < 0) mousePos.y = 0;
    if (mousePos.y > (int)windowSize.y / 1.5) mousePos.y = windowSize.y / 1.5;

    // Reset the mouse position if it was outside the window bounds
    Mouse::setPosition(mousePos, window);
}

void GameWindow(RenderWindow& window, Sprite& backgroundSprite, Font& font1, Font& font2, WhiteBird& white, BlueBird& blue, TurboBird& turbo, Bird& monster, string ScoreFile, int& score, int& highScore, int& streak)
{


    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.8));
    window.setFramerateLimit(60);

    Clock whiteCooldownClock;
    Clock blueCooldownClock;
    Clock turboCooldownClock;
    Clock monsterCooldownClock;
    float collisionCooldown = 1.2f; // Cooldown duration in seconds
    bool isCollisionEnabled = false;
    float clickCooldown = 0.75f; // 1 second cooldown between clicks
    Clock clickCooldownClock; // Clock to track time since the last click

    // Score
    Text scoreText("Score: 0", font1, 24);
    Text highScoreText("High Score: 0", font1, 24);
    Text streakText("Streak: 0", font1, 24);
    Text missText("Misses X 0", font1, 24); // Text for misses
    missText.setFillColor(Color::Red); // Set the color of the misses text to red

    // Game Over Text
    Text gameOverText("Game Over", font1, 50);
    gameOverText.setFillColor(Color::Red); // Set color to red
    gameOverText.setPosition(window.getSize().x / 2 - 120, window.getSize().y / 2 - 50); // Center the text

    Text finalScoreText("Final Score: 0", font1, 30);
    finalScoreText.setFillColor(Color::White); // Set color to white
    finalScoreText.setPosition(window.getSize().x / 2 - 100, window.getSize().y / 2 + 10); // Position below game over text

    // Positioning
    scoreText.setPosition(10, 10);
    highScoreText.setPosition(10, 40);
    streakText.setPosition(10, 70);
    missText.setPosition(10, 450);

    // In game Music
    Music gameMusic;
    gameMusic.openFromFile("Music/ingame music.ogg");
    gameMusic.setLoop(true); // Set the music to loop
    gameMusic.setVolume(100);
    gameMusic.play();

    bool turboBirdActive = false; // Flag to check if turbo bird is active
    bool monsterActive = false;

    // Pistol Sprite
    PistolSprite shotgun("Textures/pump shotgun.png", 3, 2, 0.1f); // 3 frames per row, 2 row, 0.1 sec per frame
    shotgun.setPosition(780.f, 790.f);  // Position of the pistol at the center of the screen

    Clock clock;  // To keep track of delta time for animation

    // Flag to track if the cursor is confined
    bool cursorConstrained = false;

    // Center the mouse cursor in the window
    Mouse::setPosition(Vector2i(window.getSize().x / 3, window.getSize().y / 2), window);

    Movement whiteMovement(3.0f);
    whiteMovement.randomizeStart(white.getSprite(), window.getSize());

    Movement blueMovement(4.0f);
    blueMovement.randomizeStart(blue.getSprite(), window.getSize());

    SinMovement turboMovement(300.0f, 7.0f, 10.0f); // Speed = 300, Amplitude = 7.0, Frequency = 10.0
    turboMovement.randomizeStart(turbo.getSprite(), window.getSize());

    SinMovement monsterMovement(200.0f, 7.0f, 5.0f); // Speed = 200, Amplitude = 7.0, Frequency = 5.0
    monsterMovement.randomizeStart(monster.getSprite(), window.getSize());

    Clock deltaClock;    // Clock for delta time
    Clock modeSwitch;    // Clock to toggle turbo movement mode

    // New variables for missed shots
    int missedShots = 0; // Counter for missed shots
    bool shotMissed = false; // Flag to indicate if the last shot was missed


    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }

            // Toggle the cursor confinement when the Tab key is pressed
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Tab)
            {
                cursorConstrained = !cursorConstrained;
                if (cursorConstrained)
                {
                    // Hide the cursor when it is confined
                    window.setMouseCursorVisible(false);
                }
                else
                {
                    // Show the cursor when it is free
                    window.setMouseCursorVisible(true);
                }
            }

            // Handle mouse click (shooting)
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                // Check if the cooldown has expired
                if (clickCooldownClock.getElapsedTime().asSeconds() >= clickCooldown)
                {
                    isCollisionEnabled = true; // Enable collision detection
                    shotgun.startShooting();   // Start the shooting animation
                    clickCooldownClock.restart(); // Reset the cooldown timer
                }
            }
        }
        // Update the shooting animation
        shotgun.updateAnimation();

        // If the cursor is confined, constrain its position within the window
        if (cursorConstrained)
        {
            constrainCursor(window);
        }

        // Get the current mouse position
        Vector2i mousePos = Mouse::getPosition(window);

        // Check for collisions only when the pistol is shooting
        if (isCollisionEnabled)
        {
            bool hit = false; // Flag to check if a bird was hit

            // White bird collision
            if (white.getSprite().getGlobalBounds().contains(mousePos.x, mousePos.y) && whiteCooldownClock.getElapsedTime().asSeconds() > collisionCooldown)
            {
                score += 1; // Increment score
                streak += 1; // Increment streak
                whiteMovement.randomizeStart(white.getSprite(), window.getSize()); // Respawn bird
                whiteCooldownClock.restart(); // Reset cooldown
                isCollisionEnabled = false; // Disable collision after a hit
                hit = true; // A bird was hit
            }

            // Blue bird collision
            if (blue.getSprite().getGlobalBounds().contains(mousePos.x, mousePos.y) && blueCooldownClock.getElapsedTime().asSeconds() > collisionCooldown)
            {
                score += 2; // Increment score
                streak += 1; // Increment streak
                blueMovement.randomizeStart(blue.getSprite(), window.getSize()); // Respawn bird
                blueCooldownClock.restart(); // Reset cooldown
                isCollisionEnabled = false; // Disable collision after a hit
                hit = true; // A bird was hit
            }

            // Turbo bird collision
            if (turbo.getSprite().getGlobalBounds().contains(mousePos.x, mousePos.y) && turboCooldownClock.getElapsedTime().asSeconds() > collisionCooldown)
            {
                score += 4; // Increment score
                streak += 1; // Increment streak
                turboMovement.randomizeStart(turbo.getSprite(), window.getSize()); // Respawn bird
                turboCooldownClock.restart(); // Reset cooldown
                isCollisionEnabled = false; // Disable collision after a hit
                hit = true; // A bird was hit
            }

            // Turbo bird collision
            if (monster.getSprite().getGlobalBounds().contains(mousePos.x, mousePos.y) && turboCooldownClock.getElapsedTime().asSeconds() > collisionCooldown)
            {
                score += 10; // Increment score
                streak += 1; // Increment streak
                monsterMovement.randomizeStart(monster.getSprite(), window.getSize()); // Respawn bird
                monsterCooldownClock.restart(); // Reset cooldown
                isCollisionEnabled = false; // Disable collision after a hit
                hit = true; // A bird was hit
            }

            // If no bird was hit, increment missed shots
            if (!hit)
            {
                missedShots++; // Increment missed shots
                streak = 0;
                if (missedShots >= 5)
                {
                    score -= 10; // Deduct 10 points from score
                }
            }
            // Set isCollisionEnabled to false immediately after checking for collisions
            isCollisionEnabled = false;
        }

        if (missedShots >= 10)
        {
            // Update final score text
            finalScoreText.setString("Final Score: " + to_string(score));

            // Render game over screen
            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(gameOverText);
            window.draw(finalScoreText);
            window.display();

            // Wait for a moment before closing or restarting
            sleep(seconds(3)); // Pause for 3 seconds
            window.close(); // Close the window or you can restart the game here
            return; // Exit the function
        }

        if (streak >= 6 && !turboBirdActive)
        {
            turboBirdActive = true; // Set the flag to true
            turboMovement.randomizeStart(turbo.getSprite(), window.getSize()); // Spawn the Turbo Bird
        }

        if (streak >= 8 && !monsterActive)
        {
            monsterActive = true; // Set the flag to true
            monsterMovement.randomizeStart(monster.getSprite(), window.getSize()); // Spawn the Turbo Bird
        }

        float deltaTime = deltaClock.restart().asSeconds(); // Time elapsed since the last frame

        // Update bird animations and movements
        white.updateAnimation();
        whiteMovement.update(white.getSprite(), window.getSize());
        blue.updateAnimation();
        blueMovement.update(blue.getSprite(), window.getSize());
        if (turboBirdActive) // Only update Turbo Bird if it's active
        {
            turbo.updateAnimation();
            turboMovement.update(turbo.getSprite(), window.getSize(), deltaTime);
        }

        // Toggle turbo bird's movement mode every 3 seconds
        if (modeSwitch.getElapsedTime().asSeconds() > 1.0f && turboBirdActive)
        {
            turboMovement.toggleMovementMode();
            modeSwitch.restart();
        }

        if (monsterActive) // Only update Turbo Bird if it's active
        {
            monster.updateAnimation();
            monsterMovement.update(monster.getSprite(), window.getSize(), deltaTime);
        }

        // Toggle turbo bird's movement mode every 3 seconds
        if (modeSwitch.getElapsedTime().asSeconds() > 1.0f && monsterActive)
        {
            monsterMovement.toggleMovementMode();
            modeSwitch.restart();
        }

        // Update texts
        scoreText.setString("Score: " + to_string(score));
        highScoreText.setString("High Score: " + to_string(highScore));
        streakText.setString("Streak: " + to_string(streak));
        missText.setString("Misses X " + to_string(missedShots));

        // Get mouse position
        shotgun.rotateToMouse(mousePos.x, mousePos.y);


        window.clear(Color::Black);
        window.draw(backgroundSprite);
        window.draw(shotgun.getSprite());
        window.draw(white.getSprite());
        window.draw(blue.getSprite());
        if (turboBirdActive) // Draw Turbo Bird only if it's active
        {
            window.draw(turbo.getSprite());
        }
        if (monsterActive) // Draw Turbo Bird only if it's active
        {
            window.draw(monster.getSprite());
        }
        window.draw(scoreText);
        window.draw(highScoreText);
        window.draw(streakText);
        window.draw(missText);

        // Draw the crosshair
        drawCrosshair(window);
        window.display();
    }
    // Update high score if needed
    if (score > highScore)
    {
        highScore = score;
        ofstream writeFile(ScoreFile);
        if (writeFile.is_open())
        {
            writeFile << highScore;
            writeFile.close();
        }
    }
    else
    {
        cout << "High score remains: " << highScore << endl;
    }
}

// Forward declaration of functions
void mainMenu(RenderWindow& window, Sprite& backgroundSprite, Font& font1, Font& font2, WhiteBird& white, BlueBird& blue, TurboBird& turbo, Bird& monster, string ScoreFile, int& score, int& highScore, int& streak);
void showGuidelines(RenderWindow& window, Sprite& backgroundSprite, Font& font1, Font& font2, WhiteBird& white, BlueBird& blue, TurboBird& turbo, Bird& monster, string ScoreFile, int& score, int& highScore, int& streak);

void showGuidelines(RenderWindow& window, Sprite& backgroundSprite, Font& font1, Font& font2, WhiteBird& white, BlueBird& blue, TurboBird& turbo, Bird& monster, string ScoreFile, int& score, int& highScore, int& streak)
{
    // Back Button
    Texture backbuttontex;
    Sprite backbuttonSprite;
    backbuttontex.loadFromFile("Textures/back.png");
    backbuttonSprite.setTexture(backbuttontex);

    // Set the origin to the center of the sprite (back button)
    FloatRect bounds = backbuttonSprite.getGlobalBounds();
    backbuttonSprite.setOrigin(bounds.width / 2, bounds.height / 2);

    backbuttonSprite.setPosition(450.0f, 650.0f); // Position the sprite
    Vector2f originalScale = backbuttonSprite.getScale();
    Vector2f hoverScale = originalScale * 0.97f; // Slightly smaller scale for hover effect

    // Create a text object for the guidelines
    Text guidelinesText;
    guidelinesText.setFont(font1);
    guidelinesText.setCharacterSize(25);
    guidelinesText.setFillColor(Color::White);
    guidelinesText.setPosition(50.f, 50.f);

    // Set the guidelines text
    string guidelines = "Game Guidelines:\n\n"
        "The goal of the game is to shoot as many birds as possible while avoiding\n misses.\n\n"
        "Each successful shot increases your score, and achieving a streak of 6 \nkills will introduce a new bird with unique movement patterns.\n\n"
        "However, be careful—missing shots can break your streak\n and allowing too many birds to escape will end the game!\n\n"
        "Each bird has a different point value :\n"
        "\n"
        "White Bird : 1 point\n"
        "Blue Bird : 2 points\n"
        "Turbo Bird : 4 points\n"
        "Monster : 7 points\n";

    guidelinesText.setString(guidelines);

    // Create a text object for the note about misses
    Text noteText;
    noteText.setFont(font1);
    noteText.setCharacterSize(25);
    noteText.setFillColor(Color::Red); // Set the color of the note text to red
    noteText.setString("NOTE: 10 MISSES WILL END THE GAME!"); // Set the note text
    noteText.setPosition(250.f, 570.f); // Position the note text below the guidelines


    // Main loop for the guidelines window
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2i mousePosition = Mouse::getPosition(window);
                if (backbuttonSprite.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                {
                    // Calling Main Menu
                    mainMenu(window, backgroundSprite, font1, font2, white, blue, turbo, monster, ScoreFile, score, highScore, streak);
                }
            }
        }

        // Play Button Scale down when cursor on top
        Vector2i mousePos = Mouse::getPosition(window);
        if (backbuttonSprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
        {
            backbuttonSprite.setScale(hoverScale);
        }
        else
        {
            backbuttonSprite.setScale(originalScale);         // Set back to default size if the mouse is not over playbutton
        }

        window.clear(Color::Black);
        window.draw(backgroundSprite); // Draw background if needed
        window.draw(guidelinesText);
        window.draw(noteText);
        window.draw(backbuttonSprite);
        window.display();
    }
}

void mainMenu(RenderWindow& window, Sprite& backgroundSprite, Font& font1, Font& font2, WhiteBird& white, BlueBird& blue, TurboBird& turbo, Bird& monster, string ScoreFile, int& score, int& highScore, int& streak)
{
    static Music bgMusic; // Declare bgMusic as static to maintain its state
    static bool isMusicPlaying = false; // Track if music is currently playing

    // Title
    Text GameName;
    GameName.setFont(font1);
    GameName.setCharacterSize(150);
    GameName.setPosition(250.f, 110.f);
    GameName.setFillColor(Color::White);
    GameName.setString("OOPS!");

    Text GameName1;
    GameName1.setFont(font1);
    GameName1.setCharacterSize(75);
    GameName1.setPosition(320.f, 260.f);
    GameName1.setFillColor(Color::White);
    GameName1.setString("I MISSED");

    // Subtext
    Text SubText;
    SubText.setFont(font1);
    SubText.setCharacterSize(30);
    SubText.setPosition(350.f, 120.f);
    SubText.setFillColor(Color::White);
    SubText.setString("Limited Edition");

    // Load and play music if not already playing
    if (!isMusicPlaying) {
        bgMusic.openFromFile("Music/main menu.ogg");
        bgMusic.setLoop(true); // Set the music to loop
        bgMusic.setVolume(100);
        bgMusic.play();
        isMusicPlaying = true; // Update the music state
    }

    // Play button
    Texture playbuttontex;
    Sprite playbuttonsprite;
    playbuttontex.loadFromFile("Textures/play1.png");
    playbuttonsprite.setTexture(playbuttontex);

    //Guide Button
    Texture guidebuttontex;
    Sprite guidebuttonSprite;
    guidebuttontex.loadFromFile("Textures/guide.png");
    guidebuttonSprite.setTexture(guidebuttontex);
    guidebuttonSprite.setScale(0.2f, 0.2f);

    //soundon
    Texture soundontex;
    Sprite soundonsprite;
    soundontex.loadFromFile("Textures/soundon.png");
    soundonsprite.setTexture(soundontex);
    soundonsprite.setScale(0.7f, 0.7f);
    //soundoff
    Texture soundofftex;
    Sprite soundoffsprite;
    soundofftex.loadFromFile("Textures/soundoff.png");
    soundoffsprite.setTexture(soundofftex);
    soundoffsprite.setScale(0.7f, 0.7f);

    // Set the origin to the center of the sprite (play button)
    FloatRect bounds = playbuttonsprite.getGlobalBounds();
    playbuttonsprite.setOrigin(bounds.width / 2, bounds.height / 2); // Set origin to center

    playbuttonsprite.setPosition(450.0f, 500.0f); // Position the sprite
    Vector2f originalScale = playbuttonsprite.getScale();
    Vector2f hoverScale = originalScale * 0.97f; // Slightly smaller scale for hover effect

    // Set the origin to the center of the sprite (guide button)
    FloatRect bounds1 = guidebuttonSprite.getGlobalBounds();
    guidebuttonSprite.setOrigin(bounds1.width / 2, bounds1.height / 2); // Set origin to center

    guidebuttonSprite.setPosition(550.0f, 550.0f); // Position the sprite
    Vector2f originalScale1 = guidebuttonSprite.getScale();
    Vector2f hoverScale1 = originalScale1 * 0.97f; // Slightly smaller scale for hover effect

    // Set the origin to the center of the sprite (soundon button)
    FloatRect bounds2 = soundonsprite.getGlobalBounds();
    soundonsprite.setOrigin(bounds2.width / 2, bounds2.height / 2); // Set origin to center

    soundonsprite.setPosition(300.0f, 575.0f); // Position the sprite
    Vector2f originalScale2 = soundonsprite.getScale();
    Vector2f hoverScale2 = originalScale2 * 0.97f; // Slightly smaller scale for hover effect

    // Set the origin to the center of the sprite (soundoff button)
    FloatRect bounds3 = soundoffsprite.getGlobalBounds();
    soundonsprite.setOrigin(bounds3.width / 2, bounds3.height / 2); // Set origin to center

    soundoffsprite.setPosition(274.0f, 547.0f); // Position the sprite
    Vector2f originalScale3 = soundoffsprite.getScale();
    Vector2f hoverScale3 = originalScale3 * 0.97f; // Slightly smaller scale for hover effect

    // Initialize Birds
    Movement whiteMovement(3.0f);
    whiteMovement.randomizeStart(white.getSprite(), window.getSize());

    Movement blueMovement(4.0f);
    blueMovement.randomizeStart(blue.getSprite(), window.getSize());

    SinMovement turboMovement(300.0f, 7.0f, 10.0f); // Speed = 100, Amplitude = 6.0, Frequency = 10.0
    turboMovement.randomizeStart(turbo.getSprite(), window.getSize());

    Clock deltaClock;    // Clock for delta time
    Clock modeSwitch;    // Clock to toggle turbo movement mode

    window.setFramerateLimit(60);
    bool isSoundOn = true; // Track sound state
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2i mousePosition = Mouse::getPosition(window);
                if (playbuttonsprite.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                {
                    bgMusic.stop();
                    isMusicPlaying = false; // Reset music state
                    GameWindow(window, backgroundSprite, font1, font2, white, blue, turbo, monster, ScoreFile, score, highScore, streak); // Open the new window
                }

                // Call showGuidelines when the guide button is clicked
                if (guidebuttonSprite.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                {
                    showGuidelines(window, backgroundSprite, font1, font2, white, blue, turbo, monster, ScoreFile, score, highScore, streak); // Pass the main window and font to the guidelines function
                }

                // Toggle sound on/off
                if (soundonsprite.getGlobalBounds().contains(mousePosition.x, mousePosition.y) && isSoundOn)
                {
                    bgMusic.pause(); // Pause the music
                    isSoundOn = false; // Update sound state
                }
                else if (soundoffsprite.getGlobalBounds().contains(mousePosition.x, mousePosition.y) && !isSoundOn)
                {
                    bgMusic.play(); // Resume the music
                    isSoundOn = true; // Update sound state
                }
            }

            // Play Button Scale down when cursor on top
            Vector2i mousePos = Mouse::getPosition(window);
            if (playbuttonsprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
            {
                playbuttonsprite.setScale(hoverScale);
            }
            else
            {
                playbuttonsprite.setScale(originalScale);         // Set back to default size if the mouse is not over playbutton
            }

            // Guide Button Scale down when cursor on top
            if (guidebuttonSprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
            {
                guidebuttonSprite.setScale(hoverScale1);
            }
            else
            {
                guidebuttonSprite.setScale(originalScale1);         // Set back to default size if the mouse is not over playbutton
            }

            // Sound Button Scale down when cursor on top
            if (isSoundOn)
            {
                if (soundonsprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
                {
                    soundonsprite.setScale(hoverScale2);
                }
                else
                {
                    soundonsprite.setScale(originalScale2);
                }
            }
            else
            {
                if (soundoffsprite.getGlobalBounds().contains((float)(mousePos.x), (float)(mousePos.y)))
                {
                    soundoffsprite.setScale(hoverScale3);
                }
                else
                {
                    soundoffsprite.setScale(originalScale3);
                }
            }
        }

        float deltaTime = deltaClock.restart().asSeconds(); // Time elapsed since the last frame

        // Update bird animations and movements
        white.updateAnimation();
        whiteMovement.update(white.getSprite(), window.getSize());
        blue.updateAnimation();
        blueMovement.update(blue.getSprite(), window.getSize());
        turbo.updateAnimation();
        turboMovement.update(turbo.getSprite(), window.getSize(), deltaTime);

        // Toggle turbo bird's movement mode every 3 seconds
        if (modeSwitch.getElapsedTime().asSeconds() > 1.0f)
        {
            turboMovement.toggleMovementMode();
            modeSwitch.restart();
        }

        // Render the main menu
        window.clear();
        window.draw(backgroundSprite);
        window.draw(GameName1);
        window.draw(SubText);
        window.draw(white.getSprite());
        window.draw(blue.getSprite());
        window.draw(turbo.getSprite());
        window.draw(GameName);


        window.draw(playbuttonsprite);
        window.draw(guidebuttonSprite);
        // Draw the appropriate sound button based on the sound state
        if (isSoundOn)
        {
            window.draw(soundonsprite);
        }
        else
        {
            window.draw(soundoffsprite);
        }



        window.display();
    }
}

int main()
{
    const string ScoreFile = "Score.txt";
    int score = 0;         // Current score
    int highScore = 0;     // High score
    int streak = 0;        // Current kill streak

    ifstream readFile(ScoreFile);
    if (readFile.is_open())
    {
        readFile >> highScore;
        readFile.close();
    }
    else
    {
        highScore = 0;  // Set high score to 0 if the file does not exist or can't be read
    }

    RenderWindow window(VideoMode(900, 800), "OOPS! I MISSED", Style::Default);

    // Background Image
    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/landscape.jpg");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.5));

    // Font
    Font font1;
    font1.loadFromFile("Fonts/Super Childish.ttf");
    Font font2;
    font2.loadFromFile("Fonts/Coffee Spark.ttf");

    // Birds Sprite
    WhiteBird white("Textures/flappy bird white.png", 5, 3, 0.1f); // 5 columns, 3 rows, 0.1 seconds per frame
    BlueBird blue("Textures/flappy bird blue.png", 4, 2, 0.1f); // 4 columns, 2 rows, 0.1 seconds per frame
    TurboBird turbo("Textures/turbo bird.png", 4, 1, 0.1f); // 4 columns, 1 rows, 0.1 seconds per frame
    Bird monster("Textures/monster.png", 4, 1, 0.1f); // 4 columns, 1 rows, 0.1 seconds per frame

    // Calling Main Menu
    mainMenu(window, backgroundSprite, font1, font2, white, blue, turbo, monster, ScoreFile, score, highScore, streak);
    GameWindow(window, backgroundSprite, font1, font2, white, blue, turbo, monster, ScoreFile, score, highScore, streak);

    return 0;
}
