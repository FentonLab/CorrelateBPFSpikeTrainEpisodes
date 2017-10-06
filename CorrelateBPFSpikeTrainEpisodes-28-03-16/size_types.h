
#ifndef SIZE_TYPES_IN		/* machine dependent "c" types */
#define SIZE_TYPES_IN


typedef	unsigned char	ui1;	/* one byte unsigned integer */
typedef	char		si1;	/* one byte signed integer */
typedef unsigned short	ui2;	/* two byte unsigned integer */
typedef short		si2;	/* two byte signed integer */
typedef unsigned int	ui4;	/* four byte unsigned integer */ 
typedef int		si4;	/* four byte signed integer */ 
typedef float		sf4;	/* four byte signed floating point number */ 
typedef double		sf8;	/* eight byte signed floating point number */ 
typedef long long	ui8;	/* eight byte unsigned integer */

// #define BIG_ENDIAN_MACHINE	/* define BIG_ENDIAN_MACHINE for most non-Intel machines */
#define LITTLE_ENDIAN_MACHINE	/* define LITTLE_ENDIAN_MACHINE for Intel and Intel clones, and a few others */

#endif
