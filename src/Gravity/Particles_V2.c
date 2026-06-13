// this is the upgrade structurally of Particles.c

#include <raylib.h>
#include <stdlib.h>
#include <math.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
// Structures

typedef struct Vector{
    double* Coords;
    int size;
}Vector;

typedef struct Particle{
    double mass;
    Vector* Position;
    Vector* Velocity;
    Vector* Acceleration;
    Vector* total_force;
}Particle;

typedef struct Universe{
     float Time;
     Particle* particles;
     Vector* Forces;
     float dt;
     char* color;
     double G;
     int N; // N is the number of particles  ( 0 indexed )
}Universe;


// ==================== SAFE ALLOCATION HELPERS ====================

Vector* CreateVector(int s){
    Vector* v = malloc(sizeof(Vector));
    v->size = s;
    v->Coords = malloc(sizeof(double) * s);

    for(int i=0;i<s;i++){
        v->Coords[i] = 0;
    }
    return v;
}

void FreeVector(Vector* v){
    if(!v) return;
    free(v->Coords);
    free(v);
}

void FreeParticle(Particle* p){
    if(!p) return;
    FreeVector(p->Position);
    FreeVector(p->Velocity);
    FreeVector(p->Acceleration);
    FreeVector(p->total_force);
}


// ==================== Init =================================

void Init(Vector* v,int s){
    v->size=s;
    for(int i=0;i<s;i++){
        v->Coords[i]=0;
    }
}

void InitParticle(Particle* P,double mass,int s){
  P->mass=mass;

  P->Position = CreateVector(s);
  P->Velocity = CreateVector(s);
  P->Acceleration = CreateVector(s);
  P->total_force = CreateVector(s);
}

void InitUniverse(Universe* u,float t,float dt, char* color,double G,int N){
   u->Time=t;
   u->dt=dt;
   u->color=color;
   u->G=G;
   u->N=N;

   u->particles = malloc(sizeof(Particle) * N);
}


//================== Vector Functions ===========================

void Negative(Vector* v,Vector* v1){
    int size=v->size;
    for(int i=0;i<size;i++){
        v1->Coords[i]=-v->Coords[i];
    }
}

double Modulus(Vector* v){
    int size=v->size;
    double s=0;
    for(int i=0;i<size;i++){
        double d = v->Coords[i];
        s+= d*d;
    }
    return sqrt(s);
}

void Sum(Vector* v,Vector* v1,Vector* sum){
    int size=v->size;
    for(int i=0;i<size;i++){
        sum->Coords[i]=v->Coords[i]+v1->Coords[i];
    }
}



//====================== Force Functions ==========================

void CalculateForce(Particle* p1,Particle* p2,int o1,int o2,Vector* F,Universe* u){

    Vector temp;
    temp.size = p1->Position->size;
    temp.Coords = malloc(sizeof(double) * temp.size);

    Negative(p2->Position,&temp);
    Sum(p1->Position,&temp,&temp);

    double r=Modulus(&temp);

    double norm=(u->G*p1->mass*p2->mass)/(r*r*r + 1e-5);

    if(o1<o2){
        for(int i=0;i<F->size;i++){
            F->Coords[i]=norm*temp.Coords[i];
        }
    }
    if(o2<o1){
        for(int i=0;i<F->size;i++){
            F->Coords[i]=-norm*temp.Coords[i];
        }
    }

    free(temp.Coords);
}


void ForcePerParticle(Particle* p,Universe* u,int order){

    int n=u->N;

    Vector sum_local;
    sum_local.size = p->Position->size;
    sum_local.Coords = calloc(sum_local.size, sizeof(double));

    Vector F_local;
    F_local.size = p->Position->size;
    F_local.Coords = calloc(F_local.size, sizeof(double));

    for(int i=0;i<n;i++){
        if(i==order){
            continue;
        }
        else{
            CalculateForce(p, &u->particles[i], order, i, &F_local, u);
            Sum(&sum_local, &F_local, &sum_local);
        }
    }

    *(p->total_force) = sum_local;

    free(F_local.Coords);
}

double TotalKinetic(Universe* u){

    double k = 0;
    int n = u->N;

    for(int i = 0; i < n; i++){

        double m = u->particles[i].mass;
        double v = Modulus(u->particles[i].Velocity);

        k += 0.5 * m * v * v;
    }

    return k;
}
