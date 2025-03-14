from socket import *
import tkinter as tk
import tkinter.scrolledtext as tst
import time
import tkinter.messagebox
import threading

#定义输入服务器ip地址的类
class inputIPdialog(tk.Frame):

	def __init__(self,master):
		tk.Frame.__init__(self,master)
		self.ipInput=tk.Text(self,width=30,height=5)
		self.ipInput.grid(row=0,column=0,columnspan=3)
		self.okbtn=tk.Button(self,text='确定',command=self.setIP).grid(row=1,column=3)
		self.grid()

	def setIP(self):
		#这个global变量作为类变量的话没有效果，原因不知
		global servername
		servername=self.ipInput.get('1.0','end-1c')
		#销毁窗口
		ipRootFrame.destroy()

class Application(tk.Frame):
	def __init__(self,master):
		tk.Frame.__init__(self,master)
		self.grid()
		self.createWidgets()
	def createWidgets(self):
		#显示聊天窗口
		self.textEdit=tst.ScrolledText(self,width=50,height=15)
		self.textEdit.grid(row=0,column=0,rowspan=1,columnspan=4)
		self.textEdit.config(state='disabled')
		#定义标签，改变字体颜色
		self.textEdit.tag_config('server',foreground='red')
		self.textEdit.tag_config('guest',foreground='blue')

		#编辑窗口
		self.inputText=tk.Text(self,width=40,height=5)
		self.inputText.grid(row=1,column=0,columnspan=1)
		#定义快捷键，按下回车即可发送消息
		self.inputText.bind("<KeyPress-Return>",self.textSendReturn)

		#发送按钮
		self.btnSend=tk.Button(self,text='send',command=self.textSend)
		self.btnSend.grid(row=1,column=3)

		#开启一个线程用于接收消息并显示在聊天窗口
		t=threading.Thread(target=self.getInfo)
		t.start()


	def textSend(self):
		#获取Text的所有内容
		#https://stackoverflow.com/questions/14824163/how-to-get-the-input-from-the-tkinter-text-box-widget
		str=self.inputText.get('1.0','end-1c')
		if str!="" and str!=None:
			#显示发送时间和发送消息
			timemsg='客户端'+time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())+'\n'
			#通过设置state属性设置textEdit可编辑
			self.textEdit.config(state='normal')

			self.textEdit.insert(tk.INSERT,timemsg,'guest')
			self.textEdit.insert(tk.INSERT,str+'\n')

			#将滚动条拉到最后显示最新消息
			self.textEdit.see(tk.END)
			#通过设置state属性设置textEdit不可编辑
			self.textEdit.config(state='disabled')
			self.inputText.delete(0.0,tk.END)	#删除输入框的内容
			#发送数据到服务端
			sendMessage=bytes(str,encoding='utf8')
			#发送输入的数据，与UDP有点不同，使用的是send方法，不需要指定服务器和端口，因为已经建立了一条tcp连接
			clientSocket.send(sendMessage)
		else:
			tk.messagebox.showinfo('警告',"不能发送空白信息！")

	def getInfo(self):
		global  clientSocket
		while True:
			#接收数据,1024指定缓存长度，使用的是recv方法
			recMessage=clientSocket.recv(1024).decode("utf8")+'\n'
			#接受时间和接收的数据
			recTime='服务端'+time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())+'\n'
			self.textEdit.config(state='normal')
			#server作为标签,改变字体颜色
			self.textEdit.insert(tk.END,recTime,'server')
			self.textEdit.insert(tk.END,recMessage)
			#将滚动条拉到最后显示最新消息
			self.textEdit.see(tk.END)
			self.textEdit.config(state='disabled')

	def textSendReturn(self,event):
		if event.keysym=="Return":
			self.textSend()

#指定服务器地址，端口
servername=''
serverport=12000
ipRootFrame=tk.Tk()
ipRootFrame.title('输入服务器ip')
ipDialog=inputIPdialog(ipRootFrame)
ipDialog.mainloop()
#socket第一个参数指定使用IPV4协议，第二个参数指定这是一个TCP套接字
clientSocket=None

try:
	clientSocket=socket(AF_INET,SOCK_STREAM)
except:
	tk.messagebox.showinfo('未知错误','检查服务器地址是否错误！')

#tcp连接需要先经过握手建立连接
clientSocket.connect((servername,serverport))
root=tk.Tk()
root.title('客户端')

app=Application(master=root)
app.mainloop()

