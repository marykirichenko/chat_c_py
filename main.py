import socket
import threading
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = socket.gethostname()
port = 8080
name = input("Enter your name: ")
client_socket.connect((host, port))
def receive():
    while True:
        message = client_socket.recv(1024).decode()
        print('>',message)
receive_thread = threading.Thread(target=receive)
receive_thread.start()
while True:
    message = input()
    client_socket.send(f"{name}: {message}".encode())
