#include "raylib.h"
#include <vector>

const float screenWidth = 1200;
const float screenHeight = 800;
int halfScreenWidth = screenWidth / 2;
int halfScreenHeight = screenHeight / 2;
const int paddleWidth = 200;
const int paddleHeight = 10;
const int ballRadius = 10;
const float brickWidthFactor = 0.04f;  // Adjust this factor for brick width
const float brickHeightFactor = 0.02f; // Adjust this factor for brick height

struct Paddle {
    Rectangle rect;
    Vector2 speed;
};

struct Ball {
    Vector2 position;
    Vector2 speed;
};

struct Brick {
    Rectangle rect;
    bool active;
};

std::vector<Brick> bricks;
Paddle paddle;
Ball ball;

int score = 0;
int lives = 3;

enum GameState {
    GAME_INIT,
    GAME_PLAY,
    GAME_PAUSE,
    GAME_OVER
};

GameState gameState = GAME_INIT;

void InitGame(float screenWidth, float screenHeight) {
    // Initialize game objects and their properties
    paddle.rect = { screenWidth / 2 - paddleWidth / 2, screenHeight - 20, paddleWidth, paddleHeight };
    paddle.speed = { 6, 0 };

    ball.position = { screenWidth / 2, screenHeight / 2 };
    ball.speed = { 2.5, -2.5 };

    // Calculate brick width and height based on screen dimensions
    float brickWidth = screenWidth * brickWidthFactor;
    float brickHeight = screenHeight * brickHeightFactor;
    
    // Calculate the number of bricks to fit in the screen width
    int bricksPerRow = screenWidth / (brickWidth + 10);

    // Calculate the horizontal offset to center the bricks
    float xOffset = (screenWidth - (bricksPerRow * (brickWidth + 10))) / 2;

    // Initialize bricks
    bricks.clear();
    for (int i = 0; i < bricksPerRow; i++) {
        for (int j = 0; j < 8; j++) {
            Brick brick;
            brick.rect = { i * (brickWidth + 10) + xOffset, j * (brickHeight + 10) + 30, brickWidth, brickHeight };
            brick.active = true;
            bricks.push_back(brick);
        }
    }

    score = 0;
    lives = 3;
}

void UpdateGame() {
    switch (gameState) {
        case GAME_INIT:
            // Implement logic for the initial screen here
            // Display the controls screen
            DrawText("Brick Breaker Game Controls", halfScreenWidth - MeasureText("Brick Breaker Game Controls", 40) / 2, halfScreenHeight - 100, 40, WHITE);
            DrawText("To Move The Paddle Use: Left Arrow (Left) - Right Arrow (Right)", halfScreenWidth - MeasureText("To Move The Paddle Use: Left Arrow (Left) - Right Arrow (Right)", 30) / 2, halfScreenHeight - 50, 30, WHITE);
            DrawText("Good Luck", halfScreenWidth - MeasureText("Good Luck", 30) / 2, halfScreenHeight, 30, WHITE);
            DrawText("Press SPACE to Start", halfScreenWidth - MeasureText("Press SPACE to Start", 30) / 2, halfScreenHeight + 50, 30, WHITE);

            if (IsKeyPressed(KEY_SPACE)) {
                gameState = GAME_PLAY;
            }
            break;

        case GAME_PLAY:
            // Update game logic while playing
            // Move the paddle
            if (IsKeyDown(KEY_LEFT) && paddle.rect.x > 0) {
                paddle.rect.x -= paddle.speed.x;
            }
            if (IsKeyDown(KEY_RIGHT) && paddle.rect.x + paddleWidth < screenWidth) {
                paddle.rect.x += paddle.speed.x;
            }

            // Move the ball
            ball.position.x += ball.speed.x;
            ball.position.y += ball.speed.y;

            // Ball collision with walls
            if (ball.position.x >= screenWidth - ballRadius || ball.position.x <= ballRadius) {
                ball.speed.x = -ball.speed.x;
            }
            if (ball.position.y <= ballRadius) {
                ball.speed.y = -ball.speed.y;
            }

            // Ball collision with paddle
            if (CheckCollisionCircleRec(ball.position, ballRadius, paddle.rect)) {
                ball.speed.y = -ball.speed.y;
            }

            // Ball collision with bricks
            for (double i = 0; i < bricks.size(); i++) {
                if (bricks[i].active && CheckCollisionCircleRec(ball.position, ballRadius, bricks[i].rect)) {
                    bricks[i].active = false;
                    ball.speed.y = -ball.speed.y;
                    score += 10;
                }

                if (IsKeyPressed(KEY_P)) {
                    gameState = GAME_PAUSE;
                }

                if (lives <= 0) {
                    gameState = GAME_OVER;
                }
            }

            if (ball.position.y >= screenHeight) {
                lives--;
                if (lives > 0) {
                    ball.position = { screenWidth / 2, screenHeight / 2 };
                    ball.speed = { 2.5, -2.5 };
                } else {
                    gameState = GAME_OVER;
                }
            }
            break;

        case GAME_PAUSE:
            // Implement logic for the pause screen here
            // Display a "Game Paused" message on the screen
            DrawText("Game Paused", halfScreenWidth - MeasureText("Game Paused", 40) / 2, halfScreenHeight - 20, 40, WHITE);
            if (IsKeyPressed(KEY_P)) {
                gameState = GAME_PLAY;
            }
            break;

        case GAME_OVER:
            // Implement logic for the game over screen here
            DrawText("Game Over. Press R To Restart", halfScreenWidth - MeasureText("Game Over. Press R To Restart", 40) / 2, halfScreenHeight - 20, 40, WHITE);
            if (IsKeyPressed(KEY_R)) {
                InitGame(screenWidth, screenHeight);
                gameState = GAME_PLAY;
            }
            break;
    }
}

void DrawGame() {
    // Draw game objects and UI

    BeginDrawing();

    ClearBackground(DARKGRAY);

    // Draw paddle
    DrawRectangleRec(paddle.rect, RAYWHITE);

    // Draw ball
    DrawCircleV(ball.position, ballRadius, RAYWHITE);

    // Draw bricks
    for (double i = 0; i < bricks.size(); i++) {
        if (bricks[i].active) {
            DrawRectangleRec(bricks[i].rect, ORANGE);
        }
    }

    // Display the "Press P to pause" message
    DrawText("Press P to pause the game", halfScreenWidth - MeasureText("Press P to pause the game", 20) / 2, 10, 20, WHITE);

    // Draw score
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);

    // Draw lives
    DrawText(TextFormat("Lives: %d", lives), screenWidth - 80, 10, 20, WHITE);

    EndDrawing();
}

int main() {
    InitWindow(screenWidth, screenHeight, "Brick Breaker Game");

    InitGame(screenWidth, screenHeight); // Pass screen dimensions

    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    CloseWindow();

    return 0;
}
