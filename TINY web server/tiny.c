#include "csapp.h"
#include "stdbool.h"

void doit(int connfd);
bool parse_uri(char *uri, char *filename, char *args);
bool check_auth(struct stat fstat);
void clienterror(int fd, char *code, char *msg, char *detail);

int main(int argc, char *argv[]){
    int listenfd, connfd, clientlen;
    char hostname[MAXLINE], port[MAXLINE];
    struct sockaddr_in clientaddr;

    if(argc != 2){
        fprintf(stderr, "usage: %s <port>", argv[0]);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1){
        connfd = Accept(listenfd, (SA *)&clientaddr, sizeof(clientaddr));
        doit(connfd);
        Close(connfd);
    }
    return 0;
}

void doit(int connfd){
    bool is_static;
    char buf[MAXLINE], uri[MAXLINE], method[MAXLINE], http_v[MAXLINE];
    char filename[MAXLINE], cgi_args[MAXLINE];
    struct stat fstat;
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);

    printf("Request header: \n%s", buf);
    sscanf(buf, "%s %s %s", method, uri, http_v);

    if(strcmp(method, "GET") != 0){
        clienterror(connfd, "405", "Method Not Allowed", "The request method is not allowed!");                          
        return;
    }

    is_static = parse_uri(uri, filename, cgi_args);

    if(!stat(filename, &fstat) < 0){
        clienterror(connfd, "404", "Not Found", "The file is not in this server!");
        return;
    }

    if(is_static){
        if(check_auth(fstat)){
            serve_static();
        }else{
            clienterror(connfd, "403", "Forbidden", "Forbidden to request this file");
            return;
        }
    }else{
        if(check_auth(fstat)){
            serve_dynamic();
        }else{
            clienterror(connfd, "403", "Forbidden", "Forbidden to request this file");
            return;
        }   
    }
}

void clienterror(int fd, char *code, char *msg, char *detail){
    char header[MAXLINE], body[MAXLINE];

    sprintf(header,"HTTP/1.1 %s %s\r\n", code, msg);
    Rio_writen(fd, header, strlen(header));
    sprintf(header, "Content-type: text/html\r\n");
    Rio_writen(fd, header, strlen(header));
    sprintf(header, "Content-length: %d\r\n\r\n", strlen(detail)+8);
    Rio_writen(fd, header, strlen(header));

    sprintf(body, "Reason: %s", detail);
    Rio_writen(fd, body, strlen(header));
}

bool parse_uri(char *uri, char *filename, char *cgi_args){
    char *ptr;

    if(strstr(uri, "cgi-bin")){
        cgi_args = "";
        ptr = index(uri, "?");
        if(ptr){
            strcpy(cgi_args, ptr+1);
            *ptr = '\0';
        }
        filename[0] = ".";
        strcat(filename, uri);
        return false;
    }else{
        filename[0] = ".";
        cgi_args = "";
        if(uri[strlen(uri)-1] == "/"){
            strcat(filename, "home.html");
        }else{   
            strcat(filename, uri);
        }
        return true;
    }
}

bool check_auth(struct stat fstat){
    return ((!S_ISREG(fstat.st_mode)) || (!(S_IRUSR & fstat.st_mode)));
}

void serve_static(){

}

void serve_dynamic(){}