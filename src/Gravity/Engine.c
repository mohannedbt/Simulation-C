#include <raylib.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#define G 0.0002
#define EPS 0.0001
#define RADIUS 5.0
#define DT 0.000001
#define T 50
typedef struct {
    double x, y;
} Vec2;

typedef struct {
    double mass;
    Vec2 pos;
    Vec2 vel;
    Vec2 force;
    Color c;

    Vector2* trail;
    int trailCount;
    int trailCap;
} Particle;

typedef struct {
    Particle* p;
    int N;
    double dt;
} Universe;

// ================= UTIL =================

Vec2 V(double x, double y){ return (Vec2){x,y}; }

// ================= TRAILS =================

void InitTrail(Particle* p, int cap){
    p->trail = (Vector2*)malloc(sizeof(Vector2) * cap);
    p->trailCount = 0;
    p->trailCap = cap;
}

void AddTrail(Particle* p, Vector2 v){
    if(p->trailCount >= p->trailCap){
        for(int i = 1; i < p->trailCap; i++)
            p->trail[i - 1] = p->trail[i];
        p->trailCount = p->trailCap - 1;
    }
    p->trail[p->trailCount++] = v;
}

void DrawTrailGlow(Particle* p, Color base){
    for(int i = 1; i < p->trailCount; i++){
        float t = (float)i / p->trailCount;
        Color c = base;
        c.a = (unsigned char)(255 * t);
        DrawLineEx(p->trail[i-1], p->trail[i], 2.0f * t, c);
    }
}

// ================= INIT =================

void InitSolarSystem(Universe* U, int N){
    U->N = N;
    U->dt = DT;
    U->p = malloc(sizeof(Particle) * N);

    // SUN
    U->p[0].mass = 1e15;
    U->p[0].pos = V(0, 0);
    U->p[0].vel = V(0, 0);
    U->p[0].c = YELLOW;
    InitTrail(&U->p[0], 2000);

    // PLANETS
    for(int i = 1; i < N; i++){
        double r = 40 + i * 20;
        double speed = sqrt(G * U->p[0].mass / r);

        U->p[i].mass = pow(10,i+7)+ i * 5;
        U->p[i].pos = V(r, 0);

        U->p[i].vel = V(0, speed);

        U->p[i].c = (Color){50*i, 100, 255 - 30*i, 255};

        InitTrail(&U->p[i], 2000);
    }
}

// ================= FORCES =================

void ResetForces(Universe* U){
    for(int i = 0; i < U->N; i++)
        U->p[i].force = V(0,0);
}

void ComputeForces(Universe* U){
    for(int i = 0; i < U->N; i++){
        for(int j = i+1; j < U->N; j++){

            double dx = U->p[j].pos.x - U->p[i].pos.x;
            double dy = U->p[j].pos.y - U->p[i].pos.y;

            double r2 = dx*dx + dy*dy + EPS;
            double r = sqrt(r2);

            double F = G * U->p[i].mass * U->p[j].mass / (r2 * r);

            Vec2 f = V(F*dx, F*dy);

            U->p[i].force.x += f.x;
            U->p[i].force.y += f.y;
            U->p[j].force.x -= f.x;
            U->p[j].force.y -= f.y;
        }
    }
}

// ================= INTEGRATION =================

void Integrate(Universe* U){
    for(int i = 0; i < U->N; i++){
        Particle* p = &U->p[i];

        double ax = p->force.x / p->mass;
        double ay = p->force.y / p->mass;

        p->vel.x += ax * U->dt*T;
        p->vel.y += ay * U->dt*T;

        p->pos.x += p->vel.x * U->dt*T;
        p->pos.y += p->vel.y * U->dt*T;
    }
}
void IntegrateVV(Universe* U)
{
    double dt = U->dt*T;

    // ================= FIRST KICK =================
    for (int i = 0; i < U->N; i++)
    {
        Particle* p = &U->p[i];

        double ax = p->force.x / p->mass;
        double ay = p->force.y / p->mass;

        p->vel.x += 0.5 * ax * dt;
        p->vel.y += 0.5 * ay * dt;
    }

    // ================= DRIFT =================
    for (int i = 0; i < U->N; i++)
    {
        Particle* p = &U->p[i];

        p->pos.x += p->vel.x * dt;
        p->pos.y += p->vel.y * dt;
    }

    // ================= RECOMPUTE FORCES =================
    ResetForces(U);
    ComputeForces(U);

    // ================= SECOND KICK =================
    for (int i = 0; i < U->N; i++)
    {
        Particle* p = &U->p[i];

        double ax = p->force.x / p->mass;
        double ay = p->force.y / p->mass;

        p->vel.x += 0.5 * ax * dt;
        p->vel.y += 0.5 * ay * dt;
    }
}

// ================= ENERGY =================

double Energy(Universe* U){
    double E = 0;
    for(int i = 0; i < U->N; i++){
        double v2 = U->p[i].vel.x*U->p[i].vel.x +
                    U->p[i].vel.y*U->p[i].vel.y;

        E += 0.5 * U->p[i].mass * v2;
    }
    return E;
}

// ================= CAMERA =================

Vec2 ComputeCenter(Universe* U)
{
    Vec2 c = {0, 0};
    double m = 0;

    for (int i = 0; i < U->N; i++){
        c.x += U->p[i].pos.x * U->p[i].mass;
        c.y += U->p[i].pos.y * U->p[i].mass;
        m += U->p[i].mass;
    }

    c.x /= m;
    c.y /= m;

    return c;
}

Vector2 Project(Vec2 p, Vec2 cam, double scale)
{
    return (Vector2){
        500 + (p.x - cam.x) * scale,
        350 + (p.y - cam.y) * scale
    };
}

// ================= MAIN =================

int main(){
    InitWindow(1000, 700, "Solar System Simulation");

    Universe U = {0};
    InitSolarSystem(&U, 6);

    double scale = 1.0; 

    while(!WindowShouldClose()){
     
      
        Vec2 cam = ComputeCenter(&U);

        ResetForces(&U);
        ComputeForces(&U);
        IntegrateVV(&U);

        // ================= TRAILS =================
        for(int i = 0; i < U.N; i++){
            AddTrail(&U.p[i], Project(U.p[i].pos, cam, scale)); 
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for(int i = 0; i < U.N; i++){
            Vector2 p = Project(U.p[i].pos, cam, scale);
            DrawCircleV(p, RADIUS, U.p[i].c);
            DrawTrailGlow(&U.p[i], U.p[i].c);
        }

        char text[128];
        sprintf(text, "Kinetic Energy: %.6f", Energy(&U));
        DrawText(text, 20, 20, 20, WHITE);

        EndDrawing();
    
}
    CloseWindow();
    return 0;
}

