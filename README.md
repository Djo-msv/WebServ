*this project has been created as part of the 42 curriculum by [csenelle](https://github.com/Kamisenin), [jmassavi](https://github.com/Djo-msv) and [naorakot](https://github.com/styx-trav)*

# Webserv

-  [Description](#description)
-  [General Instructions](#general-instructions)
    - [Compilation](#compilation)
    - [Running WebServ](#running-webserv)
    - [Accessing the Server](#accessing-the-server)
-  [Documentation](#documentation)
    -  [MYML Configuration File](#myml-configuration-file)
    -  [Mandatory arguments](#mandatory-arguments)
    -  [Optional arguments](#optional-arguments)
          -  [HTTP Methods](#http-methods)
          -  [Locations](#locations)
          -  [CGI Handlers](#cgi-handlers)
          -  [Custom Error Files](#custom-error-files)
    -  [Full example](#full-example)
-  [Resources](#resources)

## Description

> Webserv is one of the group project from 42 main Cursus aiming to recreate an HTTP server in C++98, building over the TCP/UDP protocol using sockets and epoll to register clients. The main instruction is to follow one the HTTP RFC, most of the people aim for the RFC of HTTP 1.0, we based most of our source documentation form the RFC of HTTP 1.1

## General Instructions

### Compilation

The project is compiled using `make`. The following targets are available:

```bash
make          # Compile the project
make clean    # Remove object files
make fclean   # Remove object files and the binary
make re       # Full recompilation
```

> Requires a C++98-compatible compiler (`c++`) and a Linux environment (uses `epoll`).

### Running WebServ

Once compiled, run the server by passing a configuration file or a folder containing `.myml` configuration files:

```bash
./webserv <configuration_file.myml>
# or
./webserv <configuration_folder/>
```

Example:

```bash
./webserv conf/server1.myml
```

Stop the server at any time with `Ctrl+C`.

### Accessing the Server

Once the server is running, you can reach it in your browser or with a client like `curl`:

- **Locally (same machine):** `http://localhost:<port>`
- **On a local network:** retrieve the machine's IP address with `ip a`, then use `http://<ip_address>:<port>`

For the full configuration reference, see the [Documentation](#documentation) section below.

---

## Documentation

### MYML Configuration File

WebServ uses a custom YAML-like format called **MYML** (`.myml`) to configure servers. Indentation is used to define hierarchy (use tabs or spaces).

A configuration file can define one or more servers. Each server is declared as a top-level dictionary:

```
<server_name> :
    <key> : <value>
    ...
```

---

#### Mandatory arguments

| Argument | Type   | Description                                         |
|----------|--------|-----------------------------------------------------|
| `port`   | number | The port the server will listen on.                 |
| `root`   | string | The root folder from which files will be served.   |

---

#### Optional arguments

| Argument            | Type           | Description                                                                                 |
|---------------------|----------------|---------------------------------------------------------------------------------------------|
| `index`             | string         | Default file to serve when a directory is requested (e.g. `/html/index.html`).             |
| `timeout`           | number         | Connection timeout in seconds. Defaults to `15`.                                            |
| `allow_methods`     | string or list | HTTP methods allowed at the root level. Accepted values: `GET`, `POST`, `DELETE`.          |
| `locations`         | list           | List of location blocks (see [Locations](#locations)).                                      |
| `cgi_handlers`      | list           | Maps file extensions to CGI executables (see [CGI Handlers](#cgi-handlers)).               |
| `execution_folder`  | string         | Path (relative to root) of the folder used for CGI script execution.                        |
| `error_files`       | list           | Maps HTTP error codes to custom error page file paths (see [Error Files](#error-files)).   |


---

##### HTTP Methods

HTTP Methods can be specified as a single value or as a MYML list:

```
# Single method
allow_methods : GET

# Multiple methods
allow_methods :
    - GET
    - POST
    - DELETE
```
> **Note:** : If placed at the root repository, it will act as default for all location. On the contrary if allowed_methods is not specified. No methods will be allowed by
> default and the server's index won't be accessible  

---

##### Locations

The `locations` block is a list of path-based or extension-based routing rules. Each entry is a dictionary keyed by a URL path (e.g. `/uploads`) or a file extension (e.g. `.py`).

```
locations :
	- path :
        allow_methods : GET
        index : index.html
        root : ./other_folder       # overrides root for this location
        alias : ./another_folder    # alternative to root (cannot be used with root)
    - .other :
        allow_methods : POST
```

| Location key     | Type           | Description                                                                                                              |
|------------------|----------------|--------------------------------------------------------------------------------------------------------------------------|
| `allow_methods`  | string or list | HTTP methods allowed for this location. Accepted values: `GET`, `POST`, `DELETE`. Overrides the server root allow_method |
| `index`          | string         | Default file to serve for this location.                                                                                 |
| `root`           | string         | Overrides the server root for this location. The location path is appended. Cannot be used with `alias`.                 |
| `alias`          | string         | Sets a fixed folder path for this location without appending the location path. Cannot be used with `root`.              |

> **Note:** `root` and `alias` are mutually exclusive within the same location block.

---

##### CGI Handlers

The `cgi_handlers` block maps file extensions to the executable that will handle them:

```
cgi_handlers :
    - .py : /usr/bin/python3
    - .bla : ./cgi_tester
```

---

##### Custom Error Files

The `error_files` block maps HTTP status codes to custom HTML error pages:

```
error_files :
    - 404 : ./errors/404.html
    - 500 : ./errors/500.html
```

---

#### Full example

```
server1 :
    port : 7501
    root : ./server00
    index : /html/index.html
    timeout : 40
    allow_methods : GET
    locations :
        - /html :
            allow_methods : GET
        - /img :
            allow_methods : GET
        - /uploads :
            allow_methods : DELETE
        - /scripts :
            allow_methods :
                - GET
                - POST
    cgi_handlers :
        - .py : /usr/bin/python3
    execution_folder : /scripts
    error_files :
        - 404 : ./errors/404.html
```

---

## Resources

- Linux Manual (epoll, sockets)
- [RFC 2616 – HTTP/1.1](https://datatracker.ietf.org/doc/html/rfc2616)
- [RFC 9112 – HTTP/1.1 Message Syntax and Routing](https://www.rfc-editor.org/rfc/rfc9112.html)
- [French tutorial on Yaml syntax](https://blog.stephane-robert.info/docs/developper/autres-langages/yaml/) by Stephane Robert
- [Ansible Yaml documentation](https://docs.ansible.com/projects/ansible/latest/reference_appendices/YAMLSyntax.html)

> **AI usage:** AI tools were used strictly for documentation purposes — simplifying RFC comprehension, helping create an early socket-testing program to better understand socket behaviour, and assisting in writing this README.
