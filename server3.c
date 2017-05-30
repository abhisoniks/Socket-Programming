                /* Server program handling multiple client at a time through use of system call*/
                

                      /*This program expects space in client's expression*/


#include <stdio.h>              // Basic Header file
#include <sys/types.h>          //all  the data types
#include <sys/socket.h>         //contains all the socket related prototype,macros,function
#include<sys/time.h>            //contains all time related prototype ,macros,function
#include <netinet/in.h>        //provides all the structure to deal in internet socket domain   
#include<unistd.h>            // includes write & read function

int pow(int x,int y)        // This is power function which will return the value of x^y
     {
         
          int result=1;
          int i=0;
          while(i<y){
            result = result*x;
            i++;}
	 return result;
   }

    void error(char *msg)   
  {
      perror(msg);
      exit(1);
  }
 
      fd_set main_fd,seco_fd;                     //Both are Sets to contain file descriptors.in our program main_fd will hold all              
                                                  //descriptors to client who are new while seco_fd will contain file descriptor to client who 
                                                  // want to go through read phase now           

      
        int main(int argc, char *argv[])            // argc will contain number of command line argument argv will contain those argument
  {
        int sockfd, newsockfd, portno, clilen,dsize,x,n;
        struct sockaddr_in serv_addr, cli_addr;          // serv_addr & cli_addr will contain server & client address
       
        if (argc < 2)                                   // to  run server code atlest two argument ./server.out & port number are must
      {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
      }
         sockfd = socket(AF_INET,SOCK_STREAM, 0);         // Creation of socket 
         if (sockfd < 0) 
         error("ERROR opening socket");
         bzero((char *) &serv_addr, sizeof(serv_addr));
         portno = atoi(argv[1]);
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_addr.s_addr = INADDR_ANY;
         serv_addr.sin_port = htons(portno);
         if(bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)    // Binding
              error("ERROR on binding");
         listen(sockfd,5);                                   // Listen
         FD_SET(sockfd,&main_fd);          // FD_SET function include a particular descriptor  in particular SET
         dsize=sockfd;
    
     while(1)
      {
           seco_fd = main_fd;
           int ret = select(dsize+1, &seco_fd, NULL, NULL, NULL);
           if(ret<0)error("error with select call");
           for(x=0;x<=dsize;x++)
        {      
              if(FD_ISSET(x,&seco_fd))                      // To check whether a particular descriptor belongs to particular SET
               {
                  if((x == sockfd)) //new connection        //If this is new client accept it
                 { 
                     clilen=sizeof(cli_addr);
                     newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
                     if(newsockfd<0)error("error with accept");
                     FD_SET(newsockfd,&main_fd);
                     if(newsockfd >dsize)
                     dsize = newsockfd;
                 }

                  else                       // if client is old perform read and write on it
                 {
                   read_write(x);            //this function does all reading & writing for client
                 }
            }     
        }
    }
  return 0;
  }
    
              void read_write(int x)                           // Following logic is extracting operands and operator from client's expression
                {
                    char buffer[256];             
                    bzero(buffer,256);
                    int  n = read(x,buffer,255);                   // it will read from socket  & put data in Buffer 
                    if (n < 0) error("ERROR reading from socket");
                    else if(n==0)                                  // if there is no data in buffer
                  {
                     printf("client at socket %d has gone\n",x);
                     FD_CLR(x,&main_fd);                          //will clear descriptor from given set
                  }
                    else                                          
                  {
                     int i=0; 
                     printf("client %d message is %s",x,buffer);
                     while(1)
                  {
                     int a= (int)buffer[i];
                     if(a==32) {break;}
                     i++;
                  }
                     int cp=i;
                     i=i-1;
                     int first=0;
                     int j=0;
                    while(i>=0)
                 {
                    int temp=buffer[j]-'0';
                    first=first+(temp *pow(10,i));
                    i--;          
                    j++;
                 }
                    char b=buffer[cp+1];
                    int temp_pointer=0;
                    cp=cp+3;temp_pointer=cp;
                   while(buffer[cp]!='\0')
                {
                    cp++;
                }
       
                   int second=0;
                   i=cp-temp_pointer-1;
                 while(i!=0)  
                {
                  int temp=buffer[temp_pointer]-'0';
                  second=second+(temp *pow(10,i-1));
                  temp_pointer++;
                  i--;
                }
                  float reply=0;                                        //float reply will contain result of expression
                  if(b=='+') {  reply=first+second;}                    
                  if(b=='*')  { reply=first*second;}
                  if(b=='-')  { reply=first-second;}
                  if(b=='/')   {reply=(float)first/second;}
                  char str[20];
                  printf("Sending Reply: %f\n",reply); 
                  sprintf(str, "%f", reply);
                  n = write(x,str,sizeof(str));                         // writing the result in socket
                  if (n < 0) error("ERROR writing to socket");}
}
