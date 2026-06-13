#include "raylib.h"
#include <math.h>

typedef struct Block {
    Vector2 position;
    Vector2 velocity;
    Vector2 accel;
    double mass;
} Block;

/* Physics: simple 2D spring + gravity */
void MoveBlock(Block *b, float dt)
{
    const float gravity = 980.0f;
    const float k = 25.0f;
    const float damping = 2.5f;

    Vector2 rest = {400, 300};

    float fx = -k * (b->position.x - rest.x);
    float fy = -k * (b->position.y - rest.y);

    fx += -damping * b->velocity.x;
    fy += -damping * b->velocity.y;

    fy += gravity * b->mass;

    b->accel.x = fx / (float)b->mass;
    b->accel.y = fy / (float)b->mass;

    b->velocity.x += b->accel.x * dt;
    b->velocity.y += b->accel.y * dt;

    b->position.x += b->velocity.x * dt;
    b->position.y += b->velocity.y * dt;
}

/* Traveling sine-wave spring */
void DrawSpring(Vector2 a, Vector2 b, float time)
{
    int segments = 90;

    Vector2 dir = {
        b.x - a.x,
        b.y - a.y
    };

    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.001f) return;

    dir.x /= len;
    dir.y /= len;

    Vector2 perp = {-dir.y, dir.x};

    float amplitude = 8.0f;
    float frequency = 0.22f;
    float waveSpeed = 6.0f;

    Vector2 prev = a;

    for (int i = 1; i <= segments; i++)
    {
        float t = (float)i / segments;

        float x = t * len;

        /* traveling wave = space + time coupling */
        float wave = sinf(x * frequency - time * waveSpeed);

        Vector2 point = {
            a.x + dir.x * x + perp.x * wave * amplitude,
            a.y + dir.y * x + perp.y * wave * amplitude
        };

        DrawLineV(prev, point, DARKBLUE);
        prev = point;
    }
}

/* velocity vector */
void DrawVelocity(Vector2 pos, Vector2 vel)
{
    Vector2 end = {
        pos.x + vel.x * 0.15f,
        pos.y + vel.y * 0.15f
    };

    DrawLineV(pos, end, RED);
}

/* block rendering */
void DrawBlock(Block b)
{
    DrawRectangleV(b.position, (Vector2){20, 20}, RED);
}

int main(void)
{
    InitWindow(800, 600, "Traveling Wave Spring Physics");

    Block block = {
        .position = {200, 100},
        .velocity = {0, 0},
        .accel = {0, 0},
        .mass = 1.0
    };

    Vector2 rest = {400, 300};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float time = GetTime();

        MoveBlock(&block, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        /* anchor */
        DrawCircleV(rest, 5, BLUE);

        /* spring (animated wave) */
        DrawSpring(rest, block.position, time);

        /* velocity vector */
        DrawVelocity(block.position, block.velocity);

        /* mass */
        DrawBlock(block);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
