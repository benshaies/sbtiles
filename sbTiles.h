#ifndef SBTILES_H
#define SBTILES_H

#include "../include/sblib.h"
#include <raylib.h>

typedef enum {
  TILE_SELECTION,
  LEVEL_EDITING,
} SBT_STATE;

typedef struct {
  int currentLayer;
  Vector2 currentDrawSize;
  Camera2D cam;

  bool showTileLines;
  bool showAllLayers;

  int selectedTile;

  SBT_STATE currentState;

  Rectangle tileSelectionRec;

} SBTILES;

// Initialize sbTiles struct
void SBTILES_Init(int windowWidth, int windowHeight);

// All main updates to sbTiles
void SBTILES_Update(SB_Level *currentLevel, Vector2 mousePos);

// All drawing to sbTiles
void SBTILES_Draw(SB_Level currentLevel, int drawTileWidth, int drawTileHeight);

#endif
