#include "../include/newFile.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

NewLevelConfig levelParam;

bool newLevelPrompt(NewLevelConfig *out){
  bool returnLevel = false; 

  memset(out, 0, sizeof(*out));
  
  // Picking level directory
  const char *filters[1] = { "*.sblevel" };
  const char *result = tinyfd_saveFileDialog(
    "New level",
    "/home/benja/coding/myGames/untitled.sblevel",   // starting folder + suggested name
    1, filters, "sbTiles levels"); 
  
  if(result == NULL) return false;
  strcpy(out->path, result);
  
  if (!IsFileExtension(out->path, ".sblevel")) {
   strncat(out->path, ".sblevel", sizeof(out->path) - strlen(out->path) - 1);
  }

  // Tileset
  const char *tilesetFiler[1] = {"*.png"};
  char assertDir[512];
  snprintf(assertDir, sizeof(assertDir), "%s/../assets", GetDirectoryPath(out->path));

  const char *tilesetResult = tinyfd_openFileDialog("Choose Tileset", assertDir, 1, tilesetFiler, "png", false);

  if(tilesetResult == NULL){
    return false;
  }

  strcpy(out->tilesetName, GetFileName(tilesetResult));

  // Layers
  const char *layersResult = tinyfd_inputBox("Layer Count", "Layers", "1");

  if(layersResult == NULL) return false;
  
  out->layers = atoi(layersResult);

  // Rows
  const char *rowsResult = tinyfd_inputBox("Rows", "Rows", "1");

  if(rowsResult == NULL) return false;

  out->rows = atoi(rowsResult);

  // Cols
  const char *colsResult = tinyfd_inputBox("Cols", "Cols", "1");

  if(colsResult == NULL) return false;

  out->cols = atoi(colsResult);

  // Tilesize
  const char *tilesizeResult = tinyfd_inputBox("Tileset Tilesize", "Tilesize", "16");

  if(tilesizeResult == NULL) return false;

  out->tileSize = atoi(tilesizeResult);

  if(out->rows < 1 || out->cols < 1 || out->tileSize < 1 || out->layers < 1){
    return false;
  }

  return true;
}

void newLevelUpdate(){
    

}
