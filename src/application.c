#include "../include/application.h"
#include <raylib.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "../external/raygui.h"



const int screenWidth = 1280;
const int screenHeight = 720;

RenderTexture2D target;
Vector2 mousePos;

Application app;
NewLevelConfig newFileConfig;

Color backgroundColors[6]={
  (Color){43,45,49,255},
  (Color){26,21,46,255},
  (Color){60,63,65,255},
  (Color){32,36,56,255},
  (Color){200,204,212,255},
  (Color){30,43,38,255},
};

Color textColors[6] = {
  (Color){230,232,236,255}, // on charcoal
  (Color){222,226,248,255}, // on deep navy
  (Color){238,238,238,255}, // on editor gray
  (Color){226,231,250,255}, // on muted purple/navy
  (Color){28,30,36,255},    // on light gray (dark text)
  (Color){220,238,228,255}, // on forest dark
};



void changeBackgroundColor(){
  if(IsKeyDown(KEY_LEFT_CONTROL)) {
    if(IsKeyPressed(KEY_RIGHT)){
      if(app.colorIndex >= 5) app.colorIndex = 0;
      app.colorIndex++; 
    }
  }
}

void appInit() {
  InitWindow(screenWidth, screenHeight, "SB_Tiles");
  SetTargetFPS(60);

  app.state = NOT_SELECTED;
  app.levelFreed = true; // Initial startup no need to free
  app.colorIndex = GetRandomValue(0, 5);

  target = LoadRenderTexture(1280, 720);

  SBTILES_Init(screenWidth, screenHeight);

}

void appUpdate() {

  changeBackgroundColor();

  switch (app.state) {
  case NOT_SELECTED:

    // Free level if needed
    if(!app.levelFreed){
      SB_Level_Free(&app.level);
      SB_Tileset_Free(&app.tileset);
      app.levelFreed = true;
    }

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
      app.levelFreed = false;
      app.state = NOT_SELECTED;
    } else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
      SB_Level_Save(app.level, app.levelFilePath);
    }

    break;
  case CREATING_NEW_FILE:
    bool newFile = newLevelPrompt(&newFileConfig);

    if(!newFile){
      app.state = NOT_SELECTED;
    }
    else{
      app.level = SB_Level_Init(newFileConfig.layers, newFileConfig.rows, newFileConfig.cols, newFileConfig.tilesetName, newFileConfig.tileSize);
      char assetDir[512];
      snprintf(assetDir, sizeof(assetDir), "%s/../assets", GetDirectoryPath(newFileConfig.path));
      app.tileset = SB_Tileset_Init(assetDir, app.level.tilesetName, app.level.tileSize);

      strcpy(app.levelFilePath, newFileConfig.path);
      strcpy(app.tilesetFilePath, assetDir); 
      
      app.state = EDITING;
    }
  }
}

void appDraw() {
  BeginTextureMode(target);

  ClearBackground(backgroundColors[app.colorIndex]);

  switch (app.state) {
  case NOT_SELECTED:
    DrawText("E - Select File", 400, 300, 50, textColors[app.colorIndex] );
    DrawText("N - New File", 400, 500, 50, textColors[app.colorIndex]);
    break;
  case LOADING_FILE:
    break;
  case EDITING:
    SBTILES_Draw(app.level, app.tileset, 50, 50, textColors[app.colorIndex]);
    break;
  case CREATING_NEW_FILE:
  }

  EndTextureMode();
  SB_GameResolution_Draw((Vector2){screenWidth, screenHeight}, &target,
                         &mousePos);
}

