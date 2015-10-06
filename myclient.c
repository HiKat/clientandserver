#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define NUMSTR  2

char *reqlines[NUMSTR] = {
    "Request from client\r\n",
    "\r\n"
};

//argcはコマンドラインオプションの数、argvはコマンドラインに入力された数値。
//コマンドラインからの引数としてはホスト名、ポート番号の順。
int main(){
    while(1){
        FILE *fp;
        char hostname[128];
        int i, s, Port;
        struct hostent *hp;//ホスト名とIPアドレスを扱うための構造体
        struct sockaddr_in sin;//ソケットを扱うための構造体
        char buf[128];
        ///////////////////////////////////////////////////////////////////////
        //ここからURL解析器
        char Url[999];
        char *Scheme;
        char *HostAndPort;
        char *HostName;
        char *PortNumber;
        char *Path;
        
        printf("Enter the URL>>"); 
        fgets(Url,sizeof(Url),stdin);
        Scheme = strtok(Url,"/");
        HostAndPort = strtok(NULL,"/");
        Path = strtok(NULL,"\n");
    
        HostName = strtok(HostAndPort,":");
        PortNumber = strtok(NULL,":");
        if(PortNumber == NULL){
            PortNumber = "80";
        }
        Port = atoi(PortNumber);

#ifdef DEBUG
        printf("Completed anlyzing the URL\n");
        printf("Host Name = %s \n",HostName);
        printf("Port Number = %s \n",PortNumber);
        printf("Path = %s \n",Path);
#endif    

        //ここまでURL解析器
        /////////////////////////////////////////////////////////////////////////
    
        //ホスト名が指定されていない場合
        if(HostName == NULL){
            perror("NO HOST NAME. ENTER THE CORRECT URL!");
            exit(1);
        }
    
        //ポート番号が指定されていない場合
        if(PortNumber == NULL){
            perror("NO PORT NUMBER. ENTER THE CORRECT URL!");
            exit(1);
        }

        //hpにIPアドレスを格納する。
        if ((hp = gethostbyname(HostName)) == NULL){
            fprintf(stderr, "%s: unknown host.\n", hostname);
            exit(1);
        }

        if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){//AF_INETはネットワークアドレスの種類。ソケットの生成。
            perror("client socket()");
            exit(1);		
        }
        bzero(&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(Port);
        memcpy(&sin.sin_addr, hp->h_addr, hp-> h_length);//IPアドレスの情報をソケットにコピーする。hpはIPアドレスを格納した変数。
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {//ソケット生成を行った後に接続を行う。
            perror("client connect()");
            exit(1);	
        }

        //クライアントとサーバのやりとりを行うための変数
        fp = fdopen(s, "r");
        char str0[9999];
        char str1[9999];

        // send request to server
        //reqlinesにhttpプロトコルを格納する。

        strcpy(str0,"GET /");
        strcat(str0,Path);
        strcat(str0," HTTP/1.1\r\n");

        strcpy(str1,"Host: ");
        strcat(str1,HostName);
        strcat(str1,"\r\n\r\n");
    
        reqlines[0] = str0;
        reqlines[1] = str1;
    
    
        //リクエストを送信する。
        for(i = 0; i < NUMSTR; i++){
            send(s, reqlines[i], strlen(reqlines[i]), 0);
        }
    
        // receive contents from server
        while (fgets(buf, sizeof(buf), fp) != NULL){
            printf("%s", buf);
        }
        close(s);
    }
    return 0;
}
 
