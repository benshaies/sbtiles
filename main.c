#include "sblib/include/sblib.h"
#include "sbtiles.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/application.h"

typedef struct {
  const char *text;
  bool active;
  float timer;
  float timeBase;
  Vector2 pos;
  float size;
} Notification;

Notification testing;

Notification createNotification(const char *text, float timeBase, float size) {
  Notification noti;

  noti.text = text;
  noti.timeBase = timeBase;
  noti.timer = timeBase;
  noti.active = true;
  noti.size = size;

  return noti;
}

void updateNotification(Notification *noti) {
  if (noti->active) {
    noti->timer -= GetFrameTime();

    if (noti->timer <= 0) {
      noti->active = false;
      noti->timer = noti->timeBase;
    }
  }
}

void displayNotification(Notification noti) {
  if (noti.active) {
    DrawText(noti.text, noti.pos.x, noti.pos.y, noti.size, MAROON);
  }
}

int main() {

  appInit();

  while (!WindowShouldClose()) {
    appUpdate();
    appDraw();
  }


  return 0;
}
