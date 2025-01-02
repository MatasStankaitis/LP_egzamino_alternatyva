import socket
import json
import multiprocessing
from queue import Queue

def receiver(port=9999):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', port))
    server_socket.listen(1)
    print(f"Listening on port {port}")
    
    conn, addr = server_socket.accept()
    print(f"Connected by {addr}")
    
    buffer = ""
    while True:
        data = conn.recv(1024).decode('utf-8')
        if not data:
            continue
        buffer += data
        # Process complete JSON objects
        try:
            print(f"Received movie: {buffer}")
            buffer = ""
            # Process movie...
        except json.JSONDecodeError:
            continue

if __name__ == "__main__":
    receiver()