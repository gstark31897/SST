#include "gui/gui.h"
#include "backend/backend.h"

int main(int argc, char *argv[])
{
  StartBackend("localhost", 6667, "octalus");
  StartGui(argc, argv);
  StopBackend();
}
