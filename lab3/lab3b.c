/* lab 3 B */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t getpid(void);
pid_t getppid(void);

int main()
{
  int  pid, myid, parentid, cid;
  int  val;

  val = 1;
  myid = getpid();
  printf("My id is %d, value is %d\n",myid,val);
  pid = fork();
  if (pid < 0) { /* error occurred */
     printf("Fork Failed\n");
     exit(1);
  } else if (pid == 0) { /* child process */
     myid = getpid();
     parentid = getppid();
     printf("Child: My id is %d, my parent pid is %d\n",myid,parentid);
     printf("Child: Pid is %d, value is %d\n",pid,val);
     val = 12;
     printf("Child: My id is %d, value is %d\n",myid,val);
     sleep(4);
     printf("Child: My id is %d, value is %d\n",myid,val);
     val = 13;
     printf("Child: My id is %d, value is %d\n",myid,val);
     sleep(4);
     printf("Child: My id is %d, value is %d\n",myid,val);
     printf("Child: Child %d completed\n",myid);
     exit(0);
  } else { /* parent process */
     printf("Parent: My child id is %d\n",pid);
     printf("Parent: Pid is %d, value is %d\n",pid,val);
     val = 2;
     printf("Parent: My id is %d, value is %d\n",myid,val);
     sleep(4);
     printf("Parent: My id is %d, value is %d\n",myid,val);
     val = 3;
     printf("Parent: My id is %d, value is %d\n",myid,val);
     sleep(4);
     printf("Parent: My id is %d, value is %d\n",myid,val);
     cid = wait(NULL);
     printf("Parent: Child %d collected\n",cid);
     exit(0);
  }
}
