#include "sblib/include/sblib.h"
#include "sbtiles.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

const int screenWidth = 1280;
const int screenHeight = 720;

typedef enum { NOT_SELECTED, BROWSING, SELECTED } Level_Selection_State;

Level_Selection_State state = NOT_SELECTED;

RenderTexture2D target;
Vector2 mousePos;

SB_Level level;
char filePath[256];
char currentDir[256] = "/home/benja/";
char *directoryNames[256];

FilePathList files;

void drawFileSelection() {
  for (int i = 0; i < files.count; i++) {
    DrawText(files.paths[i], 200, i * 150, 20, RED);
  }
}

void update() {
  switch (state) {
  case NOT_SELECTED:
    if (IsKeyPressed(KEY_B)) {
      state = BROWSING;
    }
    break;
  case BROWSING:
    break;
  case SELECTED:
    break;
  }
}

void draw() {
  BeginTextureMode(target);

  ClearBackground(RAYWHITE);

  switch (state) {
  case NOT_SELECTED:
    DrawText("PRESS B to Browse", 50, 500, 60, BLACK);
    break;
  case BROWSING:
    drawFileSelection();
    break;
  case SELECTED:
    break;
  }

  EndTextureMode();
  SB_GameResolution_Draw((Vector2){screenWidth, screenHeight}, &target,
                         &mousePos);
}

void init() {
  InitWindow(screenWidth, screenHeight, "sbTiles");
  SetTargetFPS(60);

  target = LoadRenderTexture(1280, 720);

  files = LoadDirectoryFilesEx(currentDir, " ", false);
}

int main() {

  init();

  while (!WindowShouldClose()) {
    update();
    draw();
  }

  UnloadRenderTexture(target);

  return 0;
}
