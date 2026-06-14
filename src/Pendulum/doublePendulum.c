#include "raylib.h"
#include <math.h>

#define g 9.0f

typedef struct {
    double theta1, theta2;
    double omega1, omega2;
    double L1, L2;
    double m1, m2;
} DoublePendulum;

/* REAL double pendulum equations (simplified standard form) */
void Step(DoublePendulum *p, float dt,double c)
{
    double m1 = p->m1, m2 = p->m2;
    double t1 = p->theta1, t2 = p->theta2;
    double w1 = p->omega1, w2 = p->omega2;
    double L1 = p->L1, L2 = p->L2;

    double delta = t2 - t1;

    double den1 = (m1 + m2) - m2 * cos(delta) * cos(delta);

    double a1 =
        (m2 * L1 * w1 * w1 * sin(delta) * cos(delta)
        + m2 * g * sin(t2) * cos(delta)
        + m2 * L2 * w2 * w2 * sin(delta)
        - (m1 + m2) * g * sin(t1))
        / (L1 * den1);

    double den2 = (L2 / L1) * den1;

    double a2 =
        (-m2 * L2 * w2 * w2 * sin(delta) * cos(delta)
        + (m1 + m2) * g * sin(t1) * cos(delta)
        - (m1 + m2) * L1 * w1 * w1 * sin(delta)
        - (m1 + m2) * g * sin(t2))
        / (L2 * den1);

    p->omega1 += a1 * dt -c*p->theta1; // c is the damping coefficient
    p->omega2 += a2 * dt -c*p->theta2;

    p->theta1 += p->omega1 * dt;
    p->theta2 += p->omega2 * dt;
}

typedef struct { float x, y; } Vec2;

Vec2 Pos1(DoublePendulum *p, Vec2 origin)
{
    return (Vec2){
        origin.x + p->L1 * sin(p->theta1),
        origin.y + p->L1 * cos(p->theta1)
    };
}

Vec2 Pos2(DoublePendulum *p, Vec2 origin)
{
    Vec2 b1 = Pos1(p, origin);

    return (Vec2){
        b1.x + p->L2 * sin(p->theta2),
        b1.y + p->L2 * cos(p->theta2)
    };
}

int main()
{
    InitWindow(800, 800, "Real Double Pendulum");
    SetTargetFPS(60);

    DoublePendulum p = {
        .theta1 = 0.3,
        .theta2 = -0.3,
        .omega1 = 0.0,
        .omega2 = 0.0,
        .L1 = 200,
        .L2 = 100,
        .m1 = 2.0,
        .m2 = 100.0
    };

    Vec2 origin = {400, 400};

    while (!WindowShouldClose())
    {
        float dt = 0.02;

        // substeps for stability
        for (int i = 0; i < 4; i++)
            Step(&p, dt / 4.0f,1);

        Vec2 b1 = Pos1(&p, origin);
        Vec2 b2 = Pos2(&p, origin);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // rods
        DrawLine(origin.x, origin.y, b1.x, b1.y, DARKGRAY);
        DrawLine(b1.x, b1.y, b2.x, b2.y, DARKGRAY);

        // masses
        DrawCircle(origin.x, origin.y, 6, BLACK);
        DrawCircle(b1.x, b1.y, 16, RED);
        DrawCircle(b2.x, b2.y, 16, BLUE);

        DrawText("REAL Double Pendulum (chaotic system)", 20, 20, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
