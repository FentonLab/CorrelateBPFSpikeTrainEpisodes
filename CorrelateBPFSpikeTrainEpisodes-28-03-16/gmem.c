
#include <stdlib.h>
#include <stdio.h>


void	*gmem(n_elements, size)
size_t	n_elements, size;
{
	void	 *ptr;

	ptr = calloc(n_elements, size);
	if (ptr == NULL) {
		(void) fprintf(stderr, "%c\n\tNot enough memory. Terminating program.\n\n", 7);
		exit(1);
	}

	return(ptr);
}
