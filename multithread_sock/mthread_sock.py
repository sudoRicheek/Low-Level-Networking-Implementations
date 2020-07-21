import socket
import sys
import traceback
from threading import Thread # here we use the threading module of Python3 to get an object oriented approach to the problem

LOCALHOST = "127.0.0.1"
PORT = 8000
QUEUE = 10

def main():
   start_server()


def start_server():
   soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   soc.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
   
   print("Socket created")
   
   try:
      soc.bind((LOCALHOST, PORT))
   except:
      print("Bind failed. Error : " + str(sys.exc_info()))
      sys.exit()
   
   soc.listen(QUEUE) #Set max clients queuing to 10
   print("Socket now listening. CAN SUPPORT {} CLIENTS".format(QUEUE))

   while True:
      connection, address = soc.accept()
      ip, port = str(address[0]), str(address[1])
      print("Connected with " + ip + ":" + port)
   
      try:
         Thread(target=clientThread, args=(connection, ip, port)).start()
      except:
         print("Thread did not start.")
         traceback.print_exc()
   
   soc.close()


def clientThread(connection, ip, port, max_buffer_size = 1024):
   is_active = True
   
   while is_active:
      client_input = receive_input(connection, max_buffer_size)
   
      if "--QUIT--" in client_input:
         print("Client is requesting to quit")
         connection.close()
         print("Connection " + ip + ":" + port + " closed")
         is_active = False
      else:
         print("Processed result: {}".format(client_input))
         connection.sendall("-".encode("utf8"))


def receive_input(connection, max_buffer_size):
   client_input = connection.recv(max_buffer_size)
   client_input_size = sys.getsizeof(client_input)
   
   if client_input_size > max_buffer_size:
      print("The input size is greater than expected {}".format(client_input_size))
   
   decoded_input = client_input.decode("utf8").rstrip()
   result = process_input(decoded_input)
   
   return result


def process_input(input_str):
   print("Message received from client.")
   return str(input_str).upper()


if __name__ == "__main__":
   main()