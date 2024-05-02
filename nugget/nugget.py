import socket

# Create a TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to a specific address and port
server_address = ('localhost', 12345)
server_socket.bind(server_address)

# Listen for incoming connections
server_socket.listen(1)

while True:
    # Accept a client connection
    client_socket, client_address = server_socket.accept()
    
    try:
        # Receive data from the client
        data = client_socket.recv(1024)
        
        # Process the received data
        
        # Send a response back to the client
        response = b"Hello, client!"
        client_socket.sendall(response)
        
    finally:
        # Close the client socket
        client_socket.close()
