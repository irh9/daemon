#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

void daemon_run();
void daemon_exit(int sig);

int main() {
    pid_t pid, sid;

    // create our daemon process
    pid = fork();

    // if the parent process, terminate
    if (pid > 0) {
        printf("Parent process with PID: %d terminating\n", pid);
        exit(0);
    }

    if (pid == -1) {
        perror("fork() failed");
        exit(1);
    }
    // if the child process, make it a daemon
    if (pid == 0) {

        printf("PID: %d\n", getpid());
        // first make it a group leader
        sid = setsid();
        if (sid < 0) {
            perror("setsid() failed");
            exit(1);
        }
        
        // change dir to a specific working space
        if (chdir("/tmp") == -1) {
            perror("chdir to /tmp failed\n");
            exit(1);
        }
        
        // set file permissions to prevent file creation problems with daemon
        umask(0);

        // close the std file descriptors
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        // setup signal handling
        signal(SIGUSR1, daemon_exit);

        // prepare which log priorities will listened for and logged
        setlogmask(LOG_MASK(LOG_NOTICE));

        // open the syslog file
        openlog("Daemon Server", LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);;
        syslog(LOG_NOTICE, "Daemon process with PID %d logging ...\n", getpid());
  
        // run
        daemon_run();
    }
}


/* Executes the daemon's functionality */
void daemon_run() {
    // for now just do a simple timed write to test
    for (int i=0; i<10; i++) {
        sleep(1);
        syslog(LOG_NOTICE, "Daemon %d count -- %d\n", getpid(), i);
    }
}


/* Responsible for terminating the daemon process by signal handling */
void daemon_exit(int sig) {
    // close the syslog and exit
    syslog(LOG_NOTICE, "Daemon process with PID %d terminating.\n", getpid());
    closelog();
    exit(0);
}

