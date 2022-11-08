////////////////////////////////////////////////////////////////////////
//	Projekt #1 predmetu IPK
//	soubor: main.h
//
//	17.3.2011
//	Autor:	Martin Knapovsky
//			xknapo02@stud.fit.vutbr.cz
//		
//	Webovy klient pro zjisteni informaci o objektu
//			- vyuziva sockets API pro ziskani informaci zadaneho
//			  objektu pomoci URL z WWW serveru s vyuzitim HTTP 1.1.
//			- pozadovane informace budou vytisteny na standartni vystup
//
//	Pouziti:./webinfo [-l] [-s] [-m] [-t] [-h]URL
//			[-l]		velikost objektu
//			[-s]		identifikace serveru
//			[-m]		informace o posledni modifikaci objektu
//			[-t]		typ obsahu objektu
//			[-h]		vypsani teto napovedy
////////////////////////////////////////////////////////////////////////


#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

//#define DEBUG			//zapina debugovaci vystup
#define PROTOCOL (char*)"HTTP/1.1"	//Pouzivany protocol
#define MAX_REDIRECTION 10
#define HTTP_OK 200
#define WAITTIME 0
#define TIMEOUT 10

#define HELP	(char*)"-h"
#define SIZE	(char*)"-l"
#define MINFO	(char*)"-m" 
#define ID		(char*)"-s"
#define TYPE	(char*)"-t"

#define SUCCESS 0
#define SYNTAX_ERROR -1
#define CLIENT_ERROR -2
#define CONNECTION_ERROR -3
#define COMMUNICATION_ERROR -4

#define BUFFERSIZE 1000	//velikost prijimaciho bufferu

#define CODE_BEGIN 9 //pozice http answer kodu

#define GET		10
#define	POST	11
#define HEAD	12
#define PUT		13
#define OPTIONS	14
#define TRACE	15

#define OBJECT_SIZE 21
#define SERVER_ID 22
#define LAST_MOD 23
#define OBJECT_TYPE 24

using namespace std;

//Struktura pro praci s url adresou
typedef struct t_url{
	string scheme;
	string authority;
	string path;
	string query;
	string fragment;
	string request;

	int port;
} t_url;
	
/** Vytiskne napovedu programu 
 */
void print_help();
/** Urci, zda je v poli dany retezec
 *  @param char* argv[] - prohledavane pole retezcu
 *  @param string str - hledany retezec
 */
int includes_str(char *, int , char *);

/** Funkce tiskne strukturu t_url na vystup
 */
void print_url(t_url *);

/** Funkce ziska ze zadaneho argumentu jmeno serveru
 *  a cestu k objektu na serveru
 *  @param string *hostname - jmeno vzdaleneho serveru
 *  @param string *object	- cesta k objektu na vzdalenem serveru
 */
void parse_url(string *, t_url *);
 
/**Funkce vytiskne radek obsahujici na zacatku retezec line_contains
 * pokud retezec line_contains neni pocatkem, bude radek vytisknut od nej.
 * @string *answer - prohledavany retezec
 * @char *line_contains - vyhledavany retezec
 */
void print_line(string *, int );

/**Funkce je pouzivana pri http kodech 3xx Redirection
 * pro ziskani nove adresy
 * @param string *answer - odpoved ze serveru
 * @param string *location - retezec pro ulozeni nove adresy
 */
void get_location_link(string *, string *);
/**Funkce sestavuje dotaz pro server
 */
void build_request(t_url *, string *);

/**Ziska z odpovedi serveru kod zpravy http
 */
int get_http_code(string *, string *);

/**Funkce tiskne odpoved ze serveru na standartni vystup
 */
void print_answer(string *);

/**Funkce tiskne strukturu hostent na standartni vystup
 */
void print_hostent(hostent *);

/**Funkce odesle dotaz na server a dostane odpoved
 * @param int client_socket - klientsky socket
 * @param string *request - dotaz
 * @param string *answer - odpoved
 */
int send_and_receive(string *, string *, string *);
