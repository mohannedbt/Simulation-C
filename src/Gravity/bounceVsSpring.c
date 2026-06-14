#include "raylib.h"
#include <math.h>

#define g 9.0

typedef struct {
    double y, v;
} Spring;

typedef struct {
    double y, v;
} Ball;

// ---------------- SPRING ----------------
void StepSpring(Spring* s, double dt, double k, double restY)
{
    double acc = k * (restY - s->y);

    s->v += acc * dt;
    s->y += s->v * dt;
}

// ---------------- BALL ----------------
void StepBall(Ball* b, double dt)
{
    b->v += g * dt;
    b->y += b->v * dt;

    if (b->y >= 500)
    {
        b->y = 500;
        b->v *= -1.0;
    }
}

// ---------------- MAIN ----------------
int main(void)
{
    InitWindow(900, 600, "Spring vs Ball (Matched Frequency)");

    SetTargetFPS(60);

    double dt = 0.1;

    double restY = 300;
    double ground = 500;

    double h = ground - restY;

    // MATCHED spring constant
    double k = (M_PI * M_PI * g) / (2.0 * h);

    Spring spring = {150, 0};
    Ball ball = {150, 0};

    while (!WindowShouldClose())
    {
        StepSpring(&spring, dt, k, restY);
        StepBall(&ball, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(0, 500, 900, 500, DARKGRAY);
        DrawLine(0, (int)restY, 900, (int)restY, LIGHTGRAY);

        DrawCircle(250, (int)spring.y, 18, BLUE);
        DrawCircle(650, (int)ball.y, 18, RED);

        DrawText("Synchronized Spring vs Ball", 250, 20, 20, GRAY);
        DrawText(TextFormat("k = %.3f", k), 20, 50, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
