#include "sblib/include/sblib.h"
#include "sbtiles.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

const int screenWidth = 1280;
const int screenHeight = 720;

typedef enum {
  NOT_SELECTED,
  BROWSING,
  LOADING_FILE,
  EDITING,
  CREATING_NEW_FILE,
} Level_Selection_State;

Level_Selection_State state = NOT_SELECTED;

RenderTexture2D target;
Vector2 mousePos;

SB_Level level;
char levelFilePath[256];
char tilesetFilePath[256];
char currentDir[256] = "/home/benja/coding/myGames";
char currentDirReset[256] = "/home/benja/coding/myGames";

SB_Tileset tileset;



typedef struct {
  bool firstStart;
  char *names[256];
  int lastIndex;
  int highlightedDir;
} DirectoryFolderNames;

DirectoryFolderNames directoryNames;

FilePathList files;

char newFileDirectory[250];
char newFileName[256];
bool selectingNewFileDir = true;
int layers, tileSize, rows, cols;
char tilesetName[128];
int editing = 0;

void reloadDirectory(const char *directory) {

  // Unload and reload files
  UnloadDirectoryFiles(files);
  files = LoadDirectoryFiles(directory);

  // free and reset names array
  if (!directoryNames.firstStart) {
    for (int i = 0; i < directoryNames.lastIndex; i++) {
      free(directoryNames.names[i]);
      directoryNames.names[i] = NULL;
      directoryNames.lastIndex = -1;
      directoryNames.highlightedDir = 0;
    }
  } else {
    directoryNames.firstStart = false;
  }

  // Loop through current directory and find out which ones to save
  for (int i = 0; i < files.count; i++) {
    const char *filesName = GetFileName(files.paths[i]);
    if (DirectoryExists(files.paths[i])) { // Is it a directory?

      if (filesName[0] == '.') { // Check if its a .dot directory if so skip
        continue;
      }

      directoryNames.lastIndex++;
      int fileLenght = strlen(filesName);
      directoryNames.names[directoryNames.lastIndex] =
          malloc(fileLenght * sizeof(char));
      strcpy(directoryNames.names[directoryNames.lastIndex], filesName);

    } else if (IsFileExtension(filesName, ".sblevel")) {

      directoryNames.lastIndex++;
      int fileLenght = strlen(filesName);
      directoryNames.names[directoryNames.lastIndex] =
          malloc(fileLenght * sizeof(char));
      strcpy(directoryNames.names[directoryNames.lastIndex], filesName);
    }
  }
}

void drawFileSelection() {
  for (int i = 0; i < directoryNames.lastIndex + 1; i++) {

    DrawRectangle(0, 0, 200, 200, BLACK);
    DrawText("j - down", 10, 10, 25, WHITE);
    DrawText("k - up", 10, 50, 25, WHITE);
    DrawText("l - select", 10, 90, 25, WHITE);
    DrawText("b - back", 10, 130, 25, WHITE);

    bool highlight = false;
    if (directoryNames.highlightedDir == i) {
      highlight = true;
    }

    DrawText(directoryNames.names[i], 300, i * 25 + 75, 20,
             highlight ? YELLOW : WHITE);
  }
}

// Returns
// 1 - .sblevel file found
// 2 - New file directory selected
int updateFileSelection(bool creatingFile) {

  // Scroll Down
  if (IsKeyPressed(KEY_J)) {
    if (directoryNames.highlightedDir == directoryNames.lastIndex) {
      directoryNames.highlightedDir = 0;
    } else {
      directoryNames.highlightedDir++;
    }
  }

  // Scroll Up
  if (IsKeyPressed(KEY_K)) {
    if (directoryNames.highlightedDir == 0) {
      directoryNames.highlightedDir = directoryNames.lastIndex;
    } else {
      directoryNames.highlightedDir--;
    }
  }

  // Going into a directory
  if (IsKeyPressed(KEY_L)) {
    char temp[256];
    strcpy(temp, currentDir);
    strcat(temp, directoryNames.names[directoryNames.highlightedDir]);

    if (DirectoryExists(temp)) {
      strcat(currentDir, directoryNames.names[directoryNames.highlightedDir]);
      strcat(currentDir, "/");
      reloadDirectory(currentDir);
    }
  }

  if (IsKeyPressed(KEY_ENTER)) {
    char temp[256];
    strcpy(temp, currentDir);
    strcat(temp, directoryNames.names[directoryNames.highlightedDir]);

    // Check if its a file selection
    if (!DirectoryExists(temp)) {
      if (IsFileExtension(directoryNames.names[directoryNames.highlightedDir],
                          ".sblevel")) {
        printf("FOUND .sblevel FILE!\n");
        strcpy(levelFilePath, temp);
        return 1;
      }
    } else if (creatingFile) {
      strcat(currentDir, directoryNames.names[directoryNames.highlightedDir]);
      strcat(currentDir, "/");
      strcpy(newFileDirectory, currentDir);
      reloadDirectory(currentDir);
      return 2;
    }
  }

  if (IsKeyPressed(KEY_B)) {
    int len = strlen(currentDir);
    int lastSlashIndex;

    for (int i = 0; i < len - 1; i++) {
      if (currentDir[i] == '/') {
        lastSlashIndex = i;
      }
    }

    if (lastSlashIndex != 0) {
      currentDir[lastSlashIndex + 1] = '\0';
      reloadDirectory(currentDir);
    } else {
      printf("LAST DIRECTORY\n");
    }
  }

  return 0;
}

int creatingNewFileUpdate() {
  if (selectingNewFileDir) {
    if (updateFileSelection(true) == 2) {
      selectingNewFileDir = false;
    }
  }
}

void drawNewFileCreation() {

  int oldTextSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);        // bigger text, restored at the end

    const float labelX = 120, labelW = 440;
    const float boxX   = 580, boxW   = 580;
    const float boxH   = 64;
    const float startY = 50;
    const float gap    = 84;                    // distance between rows

    GuiLabel((Rectangle){labelX, startY + gap * 0, labelW, boxH}, "LEVEL_NAME");
    if (GuiTextBox((Rectangle){boxX, startY + gap * 0, boxW, boxH}, newFileName, sizeof(newFileName), editing == 6)) editing = (editing == 6) ? 0 : 6;

    GuiLabel((Rectangle){labelX, startY + gap * 1, labelW, boxH}, "LAYER_COUNT");
    if (GuiValueBox((Rectangle){boxX, startY + gap * 1, boxW, boxH}, NULL, &layers, 1, 16, editing == 1)) editing = (editing == 1) ? 0 : 1;

    GuiLabel((Rectangle){labelX, startY + gap * 2, labelW, boxH}, "TILESET_FILENAME");
    if (GuiTextBox((Rectangle){boxX, startY + gap * 2, boxW, boxH}, tilesetName, sizeof(tilesetName), editing == 2)) editing = (editing == 2) ? 0 : 2;

    GuiLabel((Rectangle){labelX, startY + gap * 3, labelW, boxH}, "TILESET_TILESIZE");
    if (GuiValueBox((Rectangle){boxX, startY + gap * 3, boxW, boxH}, NULL, &tileSize, 1, 256, editing == 3)) editing = (editing == 3) ? 0 : 3;

    GuiLabel((Rectangle){labelX, startY + gap * 4, labelW, boxH}, "ARRAY_ROWS");
    if (GuiValueBox((Rectangle){boxX, startY + gap * 4, boxW, boxH}, NULL, &rows, 1, 1000, editing == 4)) editing = (editing == 4) ? 0 : 4;

    GuiLabel((Rectangle){labelX, startY + gap * 5, labelW, boxH}, "ARRAY_COLS");
    if (GuiValueBox((Rectangle){boxX, startY + gap * 5, boxW, boxH}, NULL, &cols, 1, 1000, editing == 5)) editing = (editing == 5) ? 0 : 5;

    bool createClicked = GuiButton((Rectangle){labelX, startY + gap * 6 + 10, 1040, 80}, "Create");

    GuiSetStyle(DEFAULT, TEXT_SIZE, oldTextSize);
 
  if(IsFileExtension(newFileName, ".sblevel") && createClicked){
    strcat(newFileDirectory, newFileName);
    strcpy(levelFilePath, newFileDirectory);
    
    level = SB_Level_Init(layers, rows, cols, tilesetName, tileSize);
    SB_Level_Save(level, newFileDirectory);

    char assetDir[512];
    const char *lastSlash = strrchr(levelFilePath, '/');

    if (lastSlash) {
      int dirLen = (int)(lastSlash - levelFilePath);
      snprintf(assetDir, sizeof(assetDir), "%.*s/../assets", dirLen,
               levelFilePath);
    } else {
      snprintf(assetDir, sizeof(assetDir), "../assets");
    }

    tileset = SB_Tileset_Init(assetDir, level.tilesetName, level.tileSize);


    state = EDITING;
    

  }
}

void update() {
  switch (state) {
  case NOT_SELECTED:
    if (IsKeyPressed(KEY_B)) {

      state = BROWSING;
    } else if (IsKeyPressed(KEY_N)) {
      state = CREATING_NEW_FILE;
    }

    break;
  case BROWSING:
    int i = updateFileSelection(false);

    if (i == 1) {
      state = LOADING_FILE;
      printf(levelFilePath);
      printf("\n");
    }
    break;
  case LOADING_FILE:
    level = SB_Level_Load(levelFilePath);

    char assetDir[512];
    const char *lastSlash = strrchr(levelFilePath, '/');

    if (lastSlash) {
      int dirLen = (int)(lastSlash - levelFilePath);
      snprintf(assetDir, sizeof(assetDir), "%.*s/../assets", dirLen,
               levelFilePath);
    } else {
      snprintf(assetDir, sizeof(assetDir), "../assets");
    }

    tileset = SB_Tileset_Init(assetDir, level.tilesetName, level.tileSize);

    state = EDITING;

    break;
  case EDITING:
    SBTILES_Update(&level, tileset, mousePos);

    if (IsKeyDown(KEY_LEFT_CONTROL)) {

      if (IsKeyPressed(KEY_C)) {
        SB_Level_Save(level, levelFilePath);
        SB_Level_Free(&level);

        directoryNames.lastIndex = -1;
        directoryNames.firstStart = true;
        directoryNames.highlightedDir = 0;

        memset(currentDir, 0, sizeof(currentDir));
        strcpy(currentDir, currentDirReset);

        reloadDirectory(currentDir);

        state = NOT_SELECTED;
      }
      else if(IsKeyPressed(KEY_S)){
        SB_Level_Save(level, levelFilePath);
      }
    }
    break;
  case CREATING_NEW_FILE:
    creatingNewFileUpdate();
  }
}

void draw() {
  BeginTextureMode(target);

  ClearBackground(BLACK);

  switch (state) {
  case NOT_SELECTED:
    DrawText("B - Select File", 400, 300, 50, WHITE);
    DrawText("N - New File", 400, 500, 50, WHITE);
    break;
  case BROWSING:
    drawFileSelection();
    break;
  case LOADING_FILE:
    break;
  case EDITING:
    SBTILES_Draw(level, tileset, 50, 50);
    break;
  case CREATING_NEW_FILE:
    if (selectingNewFileDir) {
      drawFileSelection();
    } else {
      drawNewFileCreation();
    }
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

  directoryNames.lastIndex = -1;
  directoryNames.firstStart = true;
  directoryNames.highlightedDir = 0;

  reloadDirectory(currentDir);

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
