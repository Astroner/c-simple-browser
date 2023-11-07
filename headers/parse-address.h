#if !defined(PARSE_ADDRESS_H)
#define PARSE_ADDRESS_H

typedef struct ParsedAddress {
    struct {
        int start;
        int length;
    } host;
    struct {
        int start;
        int length;
    } path;
    struct {
        int start;
        int length;
    } query;
    int port;
} ParsedAddress;

int parseAddress(char* src, ParsedAddress* result);

#endif // PARSE_ADDRESS_H
