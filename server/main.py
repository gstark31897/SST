import socket
import threading

clients = {}

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("localhost", 6667))
server.listen(5)

class clientThread(threading.Thread):
  def __init__(self, client, address, user):
    threading.Thread.__init__(self)
    self.client  = client
    self.address = address
    self.user    = user
    print(user, " connected")
    global clients
  def run(self):
    global clients
    print(len(clients))
    while 1:
      message = self.client.recv(256)
      if len(message) == 0:
        break
      if message == "STOP":
        self.client.send("STOP")
        self.client.close()
        break
      receiver = message.split(':')[0]
      clients[receiver].send(message.split(':')[1])
  def send(self, message):
    self.client.send(message)


while 1:
  (client, address) = server.accept()
  user = client.recv(256)
  clients[user] = clientThread(client, address, user)
  clients[user].start()
  clients[user].send("Hello World")
