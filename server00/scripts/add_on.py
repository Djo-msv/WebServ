import warnings
warnings.filterwarnings('ignore')
import cgi, os
import cgitb;

def get_cookie(_match): 
  # Returns the value from the matching cookie or '' if not defined.
  if 'HTTP_COOKIE' in os.environ:
    cookies = os.environ['HTTP_COOKIE']
    cookies = cookies.split('; ')

    for cookie in cookies:
      (_name, _value) = cookie.split('=')
      if (_match.lower() == _name.lower()):
        return _value
  return('')

message = "Currently not logged in"

if get_cookie('user'):
    path = './logs/logins/' + get_cookie('user')
    if os.path.exists(path):
        message = "Currently logged in as " + get_cookie('user')

msg = "<html><p style=\"background-color:powderblue;color:blue;text-align:center;\">" + message + "</p></html>"

print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %s\r\n\r\n" % str(len(msg)))
print("%s" % msg)
