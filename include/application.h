#ifndef APPLICATION_H
#define APPLICATION_H
#include "../external/tinyfiledialogs.h";
#include "../sblib/include/sblib.h"
#include "../sbtiles.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  NOT_SELECTED,
  LOADING_FILE,
  EDITING,
  CREATING_NEW_FILE,
} AppState;

typedef struct {
  SB_Level level;
  SB_Tileset tileset;
  char levelFilePath[256];
  char tilesetFilePath[256];
  AppState state;
} Application;

void appInit();

void appUpdate();

void appDraw();

#endif // !APPLICATION_H
