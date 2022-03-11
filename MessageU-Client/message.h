/**
	Message.
	@file client.h
	@author Itay Matza
	@version 1.0 01/03/21
*/

#ifndef __MESSAGE__
#define __MESSAGE__

#include <iostream>

// Supported response status codes.
enum MessageType : uint8_t {
	REQUEST_FOR_SYMMETRIC_KEY = 1,
	SYMMETRIC_KEY = 2,
	TEXT_MESSAGE = 3,
	SEND_FILE = 4
};


#endif /* __MESSAGE__ */
