#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Make any use of assert in dbg_print() go away, unless we are actually just testing
// dbg_print().  In that case, define DBG_PRINT_TEST.
#ifndef DBG_PRINT_TEST
#define NDEBUG
#endif
#include <assert.h>

static void
error() {
    const char *const m = "\ndbg_print(): Bad format.\n";
    write(2, m, strlen(m));
}

static char *
ultoa_helper(char *buf, unsigned long ul) {
    if (ul > 0) {
        buf = ultoa_helper(buf, ul/10);
        *buf++ = '0' + ul%10;
    }
    return buf;
}

// Convert ulong to ASCII.
static void
ultoa(char *buf, unsigned long ul) {
    assert(buf != 0);
    if (ul == 0) {
        *buf++ = '0';
    } else {
        buf = ultoa_helper(buf, ul);
    }
    *buf = '\0';
}

// Push diag context so we can ignore unused vars.
#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

// This is a tiny version of printf() that accepts only %lu and %%.
void
dbg_print(const char *fmt, ...) {

    enum { ST_CHUNK = 1, ST_PERCENT, ST_L } state = ST_CHUNK;
    size_t chunk_begin = 0;
    int ec;

    va_list ap;
    va_start(ap, fmt);

    size_t i = 0;
    for (; fmt[i] != '\0'; i++) {
        int ch = fmt[i];
        switch (state) {
            case ST_CHUNK:
                {
                    if (ch == '%') {
                        state = ST_PERCENT;
                        ec = write(2, &fmt[chunk_begin], i - chunk_begin);
                        assert((unsigned) ec == i - chunk_begin);
                    }
                }
                break;
            case ST_PERCENT:
                {
                    if (ch == 'l') {
                        state = ST_L;
                    } else if (ch == '%') {
                        chunk_begin = i;
                        state = ST_CHUNK;
                    } else {
                        error();
                    }
                }
                break;
            case ST_L:
                {
                    if (ch == 'u') {
                        unsigned long ul = va_arg(ap, unsigned long);
                        char buf[100];
                        ultoa(buf, ul);
                        ec = write(2, buf, strlen(buf));
                        assert((unsigned) ec == strlen(buf));
                        chunk_begin = i + 1;
                        state = ST_CHUNK;
                    } else {
                        error();
                    }
                }
                break;
            default:
                assert(0);
                abort();
                break;
        }
    }

    if (i - chunk_begin > 0) {
        ec = write(2, &fmt[chunk_begin], i - chunk_begin);
        assert((unsigned) ec == i - chunk_begin);
    }

    va_end(ap);
}

#pragma GCC diagnostic pop

typedef struct Block {
    size_t size;
    struct Block *next;
} block_t;

static block_t *head;

void *
malloc(size_t sz) {

    char msg_buf[100];

    block_t **bpp;
    for (bpp = &head; *bpp != 0; bpp = &(*bpp)->next) {
        if ((*bpp)->size >= sz) {
            block_t *found = *bpp;
            *bpp = (*bpp)->next; // Delink found block.
            void *vp = (char *) found + sizeof(size_t); // Skip past size that we saved.
            sprintf(msg_buf, ">>> Reallocated %zu bytes for %zu byte request at %p.\n", found->size, sz, vp);
            write(2, msg_buf, strlen(msg_buf));
            return vp;
        }
    }

    // If reached, need to allocate another block.  It cannot
    // be smaller than the amount of space we need for bookkeeping.
    size_t sb_req = sz + sizeof(size_t); // Total amount needed based on caller req.
    // Make sure that it's big enough to hold a Block.
    sb_req = sb_req < sizeof(block_t) ? sizeof(block_t) : sb_req;
    block_t *new_block = (block_t *) sbrk(sb_req);
    // More available than sz if sz was very small.
    new_block->size = sb_req - sizeof(size_t);

    void *vp = (char *) new_block + sizeof(size_t);
    sprintf(msg_buf, ">>> Allocated %zu new bytes for %zu byte request at %p.\n", new_block->size, sz, vp);
    write(2, msg_buf, strlen(msg_buf));

    return vp;
}

void
free(void *vp) {

    /*
    {
        char msg_buf[100];
        sprintf(msg_buf, "Freeing at %p...\n", vp);
        write(2, msg_buf, strlen(msg_buf));
    }
    */

    if (vp != 0) {

        block_t *bp = (block_t *) (((char *) vp) - sizeof(size_t));
	printf("%p\n",bp->next);
        bp->next = head;
        head = bp;

        {
            dbg_print("<<< Freed %lu bytes at %lu...\n", bp->size, vp);
        }
    }
}

int
main() {
    
    malloc(1);
    void *vp = malloc(13);
    free(vp);

    void *v = malloc(335);
    free(v);

    return 0;
}
