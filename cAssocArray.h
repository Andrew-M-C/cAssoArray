/*******************************************************************************
	Copyright (C) 2017 by Andrew Chang <laplacezhang@126.com>
	File name: 	cAssocArray.h

	Description: 	
	    Interface declaration for cAssocArray.
			
	History:
		2014-03-05: File rename as "cAssocArray.h" along with class name
		2014-03-04: File created as "cAssoArray.h"

	------------------------------------------------------------------------

	    This library is free software; you can redistribute it and/or modify it 
	under the terms of the GNU Lesser General Public License as published by the 
	Free Software Foundation, version 2.1 of the License. 
	    This library is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
	details. 
	    You should have received a copy of the GNU Lesser General Public License 
	along with this library; if not, see <http://www.gnu.org/licenses/>
		
********************************************************************************/

#ifndef __C_ASSOC_ARRAY_H__
#define __C_ASSOC_ARRAY_H__

struct _cAssocArray;
typedef struct _cAssocArray cAssocArray;

#ifndef NULL
#ifndef _DO_NOT_DEF_NULL
#define NULL	((void *)0)
#endif
#endif

#ifndef BOOL
#ifndef _DO_NOT_DEF_BOOL
#define BOOL	int
#define FALSE	(0)
#define TRUE	(!FALSE)
#endif
#endif

cAssocArray *cAssocArray_Create(BOOL locked);
int cAssocArray_Delete(cAssocArray *array);

int cAssocArray_SetValue(cAssocArray *array, const char *key, void *value, BOOL freeDuplicate);
void *cAssocArray_GetValue(cAssocArray *array, const char *key);
long cAssocArray_Size(cAssocArray *array);

int cAssocArray_RemoveValue(cAssocArray *array, const char *key, BOOL shouldFree);
void *cAssocArray_DetachValue(cAssocArray *array, const char *key);

int cAssocArray_AddValue(cAssocArray *array, const char *key, void *value);
int cAssocArray_UpdateValue(cAssocArray *array, const char *key, void *value, BOOL freeDuplicate, void **prevValueOut);

void cAssocArray_DumpToStdout(cAssocArray *array);

#endif

