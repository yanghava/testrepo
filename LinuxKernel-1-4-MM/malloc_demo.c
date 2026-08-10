#define _GNU_SOURCE

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SMALL_SIZE (64 * 1024)       // 64 KiB
#define LARGE_SIZE (256 * 1024)      // 256 KiB
#define SMALL_COUNT 16

static void touch_memory(char *memory, size_t size)
{
    /*
     * Write one byte per page.
     * This makes the allocated pages actually usable and observable.
     */
    long page_size = sysconf(_SC_PAGESIZE);

    for (size_t i = 0; i < size; i += (size_t)page_size) {
        memory[i] = 'A';
    }

    memory[size - 1] = 'Z';
}

static void show_mapping_for_address(const char *name, void *address)
{
    FILE *maps = fopen("/proc/self/maps", "r");

    if (maps == NULL) {
        perror("fopen");
        return;
    }

    unsigned long target = (unsigned long)address;
    char line[512];

    while (fgets(line, sizeof(line), maps) != NULL) {
        unsigned long start;
        unsigned long end;

        if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
            if (target >= start && target < end) {
                printf("%-15s %p -> %s", name, address, line);
                break;
            }
        }
    }

    fclose(maps);
}

int main(void)
{
    
    printf("Process ID: %d\n", getpid());
    printf("\nPress Enter to start the test...\n");
    getchar();
	
     /*
     * Set the glibc mmap threshold to 128 KiB for this demonstration.
     *
     * Requests larger than this threshold will normally use mmap().
     * Requests smaller than this threshold will normally be serviced
     * from the malloc heap arena.
     */
    if (mallopt(M_MMAP_THRESHOLD, 128 * 1024) == 0) {
        fprintf(stderr, "Warning: mallopt() could not set the threshold\n");
    }

    setbuf(stdout, NULL);

    printf("Process ID: %d\n", getpid());
    printf("Page size: %ld bytes\n", sysconf(_SC_PAGESIZE));
    printf("Small allocation: %d bytes\n", SMALL_SIZE);
    printf("Large allocation: %d bytes\n\n", LARGE_SIZE);

    printf("Initial program break: %p\n", sbrk(0));

    /*
     * Allocate several small blocks.
     *
     * They will normally be obtained from the malloc heap arena.
     * Multiple malloc() calls may be satisfied by one brk() operation.
     */
    char *small_blocks[SMALL_COUNT];

    for (int i = 0; i < SMALL_COUNT; i++) {
        small_blocks[i] = malloc(SMALL_SIZE);

        if (small_blocks[i] == NULL) {
            perror("malloc small block");
            return EXIT_FAILURE;
        }

        touch_memory(small_blocks[i], SMALL_SIZE);

	printf("Allocated %d x %d bytes\n", i+1, SMALL_SIZE);
	printf("Program break: %p\n", sbrk(0));
    }

    printf("\nAfter small allocations:\n");
    printf("Program break: %p\n", sbrk(0));

    show_mapping_for_address("small_blocks[0]", small_blocks[0]);

    printf("Small allocation completed, press Enter Key to continue\n");
    getchar();

    /*
     * Allocate a large block.
     *
     * With the configured threshold, this will normally be allocated
     * from an anonymous mmap() region rather than the [heap] mapping.
     */
    char *large_block = malloc(LARGE_SIZE);

    if (large_block == NULL) {
        perror("malloc large block");
        return EXIT_FAILURE;
    }

    touch_memory(large_block, LARGE_SIZE);

    printf("\nAfter large allocation:\n");
    printf("Program break: %p\n", sbrk(0));

    show_mapping_for_address("large_block", large_block);

    printf("\nPress Enter to free the memory and exit...\n");
    getchar();

    /*
     * The large mmap-backed allocation will normally be returned using
     * munmap(), while small allocations are returned to the malloc arena.
     */
    free(large_block);

    for (int i = 0; i < SMALL_COUNT; i++) {
        free(small_blocks[i]);
    }

    printf("Memory freed.\n");

    return EXIT_SUCCESS;
}

