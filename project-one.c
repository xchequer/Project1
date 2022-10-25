#define MAX_MSG_SIZE 2048#include <stdio.h>

#include <sys/types.h>

#include <unistd.h>

#include <stdlib.h>

#include <sys/wait.h>

#include <stdlib.h>

#include <math.h>

#include <sys/stat.h>

#include <time.h>

#include <fcntl.h>

#include <string.h>

#include <mqueue.h>

#define MAXSERVER 11

char buf[15];
int main() {
  mqd_t md;
  struct mq_attr atributos = {   // Message Queues Set up
    .mq_maxmsg = 10,
    .mq_msgsize = MAX_MSG_SIZE
  };
  char * buffer;
  char buf1[20];
  char buf[atributos.mq_msgsize];
  pid_t ParentProcess = getpid();
  pid_t LikeServer[MAXSERVER];
  int status;
  int rando;

  /* Creating Parent Process Log File*/
  FILE * fp;
  time_t now; /* for the current time */
  const char processfile[] = "/tmp/ParentProcessStatus.txt";
  /* open the "Parent Process File" for writing */
  if ((fp = fopen(processfile, "w")) == NULL) {
    perror("Can't open processfile");
    return 1;
  }
/* Here we fork 10 LikeServers and 1 PrimaryLikeServer*/
  for (int i = 0; i < MAXSERVER; i++) // loop will run 10 times
  {
    rando = (rand() % 42) + 1;
    LikeServer[i] = fork();

    if (LikeServer[i] == -1) {
      perror("Can't fork");
      return 1;
    }
    if (LikeServer[i] == 0) {

       /* LikeServer 10 is Primary Server. It reads the queue */
      if (i == 10) {
        pid_t PrimaryLikesServer = LikeServer[0];
        printf("PrimaryLikesServer has process id:%d\n", getpid());
        

     ///////////// This section of code doesn't work. I should have gotten help ///
          if ((md = mq_open("/my_queue.txt", O_CREAT | O_RDONLY, 0666, & atributos)) == -1) {
          perror("Open message queue");
          return 1;
        }
        int priority;

        /* Here it reads the 10 messages sent to the queue */
        for (int i = 0; i < 10; ++i) {
          if (mq_receive(md, buf, sizeof(buf), 1) == -1) {
            perror("mq_recieve");
            exit(1);
          }
          snprintf(buf1, 22, "/tmp/LikeServer%d.txt", i);

          /* Here is an attempt to write to the Primary Server Logs that it has 
             received a message from each log */
          const char primaryserverfile[] = "/tmp/LikerServer0";
          /*open the Primary Server Process File for writing */
          if ((fp = fopen(primaryserverfile, "w")) == NULL) {
            perror("Can't open processfile");
            return 1;
          }

          fprintf(fp, "Received (%s): %s\n", (priority == 0) ? "NORMAL" : "URGENT", buf);

        }
        free(buffer);
        mq_close(md);
        mq_unlink("/my_queue.txt");
/////////////// End of Section of Code that doesn't work  //////////////////////
      }

      /* These are the 10 LikeServers */
      if (i != 10) {
        printf("LikeServer%d: %d with %d likes from ParentProcess %d\n", i, getpid(), rando, ParentProcess);
        snprintf(buf, 15, "LikeServer%d %d", i, rando);
        printf("%s\n", buf);
        time( & now);
        fprintf(fp, "LikeServer%d created at %s", i,
          ctime( & now));
        fflush(fp); /* flush the stream */
        // sleep(5); /* Each process lives for 5 secs */

        fprintf(fp, "LikeServer%d exited at %s", i,
          ctime( & now));
        fflush(fp); // flush the stream

      //// This part of the code doesn't work ////////////
        md = mq_open("/my_queue.txt", O_CREAT | O_WRONLY, 0666, &
          atributos);
        if (md == -1) {
          perror("Creating message queue");
          return 1;
        }
        if ((mq_send(md, buf, sizeof(buf), 1)) == -1) {
          perror("Message queue send");
          return 1;
        }
        mq_close(md);
       ///// This end of section that doesn't work ////////////
      }

      exit(0)
    } else if (LikeServer[i] > 0) {
      waitpid(LikeServer[i], & status, 0);

    }
  }

  return 0;
}