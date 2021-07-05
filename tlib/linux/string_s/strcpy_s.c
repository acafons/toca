/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#include <string.h>


/* strcpy_s: copy the source string into destination string since destination
             has a large enough size to hold the source string and its
             terminating null character. Returns 0 on success or -1 on error.
*/
int strcpy_s(char *dst, size_t dstsz, const char *src)
{
        if (dstsz-1 < strlen(src)) return -1;
        strcpy(dst, src);

        return 0;
}
