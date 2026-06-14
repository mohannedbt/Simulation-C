#include "raylib.h"
#include <math.h>

#define g 10.0f

typedef struct {
    double theta;
    double omega;
    double L;
    double alpha;
} Pendulum;

typedef struct {
    double x;
    double y;
} Vector;

void ConvertToCard(Pendulum* p, Vector* v)
{
    v->x = p->L * sin(p->theta);
    v->y = p->L * cos(p->theta);
}

void EulerIntegration(Pendulum* p, float dt)
{
    p->alpha = (-g / p->L) * sin(p->theta);
    p->omega += p->alpha * dt;
    p->theta += p->omega * dt;
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Pendulum - Toggle Speed");

    SetTargetFPS(60);

    Pendulum p = {
        .theta = 2.0,
        .omega = 0.0,
        .L = 200.0,
        .alpha = 0.0
    };

  

    Vector v1, v2;
    Vector2 origin = { screenWidth / 2.0f, 150.0f };

    float timeScale = 20.0f;
    
    while (!WindowShouldClose())
    {
        float dt =0.01;
        

        dt *= timeScale;

       

        // substeps for stability
        int steps = 4;
        float stepDt = dt / steps;

        for (int i = 0; i < steps; i++)
        {
            EulerIntegration(&p, stepDt);
        }

        ConvertToCard(&p, &v1);

        Vector2 bob = {
            origin.x + (float)v1.x,
            origin.y + (float)v1.y
        };



        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(origin.x, origin.y, bob.x, bob.y, DARKGRAY);
        DrawCircle(origin.x, origin.y, 6, BLACK);
        DrawCircle(bob.x, bob.y, 18, RED);

     
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
