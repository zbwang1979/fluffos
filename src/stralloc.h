#ifndef _STRALLOC_H_
#define _STRALLOC_H_

typedef struct block_s {
    struct block_s *next;	/* next block in the hash chain */
    unsigned short refs;	/* reference count    */
    unsigned short size;	/* length of the string plus sizeof of struct
				 * + 1 */
}       block_t;

#define NEXT(x) (x)->next
#define REFS(x) (x)->refs
#define SIZE(x) (x)->size
#define BLOCK(x) (((block_t *)(x)) - 1)	/* pointer arithmetic */
#define STRING(x) ((char *)(x + 1))

#define SVALUE_STRLEN(x) (((x)->subtype == STRING_SHARED) ? \
   (SIZE(BLOCK((x)->u.string)) - sizeof(block_t) - 1) : strlen((x)->u.string))

/*
 * stralloc.c
 */
void init_strings PROT((void));
char *findstring PROT((char *));
char *make_shared_string PROT((char *));
char *ref_string PROT((char *));
void free_string PROT((char *));
int add_string_status PROT((int));

#endif
