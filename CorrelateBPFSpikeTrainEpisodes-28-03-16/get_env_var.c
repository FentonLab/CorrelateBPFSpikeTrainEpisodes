
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "size_types.h"


si1	*get_env_var(var)
si1	*var;
{
	si1	*env_var, temp_str[256], *s;
	si4	len;


	if ((env_var = getenv(var)) == NULL) {
		(void) fprintf(stderr, "%cThe variable \"%s\" is not defined in the environment. Please define it now: ", 7, var);
		(void) sprintf(temp_str, "%s=", var);
		len = strlen(temp_str);
		s = temp_str + len;
		len = 256 - len;
		(void) fgets(s, len, stdin);
		len = strlen(temp_str);
		if (temp_str[len - 1] == '\n')
			temp_str[len - 1] = '\0';
		if (putenv(temp_str)) {
			(void) fprintf(stderr, "%c\n\tCould not create the environment variable \"%s\". Terminating program.\n\n", 7, var);
			exit(1);
		}
		env_var = getenv(var);
		(void) fprintf(stderr, "Thank you.\n");
	}

	return(env_var);
}
		
