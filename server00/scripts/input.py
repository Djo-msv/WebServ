import warnings
warnings.filterwarnings('ignore')
import cgi, os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

# Get filename here.
itemlist = form['filename']
if not isinstance(itemlist, list):
    itemlist = [itemlist]

# Test if the file was uploaded
message = '<html><h1>Upload results :</h1>'
for fileitem in itemlist:
    if fileitem.filename:
    # strip leading path from file name to avoid
    # directory traversal attacks
        fn = os.path.basename(fileitem.filename)
        open('./server00/uploads/client_' + fn, 'wb').write(fileitem.file.read())
        message += '<p>The file "' + fn + '" was uploaded successfully</p>'
    else:
        message += '<p>No file was uploaded</p>'
message += '</html>'
print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %s\r\n\r\n" % str(len(message)))
print("%s" % message)
