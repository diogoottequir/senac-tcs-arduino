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
	void begin(char* httpResponse);
	int getStatus();
	char* getHeader(char* search);
	char* getBody();
	char* strtokm(char *str, const char *delim);

private:
	char* response;
};

extern HttpResponseClass HttpResponse;
#endif

