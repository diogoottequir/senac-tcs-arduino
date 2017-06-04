// 
// 
// 

#include "HttpResponse.h"

int HttpResponseClass::getStatus(char* response)
{	
	String txt = response;
	return txt.substring(9, 12).toInt();
}

char* HttpResponseClass::getHeader(char* search, char* response)
{
	//char* responseTemp = "";
	//responseTemp = (char*) calloc(strlen(response) + 1, sizeof(char));
	//strcpy(responseTemp, response);
	
	char lb[] = "\r\n";
	char* end_str;
	char* lines = strtok_r(response, lb, &end_str);
	while (lines)
	{
		char *end_token;
		char *header = strtokm(lines, ": ");
		bool found = false;
		int i = 0;
		while (header != NULL)
		{
			if (found == true) {
				return header;
			}
			if (strstr(header, search) && i == 0) {
				found = true;
			}
			i++;
			header = strtokm(NULL, ": ");
		}
		lines = strtok_r(NULL, lb, &end_str);
	}
	return "";
}

char* HttpResponseClass::getBody(char* response)
{
	//char* responseTemp = "";
	//responseTemp = (char*) calloc(strlen(response) + 1, sizeof(char));
	//strcpy(responseTemp, response);
	
	char lb[] = "\r\n\r\n";
	char* lines = strtokm(response, lb);
	int i = 0;
	while (lines)
	{
		if (i == 1) {
			return lines;
		}
		i++;
		lines = strtokm(NULL, lb);
	}
	return "";
}

char* HttpResponseClass::strtokm(char *str, const char *delim)
{
	static char *tok;
	static char *next;
	char *m;

	if (delim == NULL) return NULL;

	tok = (str) ? str : next;
	if (tok == NULL) return NULL;

	m = strstr(tok, delim);

	if (m) {
		next = m + strlen(delim);
		*m = '\0';
	}
	else {
		next = NULL;
	}

	return tok;
}

HttpResponseClass HttpResponse;
