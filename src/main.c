/*******************************************************************************
	Copyright (C) 2017 by Andrew Chang <laplacezhang@126.com>
	Licensed under the LGPL v2.1, see the file COPYING in base directory.
	
	File name: 	main.h
	
	Description: 	
	    This is the test source for cAssoArray. If you want to use cAssoArray
	itself only, please simply copy cAssoArray.c/h files to your project. But
	make sure not to break the LGPL license.
			
	History:
		2014-03-04: File created as "main.c"

	------------------------------------------------------------------------

	    This library is free software; you can redistribute it and/or modify it 
	under the terms of the GNU Lesser General Public License as published by the 
	Free Software Foundation, version 2.1 of the License. 
	    This library is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
	details. 
	    You should have received a copy of the GNU Lesser General Public License 
	along with this library; if not, see <http://www.gnu.org/licenses/>.
		
********************************************************************************/

#include "cAssocArray.h"
#include "log.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define _ALPHABET_COUNT		26

static char *g_Values[_ALPHABET_COUNT] = {
	"Alpha",
	"Bravo",
	"Charlie",
	"Delta",
	"Echo",
	"Foxtrot",
	"Golf",
	"Hotel",
	"India",
	"Juliet",
	"Kilo",
	"Lima",
	"Mike",
	"November",
	"Oscar",
	"Papa",
	"Quebec",
	"Romeo",
	"Sierra",
	"Tango",
	"Uniform",
	"Victor",
	"Whiskey",
	"Xray",
	"Yankee",
	"Zulu"
};


static uint8_t _rand_u8()
{
	uint8_t ret = 0;
	ssize_t callStat = 0;
	char errorMsg[128] = "";
	int fd = open("/dev/urandom", O_RDONLY);

	if (fd < 0) {
		snprintf(errorMsg, sizeof(errorMsg), "Failed to open ramdom device: %s\n", strerror(errno));
		goto ENDS;
	}

	callStat = read(fd, &(ret), sizeof(ret));
	if (callStat < 0) {
		snprintf(errorMsg, sizeof(errorMsg), "Failed to read from ramdom device: %s\n", strerror(errno));
		goto ENDS;
	}

ENDS:
	if (fd > 0) {
		close(fd);
		fd = -1;
	}

	if (errorMsg[0]) {
		write(2, errorMsg, strlen(errorMsg));
	}
	
	return ret;
}


static int _rand()
{
	uint32_t ret = 0;

	ret += _rand_u8() << 0;
	ret += _rand_u8() << 8;
	ret += _rand_u8() << 16;
	ret += _rand_u8() << 24;

	return (int)(ret & 0x7FFFFFFF);
}


static int _randint(int minInt, int maxInt)
{
	int result;

	if (minInt == maxInt)
	{
		result = minInt;
	}
	else
	{
		if (minInt > maxInt)
		{
			/* swap */
			result = minInt;
			minInt = maxInt;
			maxInt = result;
		}

		/* randomize */
		result = minInt + (int)((float)(maxInt - minInt + 1) * (float)(_rand() / (RAND_MAX + 1.0)));
	}
	return result;
}


int main(int argc, char* argv[])
{
	cAssocArray *array = cAssocArray_Create(TRUE);
	
#if 0
	long tmp = 0;
	for (tmp = 0; tmp < _ALPHABET_COUNT; tmp++)
	{
		char key[] = "A";
		key[0] += tmp;
		AMCLog("Add %s", key);
		cAssocArray_SetValue(array, key, g_Values[tmp], FALSE);
		cAssocArray_DumpToStdout(array);
	}
#endif

	cAssocArray_SetValue(array, "A", g_Values[0], FALSE);
	cAssocArray_SetValue(array, "B", g_Values[1], FALSE);
	cAssocArray_SetValue(array, "C", g_Values[2], FALSE);
	cAssocArray_SetValue(array, "D", g_Values[3], FALSE);
	cAssocArray_SetValue(array, "E", g_Values[4], FALSE);
	cAssocArray_SetValue(array, "F", g_Values[5], FALSE);
	cAssocArray_SetValue(array, "G", g_Values[6], FALSE);
	cAssocArray_SetValue(array, "H", g_Values[7], FALSE);
	cAssocArray_SetValue(array, "I", g_Values[8], FALSE);
	cAssocArray_SetValue(array, "J", g_Values[9], FALSE);
	cAssocArray_SetValue(array, "K", g_Values[10], FALSE);
	cAssocArray_SetValue(array, "L", g_Values[11], FALSE);
	cAssocArray_SetValue(array, "M", g_Values[12], FALSE);
	cAssocArray_SetValue(array, "N", g_Values[13], FALSE);
	cAssocArray_SetValue(array, "O", g_Values[14], FALSE);
	cAssocArray_SetValue(array, "P", g_Values[15], FALSE);
	cAssocArray_SetValue(array, "Q", g_Values[16], FALSE);
	cAssocArray_SetValue(array, "R", g_Values[17], FALSE);
	cAssocArray_SetValue(array, "S", g_Values[18], FALSE);
	cAssocArray_SetValue(array, "T", g_Values[19], FALSE);
	cAssocArray_SetValue(array, "U", g_Values[20], FALSE);
	cAssocArray_SetValue(array, "V", g_Values[21], FALSE);
	cAssocArray_SetValue(array, "W", g_Values[22], FALSE);
	cAssocArray_SetValue(array, "X", g_Values[23], FALSE);
	cAssocArray_SetValue(array, "Y", g_Values[24], FALSE);
	cAssocArray_SetValue(array, "Z", g_Values[25], FALSE);
	cAssocArray_DumpToStdout(array);
	
	/* test remove */
	{
		char key[] = "A";
		long tmp;
		for (tmp = 0; tmp < _ALPHABET_COUNT; tmp ++)
		{
			key[0] = 'A' + _randint(0, 25);
			cAssocArray_RemoveValue(array, key, FALSE);cAssocArray_DumpToStdout(array);
		}
	}

#if 0
	cAssocArray_RemoveValue(array, "R", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "J", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "S", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "M", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "U", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "L", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "Q", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "G", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "C", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "V", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "N", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "O", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "K", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "X", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "E", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "T", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "Y", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "P", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "H", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "F", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "I", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "D", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "Z", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "B", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "A", FALSE);cAssocArray_DumpToStdout(array);
	cAssocArray_RemoveValue(array, "W", FALSE);cAssocArray_DumpToStdout(array);
#endif

	cAssocArray_Delete(array, FALSE);
	AMCLog("Test finished");
	return 0;
}

