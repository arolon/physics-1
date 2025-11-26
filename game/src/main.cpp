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

    bool fixed = false;

    float radius = 10.0f;
    Color color = GREEN;
	Color initColor = GREEN;
};

struct halfSpace
{
    Vector2 position;
    Vector2 normal = { 0, -1 };
    float rotation = 0;
    Color color = BLUE;
    Color initColor = BLUE;

    void SetRotation(float degrees)
    {
        rotation = degrees;
        normal = Vector2Normalize(Vector2Rotate({ 0, -1 }, degrees * DEG2RAD));
    }
};


struct physicsWorld
{
    Vector2 gravity = { 0.0f, 9.81f };
    std::vector<birdObject> entities;
    std::vector<halfSpace> halfspaces; //vector to hold multiple walls
};

// detect collisions function
bool isColliding(const birdObject& a, const birdObject& b)
{
    float distance = Vector2Distance(a.position, b.position);
    float radiusSum = a.radius + b.radius;
    return distance <= radiusSum;
}

bool isCollidingHS(const birdObject& circle, const halfSpace& hs)
{
    Vector2 displacement = circle.position - hs.position;
    float dot = Vector2DotProduct(displacement, hs.normal);

    // Overlap when dot < radius
    return dot < circle.radius;
}

bool birdToBird(birdObject& a, birdObject& b)
{
    Vector2 diff = Vector2Subtract(a.position, b.position);
    float distance = Vector2Length(diff);
    float radiusSum = a.radius + b.radius;
    if (distance >= radiusSum) return false;

    float penetration = radiusSum - distance;

    Vector2 normal = (distance > 0) ? Vector2Scale(diff, 1.0f / distance) : Vector2{ 0, 1 }; //if distance is 0 pick arbitrary Y direction
    Vector2 mtv = Vector2Scale(normal, penetration);

    // Caso 1 if both are fixed, do nothing
    if (a.fixed && b.fixed) return true;

    // Caso 2 If A is fixed
    if (a.fixed) {
        b.position = Vector2Subtract(b.position, mtv);
    }
    // Caso3 if B is fixed
    else if (b.fixed) {
        a.position = Vector2Add(a.position, mtv);
    }
    // Caso 4 Both aren't fixed
    else {
        Vector2 halfMtv = Vector2Scale(mtv, 0.5f);
        a.position = Vector2Add(a.position, halfMtv);
        b.position = Vector2Subtract(b.position, halfMtv);
    }

    return true;
}

bool birdToHS(birdObject& circle, const halfSpace& hs)
{
    Vector2 toCircle = Vector2Subtract(circle.position, hs.position);
    float dot = Vector2DotProduct(toCircle, hs.normal);
    if (dot < circle.radius)
    {
        float penetration = circle.radius - dot;
        circle.position = Vector2Add(circle.position, Vector2Scale(hs.normal, penetration));
        return true;
    }
    return false;
}


const unsigned int TARGET_FPS = 60;
float time = 0;
float dt;

physicsWorld world;
//halfSpace halfspace;


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
        /*if (isCollidingHS(bird, halfspace))
        {
            bird.color = BROWN;
            halfspace.color = BROWN;
        }*/
        for (size_t j = 0; j < world.halfspaces.size(); j++) {
			halfSpace& halfspace = world.halfspaces[j];
            if (birdToHS(bird, halfspace)) {
                bird.color = BROWN;
                halfspace.color = BROWN;
            }
            else
            {
                halfspace.color = halfspace.initColor;
                bird.color = bird.initColor;
            }
        }   
        
    }
    for (size_t i = 0; i < world.entities.size(); i++)
    {
        for (size_t j = i + 1; j < world.entities.size(); ++j)
        {
            birdObject& bird = world.entities[i];
            birdObject& birdTwo = world.entities[j];
            /*if (isColliding(bird, birdTwo))
            {
                bird.color = RED;
                birdTwo.color = RED;
            }*/
            if (birdToBird(bird, birdTwo))
            {
                bird.color = RED;
                birdTwo.color = RED;
            }
        }
    }
}

void drawHalfspace() {
	halfSpace& halfspace = world.halfspaces[0];
    // Draw circle
    DrawCircleV(halfspace.position, 8, halfspace.color);

    // Draw normal
    DrawLineEx(
        halfspace.position,
        halfspace.position + halfspace.normal * 40,
        3,
        halfspace.color
    );

    Vector2 tangent = Vector2Rotate(halfspace.normal, PI * 0.5f);
    DrawLineEx(
        halfspace.position - tangent * 5000,
        halfspace.position + tangent * 5000,
        2,
        halfspace.color
    );

    // Controls
    GuiSliderBar({ 80, 200, 200, 20 }, "HX", TextFormat("%.0f", halfspace.position.x), &halfspace.position.x, 0, GetScreenWidth());
    GuiSliderBar({ 80, 230, 200, 20 }, "HY", TextFormat("%.0f", halfspace.position.y), &halfspace.position.y, 0, GetScreenHeight());

    float rot = halfspace.rotation;
    GuiSliderBar({ 80, 260, 200, 20 }, "Rot", TextFormat("%.0f", rot), &rot, -360, 360);
    halfspace.SetRotation(rot);

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

	//Draw halfspaces
    for (const halfSpace& hs : world.halfspaces) {
        DrawLineEx(hs.position, Vector2Add(hs.position, Vector2Scale(hs.normal, 40)), 3, hs.color);
        Vector2 tangent = Vector2Rotate(hs.normal, PI * 0.5f);
        DrawLineEx(
            Vector2Subtract(hs.position, Vector2Scale(tangent, 1000)),
            Vector2Add(hs.position, Vector2Scale(tangent, 1000)),
            4, hs.color
        );

        DrawCircleV(hs.position, 5, hs.color);
    }

	// Draw all bird objects
    for (const birdObject& e : world.entities)
    {
        Color drawColor = e.fixed ? BLACK : e.color;
        DrawCircleV(e.position, e.radius, drawColor);
    }

    float previewScale = 0.5f;
    Vector2 previewTip = Vector2Add(launchPosition, Vector2Scale(previewVel, previewScale));

    // draw the preview vector (thin so it looks nice)
    DrawLineEx(launchPosition, previewTip, 4.0f, RED);

	drawHalfspace();
    DrawText("Press SPACE to shot a bird", 400, 700, 20, DARKGRAY);
    DrawText("Press F to spawn a fixed bird", 400, 730, 20, DARKGRAY);

    EndDrawing();
}

void drawRamps() {
    // 1. Floor
    halfSpace floor;
    floor.position = { 500, 780 };
    floor.SetRotation(0);
    world.halfspaces.push_back(floor);

    //// 2. Left Ramp
    //halfSpace leftRamp;
    //leftRamp.position = { 200, 500 };
    //leftRamp.SetRotation(-30);
    //leftRamp.color = DARKBLUE;
    //world.halfspaces.push_back(leftRamp);

    //// 3. Right Ramp
    //halfSpace rightRamp;
    //rightRamp.position = { 800, 500 };
    //rightRamp.SetRotation(30);
    //rightRamp.color = MAROON;
    //world.halfspaces.push_back(rightRamp);
}


int main()
{
    InitWindow(InitialWidth, InitialHeight, "Physics Labs: Felipe Rolon 101538323");
    SetTargetFPS(TARGET_FPS);

    drawRamps();
    /*halfspace.position = { 500, 600 };
    halfspace.SetRotation(0);*/

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
            bird.fixed = false;
            world.entities.push_back(bird);
        }

        if (IsKeyPressed(KEY_F))
        {
            birdObject bird;
            bird.position = launchPosition;
            bird.velocity = { 0,0 };
            bird.radius = 30.0f;
            bird.initColor = BLACK;
            bird.color = BLACK;
            bird.fixed = true;
            world.entities.push_back(bird);
        }
		draw();
        update();

    }

    CloseWindow();
    return 0;
}


        