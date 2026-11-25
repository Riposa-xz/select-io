#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/fcntl.h>
#include<errno.h>

int net_initializer(char* ip,int port)
{
 int server_sock;
 server_sock=socket(AF_INET,SOCK_STREAM,0);
 struct sockaddr_in server_addr;
 server_addr.sin_family=AF_INET;
 if(ip==NULL)server_addr.sin_addr.s_addr=INADDR_ANY;
 else server_addr.sin_addr.s_addr=inet_addr(ip);
 
 server_addr.sin_port=htons(port);
 
 bind(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr)    );
 listen(server_sock,1000);
 return server_sock;
}


void select_netstart()
{
	fd_set set;
	fd_set oset;
	int server_sock=net_initializer(NULL,8080);
	int maxfd=server_sock;
	int client_sock[1000];
	int clientNum=0;
	FD_ZERO(&set);
 	FD_ZERO(&oset);
   	FD_SET(server_sock, &set);

while (1) {
	oset=set;
    int readynum=select(maxfd+1,&oset,NULL,NULL,NULL);
    if(readynum<0) {
        if (errno == EINTR) continue;
        perror("select");
        break;
    }
    if(FD_ISSET(server_sock, &oset)){
    struct sockaddr_in client_addr;
	socklen_t clientLen=sizeof(client_addr);
	   
	 int client=accept(server_sock,(struct sockaddr*)&client_addr,&clientLen);
     if(client>0){
			client_sock[clientNum++]=client;
			FD_SET(client,&set);
			if(client>maxfd)maxfd=client;
			printf("new client %d\n",client);
		}
    }

    for (int i=0;i<clientNum;i++) {
        int fd=client_sock[i];
        if (FD_ISSET(fd, &oset)) {
            char buf[1024];
            int n=read(fd,buf,sizeof(buf));
            if (n > 0) {
			buf[n-1]='\0';
               printf("%s\n",buf);
				char s[]="hello\n";
				send(fd,s,sizeof(s),0);
            } else if (n==0) {
                close(fd);
               	client_sock[i--]=client_sock[--clientNum];
				FD_CLR(fd,&set);
            } else {
                perror("read error");
                close(fd);
            }
        }
	else continue;
    }
}
	

}




int main()
{
select_netstart();

}


