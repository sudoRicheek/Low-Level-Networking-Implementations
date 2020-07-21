import socket
import sys

LOCALHOST = "127.0.0.1"
PORT = 8000

def main():
   soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   
   try:
      soc.connect((LOCALHOST, PORT))
   except:
      print("Connection Error")
      sys.exit()

   print("Please enter 'quit' to exit")
   
   message = input(" > ")
   
   while message != 'quit':
      soc.sendall(message.encode("utf8"))
      
      if soc.recv(1024).decode("utf8") == "-":
         pass      
      message = input(" > ")   
   
   soc.send(b'--quit--')


if __name__ == "__main__":
   main()