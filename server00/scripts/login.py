import warnings
warnings.filterwarnings('ignore')
import cgi, cgitb
import os

form = cgi.FieldStorage()
name = form.getfirst("username", "name")
password = form.getfirst("password", "pwd")
filename = "./logs/logins/" + name
folder = os.path.join('logs', 'logins')
if not os.path.exists(folder):
	os.makedirs(folder)

def success():
	body = "<html><body><h1>Successfully logged in as " + name + " !</h1></body></html>"
	msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nSet-Cookie: user=" + name + "\r\nContent-Length: " + str(len(body)) + "\r\n\r\n" + body
	print ("%s" % msg)

try:
	file = open(filename, "r")
	content = file.read()
	if content == password :
		success()
	else:
		body = "<html><body><h1>sorry, the username " + name + " is already in use and/or the password is incorrect.</h1>"
		body += "<a href=\"/html/login.html\">Retry</a></body></html>"
		msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + str(len(body)) + "\r\n\r\n" + body
		print ("%s" % msg)
	file.close()
except:
	try:
		file = open(filename, "w")
		file.write(password)
		success()
		file.close()
	except:
		body = "<html><body><h1>sorry, an error has occurred internally !</h1></body></html>"
		msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + str(len(body)) + "\r\n\r\n" + body
		print ("%s" % msg)
