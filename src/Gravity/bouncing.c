#include "raylib.h"
#include <math.h>

#define g 9.0

typedef struct {
    double x, y;
} Vector;

typedef struct {
    double mass;
    Vector Position;
    Vector Velocity;
    Vector Acceleration;
} Particle;

// ---------------- ENERGY ----------------
double KE[3], PE[3], Dissipated[3];

// damping factor (NEW)
double damping = 0.1;

// ---------------- EULER ----------------
void IntegrateEuler(Particle* p, double dt, double* diss)
{
    // gravity + damping
    p->Acceleration.y = g - damping * p->Velocity.y;

    double vBefore = p->Velocity.y;

    p->Velocity.y += p->Acceleration.y * dt;
    p->Position.y += p->Velocity.y * dt;

    // continuous dissipation tracking
    double energyLoss = 0.5 * (vBefore*vBefore - p->Velocity.y*p->Velocity.y);
    if (energyLoss > 0)
        *diss += energyLoss;

    if (p->Position.y >= 500)
    {
        p->Position.y = 500;

        double before = 0.5 * p->Velocity.y * p->Velocity.y;
        p->Velocity.y *= -0.8;
        double after = 0.5 * p->Velocity.y * p->Velocity.y;

        *diss += (before - after);
    }
}

// ---------------- SEMI-EULER ----------------
void IntegrateSemiEuler(Particle* p, double dt, double* diss)
{
    p->Acceleration.y = g - damping * p->Velocity.y;

    double vBefore = p->Velocity.y;

    p->Velocity.y += p->Acceleration.y * dt;
    p->Position.y += p->Velocity.y * dt;

    double energyLoss = 0.5 * (vBefore*vBefore - p->Velocity.y*p->Velocity.y);
    if (energyLoss > 0)
        *diss += energyLoss;

    if (p->Position.y >= 500)
    {
        p->Position.y = 500;

        double before = 0.5 * p->Velocity.y * p->Velocity.y;
        p->Velocity.y *= -0.8;
        double after = 0.5 * p->Velocity.y * p->Velocity.y;

        *diss += (before - after);
    }
}

// ---------------- VERLET ----------------
double prevY[3];
void IntegrateVerlet(Particle* p, int id, double dt, double* diss)
{
    double y = p->Position.y;
    double prev = prevY[id];

    double a = g;

    double newY = y + (y - prev) + a * dt * dt;

    // ---------------- COLLISION ----------------
    if (newY >= 500)
    {
        newY = 500;

        // compute velocity from verlet
        double vel = (newY - y) / dt;

        // bounce
        vel *= -0.8;

        // CRITICAL FIX: rebuild previous position
        prevY[id] = newY - vel * dt;

        p->Position.y = newY;
        p->Velocity.y = vel;

        return;
    }

    // normal update
    prevY[id] = y;
    p->Position.y = newY;
    p->Velocity.y = (newY - y) / dt;
}
// ---------------- ENERGY ----------------
void ComputeEnergy(Particle* p, int i)
{
    double v = p->Velocity.y;

    KE[i] = 0.5 * p->mass * v * v;
    PE[i] = p->mass * g * (500 - p->Position.y);
}

// ---------------- MAIN ----------------
int main(void)
{
    InitWindow(900, 600, "Dissipation + Integration Comparison");
    SetTargetFPS(60);

    double fixedDt = 0.001;
    double accumulator = 0.0;
    double timeScale = 15.0;

    Particle p[3];

    for (int i = 0; i < 3; i++)
    {
        p[i].mass = 1.0;
        p[i].Position = (Vector){200 + i * 250, 100};
        p[i].Velocity = (Vector){0, 0};
        p[i].Acceleration = (Vector){0, 0};
        prevY[i] = p[i].Position.y;
    }

    while (!WindowShouldClose())
    {
        double dtReal = GetFrameTime();
        accumulator += dtReal * timeScale;

        while (accumulator >= fixedDt)
        {
            IntegrateEuler(&p[0], fixedDt, &Dissipated[0]);
            IntegrateSemiEuler(&p[1], fixedDt, &Dissipated[1]);
            IntegrateVerlet(&p[2], 2, fixedDt, Dissipated);

            accumulator -= fixedDt;
        }

        for (int i = 0; i < 3; i++)
            ComputeEnergy(&p[i], i);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(0, 500, 900, 500, DARKGRAY);

        const char* names[3] = {
            "Euler + Drag",
            "Semi-Euler + Drag",
            "Verlet + Drag"
        };

        Color colors[3] = { RED, GREEN, BLUE };

        for (int i = 0; i < 3; i++)
        {
            DrawCircle((int)p[i].Position.x, (int)p[i].Position.y, 18, colors[i]);

            double total = KE[i] + PE[i] + Dissipated[i];

            DrawText(names[i], (int)p[i].Position.x - 50, 520, 15, colors[i]);

            DrawText(TextFormat("KE: %.2f", KE[i]),
                     (int)p[i].Position.x - 50, 550, 10, GRAY);

            DrawText(TextFormat("Loss: %.2f", Dissipated[i]),
                     (int)p[i].Position.x - 50, 565, 10, GRAY);
        }

        DrawText("Damping affects motion continuously (not only collisions)", 20, 20, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
