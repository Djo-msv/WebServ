import warnings
warnings.filterwarnings('ignore')
import cgi, cgitb

form = cgi.FieldStorage()
name = form.getvalue("username")
password = form.getvalue("password")
filename = "./logs/logins/" + name

def success():
    body = "<html><body><h1>Successfully logged in as " + name + " !</h1></body></html>"
    msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + str(len(body)) + "\r\n\r\n" + body
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
    file = open(filename, "w")
    file.write(password)
    success()
    file.close()
