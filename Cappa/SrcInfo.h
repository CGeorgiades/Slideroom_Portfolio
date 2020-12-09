/*
 * SrcInfo.h
 *
 *  Created on: Nov 22, 2020
 *      Author: chris
 */

#ifndef SRCINFO_H_
#define SRCINFO_H_

class SrcLocation {
public:
	int line, col;
	SrcLocation(): line(0), col(0){}
	SrcLocation(int line, int col): line(line), col(col) {}
	void advance(char ch);
};

#endif /* SRCINFO_H_ */
