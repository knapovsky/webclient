# Webclient

A C++ command-line HTTP/1.1 web client for retrieving selected metadata about a remote object identified by URL.

This was originally created as an IPK networking coursework project.

![HTTP request](http://images.knapovsky.com/http-request.png)

## Features

The compiled binary is named `webinfo` and can print selected HTTP response information:

- object size / `Content-Length`
- server identification / `Server`
- last modification time / `Last-Modified`
- content type / `Content-Type`
- help text

The client uses sockets directly and follows HTTP redirects up to the limit defined in the source.

## Repository structure

```text
.
├── main.cpp       # Main implementation
├── main.h         # Constants, structs, and function declarations
├── Makefile       # Build rules
└── README.md
```

## Building

```bash
make
```

This creates:

```text
webinfo
```

Clean build output:

```bash
make clean
```

## Usage

```bash
./webinfo [-l] [-s] [-m] [-t] [-h] URL
```

Options:

```text
-l    print object size
-s    print server identification
-m    print last-modified information
-t    print content type
-h    print help
```

Examples:

```bash
./webinfo -s -t http://example.com/
./webinfo -l http://example.com/index.html
```

## Limitations

- designed for HTTP/1.1 over plain sockets
- HTTPS is not supported unless added separately
- old coursework code style
- limited error handling compared with modern HTTP clients

## License

No explicit license is included. Treat the code as all rights reserved unless a license is added by the repository owner.
