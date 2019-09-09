/*****Shell in c++ to execute all basic commands,pipe,IO redirection redirection*****/

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#define true 1

using namespace std;


void basicsystem(int argc,char** argv) //executes basic functionalities
{
    pid_t pid;
    pid=fork(); 
    if(pid<0)                 //error
   {
     perror("ERROR");
   }
   else if(pid==0)
   {
       execvp(argv[0],argv);
       perror("ERROR");
   }
   else
   {
       wait(NULL);
   }
}//basicsystem end


void redirectfun(char** lefthalf, char** righthalf, int flag)  //redirection by dividing method
{    
      pid_t pid2;  
      int filedesc1[2]; 
      int fd1;            

     pipe(filedesc1);
     int increment;  
     char bit;  
  if (fork()==0)
   {
       if(flag==2)
       {
          fd1=open(righthalf[0], O_RDWR | O_CREAT, 0666); // for '>' to throw the file on the new file else create the new file if its absent
       }
       else
       {
           fd1=open(righthalf[0], O_RDWR | O_APPEND ,0666); // for '>>' to append
       }
      if (fd1<0)
     {
      printf("Error: %s\n", strerror(errno)); //handle error 
      return;
     }

     dup2(filedesc1[0], 0);//for reading stdin
    

    while((increment=read(0,&bit,1)) > 0)   //reads till not empty
    {
      write(fd1,&bit,1); //writes till not empty
    }

    exit(0);
   } //if
   else if ((pid2=fork())==0) 
   {
    dup2(filedesc1[1],1);//for writing stdout
    execvp(lefthalf[0],lefthalf);
    perror("ERROR");
   } 
  else
   {
       wait(NULL);
    
    }
}//redirection end

void pipefun(char** lefthalf, char** righthalf)  //pipe function
{
  int filedesc[2]; 
  pipe(filedesc);
  pid_t pid1;

  if(fork()==0) 
  {
    dup2(filedesc[0], 0);  //file desc for stdin
   execvp(righthalf[0],righthalf);
   perror("ERROR");
  } 
  else if((pid1=fork())==0)
   {
     dup2(filedesc[1],1);//file desc for stdout
    execvp(lefthalf[0],lefthalf);
    perror("ERROR");
  }
   else
    wait(NULL);

} //pipefunc end


int rinput(char **argv)
 {
  
  unsigned int argc=0;
  char *data;
  string input;

  while (cin >>input)
   {
    if(input=="exit")  //exit the shell
    {
        exit(0);
    }

    /*if(argv[0]=="cd")
    {
        if(input[1]==NULL)
        {
	      chdir('/');
        }
        else
          chdir(argv[1]);
          
    }*/

    data=new char[input.size()+1];
    strcpy(data,input.c_str());//conversion in c++ using .c_str() function
    argv[argc]=data;
    argc++;

   if (cin.get()=='\n')  //In case of 'ENTER' jump to new line
   {
      break;   //break from loop
   }
  }
    argv[argc]=NULL; 

  return argc;
}//readinput end

int main()
{
   int argc;
   char *argv[256];
   char *lefthalf[128];  //input before cut
   char *righthalf[128]; // input after cut

   while(1)     //always true,continous execution till evoked
   {
    cout<<"@#$%~ashish$$>";
     argc=rinput(argv);

     int cut=-1;
     int flag=0;
     for(int i=0;i<argc;i++)
     {
        if(strcmp(argv[i],"|")==0)
        {
            cut=i;
            flag=1;
        }
         else if(strcmp(argv[i],">")==0)
        {
            cut=i;
            flag=2;
        }
        else if(strcmp(argv[i],">>")==0)
        {
            cut=i;
            flag=3;
        }

     } 

     if(flag==1 || flag==2 || flag==3) // either pipe,redirection set flag and cut them into halves
     {
          for(int i=0;i<cut;i++)
          {
             lefthalf[i] = argv[i]; //everything before '|','>>','>'
          }
         int increment=0;
        for(int i=cut+1;i<argc;i++) 
        {
           righthalf[increment]=argv[i]; //everything after '|','>>'.'>'
          increment=increment+1;;
        }

        lefthalf[cut]=NULL;
        righthalf[increment]=NULL;
     }  
             
     if(flag==1)
     {
         pipefun(lefthalf,righthalf);  //if pipe called
     }

     if(flag==2 || flag==3)
     {
         redirectfun(lefthalf,righthalf,flag);// if redirection called
     }

     if(flag==0)
     {
         basicsystem(argc,argv);  //if normal shell called
     }


   }//while end
return 0;
}//main end