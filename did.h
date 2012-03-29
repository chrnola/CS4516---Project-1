/*
 * did.h
 */

#define ___DID_H_INCLUDE___
#ifndef ___DID_H_INCLUDE___

public class Packet{
	public:
		Packet();
		byte[] toFrame();
};

public void StartClient();

public void StartServer();

#endif
