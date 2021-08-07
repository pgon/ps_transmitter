import socket

s = socket.socket()         
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

s.bind(('0.0.0.0', 9999 ))
s.listen(0)                 

while True:

    client, addr = s.accept()

    while True:
        content = client.recv(32)

        if len(content) ==0:
           break

        else:
            print(content)

    print("Closing connection")
    client.close()
