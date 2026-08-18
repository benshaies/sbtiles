#include "sbtiles.h"
#include <raylib.h>
#include <stdio.h>

SBTILES sbt;

int scroll;
Vector2 worldMouse;

void SBTILES_Init(int windowWidth, int windowHeight) {
  sbt.currentDrawSize = (Vector2){0, 0};
  sbt.currentLayer = 0;
  sbt.selectedTile = 30;

  sbt.showTileLines = true;
  sbt.showAllLayers = false;

  sbt.cam.target = (Vector2){windowWidth / 2, windowHeight / 2};
  sbt.cam.offset = sbt.cam.target;
  sbt.cam.zoom = 1.0f;

  sbt.currentState = LEVEL_EDITING;

  sbt.tileSelectionRec = (Rectangle){0, 0, 250, 250};
}

void updateCamera() {
  int scroll = GetMouseWheelMove();

  if (scroll > 0 && sbt.cam.zoom < 1.5) {
    sbt.cam.zoom += 0.05;
    sbt.cam.target = worldMouse;
  } else if (scroll < 0 && sbt.cam.zoom >= 0.2) {
    sbt.cam.zoom -= 0.05;
  }

  // Reset cam to original
  if (IsKeyPressed(KEY_R)) {
    sbt.cam.target = sbt.cam.offset;
    sbt.cam.zoom = 1.0;
  }
}

void levelEditingUpdate(SB_Level *currentLevel, Vector2 mousePos) {

  // update mouse position
  worldMouse = GetScreenToWorld2D(GetMousePosition(), sbt.cam);

  // Update camera
  updateCamera();

  // Hide Lines
  if (IsKeyPressed(KEY_TAB)) {
    sbt.showTileLines = !sbt.showTileLines;
  }

  // Show all layers
  if (IsKeyPressed(KEY_L)) {
    sbt.showAllLayers = !sbt.showAllLayers;
  }

  // filler

  // Placing tiles
  if (!sbt.showAllLayers) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 currentLevelSize = {
          currentLevel->layer[sbt.currentLayer].cols * sbt.currentDrawSize.x,
          currentLevel->layer[sbt.currentLayer].rows * sbt.currentDrawSize.y};
      Rectangle level = {0, 0, currentLevelSize.x, currentLevelSize.y};
      int y, x;
      if (CheckCollisionPointRec(worldMouse, level)) {
        x = worldMouse.x / sbt.currentDrawSize.x;
        y = worldMouse.y / sbt.currentDrawSize.y;
        currentLevel->layer[sbt.currentLayer].data[y][x] = sbt.selectedTile;
      }
    }

    // Removing Tiles
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      Vector2 currentLevelSize = {
          currentLevel->layer[sbt.currentLayer].cols * sbt.currentDrawSize.x,
          currentLevel->layer[sbt.currentLayer].rows * sbt.currentDrawSize.y};
      Rectangle level = {0, 0, currentLevelSize.x, currentLevelSize.y};
      int y, x;
      x = worldMouse.x / sbt.currentDrawSize.x;
      y = worldMouse.y / sbt.currentDrawSize.y;

      if (CheckCollisionPointRec(worldMouse, level) &&
          currentLevel->layer[sbt.currentLayer].data[y][x] >= 0) {
        currentLevel->layer[sbt.currentLayer].data[y][x] = -1;
      }
    }

    // Change layers
    if (IsKeyPressed(KEY_UP) &&
        sbt.currentLayer <= currentLevel->layerCount - 1) {
      sbt.currentLayer++;
    } else if (IsKeyPressed(KEY_DOWN) && sbt.currentLayer > 0) {
      sbt.currentLayer--;
    }
  }
}

void tileSelectionUpdate(SB_Level *currentLevel, Vector2 mousePos) {
  if (IsKeyDown(KEY_UP)) {
    sbt.tileSelectionRec.width *= 1.025;
    sbt.tileSelectionRec.height *= 1.025;
  } else if (IsKeyDown(KEY_DOWN)) {
    sbt.tileSelectionRec.width *= 0.975;
    sbt.tileSelectionRec.height *= 0.975;
  }

  // Selecting new tile from tile selection
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
      CheckCollisionPointRec(mousePos, sbt.tileSelectionRec)) {
    int currentTileWidth =
        sbt.tileSelectionRec.width / currentLevel->tileset.cols;
    int currentTileHeight =
        sbt.tileSelectionRec.height / currentLevel->tileset.rows;

    int col = (int)(mousePos.x - sbt.tileSelectionRec.x) / currentTileWidth;
    int row = (int)(mousePos.y - sbt.tileSelectionRec.y) / currentTileHeight;

    int selectedTile = row * currentLevel->tileset.cols + col;

    sbt.selectedTile = selectedTile;
  }
}

void SBTILES_Update(SB_Level *currentLevel, Vector2 mousePos) {
  switch (sbt.currentState) {
  case TILE_SELECTION:
    if (IsKeyPressed(KEY_F2)) {
      sbt.currentState = LEVEL_EDITING;
    }
    tileSelectionUpdate(currentLevel, mousePos);
    break;

  case LEVEL_EDITING:
    levelEditingUpdate(currentLevel, mousePos);

    if (IsKeyPressed(KEY_F2)) {
      sbt.currentState = TILE_SELECTION;
    }

    break;
  }
}

// Drawing tilesecxtion
void drawTileset(SB_Level currentLevel) {

  Rectangle sourceRec = {
      0, 0, currentLevel.tileset.cols * currentLevel.tileset.tileSize,
      currentLevel.tileset.rows * currentLevel.tileset.tileSize};

  DrawTexturePro(currentLevel.tileset.texture, sourceRec, sbt.tileSelectionRec,
                 (Vector2){0, 0}, 0.0, WHITE);

  // Highlight current selected Tile
  Rectangle highlightRec = {
      (sbt.selectedTile % currentLevel.tileset.cols) *
          (sbt.tileSelectionRec.width / currentLevel.tileset.cols),
      (sbt.selectedTile / currentLevel.tileset.cols) *
          (sbt.tileSelectionRec.height / currentLevel.tileset.rows),
      sbt.tileSelectionRec.width / currentLevel.tileset.cols,
      sbt.tileSelectionRec.height / currentLevel.tileset.rows,
  };

  DrawRectangleRec(highlightRec, Fade(BLUE, 0.5));
}

void SBTILES_Draw(SB_Level currentLevel, int drawTileWidth,
                  int drawTileHeight) {

  switch (sbt.currentState) {
  case TILE_SELECTION:
    drawTileset(currentLevel);

    break;

  case LEVEL_EDITING:

    DrawText((TextFormat("%d", sbt.currentLayer)), 0, 0, 50, RED);

    BeginMode2D(sbt.cam);

    SB_Level_Draw(currentLevel, drawTileWidth, drawTileHeight,
                  sbt.showAllLayers ? -1 : sbt.currentLayer);

    sbt.currentDrawSize = (Vector2){drawTileWidth, drawTileHeight};

    // Grid lines
    if (sbt.showTileLines) {

      for (int i = 0; i < currentLevel.layer[sbt.currentLayer].rows; i++) {
        for (int j = 0; j < currentLevel.layer[sbt.currentLayer].cols; j++) {

          DrawLineEx((Vector2){j * drawTileWidth, 0},
                     (Vector2){j * drawTileWidth,
                               currentLevel.layer[sbt.currentLayer].rows *
                                   drawTileHeight},
                     7.5, BLACK);

          DrawLineEx((Vector2){0, i * drawTileHeight},
                     (Vector2){drawTileWidth *
                                   currentLevel.layer[sbt.currentLayer].cols,
                               i * drawTileHeight},
                     7.5, BLACK);
        }
      }
    }

    EndMode2D();

    break;
  }
}
