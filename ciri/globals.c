#include "globals.h"

int strcontains(char *s1, char *s2) {
    return strstr(s1, s2) != NULL ? 1 : 0;
}
int strequals(char *s1, char *s2) {
    return strcmp(s1, s2) == 0 ? 1 : 0;
}
int strstartswith(char *s1, char *s2) {
    size_t lenpre = strlen(s2),
    lenstr = strlen(s1);
    return lenstr < lenpre ? 0 : strncmp(s2, s1, lenpre) == 0;
}
void strtrim(char *s) {
    int i = (int)strlen(s)-1;
    while (s[i] == ' ' || s[i] == '\n') i--;
    s[i+1] = '\0';
    
    while (s[0] == ' ' || s[0] == '\n') {
        s++;
    }
}