 
                 /*Program of a server which can handle multiple client at a time through new processes.*/
                    
                        /*This program is expects space in client's expression*/
               

#include <stdio.h>                  //  Basic Header file
#include <sys/types.h>              //all  the data types
#include <sys/socket.h>             //contains all the socket related prototype,macros,function
#include <netinet/in.h>             //provides all the structure to deal in internet socket domain   
#include<unistd.h>                // includes write & read function

 
  int pow(int x,int y)            // This is power function which will return the value of x^y
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

     int main(int argc, char *argv[])
   {
         int sockfd, newsockfd, portno, clilen;                // sockfd-> storing file descriptor  , 
                                                                // newsockfd-> it is a kernel level file descriptor for a socket
         struct sockaddr_in serv_addr, cli_addr;
         int n;
         if (argc < 2)
         {
           fprintf(stderr,"ERROR, no port provided\n");
           exit(1);
         }
              sockfd = socket(AF_INET, SOCK_STREAM, 0);                        // Creating Socket
              if (sockfd < 0)                   
              error("ERROR opening socket");
              bzero((char *) &serv_addr, sizeof(serv_addr));                
              portno = atoi(argv[1]);                                               
              serv_addr.sin_family = AF_INET;
              serv_addr.sin_addr.s_addr = INADDR_ANY;
              serv_addr.sin_port = htons(portno);
              if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)       // Binding
              error("ERROR on binding");
              listen(sockfd,5);                                                // Listen
              clilen = sizeof(cli_addr);                                       
         
        while(1)
          {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);      //ACCEPT
                if (newsockfd < 0) 
                error("ERROR on accept");
                int pid=fork();                             // for each new client we are creating a new process which will handle that client
                if(pid==0)
                    { 
                        printf("Connected with client at socket %d\n",newsockfd);
                        read_write(newsockfd);      //calling of a function which will handle input output for a client    
                    }       
          }
                return 0; 
  }
             
                void read_write(int newsockfd)      // this function is handling all the read and write operations
               {                                    //It has one parameter i.e. newsockfd ->newsockfd is containg file desciptor for client
                
                     char buffer[256];int i=0;      // buffer is character type variable which will hold input and output data
                 while(1)
                    {
                          bzero(buffer,256);
                          int n = read(newsockfd,buffer,255);                  // Read from buffer
                          if (n < 0) error("ERROR reading from socket");       // if read unsuccesfull
                          if(n==0)                                             //If no input Data
                          {
                             printf("client at socket %d has gone\n",newsockfd);
                             break;
                          }
                            printf("client which socket %d has message: %s",newsockfd,buffer);
               
                  while(1)                                 // Following logic is extracting operands and operator from client's expression
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
      
              float reply=0;                                 //reply is a float variable which will store final result of client's expression
              if(b=='+')  {  reply=first+second;}
              if(b=='*')  { reply=first*second;}
              if(b=='-')  { reply=first-second;}
              if(b=='/')  {reply=(float)first/second;}
              char str[20];
              printf("Sending Reply: %f\n",reply);             // conversion of float to String to send on network
              sprintf(str, "%f", reply);
              n = write(newsockfd,str,sizeof(str));           // write the result im socket
              if (n < 0) error("ERROR writing to socket");
 }
 } 
