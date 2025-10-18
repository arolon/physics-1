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

    float radius = 10.0f;
    Color color = GREEN;
	Color initColor = GREEN;
};

struct physicsWorld
{
    Vector2 gravity = { 0.0f, 9.81f };
    std::vector<birdObject> entities;
};

// detect collisions function
bool isColliding(const birdObject& a, const birdObject& b)
{
    float distance = Vector2Distance(a.position, b.position);
    float radiusSum = a.radius + b.radius;
    return distance <= radiusSum;
}

const unsigned int TARGET_FPS = 60;
float time = 0;
float dt;

physicsWorld world;

//float birdRadius = 10.0f;
float launchSpeed = 100.0f;
float launchAngle = 0.0f;
float birdDrag = 1.0f;

Vector2 initialLaunchPosition = { 80.0f, 700.0f };
Vector2 launchPosition = { 80.0f, 700.0f }; 
Rectangle Slingshot = { launchPosition.x - 100, launchPosition.y - 10, 100, 20 };
Vector2 previewVel = { 0.0f, 0.0f };


void update() {
    dt = 1.0/TARGET_FPS;
    time += dt;

    birdDrag = Clamp(birdDrag, 0.0f, 1.0f);

    previewVel.x = cosf(launchAngle * DEG2RAD) * launchSpeed;
    previewVel.y = -sinf(launchAngle * DEG2RAD) * launchSpeed;

    for (size_t i = 0; i < world.entities.size(); i++)
    {
        birdObject& bird = world.entities[i];
        bird.velocity += world.gravity * dt;
        bird.velocity *= powf(bird.drag, dt);
        bird.position += bird.velocity * dt;
		bird.color = bird.initColor;
    }
    for (size_t i = 0; i < world.entities.size(); i++)
    {
        for (size_t j = i + 1; j < world.entities.size(); ++j)
        {
            birdObject& bird = world.entities[i];
            birdObject& birdTwo = world.entities[j];
            if (isColliding(bird, birdTwo))
            {
                bird.color = RED;
                birdTwo.color = RED;
            }
        }
    }
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
    GuiSliderBar(Rectangle{ 30, 130, 500, 30 }, "Gravity", TextFormat("Gravity: %.0f Px/sec^2", world.gravity), &world.gravity.y, -100, 100);


    DrawCircleV(launchPosition, 20, LIGHTGRAY);
	DrawRectangle(0, GetScreenHeight()-20, GetScreenWidth(), 50, DARKGREEN);
	DrawRectangleRec(Slingshot, BROWN);

	DrawText(TextFormat("Launch Angle: %.2f", launchAngle), 10, 50, 20, LIGHTGRAY);
	DrawText(TextFormat("Launch Speed: %.2f", launchSpeed), 10, 70, 20, LIGHTGRAY);

	// Draw all bird objects
    for (const birdObject& e : world.entities)
    {
        DrawCircleV(e.position, e.radius, e.color);
    }

    float previewScale = 0.5f;
    Vector2 previewTip = Vector2Add(launchPosition, Vector2Scale(previewVel, previewScale));

    // draw the preview vector (thin so it looks nice)
    DrawLineEx(launchPosition, previewTip, 4.0f, RED);

    EndDrawing();
}

int main()
{
    InitWindow(InitialWidth, InitialHeight, "Physics Labs: Felipe Rolon 101538323");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            birdObject bird;
            bird.drag = birdDrag;
            bird.position = initialLaunchPosition;
            bird.velocity = previewVel;
            bird.radius = float((rand() % 26) + 5);
            bird.initColor = {
                (unsigned char)(rand() % 256),
                (unsigned char)(rand() % 256),
                (unsigned char)(rand() % 256),
                255
            };
            world.entities.push_back(bird);
        }
		draw();
        update();

    }

    CloseWindow();
    return 0;
}


        