# Webserv

## Outils de travail de groupe
 - [To do list](https://github.com/Manami69/Webserv/projects/1)
 - [Excalidraw whiteboard](https://excalidraw.com/#room=fe0de34bd52dc15dfe14,sqhLO0-adrxa1B1gfd9OXw)
 - [Miro whiteboard](https://miro.com/welcome/bp6JMuEjVJfDUDUdozo5Tcc2d3fg0ejvYYn2tQtEk986IrITGLsAfQCYI6p4Mq3b)

## Configuration Settings
- Configuration is space/tab insensitive, you can do it all in one line like Nginx.
- Blocks are defined inside curvy brackets
- Every definition must be closed by  `;` (except the curvy brakets)

### Server block
A server block is defined by the `server` tag  followed by curvy brackets including the configuration of this one server.
```
server {
    //... your configuration here
}
```
#### Choose the port and host of each "server"
`port` and l'`host` can be defined with IPV4 addresses with the `listen` tag. In this configuration, you have to choose one of each for each server block.
You can't set several `port` and `host` for one server. If you don't set one, default settings are set.
The following definitions are allowed:
```
    # localhost = 127.0.0.1
    # default settings are host = 0.0.0.0 (any address) port = 80
    //...
    listen 0.0.0.0:80; # host = 0.0.0.0 # port = 80 
    //...
    listen 0.0.0.0 80; # host = 0.0.0.0 # port = 80
    //...
    listen 0.0.0.0; # host = 0.0.0.0
    listen 80; #port = 80
    //...
```
> The first server for a host:port will be the default for this host:port (meaning it will answer to all request that doesn’t belong to an other server)

#### Setup the server_names (optional)

#### setup default error pages

#### limit client body size

#### Location block (setup routes with one or multiple of the following rules/configuration (routes wont be using regexp))
Like `server` blocks, `location` blocks begin and end with curvy brackets. Each `location` is defined with a `modifier` (optional) and an `URI` specification. Unlike Nginx, `.` for any char and wildcard `*` and `?` and `$any_var` are not implemented (nothing including regex).
```
location [modifier] [URI] {
  ...
  ...
}
```
`modifier` can be :
- none: If no modifiers are present in a location block then the requested URI will be matched against the beginning of the requested URI.
- `=`: The equal sign is used to match a location block exactly against a requested URI.
- `~`: The tilde sign is used for case-sensitive regular expression match against a requested URI.
- `~*`: The tilde followed by asterisk sign is used for case insensitive regular expression match against a requested URI.
- `^~`: The carat followed by tilde sign is used to perform longest nonregular expression match against the requested URI. If the requested URI hits such a location block, no further matching will takes place.

`location` can be defined by any route that match the requested `URI`. `$` symbol is used at the end of a route to say that nothing can be found after the last char.

The following settings can only be defined inside a `location` block.

##### define a list of accepted HTTP Methods for the route
By default `GET` and `POST` methods are accepted, `DELETE` must be allowed.
Both tags `allow_methods` and `limit_methods` can be used. 
if `limit_methods` is used, not included methods can't be allowed.
If `DELETE` is in limited methods, you'll still need to allow it with `allow_methods`.

##### define an HTTP redirection
Redirection are set with the tag `return`. (it can be also with `error_page`). It is set if the page is temporarly (302) or permanently moved (301);
```
return 301 @newlocation;
return 301;
```

##### define a directory or a file from where the file should be search (for example if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is/tmp/www/pouic/toto/pouet)
with the `root` tag, define a directory with absolute path. 

##### turn on or off directory listing
If no default file is found in this location and you requested a folder, then you can set an auto index with the `autoindex` tag.

```
autoindex on;
# OR
autoindex off; #->> default setting
```

##### default file to answer if the request is a directory
Can be several file after the `index` tag.

##### execute CGI based on certain file extension (for example .php)
with `CGI_path` tag followed with an absolute path to the binary file.
```
location ~ .php$ {
        CGI_path /Users/xxx/.brew/bin/php-cgi;
}
```
