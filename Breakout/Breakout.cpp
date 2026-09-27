#include <iostream>
#include <string>
#include <conio.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <windows.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <iomanip>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

using namespace std;

// ==================== ثابت‌های بازی ====================
const int BOARD_WIDTH = 51;
const int BOARD_HEIGHT = 25;
const int BRICK_ROWS = 5;
const int BRICK_COLS = 10;
const float BALL_SPEED = 0.5f;
const float PADDLE_SPEED = 4.0f;
const int MAX_POWERUPS = 10;
const float POWERUP_SPEED = 0.2f;
const float MIN_PADDLE_WIDTH = 2.0f;
const float MAX_PADDLE_WIDTH = 20.0f;
const float PADDLE_CHANGE = 2.0f;
const float INITIAL_PADDLE_Y = BOARD_HEIGHT - 2.0f;
const float INITIAL_PADDLE_X = BOARD_WIDTH / 2.0f - 5.0f;
const float INITIAL_BALL_X = INITIAL_PADDLE_X + 5.0f;
const float INITIAL_BALL_Y = INITIAL_PADDLE_Y - 0.5f;

// حالت‌های بازی
const int GAME_MODE_SIMPLE = 0;
const int GAME_MODE_ADVANCED = 1;

// انواع پاورآپ
const int POWERUP_BOMB = 0;
const int POWERUP_HEART = 1;

// ==================== رنگ‌های ANSI ====================
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string BLUE = "\033[34m";
const string RED = "\033[31m";
const string WHITE = "\033[37m";
const string GOLD = "\033[93m";
const string YELLOW = "\033[33m";
const string GREEN = "\033[32m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string BRIGHT_RED = "\033[91m";
const string BRIGHT_GREEN = "\033[92m";
const string BRIGHT_YELLOW = "\033[93m";
const string BRIGHT_BLUE = "\033[94m";
const string BRIGHT_MAGENTA = "\033[95m";
const string BRIGHT_CYAN = "\033[96m";
const string PURPLE = "\033[38;5;129m";
const string ORANGE = "\033[38;5;208m";
const string PINK = "\033[38;5;201m";
const string BROWN = "\033[38;5;94m";

// رنگ‌های آجرها
const string BRICK_COLOR_RED = "\033[38;5;196m";
const string BRICK_COLOR_ORANGE = "\033[38;5;208m";
const string BRICK_COLOR_YELLOW = "\033[38;5;226m";
const string BRICK_COLOR_GREEN = "\033[38;5;46m";
const string BRICK_COLOR_BLUE = "\033[38;5;27m";
const string BRICK_COLOR_PURPLE = "\033[38;5;129m";
const string BRICK_COLOR_PINK = "\033[38;5;201m";
const string BRICK_COLOR_CYAN = "\033[38;5;51m";

// ==================== ساختارهای بازی ====================
struct Ball
{
    float x, y;
    float vx, vy;
    float radius;
};

struct Paddle
{
    float x, y;
    float width, height;
    float speed;
};

struct Brick
{
    float x, y;
    float width, height;
    bool visible;
};

struct Powerup
{
    float x, y;
    float vy;
    int type;
    bool active;
    bool collected;
};

struct GameRecord
{
    string playerName;
    string score;
    string gameModeStr;
    string result;
    string Date;
    string Time;
};

// ==================== متغیرهای سراسری ====================
Ball ball;
Paddle paddle;
Brick bricks[BRICK_ROWS][BRICK_COLS];
Powerup powerups[MAX_POWERUPS];
int score = 0;
int lives = 3;
int gameMode = GAME_MODE_SIMPLE;
bool gameOver = false;
bool gameStarted = false;
bool isPaused = false;
string currentPlayerName = "";
vector<string> screenBuffer;

// ==================== متغیرهای منو ====================
int currentOption = 0;
int currentSettingsOption = 0;
int currentHelpOption = 0;
int currentBallOption = 0;
int currentPaddleOption = 0;
string selectedBallType = "●";
string selectedPaddleType = "■";
bool inBallSettings = false;
bool inPaddleSettings = false;

// ==================== آرایه‌های منو ====================
string mainMenuOptions[] = {"NEW GAME", "LOAD GAME", "HELP", "GAME HISTORY", "SETTINGS", "EXIT"};
string pauseMenuOptions[] = {"RESUME", "RESTART", "MENU"};
string settingsOptions[] = {"BALL TYPE", "PADDLE TYPE", "BACK TO MENU"};
string helpOptions[] = {"CONTROLS", "ABOUT GAME", "BACK TO MENU"};
string ballTypes[] = {"◉", "✿", "✪", "✷", "✯", "✧", "●", "◍"};
string paddleTypes[] = {"✷", "❥", "▼", "▽", "□", "■", "◗", "●"};

// ==================== متغیرهای صدا ====================
bool soundsLoaded = false;

// ==================== اعلان توابع مدیریت صدا ====================
void preloadSounds();
void playSound(const string &filename);
void playBrickSound();
void playWallSound();
void playVictorySound();
void playGameOverSound();
void playLoseBallSound();

// ==================== اعلان توابع اصلی بازی ====================
void runGame();
void newGame();
void loadGame();
void saveGame();
void clearSavedGame();
void initializeBricks();
void resetBallAndPaddle();
void updateBall(float deltaTime);
void checkPaddleCollision();
void checkBrickCollision();
void checkWallCollision();
void activatePowerUps(float deltaTime);
void powerUp(float x, float y);

// ==================== اعلان توابع رندر و نمایش ====================
void initScreenBuffer();
void clearScreenBuffer();
void renderToBuffer();
void renderWithColorsOptimized();
void setCursorPosition(int x, int y);
void hideCursor();
void clearMenuArea();
// ==================== اعلان توابع منو ====================
void printMenu();
void menuMove();
void pauseMenu();
void pauseMove();
void resume();
void restart();
void showSettings();
void settingsMove();
void showBallSettings();
void ballSettingsMove();
void showPaddleSettings();
void paddleSettingsMove();
void selectGameMode();
void printLoading();
void showHelp();
void helpMove();
void aboutGame();
void controls();

// ==================== اعلان توابع UI و گرافیک ====================
void printVictory();
void printGameOver();
void printExit(int selected);
void exitMove();
void showGameHistory();
void saveGameHistory(const string &result);

// ==================== اعلان توابع تنظیمات ====================
void saveSettings();
void loadSettings();

// ==================== توابع مدیریت صدا ====================
void preloadSounds()
{
    if (soundsLoaded)
        return;

    string sounds[] = {
        "brick.wav",
        "wall.wav",
        "loseball.wav",
        "Victory.wav",
        "GameOver.wav"};

    for (int i = 0; i < 5; i++)
    {
        PlaySound(sounds[i].c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
        Sleep(20);
    }

    PlaySound(NULL, NULL, 0);
    soundsLoaded = true;
}

void playSound(const string &filename)
{
    PlaySound(filename.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

void playBrickSound() { playSound("brick.wav"); }
void playWallSound() { playSound("wall.wav"); }
void playVictorySound() { playSound("Victory.wav"); }
void playGameOverSound() { playSound("GameOver.wav"); }
void playLoseBallSound() { playSound("loseball.wav"); }

// ==================== توابع اصلی بازی ====================
void initializeBricks()
{
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            bricks[i][j].x = 1.0f + j * 5.0f; // فاصله افقی آجرها
            bricks[i][j].y = 1.0f + i * 2.0f; // فاصله عمودی آجرها
            bricks[i][j].width = 4.0f;
            bricks[i][j].height = 1.0f;
            bricks[i][j].visible = true;
        }
    }
}

void resetBallAndPaddle()
{
    paddle.x = INITIAL_PADDLE_X;
    paddle.y = INITIAL_PADDLE_Y;

    ball.x = INITIAL_BALL_X;
    ball.y = INITIAL_BALL_Y;
    ball.vx = BALL_SPEED;
    ball.vy = -BALL_SPEED;
}

void updateBall(float deltaTime)
{
    ball.x += ball.vx * deltaTime * 15.0f; // 15 ضریب برای بهتر شدن سرعت در کنسول
    ball.y += ball.vy * deltaTime * 15.0f;

    float maxSpeed = BALL_SPEED * 2.0f;
    float currentSpeed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);

    // ایجاد محدودیت برای سرعت توپ به منظور کنترل بهتر
    if (currentSpeed > maxSpeed)
    {
        float scale = maxSpeed / currentSpeed;
        ball.vx *= scale;
        ball.vy *= scale;
    }

    float minSpeed = BALL_SPEED * 0.8f;
    if (currentSpeed < minSpeed && currentSpeed > 0)
    {
        float scale = minSpeed / currentSpeed;
        ball.vx *= scale;
        ball.vy *= scale;
    }
}

void checkPaddleCollision()
{
    bool isInVerticalRange = (ball.y + ball.radius >= paddle.y) &&
                             (ball.y - ball.radius <= paddle.y + paddle.height);
    bool isInHorizontalRange = (ball.x >= paddle.x) &&
                               (ball.x <= paddle.x + paddle.width);

    if (isInVerticalRange && isInHorizontalRange)
    {
        float relativeHitPoint = (ball.x - paddle.x) / paddle.width;

        const float MAX_ANGLE = 0.7854f; // 45 درجه
        float angle = (relativeHitPoint - 0.5f) * MAX_ANGLE * 2.0f;

        float totalSpeed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);

        if (totalSpeed < BALL_SPEED)
        {
            totalSpeed = BALL_SPEED;
        }

        ball.vx = totalSpeed * sin(angle);
        ball.vy = -abs(totalSpeed * cos(angle));
        ball.y = paddle.y - ball.radius - 0.1f;

        playWallSound();
    }
}

void checkBrickCollision()
{
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            if (!bricks[i][j].visible)
                continue;

            Brick &brick = bricks[i][j];

            bool overlapX = (ball.x + ball.radius >= brick.x) &&
                            (ball.x - ball.radius <= brick.x + brick.width);
            bool overlapY = (ball.y + ball.radius >= brick.y) &&
                            (ball.y - ball.radius <= brick.y + brick.height);

            if (overlapX && overlapY)
            {
                brick.visible = false;
                score += 10;

                playBrickSound();

                if (gameMode == GAME_MODE_ADVANCED && (rand() % 100) < 70) // احتمال 70 درصد
                {
                    // ایجاد پاورآپ
                    for (int k = 0; k < MAX_POWERUPS; k++)
                    {
                        if (!powerups[k].active)
                        {
                            powerups[k].x = brick.x + brick.width / 2;
                            powerups[k].y = brick.y + brick.height / 2;
                            powerups[k].vy = POWERUP_SPEED;
                            powerups[k].type = rand() % 2; // بمب یا قلب
                            powerups[k].active = true;
                            powerups[k].collected = false;
                            break;
                        }
                    }
                }

                // بررسی عمق و جهت برخورد توپ با آجر
                float overlapLeft = (ball.x + ball.radius) - brick.x;
                float overlapRight = (brick.x + brick.width) - (ball.x - ball.radius);
                float overlapTop = (ball.y + ball.radius) - brick.y;
                float overlapBottom = (brick.y + brick.height) - (ball.y - ball.radius);
                float minOverlapX = min(overlapLeft, overlapRight);
                float minOverlapY = min(overlapTop, overlapBottom);

                if (minOverlapX < minOverlapY)
                {
                    ball.vx = -ball.vx;
                    if (overlapLeft < overlapRight)
                    {
                        ball.x = brick.x - ball.radius - 0.1f; // 0.1 برای ایجاد فاصله بین توپ و آجر و جلوگیری از همپوشانی آنها در فریم بعدی
                    }
                    else
                    {
                        ball.x = brick.x + brick.width + ball.radius + 0.1f;
                    }
                }
                else
                {
                    ball.vy = -ball.vy;
                    if (overlapTop < overlapBottom)
                    {
                        ball.y = brick.y - ball.radius - 0.1f;
                    }
                    else
                    {
                        ball.y = brick.y + brick.height + ball.radius + 0.1f;
                    }
                }

                float speedIncrease = 1.01f;
                ball.vx *= speedIncrease;
                ball.vy *= speedIncrease;
            }
        }
    }
}

void checkWallCollision()
{
    // دیوار چپ
    if (ball.x - ball.radius <= 1.0f)
    {
        ball.vx = abs(ball.vx);
        ball.x = 1.0f + ball.radius + 0.1f;
        playWallSound();
    }
    // دیوار راست
    if (ball.x + ball.radius >= BOARD_WIDTH - 1.0f)
    {
        ball.vx = -abs(ball.vx);
        ball.x = BOARD_WIDTH - 1.0f - ball.radius - 0.1f;
        playWallSound();
    }
    // دیوار بالا
    if (ball.y - ball.radius <= 1.0f)
    {
        ball.vy = abs(ball.vy);
        ball.y = 1.0f + ball.radius + 0.1f;
        playWallSound();
    }
    // دیوار پایین
    if (ball.y > BOARD_HEIGHT)
    {
        lives--;
        playLoseBallSound();
        if (lives > 0)
        {
            resetBallAndPaddle();
            gameStarted = false;
        }
        else
        {
            gameOver = true;
        }
    }
}

void activatePowerUps(float deltaTime)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (powerups[i].active && !powerups[i].collected)
        {
            powerups[i].y += powerups[i].vy;

            bool hitPaddle = (powerups[i].y >= paddle.y) &&
                             (powerups[i].y <= paddle.y + paddle.height) &&
                             (powerups[i].x >= paddle.x) &&
                             (powerups[i].x <= paddle.x + paddle.width);

            if (hitPaddle)
            {
                powerups[i].collected = true;

                if (powerups[i].type == POWERUP_BOMB)
                {
                    paddle.width = max(MIN_PADDLE_WIDTH, paddle.width - PADDLE_CHANGE); // برای کنترل طول راکت محدوده ای در نظر گرفته شده
                }
                else if (powerups[i].type == POWERUP_HEART)
                {
                    paddle.width = min(MAX_PADDLE_WIDTH, paddle.width + PADDLE_CHANGE);
                }
            }
            if (powerups[i].y > BOARD_HEIGHT)
            {
                powerups[i].active = false;
            }
        }
    }
}

// ==================== توابع رندر ====================
void initScreenBuffer()
{
    screenBuffer.resize(BOARD_HEIGHT + 3);
    for (int i = 0; i < screenBuffer.size(); i++)
    {
        screenBuffer[i].resize(BOARD_WIDTH, ' ');
    }
}

void clearScreenBuffer()
{
    for (int i = 0; i < screenBuffer.size(); i++)
    {
        for (int j = 0; j < BOARD_WIDTH; j++)
        {
            screenBuffer[i][j] = ' ';
        }
    }
}

void setCursorPosition(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void renderToBuffer()
{
    clearScreenBuffer();

    // کادر
    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        screenBuffer[0][i] = '-';
        screenBuffer[BOARD_HEIGHT - 1][i] = '-';
    }

    for (int i = 1; i < BOARD_HEIGHT - 1; i++)
    {
        screenBuffer[i][0] = '|';
        screenBuffer[i][BOARD_WIDTH - 1] = '|';
    }

    // راکت
    int paddleYPos = (int)paddle.y;
    if (paddleYPos >= 0 && paddleYPos < BOARD_HEIGHT)
    {
        int paddleStart = max(1, (int)paddle.x);
        int paddleEnd = min(BOARD_WIDTH - 2, (int)(paddle.x + paddle.width));

        for (int i = paddleStart; i <= paddleEnd; i++)
        {
            if (i >= 1 && i < BOARD_WIDTH - 1)
            {
                screenBuffer[paddleYPos][i] = '=';
            }
        }
    }

    // توپ
    int ballX = (int)ball.x;
    int ballY = (int)ball.y;
    if (ballX >= 1 && ballX < BOARD_WIDTH - 1 && ballY >= 1 && ballY < BOARD_HEIGHT - 1)
    {
        screenBuffer[ballY][ballX] = 'O';
    }

    // آجرها
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            if (bricks[i][j].visible)
            {
                int brickX = (int)bricks[i][j].x;
                int brickY = (int)bricks[i][j].y;
                int brickWidth = (int)bricks[i][j].width;

                brickX = max(1, brickX);
                brickY = max(1, brickY);

                for (int k = 0; k < brickWidth && brickX + k < BOARD_WIDTH - 1; k++)
                {
                    if (brickY >= 1 && brickY < BOARD_HEIGHT - 1)
                    {
                        screenBuffer[brickY][brickX + k] = 'B';
                    }
                }
            }
        }
    }

    // پاورآپ‌ها
    if (gameMode == GAME_MODE_ADVANCED)
    {
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            if (powerups[i].active && !powerups[i].collected)
            {
                int px = (int)powerups[i].x;
                int py = (int)powerups[i].y;

                if (px >= 1 && px < BOARD_WIDTH - 1 && py >= 1 && py < BOARD_HEIGHT - 1)
                {
                    if (powerups[i].type == POWERUP_BOMB)
                    {
                        screenBuffer[py][px] = 'X';
                    }
                    else
                    {
                        screenBuffer[py][px] = '+';
                    }
                }
            }
        }
    }
}

void renderWithColorsOptimized()
{
    if (!isPaused)
    {
        setCursorPosition(0, 0);
    }

    string fullFrame;
    fullFrame.reserve(BOARD_HEIGHT * (BOARD_WIDTH + 20));

    for (int i = 0; i < BOARD_HEIGHT; i++)
    {
        if (i < screenBuffer.size())
        {
            for (int j = 0; j < BOARD_WIDTH; j++)
            {
                char ch = screenBuffer[i][j];

                if (ch == 'B')
                {
                    bool isBrick = false;
                    int brickRow = -1, brickCol = -1;

                    for (int r = 0; r < BRICK_ROWS; r++)
                    {
                        bool foundInThisRow = false;
                        for (int c = 0; c < BRICK_COLS; c++)
                        {
                            if (bricks[r][c].visible)
                            {
                                int brickX = (int)bricks[r][c].x;
                                int brickY = (int)bricks[r][c].y;
                                int brickWidth = (int)bricks[r][c].width;

                                if (i == brickY && j >= brickX && j < brickX + brickWidth)
                                {
                                    isBrick = true;
                                    brickRow = r;
                                    brickCol = c;
                                    foundInThisRow = true;
                                    break;
                                }
                            }
                        }
                        if (foundInThisRow)
                            break;
                    }

                    if (isBrick)
                    {
                        int colorIndex = (brickRow * BRICK_COLS + brickCol) % 8;

                        switch (colorIndex)
                        {
                        case 0:
                            fullFrame += BRICK_COLOR_RED;
                            break;
                        case 1:
                            fullFrame += BRICK_COLOR_ORANGE;
                            break;
                        case 2:
                            fullFrame += BRICK_COLOR_YELLOW;
                            break;
                        case 3:
                            fullFrame += BRICK_COLOR_GREEN;
                            break;
                        case 4:
                            fullFrame += BRICK_COLOR_BLUE;
                            break;
                        case 5:
                            fullFrame += BRICK_COLOR_PURPLE;
                            break;
                        case 6:
                            fullFrame += BRICK_COLOR_PINK;
                            break;
                        case 7:
                            fullFrame += BRICK_COLOR_CYAN;
                            break;
                        }
                        fullFrame += "█";
                        fullFrame += RESET;
                    }
                    else
                    {
                        fullFrame += ' ';
                    }
                }
                else if (ch == '=')
                {
                    fullFrame += CYAN;
                    fullFrame += selectedPaddleType;
                    fullFrame += RESET;
                }
                else if (ch == 'O')
                {
                    fullFrame += GOLD;
                    fullFrame += selectedBallType;
                    fullFrame += RESET;
                }
                else if (ch == 'X')
                {
                    fullFrame += RED;
                    fullFrame += "⊙";
                    fullFrame += RESET;
                }
                else if (ch == '+')
                {
                    fullFrame += PINK;
                    fullFrame += "♡";
                    fullFrame += RESET;
                }
                else
                {
                    fullFrame += ch;
                }
            }
        }
        fullFrame += '\n';
    }

    cout << fullFrame;

    // اطلاعات بازی
    if (!isPaused)
    {
        cout << "Player: " << BRIGHT_CYAN << currentPlayerName << RESET << " | ";
        cout << "Score: " << BRIGHT_YELLOW << score << RESET << endl;
        cout << "Lives: " << BRIGHT_RED;
        for (int i = 0; i < lives; i++)
            cout << "❤️";
        cout << RESET;

        cout << " | Mode: " << (gameMode == GAME_MODE_SIMPLE ? PINK : BRIGHT_MAGENTA)
             << (gameMode == GAME_MODE_SIMPLE ? "Simple   " : "Advanced   ") << RESET;

        cout << endl;
        if (!gameStarted && !gameOver)
        {
            cout << BRIGHT_GREEN << "Press SPACE to start or continue the game " << RESET;
            cout << endl;
        }
        else
        {
            cout << CYAN << "Use A/D to move paddle, P to pause            " << RESET;
            cout << endl;
        }
    }
}
// ==================== تابع کمکی برای منوها ====================
void clearMenuArea()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    COORD topLeft = {0, 0};
    DWORD written;
    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', cellCount, topLeft, &written);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, topLeft, &written);
    SetConsoleCursorPosition(hConsole, topLeft);
}

// ==================== توابع منو ====================
void printMenu()
{
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    cout << "\n\n\n\n";
    cout << "                  ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗";
    cout << "\n\n\n\n";
    cout << "                                      ██████╗   ██████╗   ███████╗   █████╗    ██╗  ██╗   ██████╗    ██╗   ██╗   ████████╗\n";
    cout << "                                      ██╔══██╗  ██╔══██╗  ██╔════╝  ██╔══██╗   ██║ ██╔╝  ██╔═══██╗   ██║   ██║   ╚══██╔══╝\n";
    cout << "                                      ██║  ██║  ██║  ██║  █████╗    ███████║   █████╔╝   ██║   ██║   ██║   ██║      ██║   \n";
    cout << "                                      ██████╔╝  ██████╔╝  ██╔══╝    ██╔══██║   ██╔═██╗   ██║   ██║   ██║   ██║      ██║   \n";
    cout << "                                      ██╔══██╗  ██╔══██╗  ███████╗  ██║  ██║   ██║  ██╗  ╚██████╔╝   ╚██████╔╝      ██║   \n";
    cout << "                                      ██████╔╝  ██║   ██║ ╚══════╝  ╚═╝  ╚═╝   ╚═╝  ╚═╝   ╚═════╝     ╚═════╝       ╚═╝   \n";
    cout << endl
         << endl;

    int boxWidth = 18;
    cout << BOLD << WHITE << "                                                               ╔════════════════════════╗" << RESET << "\n";

    for (int i = 0; i < 6; i++)
    {
        string boxColor;
        if (i == currentOption)
            boxColor = BLUE;
        else
            boxColor = WHITE;

        string textColor;
        if (i == currentOption)
            textColor = BLUE;
        else if (mainMenuOptions[i] == "EXIT")
            textColor = RED;
        else
            textColor = WHITE;

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╔";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╗" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "║ " << RESET;
        if (i == currentOption)
            cout << BOLD << BLUE << "➤ " << RESET;
        else
            cout << "  ";

        cout << BOLD << textColor << mainMenuOptions[i] << " " << RESET;
        cout << string(boxWidth - (mainMenuOptions[i].length() + 4), ' ');
        cout << BOLD << boxColor << "║" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╚";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╝" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";
    }
    cout << BOLD << WHITE << "                                                               ╚════════════════════════╝" << RESET << "\n\n\n";

    cout << "                  ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝";
    pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void menuMove()
{
    clearMenuArea();
    printMenu();

    int previousOption = -1;

    while (true)
    {
        if (previousOption != currentOption)
        {
            printMenu();
            previousOption = currentOption;
        }

        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'w' || ch == 'W')
            {
                if (currentOption > 0)
                    currentOption--;
            }
            else if (ch == 's' || ch == 'S')
            {
                if (currentOption < 5)
                    currentOption++;
            }
            else if (ch == '\r')
            {
                switch (currentOption)
                {
                case 0:
                    printLoading();
                    newGame();
                    clearMenuArea();
                    printMenu();
                    previousOption = -1;
                    break;
                case 1:
                    clearMenuArea();
                    printLoading();
                    loadGame();
                    clearMenuArea();
                    printMenu();
                    previousOption = -1;
                    break;
                case 2:
                    printLoading();
                    clearMenuArea();
                    helpMove();
                    clearMenuArea();
                    printMenu();
                    previousOption = -1;
                    break;
                case 3:
                    printLoading();
                    clearMenuArea();
                    showGameHistory();
                    clearMenuArea();
                    printMenu();
                    previousOption = -1;
                    break;
                case 4:
                    printLoading();
                    clearMenuArea();
                    settingsMove();
                    clearMenuArea();
                    printMenu();
                    previousOption = -1;
                    break;
                case 5:
                    system("cls");
                    exitMove();
                    clearMenuArea();
                    printMenu();
                    previousOption = -1;
                    break;
                }
            }
        }
        Sleep(50);
    }
}

void pauseMenu()
{
    COORD pos = {40, 14};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    string menu = BOLD + WHITE + "                        ╔════════════════════════╗" + RESET + "\n";

    for (int i = 0; i < 3; i++)
    {
        string boxColor = (i == currentOption) ? BLUE : WHITE;

        menu += BOLD + WHITE + "                                                                ║  " + RESET;
        menu += BOLD + boxColor + "╔══════════════════╗" + RESET;
        menu += BOLD + WHITE + "  ║" + RESET + "\n";

        menu += BOLD + WHITE + "                                                                ║  " + RESET;
        menu += BOLD + boxColor + "║ ";
        if (i == currentOption)
            menu += "➤  ";
        else
            menu += "   ";
        menu += pauseMenuOptions[i];

        int totalWidth = 16; // عرض کل کادر داخلی
        int usedWidth = pauseMenuOptions[i].length() + ((i == currentOption) ? 2 : 2);
        int spaces = totalWidth - usedWidth;
        if (spaces > 0)
            menu += string(spaces, ' ');

        menu += "║" + RESET;
        menu += BOLD + WHITE + "  ║" + RESET + "\n";

        menu += BOLD + WHITE + "                                                                ║  " + RESET;
        menu += BOLD + boxColor + "╚══════════════════╝" + RESET;
        menu += BOLD + WHITE + "  ║" + RESET + "\n";
    }
    menu += BOLD + WHITE + "                                                                ╚════════════════════════╝" + RESET;
    cout << menu;
}

void pauseMove()
{
    clearMenuArea();
    currentOption = 0;

    pauseMenu();

    int previousOption = -1;

    while (isPaused)
    {
        if (previousOption != currentOption)
        {
            pauseMenu();
            previousOption = currentOption;
        }

        if (_kbhit())
        {
            char a = _getch();
            if (a == 'w' || a == 'W')
            {
                if (currentOption > 0)
                    currentOption--;
            }
            else if (a == 's' || a == 'S')
            {
                if (currentOption < 2)
                    currentOption++;
            }
            else if (a == '\r')
            {
                switch (currentOption)
                {
                case 0:
                    isPaused = false;
                    clearMenuArea();
                    return;
                case 1:
                    restart();
                    return;
                case 2:
                    saveGame();
                    gameOver = true;
                    isPaused = false;
                    menuMove();
                    return;
                }
            }
        }
        Sleep(50);
    }
}

void resume()
{
    isPaused = false;
    clearMenuArea();
}

void restart()
{
    isPaused = false;
    score = 0;
    lives = 3;
    gameOver = false;
    gameStarted = false;

    initializeBricks();

    paddle.x = INITIAL_PADDLE_X;
    paddle.y = INITIAL_PADDLE_Y;
    paddle.width = 10.0f;
    paddle.height = 1.0f;
    paddle.speed = PADDLE_SPEED;

    ball.x = INITIAL_BALL_X;
    ball.y = INITIAL_BALL_Y;
    ball.vx = BALL_SPEED;
    ball.vy = -BALL_SPEED;
    ball.radius = 0.5f;

    if (gameMode == GAME_MODE_ADVANCED)
    {
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            powerups[i].active = false;
            powerups[i].collected = false;
        }
    }

    resetBallAndPaddle();
    clearMenuArea();
}

// ==================== توابع راهنما ====================
void showHelp()
{
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

    int boxWidth = 18;
    cout << BOLD << WHITE << "                                                               ╔════════════════════════╗" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║    " << BOLD << BLUE << "    HELP MENU" << BOLD << WHITE << "       ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ╠════════════════════════╣" << RESET << "\n";

    for (int i = 0; i < 3; i++)
    {
        string boxColor;
        if (i == currentHelpOption)
            boxColor = BLUE;
        else
            boxColor = WHITE;

        string textColor;
        if (i == currentHelpOption)
            textColor = BLUE;
        else
            textColor = WHITE;

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╔";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╗" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "║ " << RESET;
        if (i == currentHelpOption)
            cout << BOLD << BLUE << "➤ " << RESET;
        else
            cout << "  ";

        cout << BOLD << textColor << helpOptions[i] << " " << RESET;
        cout << string(boxWidth - (helpOptions[i].length() + 4), ' ');
        cout << BOLD << boxColor << "║" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╚";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╝" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";
    }
    cout << BOLD << WHITE << "                                                               ╚════════════════════════╝" << RESET << "\n";

    pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void helpMove()
{
    currentHelpOption = 0;
    clearMenuArea();
    showHelp();

    int previousOption = -1;

    while (true)
    {
        if (previousOption != currentHelpOption)
        {
            showHelp();
            previousOption = currentHelpOption;
        }

        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'w' || ch == 'W')
            {
                if (currentHelpOption > 0)
                    currentHelpOption--;
            }
            else if (ch == 's' || ch == 'S')
            {
                if (currentHelpOption < 2)
                    currentHelpOption++;
            }
            else if (ch == '\r')
            {
                switch (currentHelpOption)
                {
                case 0:
                    controls();
                    clearMenuArea();
                    showHelp();
                    previousOption = -1;
                    break;
                case 1:
                    aboutGame();
                    clearMenuArea();
                    showHelp();
                    previousOption = -1;
                    break;
                case 2:
                    return;
                }
            }
            else if (ch == 27)
            {
                return;
            }
        }
        Sleep(50);
    }
}

void controls()
{
    system("cls");
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << BOLD << GOLD << "                                                   ╔════════════════════════════════════════════════╗\n";
    cout << "                                                   ║                    HELP                        ║\n";
    cout << "                                                   ╠════════════════════════════════════════════════╣\n";
    cout << "                                                   ║                                                ║\n";
    cout << "                                                   ║  Goal: Break all bricks with the ball          ║\n";
    cout << "                                                   ║                                                ║\n";
    cout << "                                                   ║  Controls:                                     ║\n";
    cout << "                                                   ║    A      : Move paddle left                   ║\n";
    cout << "                                                   ║    D      : Move paddle right                  ║\n";
    cout << "                                                   ║    P      : Pause/Resume                       ║\n";
    cout << "                                                   ║    HEART  : Increase length of paddle          ║\n";
    cout << "                                                   ║    BOMB   : Decrease length of paddle          ║\n";
    cout << "                                                   ║                                                ║\n";
    cout << "                                                   ║  You have 3 lives. Don't let ball fall!        ║\n";
    cout << "                                                   ║                                                ║\n";
    cout << "                                                   ╚════════════════════════════════════════════════╝\n"
         << RESET;
    cout << "                                                   Press any key to return...";
    _getch();
}

void aboutGame()
{
    system("cls");
    ifstream file("aboutgame.txt");
    if (!file.is_open())
    {
        cout << RED << "Nothing has found :(" << RESET << endl;
        return;
    }
    string line;
    while (getline(file, line))
    {
        cout << line << endl;
    }
    file.close();
    cout << endl;
    cout << BOLD << BRIGHT_RED << "Press any key to return..." << RESET;
    _getch();
}

// ==================== توابع تنظیمات ====================
void saveSettings()
{
    ofstream file("settings.txt");
    if (file.is_open())
    {
        file << selectedBallType << endl;
        file << selectedPaddleType << endl;
        file.close();
    }
}

void loadSettings()
{
    ifstream file("settings.txt");
    if (file.is_open())
    {
        file >> selectedBallType;
        file >> selectedPaddleType;
        file.close();
    }
}
void showSettings()
{
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

    int boxWidth = 18;
    cout << BOLD << WHITE << "                                                               ╔════════════════════════╗" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║    " << BOLD << BLUE << "SETTINGS MENU" << BOLD << WHITE << "       ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ╠════════════════════════╣" << RESET << "\n";

    for (int i = 0; i < 3; i++)
    {
        string boxColor;
        if (i == currentSettingsOption)
            boxColor = BLUE;
        else
            boxColor = WHITE;

        string textColor;
        if (i == currentSettingsOption)
            textColor = BLUE;
        else
            textColor = WHITE;

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╔";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╗" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "║ " << RESET;
        if (i == currentSettingsOption)
            cout << BOLD << BLUE << "➤ " << RESET;
        else
            cout << "  ";

        cout << BOLD << textColor << settingsOptions[i] << " " << RESET;
        cout << string(boxWidth - (settingsOptions[i].length() + 4), ' ');
        cout << BOLD << boxColor << "║" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╚";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╝" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";
    }
    cout << BOLD << WHITE << "                                                               ╚════════════════════════╝" << RESET << "\n";

    pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void settingsMove()
{
    currentSettingsOption = 0;
    clearMenuArea();
    showSettings();

    int previousOption = -1;

    while (true)
    {
        if (previousOption != currentSettingsOption)
        {
            showSettings();
            previousOption = currentSettingsOption;
        }

        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'w' || ch == 'W')
            {
                if (currentSettingsOption > 0)
                    currentSettingsOption--;
            }
            else if (ch == 's' || ch == 'S')
            {
                if (currentSettingsOption < 2)
                    currentSettingsOption++;
            }
            else if (ch == '\r')
            {
                switch (currentSettingsOption)
                {
                case 0:
                    inBallSettings = true;
                    ballSettingsMove();
                    clearMenuArea();
                    showSettings();
                    previousOption = -1;
                    break;
                case 1:
                    inPaddleSettings = true;
                    paddleSettingsMove();
                    clearMenuArea();
                    showSettings();
                    previousOption = -1;
                    break;
                case 2:
                    return;
                }
            }
        }
        Sleep(50);
    }
}
void showBallSettings()
{
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    cout << "\n\n\n\n\n";

    int boxWidth = 25;
    cout << BOLD << WHITE << "                                                               ╔═══════════════════════════════╗" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                               ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║     " << BOLD << BLUE << "BALL SETTINGS" << BOLD << WHITE << "             ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                               ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ╠═══════════════════════════════╣" << RESET << "\n";

    for (int i = 0; i < 8; i++)
    {
        string boxColor;
        if (i == currentBallOption)
            boxColor = BLUE;
        else
            boxColor = WHITE;

        string textColor;
        if (i == currentBallOption)
            textColor = BLUE;
        else
            textColor = WHITE;

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╔";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╗" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "║ " << RESET;
        if (i == currentBallOption)
            cout << BOLD << BLUE << "➤ " << RESET;
        else
            cout << "  ";

        cout << BOLD << textColor << ballTypes[i] << " " << RESET;
        cout << string(boxWidth - (ballTypes[i].length() + 2), ' ');
        cout << BOLD << boxColor << "║" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╚";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╝" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";
    }

    // گزینه بازگشت
    string backOption = "BACK TO SETTINGS";
    string backBoxColor = (currentBallOption == 8) ? BLUE : WHITE;
    string backTextColor = (currentBallOption == 8) ? BLUE : WHITE;

    cout << BOLD << WHITE << "                                                               ║  " << RESET;
    cout << BOLD << backBoxColor << "╔";
    for (int j = 0; j < boxWidth; j++)
        cout << "═";
    cout << "╗" << RESET;
    cout << BOLD << WHITE << "  ║" << RESET << "\n";

    cout << BOLD << WHITE << "                                                               ║  " << RESET;
    cout << BOLD << backBoxColor << "║ " << RESET;
    if (currentBallOption == 8)
        cout << BOLD << BLUE << "➤ " << RESET;
    else
        cout << "  ";
    cout << BOLD << backTextColor << backOption << " " << RESET;
    cout << string(boxWidth - (backOption.length() + 4), ' ');
    cout << BOLD << backBoxColor << "║" << RESET;
    cout << BOLD << WHITE << "  ║" << RESET << "\n";

    cout << BOLD << WHITE << "                                                               ║  " << RESET;
    cout << BOLD << backBoxColor << "╚";
    for (int j = 0; j < boxWidth; j++)
        cout << "═";
    cout << "╝" << RESET;
    cout << BOLD << WHITE << "  ║" << RESET << "\n";

    cout << BOLD << WHITE << "                                                               ╚═══════════════════════════════╝" << RESET << "\n";

    pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void ballSettingsMove()
{
    int previousOption = -1;
    clearMenuArea();
    showBallSettings();

    while (inBallSettings)
    {
        if (previousOption != currentBallOption)
        {
            showBallSettings();
            previousOption = currentBallOption;
        }

        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'w' || ch == 'W')
            {
                if (currentBallOption > 0)
                    currentBallOption--;
            }
            else if (ch == 's' || ch == 'S')
            {
                if (currentBallOption < 8)
                    currentBallOption++;
            }
            else if (ch == '\r')
            {
                if (currentBallOption == 8)
                {
                    inBallSettings = false;
                }
                else
                {
                    selectedBallType = ballTypes[currentBallOption];
                    saveSettings();

                    system("cls");
                    COORD pos = {0, 20};
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

                    cout << BOLD << WHITE << "                                                               ╔════════════════════════╗" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ║     " << BOLD << GREEN << "SETTING SAVED!" << BOLD << WHITE << "     ║" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ╚════════════════════════╝" << RESET << "\n";

                    this_thread::sleep_for(chrono::seconds(2));
                    clearMenuArea();
                    showBallSettings();
                    previousOption = -1;
                }
            }
        }
        Sleep(50);
    }
}

void showPaddleSettings()
{
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    cout << "\n\n\n\n\n";

    int boxWidth = 27;
    cout << BOLD << WHITE << "                                                               ╔═════════════════════════════════╗" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                                 ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║   " << BOLD << BLUE << "PADDLE SETTINGS" << BOLD << WHITE << "               ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ║                                 ║" << RESET << "\n";
    cout << BOLD << WHITE << "                                                               ╠═════════════════════════════════╣" << RESET << "\n";

    for (int i = 0; i < 8; i++)
    {
        string boxColor;
        if (i == currentPaddleOption)
            boxColor = BLUE;
        else
            boxColor = WHITE;

        string textColor;
        if (i == currentPaddleOption)
            textColor = BLUE;
        else
            textColor = WHITE;

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╔";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╗" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "║ " << RESET;
        if (i == currentPaddleOption)
            cout << BOLD << BLUE << "➤ " << RESET;
        else
            cout << "  ";

        cout << BOLD << textColor << paddleTypes[i] << " " << RESET;
        cout << string(boxWidth - (paddleTypes[i].length() + 2), ' ');
        cout << BOLD << boxColor << "║" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";

        cout << BOLD << WHITE << "                                                               ║  " << RESET;
        cout << BOLD << boxColor << "╚";
        for (int j = 0; j < boxWidth; j++)
            cout << "═";
        cout << "╝" << RESET;
        cout << BOLD << WHITE << "  ║" << RESET << "\n";
    }

    // گزینه بازگشت
    string backOption = "BACK TO SETTINGS";
    string backBoxColor = (currentPaddleOption == 8) ? BLUE : WHITE;
    string backTextColor = (currentPaddleOption == 8) ? BLUE : WHITE;

    cout << BOLD << WHITE << "                                                               ║  " << RESET;
    cout << BOLD << backBoxColor << "╔";
    for (int j = 0; j < boxWidth; j++)
        cout << "═";
    cout << "╗" << RESET;
    cout << BOLD << WHITE << "  ║" << RESET << "\n";

    cout << BOLD << WHITE << "                                                               ║  " << RESET;
    cout << BOLD << backBoxColor << "║ " << RESET;
    if (currentPaddleOption == 8)
        cout << BOLD << BLUE << "➤ " << RESET;
    else
        cout << "  ";
    cout << BOLD << backTextColor << backOption << " " << RESET;
    cout << string(boxWidth - (backOption.length() + 4), ' ');
    cout << BOLD << backBoxColor << "║" << RESET;
    cout << BOLD << WHITE << "  ║" << RESET << "\n";

    cout << BOLD << WHITE << "                                                               ║  " << RESET;
    cout << BOLD << backBoxColor << "╚";
    for (int j = 0; j < boxWidth; j++)
        cout << "═";
    cout << "╝" << RESET;
    cout << BOLD << WHITE << "  ║" << RESET << "\n";

    cout << BOLD << WHITE << "                                                               ╚═════════════════════════════════╝" << RESET << "\n";

    pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void paddleSettingsMove()
{
    int previousOption = -1;
    clearMenuArea();
    showPaddleSettings();

    while (inPaddleSettings)
    {
        if (previousOption != currentPaddleOption)
        {
            showPaddleSettings();
            previousOption = currentPaddleOption;
        }

        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'w' || ch == 'W')
            {
                if (currentPaddleOption > 0)
                    currentPaddleOption--;
            }
            else if (ch == 's' || ch == 'S')
            {
                if (currentPaddleOption < 8)
                    currentPaddleOption++;
            }
            else if (ch == '\r')
            {
                if (currentPaddleOption == 8)
                {
                    inPaddleSettings = false;
                    break;
                }
                else
                {
                    selectedPaddleType = paddleTypes[currentPaddleOption];
                    saveSettings();

                    system("cls");
                    COORD pos = {0, 20};
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

                    cout << BOLD << WHITE << "                                                               ╔════════════════════════╗" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ║     " << BOLD << GREEN << "SETTING SAVED!" << BOLD << WHITE << "     ║" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ║                        ║" << RESET << "\n";
                    cout << BOLD << WHITE << "                                                               ╚════════════════════════╝" << RESET << "\n";

                    this_thread::sleep_for(chrono::seconds(2));
                    clearMenuArea();
                    showPaddleSettings();
                    previousOption = -1;
                }
            }
            else if (ch == 27)
            {
                inPaddleSettings = false;
                break;
            }
        }
        Sleep(50);
    }
}

// ==================== توابع UI ====================
void printVictory()
{
    system("cls");
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
    playVictorySound();
    cout << GOLD;
    cout << "                                               ██╗   ██╗ ██╗  ██████╗ ████████╗  ██████╗  ██████╗  ██╗   ██╗\n";
    cout << "                                               ██║   ██║ ██║ ██╔════╝ ╚══██╔══╝ ██╔═══██╗ ██╔══██╗ ╚██╗ ██╔╝\n";
    cout << "                                               ██║   ██║ ██║ ██║         ██║    ██║   ██║ ██████╔╝  ╚████╔╝ \n";
    cout << "                                               ╚██╗ ██╔╝ ██║ ██║         ██║    ██║   ██║ ██╔══██╗   ╚██╔╝  \n";
    cout << "                                                ╚████╔╝  ██║ ╚██████╗    ██║    ╚██████╔╝ ██║  ██║    ██║   \n";
    cout << "                                                 ╚═══╝   ╚═╝  ╚═════╝    ╚═╝     ╚═════╝  ╚═╝  ╚═╝    ╚═╝   \n\n";
    cout << "                                                                   ██╗██████████████████╔██\n";
    cout << "                                                                   ╚═╝ ████████████████ ╚═╝\n";
    cout << "                                                                        ██████████████\n";
    cout << "                                                                         ████████████\n";
    cout << "                                                                          ██████████\n";
    cout << "                                                                           ████████\n";
    cout << "                                                                            ██████\n";
    cout << "                                                                             ████\n";
    cout << "                                                                              ██\n";
    cout << "                                                                              ██\n";
    cout << "                                                                           ████████\n";
    cout << "                                                                           ████████\n";
    cout << RESET;
    Sleep(2000);
}

void printGameOver()
{
    system("cls");
    playGameOverSound();
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << RED;
    cout << "                                        ██████╗   █████╗  ███╗   ███╗ ███████╗     ██████╗  ██╗   ██╗ ███████╗ ██████╗ \n";
    cout << "                                       ██╔════╝  ██╔══██╗ ████╗ ████║ ██╔════╝    ██╔═══██╗ ██║   ██║ ██╔════╝ ██╔══██╗\n";
    cout << "                                       ██║  ███╗ ███████║ ██╔████╔██║ █████╗      ██║   ██║ ██║   ██║ █████╗   ██████\n";
    cout << "                                       ██║   ██║ ██╔══██║ ██║╚██╔╝██║ ██╔══╝      ██║   ██║ ╚██╗ ██╔╝ ██╔══╝   ██╔══██╗\n";
    cout << "                                       ╚██████╔╝ ██║  ██║ ██║ ╚═╝ ██║ ███████╗    ╚██████╔╝  ╚████╔╝  ███████╗ ██║  ██║\n";
    cout << "                                        ╚═════╝  ╚═╝  ╚═╝ ╚═╝     ╚═╝ ╚══════╝     ╚═════╝    ╚═══╝   ╚══════╝ ╚═╝  ╚═╝\n";
    cout << RESET;
    Sleep(2000);
}

void printExit(int selected)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {51, 18}; // موقعیت وسط صفحه
    DWORD written;

    // پاک کردن 5 خط
    for (int i = 0; i < 5; i++)
    {
        FillConsoleOutputCharacter(hConsole, ' ', 50, topLeft, &written);
        topLeft.Y++;
    }

    // رفتن به موقعیت شروع
    COORD pos = {51, 18};
    SetConsoleCursorPosition(hConsole, pos);

    cout << BOLD << "    ╔════════════════════════════════════════╗" << endl;

    // رفتن به خط دوم
    pos.Y++;
    SetConsoleCursorPosition(hConsole, pos);
    cout << "    ║" << RED << "  Are you sure you want to quit?     " << RESET << BOLD << "   ║" << endl;

    // رفتن به خط سوم
    pos.Y++;
    SetConsoleCursorPosition(hConsole, pos);
    cout << "    ╠═══════════════════╦════════════════════╣" << endl;

    // رفتن به خط چهارم
    pos.Y++;
    SetConsoleCursorPosition(hConsole, pos);

    // کل خط چهارم رو یکجا چاپ کن
    cout << "    ║";
    if (selected == 0)
    {
        cout << BLUE << BOLD << "        ➤ YES     " << RESET << BOLD;
    }
    else
    {
        cout << "          YES       ";
    }
    cout << "║";

    if (selected == 1)
    {
        cout << BLUE << BOLD << "        ➤ NO      " << RESET << BOLD;
    }
    else
    {
        cout << "          NO        ";
    }
    cout << "║" << endl;

    // رفتن به خط پنجم
    pos.Y++;
    SetConsoleCursorPosition(hConsole, pos);
    cout << "    ╚═══════════════════╩════════════════════╝" << RESET;

    // برگرداندن مکان‌نما به ابتدای منو
    pos.X = 51;
    pos.Y = 18;
    SetConsoleCursorPosition(hConsole, pos);
}

void exitMove()
{
    int selected = 0;
    printExit(selected);
    while (true)
    {
        // فقط وقتی که گزینه تغییر کرده
        static int lastSelected = -1;
        if (lastSelected != selected)
        {
            // فقط خط چهارم رو به‌روز کن (جایی که YES/NO هست)
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD pos = {51, 21}; // خط چهارم
            SetConsoleCursorPosition(hConsole, pos);

            // خط چهارم رو دوباره بکش
            cout << "    ║";
            if (selected == 0)
            {
                cout << BLUE << BOLD << "        YES        " << RESET << BOLD;
            }
            else
            {
                cout << "        YES        ";
            }
            cout << "║";

            if (selected == 1)
            {
                cout << BLUE << BOLD << "         NO         " << RESET << BOLD;
            }
            else
            {
                cout << "         NO         ";
            }
            cout << "║";

            // فضای اضافی رو پاک کن
            cout << "   ";

            // برگرد به موقعیت
            pos.X = 51;
            pos.Y = 21;
            SetConsoleCursorPosition(hConsole, pos);

            lastSelected = selected;
        }

        if (_kbhit())
        {
            char a = _getch();
            if (a == 'a' || a == 'A')
            {
                selected = 0;
            }
            else if (a == 'd' || a == 'D')
            {
                selected = 1;
            }
            else if (a == '\r')
            {
                if (selected == 0)
                {
                    clearMenuArea();
                    exit(0);
                }
                else
                {
                    return;
                }
            }
        }
        Sleep(50);
    }
}

void printLoading()
{
    system("cls");
    hideCursor();
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << "                                                  ██╗     " << "  ██████╗ " << "  █████╗  " << " ██████╗  " << " ██╗" << " ███╗   ██╗" << "  ██████╗ \n";
    cout << "                                                  ██║     " << " ██╔═══██╗" << " ██╔══██╗ " << " ██╔══██╗ " << " ██║" << " ████╗  ██║" << " ██       \n";
    cout << "                                                  ██║     " << " ██║   ██║" << " ███████║ " << " ██║  ██║ " << " ██║" << " ██╔██╗ ██║" << " ██║  ███║\n";
    cout << "                                                  ██║     " << " ██║   ██║" << " ██╔══██║ " << " ██║  ██║ " << " ██║" << " ██║╚██╗██║" << " ██║   ██║\n";
    cout << "                                                  ███████╗" << " ╚██████╔╝" << " ██║  ██║ " << " ██████╔╝ " << " ██║" << " ██║ ╚████║" << " ╚██████╔╝\n";
    cout << "                                                  ╚══════╝" << "  ╚═════╝ " << " ╚═╝  ╚═╝ " << " ╚═════╝  " << " ╚═╝" << " ╚═╝  ╚═══╝" << "  ╚═════╝ \n\n";

    string spinner = "|/-\\";
    for (int i = 0; i < 30; i++)
    {
        cout << BOLD << WHITE << "\r                                                  LOADING " << spinner[i % spinner.size()] << flush;
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}
void selectGameMode()
{
    string modes[] = {"SIMPLE GAME", "ADVANCED GAME"};
    int selectedMode = 0;
    int previousMode = -1;

    clearMenuArea();

    while (true)
    {
        if (previousMode != selectedMode)
        {
            COORD pos = {0, 0};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

            cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << BOLD << WHITE << "                                                               ╔════════════════════════╗" << RESET << "\n";

            for (int i = 0; i < 2; i++)
            {
                string boxColor = (i == selectedMode) ? BLUE : WHITE;
                string textColor = (i == selectedMode) ? BLUE : WHITE;

                cout << BOLD << WHITE << "                                                               ║  " << RESET;
                cout << BOLD << boxColor << "╔══════════════════╗" << RESET;
                cout << BOLD << WHITE << "  ║" << RESET << "\n";

                cout << BOLD << WHITE << "                                                               ║  " << RESET;
                cout << BOLD << boxColor << "║ " << RESET;

                if (i == selectedMode)
                {
                    cout << BOLD << BLUE << "➤ " << RESET;
                }
                else
                {
                    cout << "  ";
                }

                cout << BOLD << textColor << modes[i] << RESET;
                cout << string(15 - modes[i].length(), ' ');
                cout << BOLD << boxColor << "║" << RESET;
                cout << BOLD << WHITE << "  ║" << RESET << "\n";

                cout << BOLD << WHITE << "                                                               ║  " << RESET;
                cout << BOLD << boxColor << "╚══════════════════╝" << RESET;
                cout << BOLD << WHITE << "  ║" << RESET << "\n";
            }
            cout << BOLD << WHITE << "                                                               ╚════════════════════════╝" << RESET << "\n";

            pos = {0, 0};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

            previousMode = selectedMode;
        }

        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'w' || ch == 'W')
            {
                if (selectedMode > 0)
                    selectedMode--;
            }
            else if (ch == 's' || ch == 'S')
            {
                if (selectedMode < 1)
                    selectedMode++;
            }
            else if (ch == '\r')
            {
                gameMode = selectedMode;
                return;
            }
        }
        Sleep(50);
    }
}
// ==================== توابع تاریخچه بازی ====================
void saveGameHistory(const string &result)
{
    ofstream file("gameHistory.txt", ios::app);
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string date = to_string(1900 + ltm->tm_year) + "-" +
                  to_string(1 + ltm->tm_mon) + "-" +
                  to_string(ltm->tm_mday);

    string time = to_string(ltm->tm_hour) + ":" +
                  to_string(ltm->tm_min) + ":" +
                  to_string(ltm->tm_sec);

    string gameModeStr = (gameMode == GAME_MODE_SIMPLE ? "Simple" : "Advanced");

    file << currentPlayerName << endl;
    file << gameModeStr << endl;
    file << score << endl;
    file << result << endl;
    file << date << endl;
    file << time << endl;
    file.close();
}

void showGameHistory()
{
    ifstream file("gameHistory.txt");
    if (!file.is_open())
    {
        cout << RED << "Nothing has found :(" << RESET << endl;
        cout << "\n\n";
        return;
    }

    GameRecord histories[1000];
    int count = 0;

    while (count < 1000 &&
           (file >> histories[count].playerName >>
            histories[count].gameModeStr >>
            histories[count].score >>
            histories[count].result >>
            histories[count].Date >>
            histories[count].Time))
    {
        count++;
    }
    file.close();

    if (count == 0)
    {
        cout << "\033[1;31mNo history found.\033[0m" << endl;
        return;
    }

    cout << "                           ╔════════════════╦════════════════╦════════════════╦════════════════╦════════════════╦════════════════╗" << endl;
    cout << "                           ║ Name           ║ Mode           ║ score          ║ result         ║ Date           ║  Time          ║ " << endl;
    cout << "                           ╠════════════════╬════════════════╬════════════════╬════════════════╬════════════════╬════════════════╣" << endl;

    for (int i = 0; i < count; i++)
    {
        cout << "                           ║ " << histories[i].playerName << string(15 - histories[i].playerName.size(), ' ')
             << "║ " << histories[i].gameModeStr << string(15 - histories[i].gameModeStr.size(), ' ')
             << "║ " << histories[i].score << string(15 - histories[i].score.size(), ' ')
             << "║ " << histories[i].result << string(15 - histories[i].result.size(), ' ')
             << "║ " << histories[i].Date << string(15 - histories[i].Date.size(), ' ')
             << "║ " << histories[i].Time << string(15 - histories[i].Time.size(), ' ')
             << "║" << endl;

        if (i != count - 1)
        {
            cout << "                           ╠════════════════╬════════════════╬════════════════╬════════════════╬════════════════╬════════════════╣" << endl;
        }
    }
    cout << "                           ╚════════════════╩════════════════╩════════════════╩════════════════╩════════════════╩════════════════╝" << endl;
    cout << "                           Press any key to return to main menu ...";
    _getch();
}

// ==================== توابع ذخیره و بارگذاری ====================
void saveGame()
{
    ofstream file("saved_game.txt");
    if (file.is_open())
    {
        file << gameMode << endl;
        file << ball.x << " " << ball.y << " " << ball.vx << " " << ball.vy << " " << ball.radius << endl;
        file << paddle.x << " " << paddle.y << " " << paddle.width << " " << paddle.height << " " << paddle.speed << endl;

        for (int i = 0; i < BRICK_ROWS; i++)
        {
            for (int j = 0; j < BRICK_COLS; j++)
            {
                file << bricks[i][j].x << " " << bricks[i][j].y << " "
                     << bricks[i][j].width << " " << bricks[i][j].height << " "
                     << bricks[i][j].visible << " ";
            }
            file << endl;
        }

        if (gameMode == GAME_MODE_ADVANCED)
        {
            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                file << powerups[i].x << " " << powerups[i].y << " "
                     << powerups[i].vy << " " << powerups[i].type << " "
                     << powerups[i].active << " " << powerups[i].collected << " ";
            }
            file << endl;
        }

        file << score << endl;
        file << lives << endl;
        file << gameStarted << endl;
        file << currentPlayerName << endl;

        file.close();
    }
}

void loadGame()
{
    system("cls");
    ifstream file("saved_game.txt");
    if (!file.is_open())
    {
        cout << BOLD << BRIGHT_RED << "\nNo unfinished game to load !" << endl;
        cout << "Press any key to return" << RESET;
        _getch();
        return;
    }

    file >> gameMode;
    file >> ball.x >> ball.y >> ball.vx >> ball.vy >> ball.radius;
    file >> paddle.x >> paddle.y >> paddle.width >> paddle.height >> paddle.speed;

    if (paddle.width < MIN_PADDLE_WIDTH)
        paddle.width = MIN_PADDLE_WIDTH;
    if (paddle.width > MAX_PADDLE_WIDTH)
        paddle.width = MAX_PADDLE_WIDTH;

    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            file >> bricks[i][j].x >> bricks[i][j].y >> bricks[i][j].width >> bricks[i][j].height >> bricks[i][j].visible;
        }
    }

    if (gameMode == GAME_MODE_ADVANCED)
    {
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            file >> powerups[i].x >> powerups[i].y >> powerups[i].vy >> powerups[i].type >> powerups[i].active >> powerups[i].collected;
        }
    }

    file >> score;
    file >> lives;
    file >> gameStarted;
    file.ignore();
    getline(file, currentPlayerName);

    file.close();

    gameOver = false;
    isPaused = false;

    cout << BOLD << BRIGHT_GREEN << "\nGame loaded successfully!" << endl;
    cout << "Player: " << currentPlayerName << endl;
    cout << "Score: " << score << endl;
    cout << "Lives: " << lives << RESET << endl
         << endl;
    cout << BOLD << MAGENTA << "Continue the game ------> Press C" << RESET << endl;
    cout << BOLD << BLUE << "Back to menu ------> Press M" << RESET;

    while (true)
    {
        if (_kbhit())
        {
            char a = _getch();
            if (a == 'C' || a == 'c')
            {
                initScreenBuffer();
                hideCursor();
                runGame();
            }
            else if (a == 'M' || a == 'm')
            {

                menuMove();
            }
        }
    }
}

void clearSavedGame()
{
    remove("saved_game.txt");
}

// ==================== تابع اصلی اجرای بازی ====================
void runGame()
{
    while (!gameOver)
    {
        // اگر pause نیستیم، بازی رو render کن
        if (!isPaused)
        {
            renderToBuffer();
            renderWithColorsOptimized();
        }

        if (!gameStarted && !isPaused)
        {
            while (true)
            {
                if (_kbhit())
                {
                    char key = _getch();
                    if (key == ' ')
                    {
                        gameStarted = true;
                        break;
                    }
                    else if (key == 'p' || key == 'P')
                    {
                        isPaused = true;
                        pauseMove();
                        break;
                    }
                }
                Sleep(100);
            }

            if (gameOver)
                break;
        }

        // کنترل راکت - فقط وقتی pause نیستیم
        if (!isPaused && _kbhit())
        {
            char key = _getch();
            if (key == 'a' || key == 'A')
            {
                paddle.x -= paddle.speed;
                if (paddle.x < 1)
                    paddle.x = 1;

                if (!gameStarted && !isPaused)
                {
                    ball.x = paddle.x + paddle.width / 2.0f;
                }
            }
            else if (key == 'd' || key == 'D')
            {
                paddle.x += paddle.speed;
                if (paddle.x + paddle.width > BOARD_WIDTH - 1)
                    paddle.x = BOARD_WIDTH - 1 - paddle.width;

                if (!gameStarted && !isPaused)
                {
                    ball.x = paddle.x + paddle.width / 2.0f;
                }
            }
            else if (key == 'p' || key == 'P')
            {
                isPaused = true;
                pauseMove();
                continue;
            }
        }

        if (gameStarted && !isPaused)
        {
            float deltaTime = 0.016f; // 60 فریم در ثانیه
            updateBall(deltaTime);

            if (gameMode == GAME_MODE_ADVANCED)
            {
                activatePowerUps(deltaTime);
            }

            // بررسی تمام شدن آجرها
            bool allBricksDestroyed = true;
            for (int i = 0; i < BRICK_ROWS; i++)
            {
                for (int j = 0; j < BRICK_COLS; j++)
                {
                    if (bricks[i][j].visible)
                    {
                        allBricksDestroyed = false;
                        break;
                    }
                }
                if (!allBricksDestroyed)
                    break;
            }

            checkPaddleCollision();
            checkBrickCollision();
            checkWallCollision();

            if (allBricksDestroyed)
            {
                gameOver = true;
                renderToBuffer();
                renderWithColorsOptimized();
                printVictory();
                saveGameHistory("Won");
                clearSavedGame();
            }
        }

        Sleep(8);
    }

    if (lives == 0)
    {
        clearMenuArea();
        saveGameHistory("Lost");
        printGameOver();
        clearSavedGame();
    }

    cout << "\nPress any key to return to main menu..." << endl;
    _getch();

    clearMenuArea();
    menuMove();
}

void newGame()
{
    system("cls");
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
         << BOLD << BLUE << "                                                         ENTER YOUR NAME : " << RESET << endl;
    cout << "                                                         ";
    getline(cin, currentPlayerName);
    while (currentPlayerName.empty())
    {
        cout << BOLD << RED << "                                                         Enter your name please :)" << RESET;
        Sleep(1000);
        system("cls");
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
             << BOLD << BLUE << "                                                         ENTER YOUR NAME : " << RESET << endl;
        cout << "                                                         ";
        getline(cin, currentPlayerName);
    }
    system("cls");
    selectGameMode();

    initScreenBuffer();

    initializeBricks();

    if (gameMode == GAME_MODE_ADVANCED)
    {
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            powerups[i].active = false;
            powerups[i].collected = false;
        }
    }

    system("cls");

    paddle.x = INITIAL_PADDLE_X;
    paddle.y = INITIAL_PADDLE_Y;
    paddle.width = 10.0f;
    paddle.height = 1.0f;
    paddle.speed = PADDLE_SPEED;

    ball.x = INITIAL_BALL_X;
    ball.y = INITIAL_BALL_Y;
    ball.vx = BALL_SPEED;
    ball.vy = -BALL_SPEED;
    ball.radius = 0.5f;

    score = 0;
    lives = 3;
    gameOver = false;
    gameStarted = false;
    isPaused = false;

    resetBallAndPaddle();
    runGame();
}

// ==================== تابع main ====================
int main()
{
    system("cls");
    hideCursor();
    // پیش‌ بارگذاری صداها برای کاهش تاخیر
    preloadSounds();
    loadSettings();
    menuMove();

    return 0;
}