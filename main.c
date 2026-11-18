#include "sblib/include/sblib.h"
#include "sbtiles.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"
#include "external/tinyfiledialogs.h"

const int screenWidth = 1280;
const int screenHeight = 720;

typedef enum {
  NOT_SELECTED,
  LOADING_FILE,
  EDITING,
  CREATING_NEW_FILE,
} AppState;

AppState state = NOT_SELECTED;

RenderTexture2D target;
Vector2 mousePos;

SB_Level level;
char levelFilePath[256];
char tilesetFilePath[256];
SB_Tileset tileset;

typedef struct {
  const char *text;
  bool active;
  float timer;
  float timeBase;
  Vector2 pos;
  float size;
} Notification;

Notification testing;

Notification createNotification(const char *text, float timeBase, float size) {
  Notification noti;

  noti.text = text;
  noti.timeBase = timeBase;
  noti.timer = timeBase;
  noti.active = true;
  noti.size = size;

  return noti;
}

void updateNotification(Notification *noti) {
  if (noti->active) {
    noti->timer -= GetFrameTime();

    if (noti->timer <= 0) {
      noti->active = false;
      noti->timer = noti->timeBase;
    }
  }
}

void displayNotification(Notification noti) {
  if (noti.active) {
    DrawText(noti.text, noti.pos.x, noti.pos.y, noti.size, MAROON);
  }
}

void update() {

  switch (state) {
  case NOT_SELECTED:
    if (IsKeyPressed(KEY_N)) {
      state = CREATING_NEW_FILE;
    }

    if (IsKeyPressed(KEY_E)) {
      const char *filters[1] = {"*.sblevel"};
      const char *result =
          tinyfd_openFileDialog("Open level", "/home/benja/coding/myGames/", 1,
                                filters, "sbTiles levels", 0);

      if (result) {
        printf("Picked: %s\n", result);
        strcpy(levelFilePath, result);
        state = LOADING_FILE;

      } else {
        printf("Cancelled\n");
      }
    }

    break;
  case LOADING_FILE:
    level = SB_Level_Load(levelFilePath);

    char assetDir[512];
    snprintf(assetDir, sizeof(assetDir), "%s/../assets", GetDirectoryPath(levelFilePath));
    tileset = SB_Tileset_Init(assetDir, level.tilesetName, level.tileSize);

    state = EDITING;

    break;
  case EDITING:
    SBTILES_Update(&level, tileset, mousePos);

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
      SB_Level_Save(level, levelFilePath);
      SB_Level_Free(&level);
      SB_Tileset_Free(&tileset);
      state = NOT_SELECTED;
    } else if (IsKeyPressed(KEY_S)) {
      SB_Level_Save(level, levelFilePath);
    }

    break;
  case CREATING_NEW_FILE:
  }
}

void draw() {
  BeginTextureMode(target);

  ClearBackground(BLACK);

  switch (state) {
  case NOT_SELECTED:
    DrawText("E - Select File", 400, 300, 50, WHITE);
    DrawText("N - New File", 400, 500, 50, WHITE);
    break;
  case LOADING_FILE:
    break;
  case EDITING:
    SBTILES_Draw(level, tileset, 50, 50);
    break;
  case CREATING_NEW_FILE:
  }

  EndTextureMode();
  SB_GameResolution_Draw((Vector2){screenWidth, screenHeight}, &target,
                         &mousePos);
}
void init() {
  InitWindow(screenWidth, screenHeight, "sbTiles");
  SetTargetFPS(60);

  target = LoadRenderTexture(1280, 720);

  SBTILES_Init(screenWidth, screenHeight);
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
