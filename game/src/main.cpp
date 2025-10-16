#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"

const unsigned int TARGET_FPS = 50;
float time = 0;
float dt;
float x = 500, y = 500;
float frequency = 1.0f;
float amplitude = 70;

Vector2 launchPosition = { 80.0f, 700.0f }; 
Rectangle Slingshot = { launchPosition.x - 100, launchPosition.y - 10, 100, 20 };
float launchSpeed = 100.0f;
float launchAngle = 45.0f;
Vector2 birdVelocity = Vector2Rotate(Vector2{ launchSpeed, 0 }, launchAngle) * launchSpeed;
Vector2 gravity = { 0.0f, 9.81f };


//Add sliders for frequency, amplitude and fps

void update() {
    dt = 1.0/TARGET_FPS;
    time += dt;

	x = x + (-sin(time * frequency)) * frequency * amplitude * dt;
	y = y + (cos(time * frequency)) * frequency * amplitude * dt;

	birdVelocity += gravity * dt;
	launchPosition += birdVelocity * dt;
}

void draw() {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawText("Felipe Rolon - 101538323", 10, 10, 20, LIGHTGRAY);

    GuiSliderBar(Rectangle{ 60, 5, 1000, 10 }, "Time", TextFormat("%.2f", time), &time, 0, 240);
    DrawText(TextFormat("FPS: %i TIME: %.2f", TARGET_FPS, time), GetScreenWidth()-250, 30, 20, LIGHTGRAY);

    DrawCircleV(launchPosition, 20, LIGHTGRAY);
	DrawRectangle(0, GetScreenHeight()-20, GetScreenWidth(), 50, DARKGREEN);
	DrawRectangleRec(Slingshot, BROWN);

	DrawText(TextFormat("Launch Angle: %.2f", launchAngle), 10, 50, 20, LIGHTGRAY);
	DrawText(TextFormat("Launch Speed: %.2f", launchSpeed), 10, 80, 20, LIGHTGRAY);


	/*DrawCircle(x, y, 60, RED);
    DrawCircle(GetScreenWidth()/2 + cos((time * frequency) * amplitude), GetScreenHeight() / 2 + sin((time * frequency) * amplitude), 60, GREEN);*/

    EndDrawing();
}

int main()
{
    InitWindow(InitialWidth, InitialHeight, "Physics Labs: Felipe Rolon 101538323");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
		update();
		draw();
    }

    CloseWindow();
    return 0;
}


        