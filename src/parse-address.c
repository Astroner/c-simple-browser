#include "parse-address.h"
#include <stdio.h>

typedef enum ParsingResult {
    ParsingResultError = -1,
    ParsingResultTerminatedWithNul = 0,
    ParsingResultTerminatedWithSlash = 1,
    ParsingResultTerminatedWithSpace = 2,
    ParsingResultTerminatedWithColon = 3,
    ParsingResultTerminatedWithQM = 4
} ParsingResult;

ParsingResult parseHost(char* src, size_t* index, int* start, int* length) {
    *start = 0;
    *length = 0;

    ParsingResult result = ParsingResultTerminatedWithNul;

    char ch;
    while((ch = src[*index])) {
        if(ch == ' ') {
            if(*length == 0) {
                *start += 1;
            } else {
                result = ParsingResultTerminatedWithSpace;
                break;
            }
        } else if((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '.') {
            *length += 1;
        } else if(*length > 0) {
            if(ch == '/') {
                result = ParsingResultTerminatedWithSlash;
                break;
            } else if(ch == ':') {
                result = ParsingResultTerminatedWithColon;
                break;
            } else if(ch == '?') {
                result = ParsingResultTerminatedWithQM;
                break;
            } else {
                printf("Unexpected char %c\n", ch);
                return ParsingResultError;
            }
        } else {
            printf("Unexpected char %c\n", ch);
            return ParsingResultError;
        }

        *index += 1;
    }

    if(*length == 0) {
        return ParsingResultError;
    }

    return result;
}

ParsingResult parsePort(char* src, size_t* index, int* port) {
    *port = 0;

    int started = 0;

    ParsingResult result = ParsingResultTerminatedWithNul;

    char ch;
    while((ch = src[*index])) {
        if(ch == ' ') {
            if(!started) {
                return ParsingResultError;
            } else {
                return ParsingResultTerminatedWithSpace;
            }
        } else if(ch >= '0' && ch <= '9') {
            started = 1;

            *port *= 10;
            *port += ch - '0';
        } else if(started) {
            if(ch == '/') {
                result = ParsingResultTerminatedWithSlash;
                break;
            } else if(ch == '?') {
                result = ParsingResultTerminatedWithQM;
                break;
            } else {
                printf("Unexpected char %c\n", ch);
                return ParsingResultError;
            }
        } else {
            printf("Unexpected char %c\n", ch);
            return ParsingResultError;
        }

        *index += 1;
    }

    if(!started) {
        *port = -1;
    }

    return result;
}

int parseAddress(char* src, ParsedAddress* result) {
    result->port = -1;

    size_t index = 0;

    ParsingResult status;

    if((status = parseHost(src, &index, &result->host.start, &result->host.length)) == ParsingResultError) {
        return -1;
    }

    if(status == ParsingResultTerminatedWithColon) {
        index++;
        if((status = parsePort(src, &index, &result->port)) == ParsingResultError) {
            return -1;   
        }
    }

    if(status == ParsingResultTerminatedWithSlash) {
        return 0;
    }

    if(status == ParsingResultTerminatedWithQM) {
        return 0;
    }

    return 0;
}