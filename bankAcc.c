#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>


int  main()
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    Turn = 0;
     int    a;
    int    status;
    int    BankAccount = 0;

     sem_t mutex;
	
		 srand(time(NULL));   // Initialization, should only be called once
	   ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error\n");
          exit(1);
     }
		 ShmPTR = (int *) shmat(ShmID, NULL, 0);
		 if (*ShmPTR == -1) {
          printf("*** shmat error\n");
          exit(1);
     }
		 ShmPTR[0] = BankAccount;
     ShmPTR[1] = Turn;
     // attach to shared memory
		 pid = fork();
		 if (pid < 0) 
    { 
        fprintf(stderr, "fork fauilure" );
        return 1; 
    } 
    // Parent process 
    else if (pid > 0) 
    { 		
			printf("Parent");
		  for( a = 0 ; a < 25 ; a++ ) {
				sem_wait(&mutex);
				sleep(rand() % 5);
				while(ShmPTR[1] != 0) {}
				if (ShmPTR[0] <= 100) {
					// Deposit Money
					int Bal = rand() % 100;
					if ((Bal % 2) == 0 ) {
						ShmPTR[0] = ShmPTR[0] + Bal;
						printf("Dear old Dad: Deposits $%d / balance = $%d\n", Bal, ShmPTR[0]);

					} else {
						printf("Dear old Dad: Doesn't have any money to give\n");
					}
					ShmPTR[1] = 1;
				} else {
						printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", ShmPTR[0]);
						ShmPTR[1] = 1;
				}
				sem_post(&mutex);
     } 
		}
    // child process 
    else
    { 
			for( a = 0 ; a < 25 ; a++ ) {
				sem_wait(&mutex);
				sleep(rand() % 5);
				while(ShmPTR[1] != 1) {}
				int Bal = rand() % 50;
				printf("Poor Student needs $%d\n", Bal);
				if (Bal <= ShmPTR[0]) {
					// Withdraw Money
					ShmPTR[0] = ShmPTR[0] - Bal;
					printf("Poor Student: Withdraws $%d / balance = $%d\n", Bal, ShmPTR[0]);
				} else {
					printf("Poor Student: Not Enough Cash ($%d)\n", ShmPTR[0] );
				}
				ShmPTR[1] = 0;
				sem_post(&mutex);
			}
		}
}
	
