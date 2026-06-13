#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION // Required to define the GUI functions          // Include raygui after implementation
#include "raygui.h"
#define G 6.6743 * pow(10,-11)
#define dt 5

typedef struct particle{
    double mass;
    double x;
    double y;
    double z;
    double velocity[3];
    double accel[3];
}Particle;

typedef struct Force{
    double x;
    double y;
    double z;
}Force;
Force f1_total, f2_total, f3_total,f4_total;
// ---------------- YOUR ORIGINAL FUNCTIONS (UNCHANGED) ----------------

void advanceTimer(double* T){
    *T=*T + dt;
}

void updatePosition(Particle* p){
    p->x += p->velocity[0] * dt;
    p->y += p->velocity[1] * dt;
    p->z += p->velocity[2] * dt;
}

void updateVelocity(Particle* p){
    p->velocity[0] += p->accel[0] * dt;
    p->velocity[1] += p->accel[1] * dt;
    p->velocity[2] += p->accel[2] * dt;
}

void setAccel(Particle* p, Force* F){
    p->accel[0] = (F->x) / p->mass;
    p->accel[1] = (F->y) / p->mass;
    p->accel[2] = (F->z) / p->mass;
}
// make so the screen center is the center of gravity
Vector2 projectCOM(Particle* p, Particle* p1, Particle* p2, double scale, int w, int h)
{
    double m1 = p1->mass;
    double m2 = p2->mass;

    double comx = (m1*p1->x + m2*p2->x) / (m1 + m2);
    double comy = (m1*p1->y + m2*p2->y) / (m1 + m2);

    Vector2 v;
    v.x = w/2 + (p->x - comx) * scale;
    v.y = h/2 + (p->y - comy) * scale;

    return v;
}
void initialize(Particle* p, double* pos, double* speed, double* accel, double mass){
    p->x = pos[0];
    p->y = pos[1];
    p->z = pos[2];
    p->mass = mass;

    for(int i = 0; i < 3; i++){
        p->velocity[i] = speed[i];
        p->accel[i] = accel[i];
    }
}

double Distance(Particle* p1, Particle* p2){
    double s = 0;
    s += pow((p1->x - p2->x), 2);
    s += pow((p1->y - p2->y), 2);
    s += pow((p1->z - p2->z), 2);
    return sqrt(s);
}
Vector2 getCOM3(Particle* p1, Particle* p2, Particle* p3)
{
    double m1 = p1->mass;
    double m2 = p2->mass;
    double m3 = p3->mass;

    double total = m1 + m2 + m3;

    Vector2 com;
    com.x = (m1*p1->x + m2*p2->x + m3*p3->x) / total;
    com.y = (m1*p1->y + m2*p2->y + m3*p3->y) / total;

    return com;
}
double computeScale(Particle* p1, Particle* p2, Particle* p3, double screenW, double screenH)
{
    Vector2 com = getCOM3(p1, p2, p3);

    double d1 = hypot(p1->x - com.x, p1->y - com.y);
    double d2 = hypot(p2->x - com.x, p2->y - com.y);
    double d3 = hypot(p3->x - com.x, p3->y - com.y);

    double maxD = fmax(d1, fmax(d2, d3));

    if (maxD < 1e-6) return 50.0;

    // keep some margin so objects don't touch edges
    double margin = 0.4;

    double scaleX = (screenW * margin) / maxD;
    double scaleY = (screenH * margin) / maxD;

    return fmin(scaleX, scaleY);
}
Vector2 projectCentered(Particle* p, Vector2 com, double scale, int w, int h)
{
    Vector2 v;

    v.x = w/2 + (p->x - com.x) * scale;
    v.y = h/2 + (p->y - com.y) * scale;

    return v;
}
void ModifyForce(Force* f, Particle* p1, Particle* p2){
    double r = Distance(p1, p2);

    f->x = (G * p1->mass * p2->mass * (p2->x - p1->x)) / (pow(r,3)+1e-2 );
    f->y = (G * p1->mass * p2->mass * (p2->y - p1->y)) / (pow(r,3)+1e-2);
    f->z = (G * p1->mass * p2->mass * (p2->z - p1->z)) / (pow(r,3)+1e-2);
}

void stable_orbit(Particle* p1,Particle* p2,int c){
    double omega=sqrt((G*(p1->mass+p2->mass))/(pow(Distance(p1,p2),3)));
    double dy = p2->y - p1->y;
    double dx = p2->x - p1->x;
    double m1=p1->mass;
    double m2=p2->mass;
    p1->velocity[0] = -omega * (m2 / (m1 + m2)) * dy + c;
    p1->velocity[1] =  omega * (m2 / (m1 + m2)) * dx ;
    p1->velocity[2] = 0;
    p2->velocity[0] =  omega * (m1 / (m1 + m2)) * dy + c;
    p2->velocity[1] = -omega * (m1 / (m1 + m2)) * dx ;
    p2->velocity[2] = 0;
}

void updateState(Particle* p, Force* f){
    setAccel(p, f);
    updateVelocity(p);
    updatePosition(p);
}


// ---------------- VISUAL HELPERS ----------------

Vector2 project(Particle* p, double scale, int screenW, int screenH){
    Vector2 v;
    v.x = screenW/2 + p->x * scale;
    v.y = screenH/2 + p->y * scale;
    return v;
}
void CheckCollision(Particle* p1, Particle* p2){
     
        double dx = p2->x - p1->x;
        double dy = p2->y - p1->y;
        double d = sqrt(dx*dx + dy*dy);

    if(d>1e-2){return;}
    else{
        dx /= d;
        dy /= d;

        double v1n = p1->velocity[0]*dx + p1->velocity[1]*dy;
        double v2n = p2->velocity[0]*dx + p2->velocity[1]*dy;

        // simple elastic swap (stable minimal model)
        double tmp = v1n;
        v1n = v2n;
        v2n = tmp;

        p1->velocity[0] = (v1n+5) * dx;
        p1->velocity[1] = v1n * dy;

        p2->velocity[0] = (v2n+5) * dx;
        p2->velocity[1] = v2n * dy;
    }
    
}
/* ---------------- MAIN ---------------- */

int main(){
    float sliderValue = 0.0f;
    float sliderMin = -5.0f;
    float sliderMax = 5.0f;
    Rectangle sliderBounds = { 20, 400, 200, 20 };

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "3-Body Gravity Simulation");
    SetTargetFPS(60);

    Particle* p1 = malloc(sizeof(Particle));
    Particle* p2 = malloc(sizeof(Particle));
    Particle* p3 = malloc(sizeof(Particle));   // NEW
    Particle* p4 = malloc(sizeof(Particle));   // NEW
    
    double accel1[] = {0,0,0};
    double accel2[] = {0,0,0};
    double accel3[] = {0,0,0};                 // NEW
    double accel4[] = {0,0,0};                 // NEW

    double mass1 = 1e12;
    double mass2 = 1e12;
    double mass3 = 1e12;
    double mass4= 1e4;                        // NEW

    double pos1[] = {5,1,0};
    double pos2[] = {-4,3,0};
    double pos3[] = {2,-3,0};                  // NEW
    double pos4[] = {6,-2,0};                  // NEW
    
    double speed1[] = {2,1,0};
    double speed2[] = {-1,1,0};
    double speed3[] = {1,2,0};
    double speed4[] = {1,2,0};                 // NEW

    initialize(p1, pos1, speed1, accel1, mass1);
    initialize(p2, pos2, speed2, accel2, mass2);
    initialize(p3, pos3, speed3, accel3, mass3); // NEW
    initialize(p4, pos4, speed4, accel4, mass4);

    double t = 0;
    
    double scale = 20.0f;

    while (!WindowShouldClose()){
    // reset forces each frame
        f1_total = (Force){0,0,0};
        f2_total = (Force){0,0,0};
        f3_total = (Force){0,0,0};
        f4_total = (Force){0,0,0};
        // compute pairwise forces
        Force f12, f13,f14,f24, f23,f34;
        
        ModifyForce(&f12, p1, p2);
        ModifyForce(&f13, p1, p3);
        ModifyForce(&f23, p2, p3);
        
        ModifyForce(&f14, p1, p4);
        ModifyForce(&f34, p3, p4);
        ModifyForce(&f24, p2, p4);
        
        
        // Newton 3rd law (correct + no malloc)
        f1_total.x += f12.x + f13.x + f14.x;
        f1_total.y += f12.y + f13.y + f14.y;
        
        f2_total.x += -f12.x + f23.x + f24.x;
        f2_total.y += -f12.y + f23.y + f24.y;
        
        f3_total.x += -f13.x + -f23.x + f34.x;
        f3_total.y += -f13.y + -f23.y + f34.y;

        f4_total.x += -f14.x + -f24.x + -f34.x;
        f4_total.y += -f14.y + -f24.y + -f34.y;
    

        advanceTimer(&t);

        GuiSlider(sliderBounds, "Force", "", &sliderValue, sliderMin, sliderMax);
        updateState(p1, &f1_total);
        updateState(p2, &f2_total);
        updateState(p3, &f3_total);   // NEW (approx state update)
        updateState(p4, &f4_total);
        BeginDrawing();
        ClearBackground(BLACK);
        Vector2 com = getCOM3(p1, p2, p3);
        double scale = computeScale(p1, p2, p3, screenWidth, screenHeight);
        
        Vector2 r1 = projectCentered(p1, com, scale, screenWidth, screenHeight);
        Vector2 r2 = projectCentered(p2, com, scale, screenWidth, screenHeight);
        Vector2 r3 = projectCentered(p3, com, scale, screenWidth, screenHeight);
        Vector2 r4 = projectCentered(p4, com, scale, screenWidth, screenHeight);
        
        DrawCircle(r1.x, r1.y, 6, RED);
        DrawCircle(r2.x, r2.y, 6, BLUE);
        DrawCircle(r3.x, r3.y, 6, GREEN);
        DrawCircle(r4.x, r4.y, 6, YELLOW);   // NEW

        DrawLine(r1.x, r1.y, r2.x, r2.y, DARKGRAY);
        DrawLine(r1.x, r1.y, r3.x, r3.y, DARKGRAY);
        DrawLine(r2.x, r2.y, r3.x, r3.y, DARKGRAY);
        DrawLine(r1.x, r1.y, r4.x, r4.y, DARKGRAY);
        DrawLine(r4.x, r4.y, r3.x, r3.y, DARKGRAY);
        DrawLine(r2.x, r2.y, r4.x, r4.y, DARKGRAY);
        
        DrawText("4-Body Gravity Simulation", 20, 20, 20, WHITE);
        
        CheckCollision(p1,p2);
        CheckCollision(p2,p3);
        CheckCollision(p1,p3);
        CheckCollision(p1,p4);
        CheckCollision(p2,p4);
        CheckCollision(p4,p3);
        
        EndDrawing();
    }  

    CloseWindow();
    return 0;
}
