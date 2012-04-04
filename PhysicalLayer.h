
/*
 * PhysicalLayer.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Aaron Sikes
 */

#ifndef PHYSICAL_H_
#define PHYSICAL_H_

class PhysicalLayer {
private:
	int sockfd;
	void run();
public:
	PhysicalLayer(const char *hostname);
	PhysicalLayer(int sockfd);
	virtual ~PhysicalLayer();
};

#endif /* PHYSICAL_H_ */
