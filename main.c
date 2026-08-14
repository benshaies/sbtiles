#include "sblib/include/sblib.h"
#include "sbtiles.h"
#include <raylib.h>

int screenWidth = 1280;
int screenHeight = 720;

RenderTexture2D target;
Vector2 mousePos;

void draw() {
  BeginTextureMode(target);

  DrawRectangle(500, 500, 50, 50, BLUE);

  EndTextureMode();

  SB_GameResolution_Draw((Vector2){screenWidth, screenHeight}, &target,
                         &mousePos);
}

void init() {
  InitWindow(screenWidth, screenHeight, "sbTiles");
  SetTargetFPS(60);
}

int main() {

  init();

  while (!WindowShouldClose()) {

    draw();
  }

  return 0;
}
