/*
 * mac.h
 *
 * Implementation of the physical and MAC layer.
 */

#ifndef MAC_H_
#define MAC_H_

// Geographic Coordinate of a wireless sensor node
typedef struct {
	int x, y, z;
} Coordinate;

class MAC {
public:
	MAC() {	}
	static double propagationSpeed_;
};
#endif
