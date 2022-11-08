////////////////////////////////////////////////////////////////////////
//	Projekt #1 predmetu IPK
//	soubor: main.c
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
			
#include "main.h"

using namespace std;

/** Vytiskne napovedu programu 
 */
void print_help()
{
	cout << "Pouziti:./webinfo [-l] [-s] [-m] [-t] [-h] URL" << endl
		 << "\t\t  " << "[-l] velikost objektu" << endl
		 << "\t\t  " <<  "[-s] identifikace serveru" << endl
		 << "\t\t  " <<  "[-m] informace o posledni modifikaci objektu" << endl
		 << "\t\t  " <<  "[-t] typ obsahu objektu" << endl
		 << "\t\t  " <<  "[-h] vypsani teto napovedy" << endl;
		 
	return;
}

void wait_some_time(int time){
		
		for(int i = 0; i < time; i++) ;
		return;
}

/** Urci, zda je v poli dany retezec
 *  @param char* argv[] - prohledavane pole retezcu
 *  @param string str - hledany retezec
 */
int includes_str(char *argv[], int argc, char *string)
{
	bool present = false; //defaultne se v poli dany retezec nenachazi
	for(int i = 1; i < argc; i++)
	{
		if((strcmp(argv[i], string)) == 0)
		{
			present = i;
		}
	}
	return present;
}

/** Funkce tiskne strukturu t_url na vystup
 */
void print_url(t_url *parsed_url)
{
	cout << "url scheme: " << parsed_url->scheme << endl;
	cout << "authority: " << parsed_url->authority << endl;
	cout << "path: " << parsed_url->path << endl;
	cout << "query: " << parsed_url->query << endl;
	cout << "nose: " << parsed_url->fragment << endl;	
	cout << "request: " << parsed_url->request << endl;
	cout << "port: " << parsed_url->port << endl;	
	return;
}

/** Funkce ziska ze zadaneho argumentu jmeno serveru
 *  a cestu k objektu na serveru
 *  @param string *hostname - jmeno vzdaleneho serveru
 *  @param string *object	- cesta k objektu na vzdalenem serveru
 */
 void parse_url(string *urlstream, t_url *parsed_url)
 {
	 #ifdef DEBUG
		cout << endl << "#PARSING URL: " << *urlstream << endl;
	 #endif
	 
	 size_t position;
	 size_t offset = 0;
	 
	 string port;
	 size_t port_start = 0;
	 bool  port_presence = false;
	  
	 position=(urlstream->find("//", offset));
	 if(position!=string::npos){
		 parsed_url->scheme.assign(*urlstream, offset, position + 2);
		 offset = position + 2;
	}

	port_start=urlstream->find(":", offset);
	if(port_start!=string::npos){
	  port_presence = true;
	}
	 	
	position=urlstream->find("/", offset);
	if(position!=string::npos){
		if(port_presence){
			port.assign(*urlstream, port_start + 1, position - port_start - 1);
			parsed_url->port = atoi(port.c_str());
			parsed_url->authority.assign(*urlstream, offset, position - offset - (position - port_start - 1) - 1);
		}
		else{
			parsed_url->authority.assign(*urlstream, offset, position - offset);
			parsed_url->request.assign(*urlstream, position, urlstream->length() - position);
			parsed_url->port = 80;
		}
		offset = position + 1;
	}
	
	
	position=urlstream->find("?");
	if(position!=string::npos){
		parsed_url->path.assign(*urlstream, offset, position - offset);
		offset = position + 1;
	}
	
	position=urlstream->find("#", offset);
	if(position!=string::npos){
		parsed_url->query.assign(*urlstream, offset, position - offset);
		offset = position + 1;
	}
	
	if(parsed_url->authority.length() != 0){
		parsed_url->fragment.assign(*urlstream, offset, urlstream->length() - offset);
	}
	else if(!port_presence){
		parsed_url->authority.assign(*urlstream, offset, urlstream->length() - offset);
		parsed_url->port = 80;
	}
	else if(port_presence){
		parsed_url->authority.assign(*urlstream, offset, port_start - offset);
		port.assign(*urlstream, port_start + 1, urlstream->length() - port_start - 1);
		parsed_url->port = atoi(port.c_str());
	}
	
	if(parsed_url->request.length() == 0) parsed_url->request = "/";
	
	#ifdef DEBUG
		print_url(parsed_url);
		cout << "#PARSING DONE" << endl << endl;
	#endif
	
	return;
}

/**Funkce vytiskne radek obsahujici na zacatku retezec line_contains
 * pokud retezec line_contains neni pocatkem, bude radek vytisknut od nej.
 * @string *answer - prohledavany retezec
 * @char *line_contains - vyhledavany retezec
 */
void print_line(string *answer, int type){
	size_t offset = 0;
	size_t line_end = 0;
	string line_contains;
	string out;

	switch (type){
		case OBJECT_SIZE: 	line_contains = "Content-Length"; break;
		case SERVER_ID:		line_contains = "Server"; break;
		case LAST_MOD:		line_contains = "Last-Modified"; break;
		case OBJECT_TYPE:	line_contains = "Content-Type"; break;
	}
	
	offset = answer->find(line_contains.c_str());
	if(offset!=string::npos){
		line_end = answer->find('\n', offset);
		if(line_end!=string::npos){
			out.assign(*answer, offset, line_end - offset);
			cout << out << endl;
		}
		else{
			cerr << "end of line containing: " << line_contains << " not found" << endl;
		}
	}
	else{
		cerr << "requested information: " << line_contains << " not found" << endl;
	}
	
	return;
}

/**Funkce je pouzivana pri http kodech 3xx Redirection
 * pro ziskani nove adresy
 * @param string *answer - odpoved ze serveru
 * @param string *location - retezec pro ulozeni nove adresy
 */
void get_location_link(string *answer, string *location){
	
	size_t offset = 0;
	size_t line_end = 0;
	
	location->clear();
	
	offset = answer->find("Location");
	if(offset!=string::npos){
		line_end = answer->find('\n', offset + 11);
		if(line_end!=string::npos){
			location->assign(*answer, offset + 10, line_end - offset - 11);
			#ifdef DEBUG
				cout << "#New location link: " << location->c_str() << endl;
			#endif
		}
	}
	
	return;
} 

/**Funkce sestavuje dotaz pro server
 */
void build_request(t_url *url, string *request){
	string new_request;
	
	new_request.append("HEAD ");
	new_request.append(url->request.c_str());
	new_request.append(" ");
	new_request.append(PROTOCOL);
	new_request.append("\r\n");
	new_request.append("Host: ");
	//new_request.append(url->scheme.c_str());
	new_request.append(url->authority.c_str());
	//new_request.append("/");
	new_request.append("\r\nConnection: close");
	new_request.append("\r\n\r\n");
	
	#ifdef DEBUG
		cout << "#REQUEST:\n"<< new_request.c_str() << "#END OF REQUEST\n\n";
	#endif

	*request = new_request;
	
	return;
}

/**Ziska z odpovedi serveru kod zpravy http
 */
int get_http_code(string *answer, string *answer_code_text){
	int answer_code_end_line = 0;
	int answer_icode = 0;
	string answer_code;
	
	answer_code_end_line = answer->find('\n', CODE_BEGIN);
	answer_code_text->assign(*answer, CODE_BEGIN, answer_code_end_line - CODE_BEGIN);
	answer_code.assign(*answer, CODE_BEGIN, CODE_BEGIN + 2);
	answer_icode = atoi(answer_code.c_str());
	#ifdef DEBUG
		cout << "#New http code generated: " << answer_icode << endl;
	#endif 
	
	return answer_icode;
}

/**Funkce tiskne odpoved ze serveru na standartni vystup
 */
void print_answer(string *answer){
	int offset = answer->find("\r\n\r\n");
	string print;
	copy(answer->begin(), answer->begin() + offset, print.begin());
	cout << print.c_str() << endl;
	
	return;
}

/**Funkce tiskne strukturu hostent na standartni vystup
 */
void print_hostent(hostent *host){
	int i = 0;
	
	cout << "#HOSTENT" << endl;
	cout << "Host name: " << host->h_name << endl;
	while(host->h_aliases[i] != NULL){
		cout << "Aliases:\n" << "\t" <<host->h_aliases[i] << "\n" << endl;
		i++;
	}
	cout << "Host addrtype: " << host->h_addrtype << endl;
	cout << "Length: " << host->h_length << endl;
	i = 0;
	while(host->h_addr_list[i] != NULL){
		cout << "Address list:\n" << "\t" <<host->h_addr_list[i] << "\n" << endl;
		i++;
	}
	cout << "#END_OF_HOSTENT\n" << endl;
	return;
}

void catch_alarm(int sig)
{
	cerr << "receiving timeout" << endl;
	exit(1);
}

/**Funkce odesle dotaz na server a dostane odpoved
 * @param int client_socket - klientsky socket
 * @param string *request - dotaz
 * @param string *answer - odpoved
 */
int send_and_receive(string *urlstream, string *answer, string *answer_code_text){
	string request;
	char buffer[BUFFERSIZE];		//prijimaci buffer
	int size = 0, total_size = 0;		//pocet prijatych a odeslany bytu
	hostent *host;					//vzdaleny pocitac
	sockaddr_in server_socket;		//socket vzdaleneho pocitace
	int client_socket = 0;				//socket klienta
	t_url url;
	int answer_icode = 0;
	
	signal(SIGALRM, catch_alarm);
	  
	//Parsovani URL a sestaveni requestu--------------------------------
	parse_url(urlstream, &url);
	build_request(&url, &request);
	#ifdef DEBUG
		cout << "#URL_AUTHORITY: " << url.authority.c_str() << endl;
	#endif
	//------------------------------------------------------------------
	//Preklad adresy----------------------------------------------------
	if((host = gethostbyname(url.authority.c_str())) == NULL)
	{
		cerr << "wrong destination address" << endl;
		return SYNTAX_ERROR;
	}
	#ifdef DEBUG
		cout << "#Hostname resolved"  << endl;
		print_hostent(host);
	#endif
	//------------------------------------------------------------------
	//Vytvoreni socketu-------------------------------------------------
	if((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		cerr << "cannot create socket" << endl;
		return 	CLIENT_ERROR;
	}
	#ifdef DEBUG
		cout << "#Socket created, socket number: " << client_socket << endl;
	#endif
	//------------------------------------------------------------------
	//Naplneni socaddr_in-----------------------------------------------
	server_socket.sin_family = AF_INET;		//rodina protokolu
	server_socket.sin_port = htons(url.port);	//cislo portu, ke kteremu se pripojime
	////nastaveni IP adresy, ke ktere se pripojime
	memcpy(&(server_socket.sin_addr), host->h_addr, host->h_length);
	/*------------------------------------------------------------------
	 * Pripojeni socketu
	 */
	if(connect(client_socket, (sockaddr *)&server_socket, sizeof(server_socket)) == -1)
	{
		cerr << "connection establishment failed" << endl;
		return CONNECTION_ERROR;
	}
	#ifdef DEBUG
		cout << "#Connection established" << endl;
	#endif
	#ifdef DEBUG
		cout << "#TRYING TO SEND REQUEST:\n" << request.c_str() << "#END OF REQUEST" << endl;
	#endif
	//------------------------------------------------------------------
	/*------------------------------------------------------------------
	 * Odesilani dat 
	 */
	if((size = send(client_socket, request.c_str(), request.size() + 1, 0)) == -1)
	{
		cerr << "sending problem" << endl;
		return COMMUNICATION_ERROR;
	}
	#ifdef DEBUG
		cout << "#Data sent" << endl;
	#endif
	//------------------------------------------------------------------	
	/*------------------------------------------------------------------
	 * Prijimani dat a ukladani do bufferu
	 */
  	
	alarm(5);
	while(((size = recv(client_socket, buffer, BUFFERSIZE - 1, 0)) != -1) && (size != 0))
	{
		buffer[size] = '\0';
		total_size += size;
		*answer += buffer;
	}
	
	/*
	int n;
	char buf[100];
	int checker = 0;
	int cunter = 0;
	string resp_head = "";

	while((n = read(client_socket, buf, 1)) > 0){
		buf[1] = '\0';
		resp_head.append(buf);
		cunter++;
	}
	
	*answer = resp_head;
	*/
		
	#ifdef DEBUG
		cout << "#Received: " << total_size << " bytes" << endl;
		cout << "#ANSWER:\n" <<  endl;
		print_answer(answer);
		cout << "#END_OF_ANSWER\n" << endl;
	#endif
	//------------------------------------------------------------------	
	close(client_socket);										//Uzavreni socketu
	answer_icode = get_http_code(answer, answer_code_text);		//Generovani nove vraceneho HTTP status kodu
	return answer_icode;
}


/**Hlavni funkce programu
 * @param argc pocet parametru prikazove radky
 * @param argv pole retezcu obsahujici jednotlive parametry
 */
int main(int argc, char *argv[])
{
	string request;					//dotaz, ktery budeme odesilat na server
	string answer;					//string pro odpoved ze serveru
	string urlstream;				//zde bude ulozeno jmeno vzdaleneho serveru;
	string answer_code_text;		//text kodu odpovedi
	int	answer_icode;				//kod odpovedi
	t_url url;						//struktura zpracovavane url
	int redirection = 0;
	
	/*------------------------------------------------------------------
	 * Pocatecni zpracovani parametru
	 */
	if((argc == 2 && argv[1][0] == '-') || (argc == 1)){
		print_help();
		return SUCCESS;
	}
	else if(argc > 1)
	{
		urlstream = argv[argc - 1];
	}
	//------------------------------------------------------------------
	
	/*------------------------------------------------------------------
	 * Dotaz HEAD na zadany server, kontrola vraceneho HTTP status kodu
	 * a jeho nasledne zpracovani a pri presmerovani (status kod 301, 302)
	 * cekani na kod 2xx
	 */
	if(((answer_icode = send_and_receive(&urlstream, &answer, &answer_code_text)) != HTTP_OK)){
	do{
	if(redirection++ >= MAX_REDIRECTION){
		cerr << "maximal redirection reached (" << MAX_REDIRECTION << ")" << endl;
		return COMMUNICATION_ERROR;
	}
	//3xx Redirection
	#ifdef DEBUG
		cout << "#ANSWER_ICODE: " << answer_icode << endl;
		cout << "#REDIRECTION: " << redirection << endl; 
	#endif
	
	if(answer_icode >= 300 && answer_icode <= 307){
		switch (answer_icode){
			case 300: cerr << answer_code_text.c_str() << endl; 
					  return SUCCESS; break;
			case 301: get_location_link(&answer, &urlstream);
						answer.clear();
					  if(send_and_receive(&urlstream, &answer, &answer_code_text) < SUCCESS){
						return COMMUNICATION_ERROR;
					  }
					  break;
			case 302: 
			      get_location_link(&answer, &urlstream);
						answer.clear();
					  if(send_and_receive(&urlstream, &answer, &answer_code_text) < SUCCESS){
						return COMMUNICATION_ERROR;
					  }
					  break;
			case 303: cerr << answer_code_text.c_str() << endl; 
					  return SUCCESS; break;
			case 304: cerr << answer_code_text.c_str() << endl; 
					  return SUCCESS; break;
			case 305: cerr << answer_code_text.c_str() << endl; 
					  return SUCCESS; break;
			case 306: cerr << answer_code_text.c_str() << endl; 
					  return SUCCESS; break;
			case 307: cerr << answer_code_text.c_str() << endl; 
					  return SUCCESS; break;
		}
	}			  
	//4xx Client Error Codes
	else if(answer_icode >= 400 && answer_icode <= 417){
		cerr << answer_code_text.c_str() << endl; 
		return SUCCESS;
	}
	//5xx Server Error Codes
	else if(answer_icode >= 500 && answer_icode <= 505){
		cerr << answer_code_text.c_str() << endl; 
		return SUCCESS;
	}
	}while(!(answer_icode >= 200 || answer_icode <= 206));
	}
	//------------------------------------------------------------------
	
	
	#ifdef DEBUG
		cout << "#ANSWER_ICODE: " << answer_icode << endl;
		cout << "#REDIRECTION: " << redirection << endl;
	#endif
	
	/*------------------------------------------------------------------
	 * Vypis programu na standartni vystup pomoci funkce print_line()
	 * podle zadanych parametru prikazove radky
	 */
	for(int i = 1; i < argc; i++){
		//Pokud se jedna o parametr
		if(argv[i][0] == '-'){
			switch (argv[i][1]){
				case 'h': print_help(); return SYNTAX_ERROR; break;
				case 'l': print_line(&answer, OBJECT_SIZE); break;
				case 's': print_line(&answer, SERVER_ID); break;
				case 'm': print_line(&answer, LAST_MOD); break;
				case 't': print_line(&answer, OBJECT_TYPE); break;
				default  : cerr << "wrong parameter" << endl; break;
			}
		}
		else if(argc == 2){
				print_answer(&answer);
		}
	}
	//------------------------------------------------------------------
			
	return SUCCESS;
}
	
	
	
