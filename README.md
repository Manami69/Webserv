# Webserv

## Configuration Settings
- Configuration is space/tab insensitive, you can do it all in one line like Nginx.
- Blocks are defined inside curvy brackets
- Every definition must be closed by  `;` (except the curvy brakets)

### :cookie: Server block
A server block is defined by the `server` tag  followed by curvy brackets including the configuration of this one server.
```
server {
    //... your configuration here
}
```
#### :cookie: Choose the port and host of each "server"
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

#### :cookie: Setup the server_names (optional)

#### :cookie: setup default error pages
you can set default error page for error from 300 to 599 (as nginx does). This is going to make a redirection to the choosen page.
```
error_page 404 /error/404.html;
error_page 501 502 503 /error/50x.html;
```

#### :cookie: limit client body size

#### :cookie: Location block (setup routes with one or multiple of the following rules/configuration (routes wont be using regexp))
Like `server` blocks, `location` blocks begin and end with curvy brackets. Each `location` is defined with a `modifier` (optional) and an `URI` specification. Unlike Nginx, `.` for any char and wildcard `*` and `?` and `$any_var` are not implemented (nothing including regex).

```
location [modifier] [URI] {
  ...
  ...
}
```
If no location is found, the webserv will use default settings;
`modifier` can be :
- none: If no modifiers are present in a location block then the requested URI will be matched against the beginning of the requested URI.
- `=`: The equal sign is used to match a location block exactly against a requested URI.
- `~`: The tilde sign is used for case-sensitive regular expression match against a requested URI.
- `~*`: The tilde followed by asterisk sign is used for case insensitive regular expression match against a requested URI.

`location` can be defined by any route that match the requested `URI`. `$` symbol is used at the end of a route to say that nothing can be found after the last char.

The following settings can only be defined inside a `location` block.

##### :cookie: define a list of accepted HTTP Methods for the route
By default `GET` and `POST` methods are accepted, `DELETE` must be allowed.
Both tags `allow_methods` and `limit_methods` can be used. 
if `limit_methods` is used, not included methods can't be allowed.
If `DELETE` is in limited methods, you'll still need to allow it with `allow_methods`.

##### :cookie: define an HTTP redirection
Redirection are set with the tag `return`. (it can be also with `error_page`). It is set if the page is temporarly (302) or permanently moved (301);
```
return 301 @newlocation;
return 301;
```

##### :cookie: define a directory or a file from where the file should be search (for example if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is /tmp/www/pouic/toto/pouet)
with the `root` tag, define a directory with absolute path. If no root is choosen, it goes by default on the /www folder of the project.

##### :cookie: turn on or off directory listing
If no default file is found in this location and you requested a folder, then you can set an auto index with the `autoindex` tag.

```
autoindex on;
# OR
autoindex off; #->> default setting
```

##### :cookie: default file to answer if the request is a directory
Can be several file after the `index` tag. If none is set, you'll have to type the page's name by hand.

##### :cookie: execute CGI based on certain file extension (for example .php)
with `CGI_path` tag followed with an absolute path to the binary file.
```
location ~ .php$ {
        CGI_path /Users/xxx/.brew/bin/php-cgi;
}
```
