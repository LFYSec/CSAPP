#include "../csapp.h"

int main(void){

    char *buf, *p;
    int n1, n2;
    char arg1[MAXLINE], arg2[MAXLINE];
    char header[MAXLINE], content[MAXLINE];

    //setenv("QUERY_STRING", "1&2", 1);

    if((buf=getenv("QUERY_STRING")) != NULL){
        p = strchr(buf, '&');
        *p = '\0';
        strcpy(arg1, buf);
        strcpy(arg2, p+1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
    }

    sprintf(content, "QUERY_STRING=%s\r\n", buf);
    sprintf(content, "%sResult is: %d", content, n1+n2);
    
    printf("Connection: close\r\n");
    printf("Content-length: %d\r\n", strlen(content));
    printf("Content-type: text/html\r\n\r\n");

    printf("%s", content);

    fflush(stdout);

    exit(0);
}