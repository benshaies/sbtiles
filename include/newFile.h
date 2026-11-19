#ifndef NEWFILE_H
#define NEWFILE_H
#include <raylib.h>
#include "../external/tinyfiledialogs.h"

typedef struct{
  char path[512];
  char tilesetName[128];
  int layers;
  int rows;
  int cols;
  int tileSize;
}NewLevelConfig;

bool newLevelPrompt(NewLevelConfig *out);

void newLevelUpdate();


#endif // !NEWFILE_H
