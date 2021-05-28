# Webserv

## Outils de travail de groupe
 - [To do list](https://github.com/Manami69/Webserv/projects/1)
 - [Excalidraw whiteboard](https://excalidraw.com/#room=fe0de34bd52dc15dfe14,sqhLO0-adrxa1B1gfd9OXw)
 - [Miro whiteboard](https://miro.com/welcome/bp6JMuEjVJfDUDUdozo5Tcc2d3fg0ejvYYn2tQtEk986IrITGLsAfQCYI6p4Mq3b)

## Test du tester
```
Before starting please follow the next few steps (files content can be anything and will be shown to you by the test):
- Download the cgi_test executable on the host
- Create a directory YoupiBanane with:
        -a file name youpi.bad_extension
        -a file name youpi.bla
        -a sub directory called nop
                -a file name youpi.bad_extension in nop
                -a file name other.pouic in nop
        -a sub directory called Yeah
                -a file name not_happy.bad_extension in Yeah

Setup the configuration file as follow:
- / must answer to GET request ONLY
- /put_test/* must answer to PUT request and save files to a directory of your choice
- any file with .bla as extension must answer to POST request by calling the cgi_test executable
- /post_body must answer anything to POST request with a maxBody of 100
- /directory/ must answer to GET request and the root of it would be the repository YoupiBanane and if no file are requested, it should search for youpi.bad_extension files
```

### test 1
```
GET / HTTP/1.1
Host: localhost:80
User-Agent: Go-http-client/1.1
Accept-Encoding: gzip
```
>>> 200
### test 2
```
POST / HTTP/1.1
Host: localhost:80
User-Agent: Go-http-client/1.1
Transfer-Encoding: chunked
Content-Type: test/file
Accept-Encoding: gzip
```
>>> 405
### test 3
```
HEAD / HTTP/1.1
Host: localhost:8080
User-Agent: Go-http-client/1.1
```
>>> 405
### test 4
```
GET /directory/oulalala HTTP/1.1
Host: localhost:8080
User-Agent: Go-http-client/1.1
Accept-Encoding: gzip
```
>>> ? 
