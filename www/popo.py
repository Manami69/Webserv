import cgi
form = cgi.FieldStorage()

print ("{} {}".format(form.getvalue("say"), form.getvalue("to")))