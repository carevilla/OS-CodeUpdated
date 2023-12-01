#include "kernel/types.h"
#include "user/user.h"

void wait_or_die() {
    int rc = wait(0);
    if (rc < 0)
        exit(-1);
}

int fork_or_die() {
    int rc = fork();
    if (rc < 0)
        exit(-1);
    return rc;
}

int main(int argc, char *argv[]) {
    // process a
    if (fork_or_die() == 0) {
        sleep(10);
        // process b
        if (fork_or_die() == 0) {
            sleep(40);
            // process c
            if (fork_or_die() == 0) {
                sleep(80);
                // process d
                exit(0);
            }
            if (fork_or_die() == 0) {
                sleep(40);
                // process e
                exit(0);
            }
            wait_or_die();
            wait_or_die();
            if (fork_or_die() == 0) {
                sleep(60);
                // process f
                exit(0);
            }
            wait_or_die();
            exit(0);
        }
        wait_or_die();
        exit(0);
    }
    wait_or_die();
    exit(0);
}
