// HttpResponse.h

#ifndef _HTTPRESPONSE_h
#define _HTTPRESPONSE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

class HttpResponseClass
{ 
 public:
	int getStatus(char* response);
	char* getHeader(char* search, char* response);
	char* getBody(char* response);
	char* strtokm(char *str, const char *delim);

private:
	
};

extern HttpResponseClass HttpResponse;
#endif

