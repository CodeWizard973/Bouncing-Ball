#include "iGraphics.h"
#include <iostream>
#include <windows.h> 
#include <cstdlib>
#include <ctime>
using namespace std;

#define screenWidth 1024
#define screenHeight 576
#define yAxisMargin 50

// Bricks Setup
#define rows 5
#define cols 10
#define brickWidth 80
#define brickHeight 25
#define brickSpacing 5
bool bricks[rows][cols];

// Variables
int Screen, Start, Setting, Highscore, Exit, Level, Level1, Level2;
bool level2Active = false;
bool musicOn = true;
bool gameStarted = false;
bool levelSelect = false;
bool ballStart = false;
bool ballSetting = false;
bool ballHighscore = false;
bool ballExit = false;

int background;
int red = 255, green = 0, blue = 0;

// Ball Variables
int xBall = 512, yBall = yAxisMargin, radiusBall = 10;
int dx = 7, dy = 5;

// Paddle 1 (Bottom)
int xBoard1 = 512 - 50, yBoard1 = yAxisMargin;
int dxBoard1 = 100, dyBoard1 = 10;

// Paddle 2 (Top for Level 2)
int xBoard2 = 512 - 50, yBoard2 = screenHeight - 50;
int dxBoard2 = 100, dyBoard2 = 10;

int lives = 3;
int score = 0;

// Power-Up Variables
bool powerUpActive = false;
int powerUpX, powerUpY;
int powerUpType = 0; // 1 = Extra Life, 2 = Paddle Increase, 3 = Ball Increase
bool powerUpFalling = false;

// Functions
void ballChange();
void restartBall();
void resetBricks();
void spawnPowerUp(int x, int y);
void applyPowerUp();


void iDraw()
{
	iClear();

	if (gameStarted) {
		iShowImage(0, 0, screenWidth, screenHeight, background);

		// Ball
		iSetColor(red, green, blue);
		iFilledCircle(xBall, yBall, radiusBall);

		// Paddle 1 (Bottom)
		iSetColor(255, 0, 0);
		iFilledRectangle(xBoard1, yBoard1, dxBoard1, dyBoard1);

		// Paddle 2 (Top) for Level 2
		if (level2Active) {
			iSetColor(0, 255, 255);
			iFilledRectangle(xBoard2, yBoard2, dxBoard2, dyBoard2);
		}

		// Draw Power-Up
		if (powerUpFalling) {
			if (powerUpType == 1) iSetColor(0, 255, 0); // Green for Extra Life
			else iSetColor(0, 0, 255); // Blue for Paddle Increase
			iFilledCircle(powerUpX, powerUpY, 10);
		}

		// Draw Score
		iSetColor(255, 255, 255);
		char scoreText[20];
		sprintf(scoreText, "Score: %d", score);
		iText(20, screenHeight - 30, scoreText, GLUT_BITMAP_HELVETICA_18);

		int totalBrickWidth = cols * brickWidth + (cols - 1) * brickSpacing; // Total width of all bricks
		int startX = (screenWidth - totalBrickWidth) / 2;  // Centered horizontally
		int startY = (screenHeight / 2) + 100;

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (bricks[i][j]) {  // Draw only if the brick exists
					iSetColor(rand() % 256, rand() % 256, rand() % 256);  // Random colors
					int brickX = startX + j * (brickWidth + brickSpacing);
					int brickY = startY - i * (brickHeight + brickSpacing);
					iFilledRectangle(brickX, brickY, brickWidth, brickHeight);
				}
			}
		}
	}
	else if (levelSelect) {
		iShowImage(0, 0, screenWidth, screenHeight, Level);
		int buttonWidth = 250;
		int buttonHeight = 80;
		int startX = (screenWidth - buttonWidth) / 2;
		int startY = screenHeight / 2 + 100;
		int spacing = 20;

		// Showing Levels
		iShowImage(startX, startY, buttonWidth, buttonHeight, Level1);
		iShowImage(startX, startY - (buttonHeight + spacing), buttonWidth, buttonHeight, Level2);
	}
	else {
		// Main Menu
		int buttonWidth = 250;
		int buttonHeight = 60;
		int startX = (screenWidth - buttonWidth) / 2;
		int startY = screenHeight / 2 + 100;
		int spacing = 20;

		iShowImage(0, 0, screenWidth, screenHeight, Screen);
		iShowImage(startX, startY, buttonWidth, buttonHeight, Start);
		iShowImage(startX, startY - (buttonHeight + spacing), buttonWidth, buttonHeight, Setting);
		iShowImage(startX, startY - 2 * (buttonHeight + spacing), buttonWidth, buttonHeight, Highscore);
		iShowImage(startX, startY - 3 * (buttonHeight + spacing), buttonWidth, buttonHeight, Exit);
	}
}



void iPassiveMouseMove(int mx, int my)
{
	ballStart = (mx >= 40 && mx <= 196) && (my >= 408 && my <= 454);
	//ballLevel = (mx >= 40 && mx <= 196) && (my >= 350 && my <= 396);
	ballSetting = (mx >= 40 && mx <= 196) && (my >= 290 && my <= 336);
	ballHighscore = (mx >= 40 && mx <= 196) && (my >= 215 && my <= 263);
	ballExit = (mx >= 40 && mx <= 196) && (my >= 131 && my <= 176);
}

void iKeyboard(unsigned char key)
{
	if (level2Active) {
		if ((key == 'q' || key == 'Q') && xBoard2 > 0)
			xBoard2 -= 20;
		if ((key == 'e' || key == 'E') && xBoard2 < screenWidth - dxBoard2)
			xBoard2 += 20;
	}
}


void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (!levelSelect) {
			int startX = (screenWidth - 250) / 2;

			// Clicking Start Button
			if (mx >= startX && mx <= startX + 250 && my >= screenHeight / 2 + 100 && my <= screenHeight / 2 + 160) {
				levelSelect = true;  // Go to level selection
			}
		}
		else {
			int startX = (screenWidth - 250) / 2;
			int startY = screenHeight / 2 + 100;
			int buttonHeight = 80;
			int spacing = 20;

			// Clicking Level 1
			if (mx >= startX && mx <= startX + 250 && my >= startY && my <= startY + buttonHeight) {
				gameStarted = true;
				levelSelect = false;
				level2Active = false;
				restartBall();
			}

			// Clicking Level 2
			else if (mx >= startX && mx <= startX + 250 && my >= startY - (buttonHeight + spacing) && my <= startY - (buttonHeight + spacing) + buttonHeight) {
				gameStarted = true;
				levelSelect = false;
				level2Active = true;
				restartBall();
			}
		}
	}
}



void iMouseMove(int mx, int my)
{

}
void iSpecialKeyboard(unsigned char key)
{
	// Paddle 1 (Bottom)
	if (key == GLUT_KEY_LEFT && xBoard1 > 0) {
		xBoard1 -= 20;
	}
	if (key == GLUT_KEY_RIGHT && xBoard1 < screenWidth - dxBoard1) {
		xBoard1 += 20;
	}

	// Paddle 2 (Top) - Only in Level 2
	if (level2Active) {
		if (key == GLUT_KEY_F1 && xBoard2 > 0) {
			xBoard2 -= 20;
		}
		if (key == GLUT_KEY_F2 && xBoard2 < screenWidth - dxBoard2) {
			xBoard2 += 20;
		}
	}
}

void ballChange()
{
	if (gameStarted) {
		xBall += dx;
		yBall += dy;

		if (xBall >= screenWidth - radiusBall || xBall <= radiusBall) dx *= -1;
		if (yBall >= screenHeight - radiusBall) dy *= -1;

		if (yBall <= yBoard1 + dyBoard1 && yBall >= yBoard1 && xBall >= xBoard1 && xBall <= xBoard1 + dxBoard1) dy *= -1;
		if (level2Active && yBall >= yBoard2 - dyBoard2 && yBall <= yBoard2 && xBall >= xBoard2 && xBall <= xBoard2 + dxBoard2) dy *= -1;

		int totalBrickWidth = cols * brickWidth + (cols - 1) * brickSpacing;
		int startX = (screenWidth - totalBrickWidth) / 2;
		int startY = screenHeight * 2 / 3;

		bool brickHit = false;

		for (int i = 0; i < rows && !brickHit; i++) {
			for (int j = 0; j < cols; j++) {
				if (bricks[i][j]) {
					int brickX = startX + j * (brickWidth + brickSpacing);
					int brickY = startY - i * (brickHeight + brickSpacing);
					if (xBall >= brickX && xBall <= brickX + brickWidth && yBall + radiusBall >= brickY && yBall - radiusBall <= brickY + brickHeight) {
						bricks[i][j] = false;
						dy *= -1;
						brickHit = true;
						score += 5;
						if (rand() % 5 == 0) spawnPowerUp(brickX + brickWidth / 2, brickY);
						break;
					}
				}
			}
		}

		if (powerUpFalling) {
			powerUpY -= 5;
			if (powerUpY <= yBoard1 + dyBoard1 && powerUpY >= yBoard1 && powerUpX >= xBoard1 && powerUpX <= xBoard1 + dxBoard1) {
				applyPowerUp();
			}
		}

		if (yBall < 0) {
			lives--;
			if (lives <= 0) gameStarted = false;
			else restartBall();
		}
	
		if (level2Active && yBall > screenHeight + radiusBall) {
			cout << "Ball missed Paddle 2! Restarting game." << endl;

			// Reset game variables
			gameStarted = false;
			lives = 3;         // Reset lives
			resetBricks();     // Reset all bricks

			// Restart game
			restartBall();
			return;
		}
		// If ball misses Paddle 1 (Bottom)
		if (yBall < 0) {
			cout << "Ball missed Paddle 1! Losing a life." << endl;
			lives--;  // Decrease Life
			if (lives <= 0) {
				cout << "Game Over! No lives left." << endl;
				gameStarted = false;
				resetBricks();    // Reset all bricks
			}
			else {
				restartBall();    // Restart with remaining lives
			}
		}

	}
}
void spawnPowerUp(int x, int y)
{
	powerUpX = x;
	powerUpY = y;
	powerUpType = (rand() % 3) + 1; // Random 1, 2, or 3
	powerUpFalling = true;
}
void applyPowerUp()
{
	if (powerUpType == 1) {
		lives += 1;
	}
	else if (powerUpType == 2) {
		dxBoard1 += 50;
	}
	else if (powerUpType == 3) {
		radiusBall += 3; // Increase ball size
	}
	powerUpFalling = false;
}

void restartBall()
{
	if (lives > 0) {
		xBall = screenWidth / 2;
		yBall = yBoard1 + 20;  // Start slightly above bottom paddle

		dx = 7;
		dy = -5;  // Move downward after reset

		xBoard1 = screenWidth / 2 - dxBoard1 / 2;
		yBoard1 = yAxisMargin;

		if (level2Active) {
			xBoard2 = screenWidth / 2 - dxBoard2 / 2;
			yBoard2 = screenHeight - 50;
		}
	}
	else {
		gameStarted = false;
		cout << "Game Over! Press Start to play again." << endl;
	}
}

void resetBricks()
{

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			bricks[i][j] = true;  // Mark all bricks as "present"
		}
	}
}

int main()
{
	srand((unsigned)time(NULL));
	iInitialize(screenWidth, screenHeight, "Bouncing Ball");

	// Initialize bricks before game starts
	resetBricks();
	Screen = iLoadImage("Image\\Screen.bmp");
	Start = iLoadImage("Image\\Start.bmp");
	Setting = iLoadImage("Image\\Setting.bmp");
	Highscore = iLoadImage("Image\\Highscore.bmp");
	Exit = iLoadImage("Image\\Exit.bmp");
	background = iLoadImage("Image\\background.bmp");
	Level = iLoadImage("Image\\Level.bmp");
	Level1 = iLoadImage("Image\\Level 1.bmp");
	Level2 = iLoadImage("Image\\Level 2.bmp");

	if (musicOn)
		PlaySound("Music\\backgroundmusic1.wav", NULL, SND_LOOP | SND_ASYNC);

	iSetTimer(25, ballChange);
	iStart();
	return 0;
}