             /*  Server Program to handle a single client at a time*/
             /*This program  expects space in client's expression*/

#include <stdio.h>                 //  Basic Header file
#include <sys/types.h>             //all  the data types
#include <sys/socket.h>            //contains all the socket related prototype,macros,function
#include <netinet/in.h>            //provides all the structure to deal in internet socket domain   
#include<unistd.h>            // includes write & read function

int pow(int x,int y)          // This is power function which will return the value of x^y
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

int main(int argc, char *argv[])                     // argc will containg number of commmand line argument and argv will contain these argument
{
     int sockfd, newsockfd, portno, clilen;         // sockfd-> will store socket desciptor of socket created
                                                    // newsockfd -> it is kernel level socket descriptor
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2)                                //to run server code two argument are required . one is ./server.out and second is port number
     {
       fprintf(stderr,"ERROR, no port provided\n");
       exit(1);
     }
         sockfd = socket(AF_INET, SOCK_STREAM, 0);                 // creation of socket 
         if (sockfd < 0){error("ERROR opening socket");}
     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
    
     int ret=bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));    // Binding
     if(ret<0)error("ERROR on binding");
    
     listen(sockfd,5);                                                          // Listen
     clilen = sizeof(cli_addr); 
    
    while(1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);        //Accept
    printf("Client is connected at socket number : %d\n",newsockfd);   
     if (newsockfd < 0) 
          error("ERROR on accept");
          read_write(newsockfd);                                              // function doing all the read from socket and writing in socket
    }   
 return 0;
}
  
   void read_write(int newsockfd)
    {
         char buffer[256];                                        //Buffer will hold client's input to server and server's output to client
      
       while(1)                                  // Following logic is extracting operands and operator from client's expression
        {
                 bzero(buffer,256);
                 int n = read(newsockfd,buffer,255);                 // Reading client's input from socket
                 if (n < 0) error("ERROR reading from socket");
                 if(n==0)                                                      //If  client has left
                 { 
                    printf("client at socket %d has gone\n" ,newsockfd);
                     break;
                 } 
                   int i=0; 
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
             float reply=0;                                        // reply -> it will hold final result of expression of client query
             if(b=='+') {  reply=first+second;}
             if(b=='*')  { reply=first*second;}
             if(b=='-')  { reply=first-second;}
             if(b=='/')   {reply=(float)first/second;}
             char str[20];
             sprintf(str, "%f", reply);                            //This is to convert float to a String
             printf("Sending Reply:%f\n",reply);
             n = write(newsockfd,str,sizeof(str));                  // writing the result in buffer
            if (n < 0) error("ERROR writing to socket");
 }

}


