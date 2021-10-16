
import socket
import subprocess
import sys

SERVER_IP = "0.0.0.0"
SERVER_PORT = int(sys.argv[1])
is_running = True

def run_command(msg):
    parts = list(map(lambda x: x.decode("utf-8").strip(), msg.split(b" ")))
    print("got cmd " + parts[0])
    if parts[0] == "make":
        subprocess.run(parts, stdout=cl, stderr=cl)
    else:
        print("no matching action for cmd")

server = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
server.bind((SERVER_IP, SERVER_PORT))
server.listen(5)

try:
    while is_running == True:
        cl, addr = server.accept()
        print("got connection from client")
        msg = cl.recv(1024)
        print("running message")
        output = run_command(msg)
        cl.close()
finally:
    server.close()
