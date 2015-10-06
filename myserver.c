#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define NUMSTR  3
///*
//#define DEBUG //複数クライアントからの同時接続時の動作確認用
//*/


char *strs[NUMSTR] = {
	"1\n",
	"2\n",
	"3\n"
};

int main(int argc,char* argv[]){//argcはコマンドラインからの引数の個数、argvはコマンドラインから入力した文字列。
	int i, s, ns, port;
    //sはソケットの生成に成功した時にsocket()から返される識別子。
    //nsは受け付けたソケットに与えられた識別子を返り値とするaccept()から返される値。
	struct sockaddr_in sin, fsin;

    //以下はプロセス分岐後子プロセスの処理で使用する変数。
    FILE *fp,*fp2;//クライアントからのリクエストを指すファイルポインタとクライアントから指定されたファイルを指すファイルポインタ。
    char *tp;//クライアントからのリクエスト文の先頭を指すポインタ。
    int pid;//各プロセスに与えられるIDを格納する変数。
    char buf[128],buf2[128];

    
	socklen_t fromlen = sizeof(struct sockaddr_in);
	
	if((argc <= 1) || ((port = atoi(argv[1])) == 0)) { //argcの個数が１以下の時またはポート番号が0の時はポート番号が入力されていないものとしてエラーを表示させる。
        perror("no port number");
        exit(1);		
	}

    //ソケット生成
	if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("client socket()");
		exit(1);		
	}
	
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;
	
    //ソケット登録
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("server bind()");
		exit(1);	
	}
	
    //ソケット接続準備
	if (listen(s, 128) == -1) {
		perror("server listen()");
		exit(1);	
	}
    
    /////////////////////////////
    for (;;) {
        ns = accept(s, (struct sockaddr *)&fsin, &fromlen);
        pid = fork(); /*プロセスを分岐する。fork()関数は子プロセスには0、親プロセスには0以外の値を返す。*/ 
        ////////////////////
        if(pid == -1){//子プロセス作成時にエラー発生
            perror("error fork()");
            exit(1);
        }
        ////////////////////
        if(pid == 0){//fork()関数の子プロセスへの返り値は0。
            /* 子プロセスでクライアントの要求を受け付ける */ 
            close(s);

            if (ns == -1){
                perror("server accept()");	
                exit(1);
            }
            fp = fdopen(ns, "r");
	
            // receive requests from client (until an empty line)
            while (fgets(buf, sizeof(buf), fp) != NULL) {
                if(strcmp(buf,"\r\n") != 0) break;
            }

            tp = strtok(buf,"/");
            tp = strtok(NULL," ");
            fp2 = fopen(tp,"r");
            if(fp2 == NULL){
                //失敗時
                strs[0] = "HTTP/1.1 NOT 404 Fonund";
                strs[1] = "Content-Type: text/html; charset=us-ascii\r\n";
                strs[2] = "<HTML><HEAD>Not Found</HEAD>\n<BODY>\nThe requested URL /index.html was not found on this server.\n</BODY></HTML>\r\n";
                for (i = 0; i < NUMSTR; i++){
                    send(ns, strs[i], strlen(strs[i]), 0);
                }
            }
            else{
                //成功時
                strs[0] = "HTTP/1.1 200 OK\r\n";
                strs[1] = "Content-Type: text/html; charset=us-ascii\r\n";
                for (i = 0; i < NUMSTR-1; i++){
                    send(ns, strs[i], strlen(strs[i]), 0);
                }
                while (fgets(buf2, sizeof(buf2), fp2) != NULL) {
                    if(feof(fp2) != 0) break;
                    ////
                    #ifdef DEBUG
                    sleep(1);
                    #endif
                    ////
                    send(ns,buf2,strlen(buf2),0);
                }
            }
            fclose(fp2);
            exit(0);
        } 
        ////////////////////
        else{//親プロセスは次のクライアントからの接続を待つ。
            //親プロセスは接続要求への応答のみを行うから子プロセスに処理を渡した後は一旦ソケットを切断する。
            close(ns);
        } 
    }
	return 0;
}

