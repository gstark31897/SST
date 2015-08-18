#include "gui/gui.h"
#include "backend/backend.h"

int main(int argc, char *argv[])
{
  StartBackend("localhost", 6667, "test");
  StartGui(argc, argv);
  StopBackend();
}
