#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
#include <vector>


struct birdObject {
	Vector2 position;
	Vector2 velocity;
    float drag = 1.0f;
    float mass = 1.0f;
};

struct physicsWorld
{
    Vector2 gravity = { 0.0f, 9.81f };
    std::vector<birdObject> entities;
};

const unsigned int TARGET_FPS = 50;
float time = 0;
float dt;
float x = 500, y = 500;
float frequency = 1.0f;
float amplitude = 70;

Vector2 launchPosition = { 80.0f, 700.0f }; 
Rectangle Slingshot = { launchPosition.x - 100, launchPosition.y - 10, 100, 20 };
float launchSpeed = 100.0f;
float launchAngle = -45.0f;
float worldGravity = 9.81f;
Vector2 birdVelocity = Vector2Rotate(Vector2UnitX, launchAngle) * launchSpeed;
Vector2 gravity = { 0.0f, worldGravity };


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

    //CONTROLS
    GuiSliderBar(Rectangle{ 30, 90, 500, 30 }, "Speed", TextFormat("Speed: %.0f", launchSpeed), &launchSpeed, -1000, 1000);
    GuiSliderBar(Rectangle{ 30, 110, 500, 30 }, "Angle", TextFormat("Angle: %.0f Degrees", launchAngle), &launchAngle, -180, 180);
    GuiSliderBar(Rectangle{ 30, 130, 500, 30 }, "Gravity", TextFormat("Gravity: %.0f Px/sec^2", worldGravity), &worldGravity, -100, 100);


    DrawCircleV(launchPosition, 20, LIGHTGRAY);
	DrawRectangle(0, GetScreenHeight()-20, GetScreenWidth(), 50, DARKGREEN);
	DrawRectangleRec(Slingshot, BROWN);

	DrawText(TextFormat("Launch Angle: %.2f", launchAngle), 10, 50, 20, LIGHTGRAY);
	DrawText(TextFormat("Launch Speed: %.2f", launchSpeed), 10, 70, 20, LIGHTGRAY);


    //Line to show the shot
    Vector2 previewVel = { cosf(launchAngle*DEG2RAD) * launchSpeed, -sinf(launchAngle * DEG2RAD) * launchSpeed };
    float previewScale = 1.0f;
    Vector2 previewTip = Vector2Add(launchPosition, Vector2Scale(previewVel, previewScale));

    // draw the preview vector (thin so it looks nice)
    DrawLineEx(launchPosition, previewTip, 4.0f, RED);

    EndDrawing();
}

int main()
{
    InitWindow(InitialWidth, InitialHeight, "Physics Labs: Felipe Rolon 101538323");
    SetTargetFPS(TARGET_FPS);

    bool shot = false;
    while (!WindowShouldClose())
    {
		draw();
        if (IsKeyDown(KEY_SPACE)) {
            shot = !shot;
        }
        if (shot) {
            update();
        }

    }

    CloseWindow();
    return 0;
}


        