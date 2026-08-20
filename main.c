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
  EDITING
} Level_Selection_State;

Level_Selection_State state = NOT_SELECTED;

RenderTexture2D target;
Vector2 mousePos;

SB_Level level;
char levelFilePath[256];
char tilesetFilePath[256];
char currentDir[256] = "/home/benja/";

typedef struct {
  bool firstStart;
  char *names[256];
  int lastIndex;
  int highlightedDir;
} DirectoryFolderNames;

DirectoryFolderNames directoryNames;

FilePathList files;

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

// Returns 1 if .sblevel file is selected and found otherwise returns 0
int updateFileSelection() {

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

    // Check if its a file selection
    if (!DirectoryExists(temp)) {
      printf("FOUND .sblevel FILE!\n");
      strcpy(levelFilePath, temp);
      return 1;

    } else {
      strcat(currentDir, directoryNames.names[directoryNames.highlightedDir]);
      strcat(currentDir, "/");
      reloadDirectory(currentDir);
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

void update() {
  switch (state) {
  case NOT_SELECTED:
    if (IsKeyPressed(KEY_B)) {
      state = BROWSING;
    }
    break;
  case BROWSING:
    int i = updateFileSelection();

    if (i == 1) {
      state = LOADING_FILE;
      printf(levelFilePath);
      printf("\n");
    }
    break;
  case LOADING_FILE:
    level = SB_Level_Load(levelFilePath, true);
    char oneUp[256];
    strncpy(oneUp, GetDirectoryPath(levelFilePath), sizeof(oneUp) - 1);
    oneUp[sizeof(oneUp) - 1] = '\0';

    char twoUp[256];
    strncpy(twoUp, GetDirectoryPath(oneUp), sizeof(twoUp) - 1);
    twoUp[sizeof(twoUp) - 1] = '\0';
    strcpy(tilesetFilePath, twoUp);

    strcat(tilesetFilePath, "/assets/");
    printf("CURRENT PATH:");
    printf(level.tileset.fileName);
    printf("\n");
    strcat(tilesetFilePath, level.tileset.fileName);

    level.tileset.texture = LoadTexture(tilesetFilePath);
    level.tileset.cols = level.tileset.texture.width / level.tileset.tileSize;
    level.tileset.rows = level.tileset.texture.height / level.tileset.tileSize;

    state = EDITING;

    break;
  }
}

void draw() {
  BeginTextureMode(target);

  ClearBackground(GRAY);

  switch (state) {
  case NOT_SELECTED:
    DrawText("Press B to browse", 300, 400, 50, WHITE);
    DrawText("Press N to make a new level", 300, 500, 50, WHITE);
    break;
  case BROWSING:
    drawFileSelection();
    break;
  case LOADING_FILE:
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
