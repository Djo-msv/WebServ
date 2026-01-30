import warnings
warnings.filterwarnings('ignore')
import cgi, cgitb

def success():
    print ("<html><body><h1>Successfully logged in as %s !</h1></body></html>" % (name))

form = cgi.FieldStorage()
name = form.getvalue("username")
password = form.getvalue("password")
filename = "./logs/logins/" + name

try:
    file = open(filename, "r")
    content = file.read()
    if content == password :
        success()
    else:
        print("<html><body><h1>sorry, this username is already in use and/or the password is incorrect.</h1>")
        print("<a href=\"/html/login.html\">Retry</a></body></html>")
    file.close()
except:
    file = open(filename, "w")
    file.write(password)
    success()
    file.close()
