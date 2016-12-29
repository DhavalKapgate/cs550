/*
 * To call wrappers, define CS550_RENAME with
 *    -DCS550_RENAME
 *
 * To run this use:
 *
 *     ./a.out check 100 1000 max_size
 *
 * The arguments are:
 *
 *     check
 *          Either 0 or 1. 0 means don't check for overwriting. Use 0 when testing performance.
 *
 *      100
 *          Number of iterations to do.
 *
 *      100
 *          Maximum possible number of blocks to be allocated at any one time.
 *
 *      max_size
 *          Maximum size of a block.
 *
 */

#ifdef CS550_RENAME
    #define malloc cs550_malloc
    #define free cs550_free
    #define calloc cs550_calloc
    #define realloc cs550_realloc
    #define sbrk cs550_sbrk
#endif

#include "utility.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void
check_mem(unsigned char *p, unsigned char v, size_t sz) {
    size_t i;
	for (i = 0; i < sz; i++) {
        if (*p != v) {
            cs550_print("Contents overwritten in block at 0x%lx at offset %lu: Should have been %lu, but was %lu.\n",
             p, i, (unsigned long) v, (unsigned long) *p);
            exit(1);
        }
    }
}

int
main() {

    char *begin = sbrk(0);

   // Some basic tests.
    {
        void *vp1, *vp2;
        vp1 = malloc(10);
        free(vp1);
        vp1 = malloc(10);
        vp2 = malloc(30);
        free(vp1);
        free(vp2);
        vp2 = malloc(30);
        vp1 = malloc(10);
        free(vp1);
        free(vp2);
        free(malloc(100));
        free(malloc(48));
        int i;
        void *array[1000000];
        for(i=0;i<1000000;i++)
          array[i]=malloc(sizeof(8));
        for(i=0;i<1000000;i++)
          free(array[i]);
    }
    {

        cs550_print("    %lu iterations, %lu blocks, %lu max block size\n",
         (unsigned long) 100, (unsigned long) 100, (unsigned long) 1000);

        struct Block {
            void *ptr;
            size_t sz;
            unsigned char val;
        };
        struct Block blocks[100];
        size_t i;
	for (i = 0; i < 100; i++) {
            blocks[i].ptr = 0;
        }
	size_t k;
        for (k = 0; k < 20; k++) {
            struct Block *b = &blocks[rand()%100];
            if (b->ptr == 0) {
                if (rand()%2) {
                    b->sz = rand()%1000;
                    b->ptr = malloc(b->sz);
                } else {
                    size_t sz = rand()%1000;
                    size_t n_membs = rand()%100 + 1, memb_size = sz/n_membs;
                    b->ptr = calloc(n_membs, memb_size);
                    b->sz = n_membs*memb_size;
                }
                b->val = rand()%256;
                memset(b->ptr, b->val, b->sz);
            } else {
                if (rand()%2) {
                    free(b->ptr);
                    b->ptr = 0;
                } else {
                    b->sz = rand()%1000 + 1;
                    b->ptr = realloc(b->ptr, b->sz);
                    b->val = rand()%256;
                    memset(b->ptr, b->val, b->sz);
                }
            }

            if (1) {
		            size_t i;
                for (i = 0; i < 100; i++) {
                    struct Block *b = &blocks[i];
                    if (b->ptr != 0) {
                        check_mem(b->ptr, b->val, b->sz);
                    }
                }
            }
        }

        for (i = 0; i < 100; i++) {
            free(blocks[i].ptr);
        }
    }

    char *end = sbrk(0);
    cs550_print("Break usage is %lu\n", end - begin);
	return 0;
}
