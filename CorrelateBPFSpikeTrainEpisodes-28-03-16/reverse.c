#include <stdlib.h>
#include <stdio.h>
#include "size_types.h"

ui4      reverse_ui4(b)
ui1             *b;
{       
        ui1     *f;
        static ui4     u;

        f = (ui1 *) &u;

#ifdef BIG_ENDIAN_MACHINE
        b += 3;

        *f++ = *b--;
        *f++ = *b--;
        *f++ = *b--;
        *f = *b;
#else 
        *f++ = *b++;
        *f++ = *b++;
        *f++ = *b++;
        *f = *b;
#endif  

        return(u);
}       

ui4      ts_reverse_ui4(b)
ui1             *b;
{       
        ui1     *f;
        static ui4     u;

        f = (ui1 *) &u;

#ifdef LITTLE_ENDIAN_MACHINE
        b += 3;

        *f++ = *b--;
        *f++ = *b--;
        *f++ = *b--;
        *f = *b;
#else 
        *f++ = *b++;
        *f++ = *b++;
        *f++ = *b++;
        *f = *b;
#endif  

        return(u);
}       

si2      reverse_si2(b)  
ui1             *b;
{       
        ui1     *f;
        static si2     u;
                        
        f = (ui1 *) &u;

#ifdef BIG_ENDIAN_MACHINE
        ++b;

        *f++ = *b--;
        *f = *b;        
#else
        *f++ = *b++;
        *f = *b;        
#endif

        return(u);
}

ui4      reorder_ui4(b)
ui1             *b;
{       
        ui1     *f;
        static ui4     u;

        f = (ui1 *) &u;

#ifdef BIG_ENDIAN_MACHINE
        b += 3;
        *f++ = *b--;
        *f++ = *b--;
        *f++ = *b--;
        *f = *b;
#else
        *f++ = *b++;
        *f++ = *b++;
        *f++ = *b++;
        *f = *b;
#endif
        return(u);
}       
