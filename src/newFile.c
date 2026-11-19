#include "../include/newFile.h"
#include <stdio.h>
#include <string.h>

NewLevelConfig levelParam;

bool newLevelPrompt(NewLevelConfig *out){
  bool returnLevel = false; 
  
  // Picking level directory
  const char *filters[1] = { "*.sblevel" };
const char *result = tinyfd_saveFileDialog(
    "New level",
    "/home/benja/coding/myGames/untitled.sblevel",   // starting folder + suggested name
    1, filters, "sbTiles levels"); 

  strcpy(out->path, result);
  
  
  const char *reply = tinyfd_inputBox("New level", "Number of layers", "1");






  return true;
}

void newLevelUpdate(){
    

}
