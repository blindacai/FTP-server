#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <regex.h>


/*
    .
    ..
    /.
    /..
    ./
    ../
    /./
    /../
*/

int main(){
    regex_t regex;
    int reti;
    char msgbuf[100];

/* Compile regular expression */
    //reti = regcomp(&regex, "^a[[:alnum:]]", 0);     // not sure
    //reti = regcomp(&regex, "a[[:digit:]]b", 0);     // a3b
    //reti = regcomp(&regex, "[hc]at", 0);     // hat, cat
    //reti = regcomp(&regex, "\\[.\\]", 0);     // [a]
    //reti = regcomp(&regex, "a\\{1\\}", 0);  // anything?
    //reti = regcomp(&regex, "ab*c", 0);     // abbbbc, ac
    //reti = regcomp(&regex, "^a{2,3}$", REG_EXTENDED);  // turn on extended mode  aa, aaa
    reti = regcomp(&regex, "/{0,1}.{0,1}[.]/{0,1}", REG_EXTENDED);     // 

    if(reti){ 
        fprintf(stderr, "Could not compile regex\n"); 
        exit(1); 
    }

/* Execute regular expression */
    reti = regexec(&regex, "...", 0, NULL, 0);
    if(!reti){
        puts("Match");
    }
    else if(reti == REG_NOMATCH){
        puts("No match");
    }
    else{
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        exit(1);
    }

/* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);

    return 0;
}