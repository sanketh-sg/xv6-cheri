#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
	char * __volatile cp; /* volatile to prevent compiler optimizations */

	cp = malloc(sizeof(*cp)); 
	assert(cp != NULL);

	/* Use while allocated. */
	*cp = 'a';
    printf("cp = %c\n", *cp);
	/* Use after free. */
	free(cp);
	*cp = 'b';
    printf("cp = %c\n", *cp);
	/* Use after synchronous revocation. */
	// malloc_revoke(); deprecated doesnt return status
    malloc_revoke_quarantine_force_flush();
	*cp = 'c';
    printf("cp = %c\n", *cp);
	exit(1);
}