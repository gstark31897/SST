#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <algorithm>

using namespace std;

GtkWidget *window;
GtkWidget *mainpane;
GtkWidget *friendbar;
GtkWidget *messagepane;
GtkWidget *messagehistory;
GtkWidget *messageinput;

vector<string>         friends;
vector<GtkWidget*>     friendLabels;
vector<GtkTextBuffer*> messageBuffers; 

bool enterkey = false;
bool shiftkey = false;

int getdir (char* path, vector<string> &files)
{
  struct dirent *ent;
  DIR *dir;

  dir = opendir(path);
  if(dir == NULL)
  {
    return 0;
  }

  while(ent = readdir(dir))
  {
    string filename = ent->d_name;
    if(strcmp(filename.c_str(), ".") == 0)
      continue;
    if(strcmp(filename.c_str(), "..") == 0)
      continue;
    files.push_back(ent->d_name);
  }

  closedir(dir);
  sort(files.begin(), files.end());
  
  return files.size();
}

void loadMessages(string friendName, GtkTextBuffer* messageBuffer)
{
  FILE   *file;
  char   *buffer;
  size_t filesize;

  string filepath("friends/");
  filepath += friendName;

  file = fopen(filepath.c_str(), "r");
  fseek(file, 0, SEEK_END);
  filesize = ftell(file);
  rewind(file);
  buffer = (char*)malloc(sizeof(char) * filesize);
  fread(buffer, 1, filesize, file);
  fclose(file);
  gtk_text_buffer_set_text(messageBuffer, buffer, filesize);
}

int findFriend(string friendName)
{
  for(int i = 0; i < friends.size(); ++i)
  {
    if(strcmp(friendName.c_str(), friends.at(i).c_str()) == 0)
      return i;
  }
  return -1;
}

void user_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data)
{
  if(row != NULL)
  {
     string friendName(gtk_label_get_label((GtkLabel*)gtk_bin_get_child((GtkBin*)row)));
     int index = findFriend(friendName);
     gtk_text_view_set_buffer(GTK_TEXT_VIEW(messagehistory), messageBuffers.at(index));
  }
}

bool MessageInputKeyDown(GtkTextView *text_view, GdkEventKey *event, gpointer user_data)
{
  if(event->keyval == GDK_KEY_Shift_L || event->keyval == GDK_KEY_Shift_R)
  {
    shiftkey = true;
    return false;
  }

  if(event->keyval == GDK_KEY_Return && !shiftkey)
  {
    printf("Sending message\n");
    return true;
  }

  return false;
}

bool MessageInputKeyUp(GtkTextView *text_view, GdkEventKey *event, gpointer user_data)
{
  if(event->keyval == GDK_KEY_Shift_L || event->keyval == GDK_KEY_Shift_R)
    shiftkey = false;

  return false;
}

int InitGui (int argc, char *argv[])
{
  getdir("friends", friends);

  gtk_init (&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show(window);

  mainpane = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add(GTK_CONTAINER(window), mainpane);
  gtk_widget_show(mainpane);

  friendbar = gtk_list_box_new();
  g_signal_connect(friendbar, "row-selected", G_CALLBACK(user_selected), NULL);
  gtk_paned_add1(GTK_PANED(mainpane), friendbar);
  gtk_widget_show(friendbar);

  messagepane = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  gtk_paned_add2(GTK_PANED(mainpane), messagepane);
  gtk_widget_show(messagepane);

  messagehistory = gtk_text_view_new();
  gtk_widget_set_size_request(messagehistory, 16, 900);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(messagehistory), false);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(messagehistory), false);
  gtk_paned_add1(GTK_PANED(messagepane), messagehistory);
  gtk_widget_show(messagehistory);

  messageinput = gtk_text_view_new();
  g_signal_connect(messageinput, "key-press-event", G_CALLBACK(MessageInputKeyDown), NULL);
  g_signal_connect(messageinput, "key-release-event", G_CALLBACK(MessageInputKeyUp), NULL);
  gtk_widget_set_size_request(messageinput, 16, 16);
  gtk_paned_add2(GTK_PANED(messagepane), messageinput);
  gtk_widget_show(messageinput);

  for(int i = 0; i < friends.size(); ++i)
  {
    GtkWidget *friend_label = gtk_label_new(friends.at(i).c_str());
    gtk_container_add(GTK_CONTAINER(friendbar), friend_label);
    gtk_widget_show(friend_label);
    friendLabels.push_back(friend_label);

    GtkTextTagTable *text_tag_table = gtk_text_tag_table_new();
    GtkTextBuffer *messageBuffer = gtk_text_buffer_new(text_tag_table);
    loadMessages(friends.at(i), messageBuffer);
    messageBuffers.push_back(messageBuffer);
  }

  gtk_main ();

  return 0;
}
