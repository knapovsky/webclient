# Webclient

![ArticleTitle](http://images.knapovsky.com/http-request.png)

Webový klient pro zjištění informací o objektu. Klient využívá sockets API pro získání informací zadaného objektu pomocí URL z WWW serveru s využitím HTTP 1.1. Požadované informace jsou vytištěny na standartní výstup.

## Použití
```
./webinfo \[-l\] \[-s\] \[-m\] \[-t\] \[-h\]URL
\[-l\]		velikost objektu
\[-s\]		identifikace serveru
\[-m\]		informace o posledni modifikaci objektu
\[-t\]		typ obsahu objektu
\[-h\]		vypsani napovedy
```