import socket
import threading

class listenerThread(threading.Thread):
  def __init__(self, server):
    threading.Thread.__init__(self)
    self.server = server
    self.stoped = False
  def run(self):
    while 1:
      message = self.server.recv(256)
      if len(message) == 0:
        break
      if message == "STOP":
        break
      print message
  def stop(self):
    self.stopped = True

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect(("localhost", 6667))
socket.send("Octalus")
listener = listenerThread(socket)
listener.start()

while 1:
  message = raw_input("message:")
  socket.send(message)
