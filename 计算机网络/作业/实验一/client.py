from socket import *
import tkinter as tk
import tkinter.scrolledtext as tst
import tkinter.messagebox
import threading
import time
from tkinter import filedialog

# 定义输入服务器ip地址的类
class inputIPdialog(tk.Frame):
    def __init__(self, master):
        tk.Frame.__init__(self, master)
        self.ipInput = tk.Text(self, width=30, height=5)
        self.ipInput.grid(row=0, column=0, columnspan=3)
        self.okbtn = tk.Button(self, text='确定', command=self.setIP)
        self.okbtn.grid(row=1, column=3)
        self.grid()

    def setIP(self):
        global servername
        servername = self.ipInput.get('1.0', 'end-1c')
        ipRootFrame.destroy()

class Application(tk.Frame):
    def __init__(self, master):
        tk.Frame.__init__(self, master)
        self.grid()
        self.createWidgets()

    def createWidgets(self):
        # 显示聊天窗口
        self.textEdit = tst.ScrolledText(self, width=50, height=15)
        self.textEdit.grid(row=0, column=0, rowspan=1, columnspan=4)
        self.textEdit.config(state='disabled')
        # 定义标签，改变字体颜色
        self.textEdit.tag_config('server', foreground='red')
        self.textEdit.tag_config('guest', foreground='blue')

        # 编辑窗口
        self.inputText = tk.Text(self, width=40, height=5)
        self.inputText.grid(row=1, column=0, columnspan=1)
        # 定义快捷键，按下回车即可发送消息
        self.inputText.bind("<KeyPress-Return>", self.textSendReturn)

        # 发送按钮
        self.btnSend = tk.Button(self, text='send', command=self.textSend)
        self.btnSend.grid(row=1, column=3)

        # 发送文件按钮
        self.btnSendFile = tk.Button(self, text='发送文件', command=self.send_file)
        self.btnSendFile.grid(row=1, column=4)

        # 开启一个线程用于接收消息并显示在聊天窗口
        t = threading.Thread(target=self.getInfo)
        t.start()

    def textSend(self):
        str_msg = self.inputText.get('1.0', 'end-1c')
        if str_msg != "" and str_msg != None:
            timemsg = '客户端' + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()) + '\n'
            self.textEdit.config(state='normal')
            self.textEdit.insert(tk.INSERT, timemsg, 'guest')
            self.textEdit.insert(tk.INSERT, str_msg + '\n')
            self.textEdit.see(tk.END)
            self.textEdit.config(state='disabled')
            self.inputText.delete(0.0, tk.END)
            sendMessage = bytes(str_msg, encoding='utf8')
            clientSocket.send(sendMessage)
        else:
            tk.messagebox.showinfo('警告', "不能发送空白信息！")

    def getInfo(self):
        global clientSocket
        while True:
            recMessage = clientSocket.recv(1024).decode("utf8") + '\n'
            recTime = '服务端' + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()) + '\n'
            self.textEdit.config(state='normal')
            self.textEdit.insert(tk.END, recTime, 'server')
            self.textEdit.insert(tk.END, recMessage)
            self.textEdit.see(tk.END)
            self.textEdit.config(state='disabled')

    def textSendReturn(self, event):
        if event.keysym == "Return":
            self.textSend()

    def send_file(self):
        file_path = filedialog.askopenfilename()
        if file_path:
            try:
            #     with open(file_path, 'rb') as file:
            #         file_data = file.read(1024)
            #         print("DEBUG:",file_data)
            #         while file_data:
            #             clientSocket.send(file_data)
            #             print("DEBUG:传输完成")
            #             file_data = file.read(1024)
            #     tk.messagebox.showinfo('文件发送', '文件已成功发送：{}'.format(file_path))
            #     #clientSocket.send("EOF".encode())  # 最后自动发送EOF表示文件传输完毕

                with open(file_path, 'rb') as file:
                        file_data = file.read(1024)
                        clientSocket.send(file_data)
                        print("DEBUG:传输完成,数据为：",file_data)
                tk.messagebox.showinfo('文件发送', '文件已成功发送：{}'.format(file_path))
                    #clientSocket.send("EOF".encode())  # 最后自动发送EOF表示文件传输完毕
            except Exception as e:
                tk.messagebox.showerror('文件发送错误', '发生错误：{}'.format(str(e)))


# 指定服务器地址，端口
servername = ''
serverport = 12000
ipRootFrame = tk.Tk()
ipRootFrame.title('输入服务器ip')
ipDialog = inputIPdialog(ipRootFrame)
ipDialog.mainloop()

clientSocket = None
try:
    clientSocket = socket(AF_INET, SOCK_STREAM)
except:
    tk.messagebox.showinfo('未知错误', '检查服务器地址是否错误！')

clientSocket.connect((servername, serverport))
root = tk.Tk()
root.title('客户端')

app = Application(master=root)
app.mainloop()
