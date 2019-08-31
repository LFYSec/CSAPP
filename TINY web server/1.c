#include "csapp.h"

int main(){

    char buf[10] = {"abcdaaaa"};

    //write(STDIN_FILENO, buf, strlen(buf));

    sprintf(buf, "%s", "123\n123");

    printf("%s", buf);

    return 0;
}