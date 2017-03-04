/*******************************************************************************
	Copyright (C) 2017 by Andrew Chang <laplacezhang@126.com>
	File name: 	cAssoArray.h

	Description: 	
	    Log and print tools.
			
	History:
		2014-03-04: File created as "cAssoArray.c"

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

/********/
#define __HEADERS_AND_DEFIITIONS
#ifdef __HEADERS_AND_DEFIITIONS

#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define _CFG_LOG_BUFFER_LEN		1024

#endif


/********/
#define __PUBLIC_INTERFACES
#ifdef __PUBLIC_INTERFACES

ssize_t AMCLog(const char *format, ...)
{
	char buff[_CFG_LOG_BUFFER_LEN];
	va_list vaList;
	size_t dateLen = 0;
	
	tzset();
	time_t currSec = time(0);
	struct tm currTime;
	struct tm *pTime = localtime(&currSec);
	struct timeval currDayTime;

	gettimeofday(&currDayTime, NULL);
	if (pTime)
	{
		memcpy(&currTime, pTime, sizeof(currTime));
		dateLen = sprintf(buff, "%04d-%02d-%02d,%02d:%02d:%02d.%06ld ", 
							currTime.tm_year + 1900, currTime.tm_mon + 1, currTime.tm_mday,
							currTime.tm_hour, currTime.tm_min, currTime.tm_sec, currDayTime.tv_usec);
	}

	va_start(vaList, format);
	vsnprintf((char *)(buff + dateLen), sizeof(buff) - dateLen - 1, format, vaList);
	va_end(vaList);

	dateLen = strlen(buff);
	buff[dateLen + 0] = '\n';
	buff[dateLen + 1] = '\0';

	return (write(1, buff, dateLen + 1));
}


#endif

/* EOF */

