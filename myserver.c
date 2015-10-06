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
//#define DEBUG //ʣ�����饤����Ȥ����Ʊ����³����ư���ǧ��
//*/


char *strs[NUMSTR] = {
	"1\n",
	"2\n",
	"3\n"
};

int main(int argc,char* argv[]){//argc�ϥ��ޥ�ɥ饤�󤫤�ΰ����θĿ���argv�ϥ��ޥ�ɥ饤�󤫤����Ϥ���ʸ����
	int i, s, ns, port;
    //s�ϥ����åȤ�������������������socket()�����֤���뼱�̻ҡ�
    //ns�ϼ����դ��������åȤ�Ϳ����줿���̻Ҥ��֤��ͤȤ���accept()�����֤�����͡�
	struct sockaddr_in sin, fsin;

    //�ʲ��ϥץ���ʬ����ҥץ����ν����ǻ��Ѥ����ѿ���
    FILE *fp,*fp2;//���饤����Ȥ���Υꥯ�����Ȥ�ؤ��ե�����ݥ��󥿤ȥ��饤����Ȥ�����ꤵ�줿�ե������ؤ��ե�����ݥ��󥿡�
    char *tp;//���饤����Ȥ���Υꥯ������ʸ����Ƭ��ؤ��ݥ��󥿡�
    int pid;//�ƥץ�����Ϳ������ID���Ǽ�����ѿ���
    char buf[128],buf2[128];

    
	socklen_t fromlen = sizeof(struct sockaddr_in);
	
	if((argc <= 1) || ((port = atoi(argv[1])) == 0)) { //argc�θĿ������ʲ��λ��ޤ��ϥݡ����ֹ椬0�λ��ϥݡ����ֹ椬���Ϥ���Ƥ��ʤ���ΤȤ��ƥ��顼��ɽ�������롣
        perror("no port number");
        exit(1);		
	}

    //�����å�����
	if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("client socket()");
		exit(1);		
	}
	
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;
	
    //�����å���Ͽ
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("server bind()");
		exit(1);	
	}
	
    //�����å���³����
	if (listen(s, 128) == -1) {
		perror("server listen()");
		exit(1);	
	}
    
    /////////////////////////////
    for (;;) {
        ns = accept(s, (struct sockaddr *)&fsin, &fromlen);
        pid = fork(); /*�ץ�����ʬ�����롣fork()�ؿ��ϻҥץ����ˤ�0���ƥץ����ˤ�0�ʳ����ͤ��֤���*/ 
        ////////////////////
        if(pid == -1){//�ҥץ����������˥��顼ȯ��
            perror("error fork()");
            exit(1);
        }
        ////////////////////
        if(pid == 0){//fork()�ؿ��λҥץ����ؤ��֤��ͤ�0��
            /* �ҥץ����ǥ��饤����Ȥ��׵������դ��� */ 
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
                //���Ի�
                strs[0] = "HTTP/1.1 NOT 404 Fonund";
                strs[1] = "Content-Type: text/html; charset=us-ascii\r\n";
                strs[2] = "<HTML><HEAD>Not Found</HEAD>\n<BODY>\nThe requested URL /index.html was not found on this server.\n</BODY></HTML>\r\n";
                for (i = 0; i < NUMSTR; i++){
                    send(ns, strs[i], strlen(strs[i]), 0);
                }
            }
            else{
                //������
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
        else{//�ƥץ����ϼ��Υ��饤����Ȥ������³���Ԥġ�
            //�ƥץ�������³�׵�ؤα����Τߤ�Ԥ�����ҥץ����˽������Ϥ�����ϰ�ö�����åȤ����Ǥ��롣
            close(ns);
        } 
    }
	return 0;
}

