#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int loop_count = 0;
    
    printf("Process ID: %d\n", getpid());
    printf("\nPress Enter to start the test...\n");
    getchar();

    while (1) {
        char *ptr = (char *)malloc(100 * 1024);
		for (int i = 0; i < 100 * 1024; i++) {
            ptr[i] = 'A';
        }
        loop_count++;
		if (loop_count % 1024 == 0) {
			printf("Leaked: %d00 MB total\n", loop_count/1024);
		}
        usleep(1000);
    }

    return 0;
}
