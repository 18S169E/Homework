from socket import *
import tkinter as tk
import tkinter.scrolledtext as tst
import time
import tkinter.messagebox
import threading

class Application(tk.Frame):
    def __init__(self, master):
        tk.Frame.__init__(self, master)
        self.grid()
        self.createWidgets()

    def createWidgets(self):
        # 显示聊天窗口
        self.textEdit = tst.ScrolledText(self, width=50, height=15)
        self.textEdit.grid(row=0, column=0, rowspan=1, columnspan=4)
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
        # 添加文件接收按钮
        self.btnReceiveFile = tk.Button(self, text='接收文件', command=self.receiveFile)
        self.btnReceiveFile.grid(row=2, column=0, columnspan=1)

        # 开启一个线程用于接收消息并显示在聊天窗口
        t = threading.Thread(target=self.getInfo)
        t.start()

    def textSend(self):
        # 获取Text的所有内容
        str = self.inputText.get('1.0', 'end-1c')
        if str != "":
            # 显示发送时间和发送消息
            timemsg = '服务端' + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()) + '\n'
            self.textEdit.config(state='normal')
            self.textEdit.insert(tk.END, timemsg, 'server')
            self.textEdit.insert(tk.END, str + '\n')
            # 将滚动条拉到最后显示最新消息
            self.textEdit.see(tk.END)
            self.textEdit.config(state='disabled')
            self.inputText.delete(0.0, tk.END)  # 删除输入框的内容
            # 发送数据到客户端
            sendMessage = bytes(str, encoding='utf8')
            # 发送输入的数据，与UDP有点不同，使用的是send方法，不需要指定客户端地址和端口，因为已经建立了一条 TCP 连接
            connectionSocket.send(sendMessage)
        else:
            tk.messagebox.showinfo('警告', "不能发送空白信息！")

    def getInfo(self):
        while True:
            recMsg = connectionSocket.recv(1024).decode("utf-8") + '\n'
            revTime = '客户端' + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()) + '\n'
            # 通过设置 state 属性设置 textEdit 可编辑
            self.textEdit.config(state='normal')
            self.textEdit.insert(tk.END, revTime, 'guest')
            self.textEdit.insert(tk.END, recMsg)
            # 将滚动条拉到最后显示最新消息
            self.textEdit.see(tk.END)
            # 通过设置 state 属性设置 textEdit 不可编辑
            self.textEdit.config(state='disabled')

    def textSendReturn(self, event):
        if event.keysym == "Return":
            self.textSend()

    def receiveFile(self):
        print("DEBUG:receive file use")
        try:
            file = open("received_file.txt", "wb")
            print("DEBUG：file open")
            """
            while True:
                data = connectionSocket.recv(1024)
                print("DEBUG:进入循环",data)
                if data == b'EOF':
                    print("DEBUG:exit")
                    break
                file.write(data)
                file.write('1')
                print("DEBUG：file write")
            """

            data = connectionSocket.recv(1024)
            print("DEBUG:进入循环", data)
            if data == b'EOF':
                print("DEBUG:exit")
            file.write(data)
            print("DEBUG：file write")


            file.close()
            print('文件接收完成：received_file.txt')
        except Exception as e:
            print('文件接收出错：', str(e))


root = tk.Tk()
root.title('服务端')

# 指定服务器使用的端口
serverPort = 12000
serverSocket = socket(AF_INET, SOCK_STREAM)
# 绑定端口
serverSocket.bind(('', serverPort))
# 定义最大连接数
serverSocket.listen(1)
print('等待连接....')
# 接受请求则建立一个连接
connectionSocket, addr = serverSocket.accept()
print('一个连接')
app = Application(master=root)
app.mainloop()
