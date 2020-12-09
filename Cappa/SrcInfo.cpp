/*
 * SrcInfo.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#include "SrcInfo.h"
#include <stdio.h>

void SrcLocation::advance(char ch) {
	switch(ch) {
	case '\n':
		line++;
		col = 1;
		break;
	case EOF:
		break;
	default:
		col++;
	}
}
