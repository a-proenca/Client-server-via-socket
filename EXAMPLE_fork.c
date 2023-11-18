#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
int n = 1;
pid_t a = fork(), b = fork(), c = fork(), d = fork();
if(a==0 || b==0 || c==0 || d==0){ 
 n=n*5;
 if(a!=0 || b==0 || c==0 || d==0){ 
  n=n*5;
   if(a==0 || b!=0 || c==0 || d==0){ 
   n=n*5;
    if(a==0 || b==0 || c!=0 || d==0){ 
    n=n*5;
     if(a!=0 || b!=0 || c==0 || d==0){ 
     n=n*5;
 	if(a==0 || b==0 || c==0 || d!=0){ 
	n=n*5;
	  if(a==0 || b!=0 || c!=0 || d==0){ 
	   n=n*5;
	    if(a!=0 || b==0 || c!=0 || d==0){
	     n=n+10;
	      if(a==0 || b==0 || c!=0 || d!=0){
	      n=n+10;
               if(a!=0 || b!=0 || c!=0 || d==0){
		n=n+10;
		 if(a==0 || b!=0 || c==0 || d!=0){
		 n=n+10;
		  if(a!=0 || b==0 || c==0 || d!=0){
		  n=n+10;
		   if(a!=0 || b==0 || c!=0 || d!=0){
		    n=n+10;
		     if(a!=0 || b!=0 || c==0 || d!=0){
    		     n=n+10;
		      if(a==0 || b!=0 || c!=0 || d!=0){
		       n=n+10;
		      }
 		     }
		   }
		  }
		}
	       }
	     }
	    }
	  }
	}
     }
   }
  } 
 }
}
sleep(1);
printf("PID = %d -> pai: %d -> n = %d\n", getpid(), getppid(), n);
return 0;
}
