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

typedef enum { NOT_SELECTED, BROWSING, SELECTED } Level_Selection_State;

Level_Selection_State state = NOT_SELECTED;

RenderTexture2D target;
Vector2 mousePos;

SB_Level level;
char filePath[256];
char currentDir[256] = "/home/benja/MyGames";

typedef struct {
  bool firstStart;
  char *names[256];
  int arraySize;
  bool showHiddenDirectories;
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
    for (int i = 0; i < directoryNames.arraySize; i++) {
      free(directoryNames.names[i]);
      directoryNames.names[i] = NULL;
      directoryNames.arraySize = -1;
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

      directoryNames.arraySize++;
      int fileLenght = strlen(filesName);
      directoryNames.names[directoryNames.arraySize] =
          malloc(fileLenght * sizeof(char));
      strcpy(directoryNames.names[directoryNames.arraySize], filesName);

    } else if () {

      directoryNames.arraySize++;
      int fileLenght = strlen(filesName);
      directoryNames.names[directoryNames.arraySize] =
          malloc(fileLenght * sizeof(char));
      strcpy(directoryNames.names[directoryNames.arraySize], filesName);
    }
  }
}

void drawFileSelection() {
  for (int i = 0; i < directoryNames.arraySize; i++) {

    if (directoryNames.highlightedDir == i) {
      DrawRectangle(270, i * 25 + 75, 25, 25, BLUE);
    }
    DrawText(directoryNames.names[i], 300, i * 25 + 75, 20, BLACK);
  }
}

void updateFileSelection() {

  // Scroll Down
  if (IsKeyPressed(KEY_J)) {
    if (directoryNames.highlightedDir == directoryNames.arraySize) {
      directoryNames.highlightedDir = 1;
    } else {
      directoryNames.highlightedDir++;
    }
  }

  // Scroll Up
  if (IsKeyPressed(KEY_K)) {
    if (directoryNames.highlightedDir == 0) {
      directoryNames.highlightedDir = directoryNames.arraySize - 1;
    } else {
      directoryNames.highlightedDir--;
    }
  }

  // Change Directory
  if (IsKeyPressed(KEY_L)) {
    strcat(currentDir, "/");
    strcat(currentDir, directoryNames.names[directoryNames.highlightedDir]);
    reloadDirectory(currentDir);
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
    if (IsKeyPressed(KEY_S)) {
      directoryNames.showHiddenDirectories =
          !directoryNames.showHiddenDirectories;
    }
    updateFileSelection();
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

  directoryNames.arraySize = -1;
  directoryNames.firstStart = true;
  directoryNames.showHiddenDirectories = false;
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
