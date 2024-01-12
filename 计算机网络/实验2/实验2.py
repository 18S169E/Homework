import sys
import socket
import threading
import time
import tkinter as tk

QUIT = False

class ClientThread(threading.Thread):
    def __init__(self, client_sock):
        threading.Thread.__init__(self)
        self.client = client_sock

    def run(self):
        global QUIT
        done = False
        cmd = self.readline()

        while not done:
            if 'quit' == cmd:
                self.writeline('Ok, bye')
                QUIT = True
                done = True
            elif 'bye' == cmd:
                self.writeline('Ok, bye')
                done = True
            else:
                self.writeline(self.name)

            cmd = self.readline()

        self.client.close()

    def readline(self):
        result = self.client.recv(1024)
        if result is not None:
            result = result.strip().lower()
        return result

    def writeline(self, text):
        self.client.send((text.strip() + '\n').encode())

class Server:
    def __init__(self):
        self.sock = None
        self.thread_list = []

    def run(self):
        all_good = False
        try_count = 0

        while not all_good:
            if 3 < try_count:
                sys.exit(1)
            try:
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.sock.bind(('127.0.0.1', 5050))
                self.sock.listen(5)
                all_good = True
                break
            except socket.error as err:
                print('Socket connection error... Waiting 10 seconds to retry.')
                del self.sock
                time.sleep(10)
                try_count += 1

        print("Server is listening for incoming connections.")
        print("Try to connect through the command line, with:")
        print("telnet localhost 5050")
        print("and then type whatever you want.")
        print()
        print("typing 'bye' finishes the thread, but not the server ", end='')
        print("(eg. you can quit telnet, run it again and get a different ", end='')
        print("thread name")
        print("typing 'quit' finishes the server")

        try:
            while not QUIT:
                try:
                    self.sock.settimeout(0.500)
                    client = self.sock.accept()[0]
                except socket.timeout:
                    time.sleep(1)
                    if QUIT:
                        print("Received quit command. Shutting down...")
                        break
                    continue

                new_thread = ClientThread(client)
                print('Incoming Connection. Started thread ', end='')
                print(new_thread.getName())
                self.thread_list.append(new_thread)
                new_thread.start()

                for thread in self.thread_list:
                    if not thread.isAlive():
                        self.thread_list.remove(thread)
                        thread.join()

        except KeyboardInterrupt:
            print('Ctrl+C pressed... Shutting Down')
        except Exception as err:
            print('Exception caught: %s\nClosing...' % err)

        for thread in self.thread_list:
            thread.join(1.0)

        self.sock.close()

class ServerGUI:
    def __init__(self, root):
        self.root = root
        self.server = Server()

        self.start_button = tk.Button(root, text="Start Server", command=self.start_server)
        self.start_button.pack()

        self.stop_button = tk.Button(root, text="Stop Server", command=self.stop_server)
        self.stop_button.pack()

    def start_server(self):
        self.start_button.config(state=tk.DISABLED)
        self.server_thread = threading.Thread(target=self.server.run)
        self.server_thread.start()

    def stop_server(self):
        self.server.stop_server()
        self.server_thread.join()
        self.start_button.config(state=tk.NORMAL)

def main():
    root = tk.Tk()
    root.title("Server Control")
    app = ServerGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
