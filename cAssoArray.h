/*******************************************************************************
	Copyright (C) 2017 by Andrew Chang <laplacezhang@126.com>
	File name: 	cAssoArray.h

	Description: 	
	    Interface declaration for cAssoArray.
			
	History:
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

#ifndef __C_ASSO_ARRAY_H__
#define __C_ASSO_ARRAY_H__

struct cAssoArray;
typedef struct cAssoArray cAssoArray;

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

cAssoArray *cAssoArray_Create(void);
int cAssoArray_Delete(cAssoArray *array);

int cAssoArray_Set(cAssoArray *array, const char *key, void *value, BOOL freeDuplicate);
int cAssoArray_Get(cAssoArray *array, const char *key);

int cAssoArray_Remove(cAssoArray *array, const char *key, BOOL shouldFree);
void *cAssoArray_Drain(cAssoArray *array, const char *key);

int cAssoArray_Add(cAssoArray *array, const char *key, void *value);
int cAssoArray_Update(cAssoArray *array, const char *key, void *value, BOOL freeDuplicate, void **prevValueOut);


#endif

