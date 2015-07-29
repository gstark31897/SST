import socket
import sys
import threading

class writerThread(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
  def run(self):
    global socket
    while 1:
      message = raw_input("message:")
      socket.send(message)
      if message == "STOP":
        break
    socket.shutdown(1)

if len(sys.argv) < 4:
  print("Not enough arguments")

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((str(sys.argv[1]), int(sys.argv[2])))
socket.send(str(sys.argv[3]))
writer = writerThread()
writer.start()

while 1:
  message = socket.recv(256)
  if len(message) == 0:
    break
  print(message)
