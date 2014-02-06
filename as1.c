#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
	char hostname[255];
	struct timeval tv;
	struct rusage res;
	pid_t pid, wpid;
	int A = 100,
		status,	
		mypid = getpid();

	// fork 3 children
	for(int i = 0; i < 3; i++) {
		pid = fork();
		if(pid == -1) {
			perror("Fork error");
		} else if(pid == 0) { // execute children
			mypid = getpid();
			sleep(i);
			A += 100;
			printf("\t%d: A = %d\n", getpid(), A);
			if(i == 0) { // first child
				if(gethostname(hostname, 255) == -1) {
					perror("Hostname error");
					_exit(-1);
				}
				printf("\t%d: username = %s\n", mypid, getlogin());
				printf("\t%d: ppid = %d\n", mypid, getppid());
				printf("\t%d: hostname = %s\n", mypid, hostname);
			} else if(i == 1) { // second child
				if(gettimeofday(&tv, NULL) == -1) {
					perror("Time of day error");
					_exit(-1);
				}
				printf("\t%d: time = %s", mypid, ctime(&tv.tv_sec));
				printf("\t%d: userid = %d\n", mypid, getuid());
			} else if(i == 2) { // third child
				if(getrusage(RUSAGE_SELF, &res) == -1) {
					perror("Resource error");
					_exit(-1);
				}
				printf("\t%d: groupid = %d\n", mypid, getgid());
				printf("\t%d: user time = %ld.%06lds\n", mypid,
					res.ru_utime.tv_sec,
					(long) res.ru_utime.tv_usec);
				printf("\t%d: sys time = %ld.%06lds\n", mypid,
					res.ru_stime.tv_sec,
					(long) res.ru_stime.tv_usec);
				printf("\t%d: max resident set size = %ld\n", mypid,res.ru_maxrss);
				printf("\t%d: shared memory = %ld\n", mypid, res.ru_ixrss);
				printf("\t%d: unshared data = %ld\n", mypid, res.ru_idrss);
				printf("\t%d: unshared stack = %ld\n", mypid, res.ru_isrss);
				printf("\t%d: page reclaims = %ld\n", mypid, res.ru_minflt);
				printf("\t%d: page faults = %ld\n", mypid, res.ru_majflt);
				printf("\t%d: swaps = %ld\n", mypid, res.ru_nswap);
				printf("\t%d: block inputs = %ld\n", mypid, res.ru_inblock);
				printf("\t%d: block outputs = %ld\n", mypid, res.ru_oublock);
				printf("\t%d: messages sent = %ld\n", mypid, res.ru_msgsnd);
				printf("\t%d: messages received = %ld\n", mypid, res.ru_msgrcv);
				printf("\t%d: signals received = %ld\n", mypid, res.ru_nsignals);
				printf("\t%d: voluntary context switches = %ld\n", mypid, res.ru_nvcsw);
				printf("\t%d: involuntary context switches = %ld\n", mypid, res.ru_nivcsw);
			}
			fflush(stdout);
			_exit(-1);
		}
	}

	// wait for children to complete
	while((wpid = wait(&status)) > 0)
		printf("%d: %d exit with status %d\n\n", mypid, (int)wpid, status);
	printf("%d: A = %d\n\n", mypid, A);
	// A should still be 100.  The modifications made by the child processes
	// do not affect the parent process because forking gives them each their
	// copy of the variable
	if(execlp("last", NULL) == -1) {
		perror("Exec");
	}
	return -1;
}