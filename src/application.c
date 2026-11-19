#include "../include/application.h"
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "../external/raygui.h"



const int screenWidth = 1280;
const int screenHeight = 720;

RenderTexture2D target;
Vector2 mousePos;

Application app;
NewLevelConfig newParam;


/***********************NEW FILE CREATTION********************/



void appInit() {
  InitWindow(screenWidth, screenHeight, "SB_Tiles");
  SetTargetFPS(60);

  app.state = NOT_SELECTED;

  target = LoadRenderTexture(1280, 720);

  SBTILES_Init(screenWidth, screenHeight);

}

void appUpdate() {
  switch (app.state) {
  case NOT_SELECTED:
    if (IsKeyPressed(KEY_N)) {
      app.state = CREATING_NEW_FILE;
    }
    if (IsKeyPressed(KEY_E)) {
      const char *filters[1] = {"*.sblevel"};
      const char *result =
          tinyfd_openFileDialog("Open level", "/home/benja/coding/myGames/", 1,
                                filters, "sbTiles levels", 0);

      if (result) {
        printf("Picked: %s\n", result);
        strcpy(app.levelFilePath, result);
        app.state = LOADING_FILE;

      } else {
        printf("Cancelled\n");
      }
    }

    break;
  case LOADING_FILE:
    app.level = SB_Level_Load(app.levelFilePath);

    char assetDir[512];
    snprintf(assetDir, sizeof(assetDir), "%s/../assets",
             GetDirectoryPath(app.levelFilePath));
    app.tileset =
        SB_Tileset_Init(assetDir, app.level.tilesetName, app.level.tileSize);

    app.state = EDITING;

    break;
  case EDITING:
    SBTILES_Update(&app.level, app.tileset, mousePos);

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
      SB_Level_Save(app.level, app.levelFilePath);
      SB_Level_Free(&app.level);
      SB_Tileset_Free(&app.tileset);
      app.state = NOT_SELECTED;
    } else if (IsKeyPressed(KEY_S)) {
      SB_Level_Save(app.level, app.levelFilePath);
    }

    break;
  case CREATING_NEW_FILE:
    newLevelPrompt(&newParam) ;
  }
}

void appDraw() {
  BeginTextureMode(target);

  ClearBackground(BLACK);

  switch (app.state) {
  case NOT_SELECTED:
    DrawText("E - Select File", 400, 300, 50, WHITE);
    DrawText("N - New File", 400, 500, 50, WHITE);
    break;
  case LOADING_FILE:
    break;
  case EDITING:
    SBTILES_Draw(app.level, app.tileset, 50, 50);
    break;
  case CREATING_NEW_FILE:
  }

  EndTextureMode();
  SB_GameResolution_Draw((Vector2){screenWidth, screenHeight}, &target,
                         &mousePos);
}

