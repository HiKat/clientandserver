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

//argc�ϥ��ޥ�ɥ饤�󥪥ץ����ο���argv�ϥ��ޥ�ɥ饤������Ϥ��줿���͡�
//���ޥ�ɥ饤�󤫤�ΰ����Ȥ��Ƥϥۥ���̾���ݡ����ֹ�ν硣
int main(){
    while(1){
        FILE *fp;
        char hostname[128];
        int i, s, Port;
        struct hostent *hp;//�ۥ���̾��IP���ɥ쥹�򰷤�����ι�¤��
        struct sockaddr_in sin;//�����åȤ򰷤�����ι�¤��
        char buf[128];
        ///////////////////////////////////////////////////////////////////////
        //��������URL���ϴ�
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

        //�����ޤ�URL���ϴ�
        /////////////////////////////////////////////////////////////////////////
    
        //�ۥ���̾�����ꤵ��Ƥ��ʤ����
        if(HostName == NULL){
            perror("NO HOST NAME. ENTER THE CORRECT URL!");
            exit(1);
        }
    
        //�ݡ����ֹ椬���ꤵ��Ƥ��ʤ����
        if(PortNumber == NULL){
            perror("NO PORT NUMBER. ENTER THE CORRECT URL!");
            exit(1);
        }

        //hp��IP���ɥ쥹���Ǽ���롣
        if ((hp = gethostbyname(HostName)) == NULL){
            fprintf(stderr, "%s: unknown host.\n", hostname);
            exit(1);
        }

        if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){//AF_INET�ϥͥåȥ�����ɥ쥹�μ��ࡣ�����åȤ�������
            perror("client socket()");
            exit(1);		
        }
        bzero(&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(Port);
        memcpy(&sin.sin_addr, hp->h_addr, hp-> h_length);//IP���ɥ쥹�ξ���򥽥��åȤ˥��ԡ����롣hp��IP���ɥ쥹���Ǽ�����ѿ���
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {//�����å�������Ԥä������³��Ԥ���
            perror("client connect()");
            exit(1);	
        }

        //���饤����Ȥȥ����ФΤ��Ȥ��Ԥ�������ѿ�
        fp = fdopen(s, "r");
        char str0[9999];
        char str1[9999];

        // send request to server
        //reqlines��http�ץ�ȥ�����Ǽ���롣

        strcpy(str0,"GET /");
        strcat(str0,Path);
        strcat(str0," HTTP/1.1\r\n");

        strcpy(str1,"Host: ");
        strcat(str1,HostName);
        strcat(str1,"\r\n\r\n");
    
        reqlines[0] = str0;
        reqlines[1] = str1;
    
    
        //�ꥯ�����Ȥ��������롣
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
 
