/*******************************************************************************************
*
*   raylib [models] example - rotating cube
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   Example originally created with raylib 6.0, last time updated with raylib 6.0
*
*   Example contributed by Jopestpe (@jopestpe)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2025 Jopestpe (@jopestpe)
*
********************************************************************************************/
#include <stdlib.h>
#include "raylib.h"
#include <stdio.h>
#include "rlgl.h"
typedef struct Cube{
    Vector3 position;
    Vector3 size;
    Color color[3];
}Cube;

typedef struct RubixCube{
    Cube cube[27];
    int size; 
}Rubix;
float Gety(int index,int s){
    float possible[3];
    possible[0]=(((float)s)/3)*0.5;
    possible[1]=possible[0]+((float)s)/3;
    possible[2]=possible[1]+((float)s)/3;
    if( 0 <= ( index%9 ) && ( index%9 ) < 3){
        return possible[0];
    }
    else if( 3 <= ( index%9 ) && ( index%9 ) < 6){
        return possible[1];
    }
    else {
        return possible[2];
    }
}
float Getx(int index,int s){
    float possible[3];
    possible[0]=(((float)s)/3)*0.5;
    possible[1]=possible[0]+((float)s)/3;
    possible[2]=possible[1]+((float)s)/3;
    return possible[index%3];
}
float Getz(int index,int s){
    float possible[3];
    possible[0]=(((float)s)/3)*0.5;
    possible[1]=possible[0]+((float)s)/3;
    possible[2]=possible[1]+((float)s)/3;
    if( 0 <= ( index%27 ) && ( index%27 ) < 9){
        return possible[0];
    }
    else if( 9 <= ( index%27 ) && ( index%27 ) < 18){
        return possible[1];
    }
    else {
        return possible[2];
    }
}

Vector3 map_to_pos(int index,int s){
      float xpos=Getx(index,s); 
      float ypos=Gety(index,s);
      float zpos=Getz(index,s);
      return (Vector3){xpos,ypos,zpos};
}
void initRandom(Cube* c,int size,int index,Color col){
    c->position=map_to_pos(index,size);
    c->size=(Vector3){size/3,size/3,size/3};
    for(int i=0;i<2;i++){
        c->color[i]=col;
    }
    
}
void initRubixCube(Rubix* R,int size){
    Color RColors[6]={GREEN,RED,WHITE,ORANGE,YELLOW,BLUE};
    int colorindex;
    for(int i=0;i<27;i++){
        colorindex=rand()%6;
        initRandom(&R->cube[i],size,i,RColors[colorindex]);
    }
}
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
     Rubix* R=malloc(sizeof(Rubix));
     initRubixCube(R,20);
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - rotating cube");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 100.0f, 100.0f, 100.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;   
    camera.projection = CAMERA_PERSPECTIVE;

  

    float rotation = 0.0f;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        rotation += 1.0f;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            for(int i=0;i<27;i++){
            rlPushMatrix()  ;  
            rlRotatef(rotation, 0, 1, 0); // rotate around Y axis
               DrawCube(R->cube[i].position, R->cube[i].size.x, R->cube[i].size.y, R->cube[i].size.z, R->cube[i].color[0]);
               DrawCubeWires(R->cube[i].position, R->cube[i].size.x, R->cube[i].size.y, R->cube[i].size.z, R->cube[i].color[0]);
               DrawGrid(10, 1.0f);
             rlPopMatrix();
            }                   
            EndMode3D();

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
