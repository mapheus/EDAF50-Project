#include "messagehandler.h"
#include "../protocol.h"

#include <sstream>
#include <iostream>

/**
 * Create a message handler.
 * 
 * @param conn
 *            The connection to use messages
 */
MessageHandler::MessageHandler(const std::shared_ptr<Connection>& connection) : conn{connection} {}

void MessageHandler::changeConnection(const std::shared_ptr<Connection>& newConn) {
	conn = newConn;
}

void MessageHandler::sendByte(int code) 
{
	try {
		conn->write(char(code));
	} catch (ConnectionClosedException e) {
		throw new ConnectionClosedException();
	}
}

/**
 * Transmit a code (a constant from the Protocol class).
 * 
 * @param code
 *            The code to transmit.
 * @throws ConnectionClosedException
 *             If the server died
 */
void MessageHandler::sendCode(int code) 
{
	sendByte(code);
}

/**
 * Transmit an int value, according to the protocol.
 * 
 * @param value
 *            The value to transmit
 * @throws ConnectionClosedException
 *             If the server died
 */
void MessageHandler::sendInt(int value) 
{
	sendByte((value >> 24) & 0xFF);
	sendByte((value >> 16) & 0xFF);
	sendByte((value >> 8) & 0xFF);
	sendByte(value & 0xFF);
}

/**
 * Transmit an int parameter, according to the protocol.
 * 
 * @param value
 *            The parameter to transmit
 * @throws ConnectionClosedException
 *             If the server died
 */

void MessageHandler::sendIntParameter(int param) 
{
	sendCode(static_cast<int>(Protocol::PAR_NUM));
	sendInt(param);
}


/**
 * Transmit a string parameter, according to the protocol.
 * 
 * @param value
 *            The parameter to transmit
 * @throws ConnectionClosedException
 *             If the server died
 */
void MessageHandler::sendStringParameter(string param) 
{
	sendCode(static_cast<int>(Protocol::PAR_STRING));
	sendInt(param.length());
	for (std::size_t i = 0; i < param.length(); i++) 
	{
		sendByte(param[i]);
	}
}

int MessageHandler::recvByte() 
{
	try {
		int code = conn->read();
		return code;
	} catch(ConnectionClosedException c) {
		throw c;
	}
	return 0;
}

/**
 * Receive a command code or an error code from the server.
 * 
 * @return The code
 * @throws ConnectionClosedException
 *             If the server died
 */
int MessageHandler::recvCode() 
{
	int code = recvByte();
	return code;
}

/**
 * Receive an int value from the server.
 * 
 * @return The int value
 * @throws ConnectionClosedException
 *             If the server died
 */
int MessageHandler::recvInt() 
{
	int b1 = recvByte();
	int b2 = recvByte();
	int b3 = recvByte();
	int b4 = recvByte();

	return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

/**
 * Receive an int parameter from the server.
 * 
 * @return The parameter value
 * @throws ConnectionClosedException
 *             If the server died
 */

int MessageHandler::recvIntParameter() 
{
	int code = recvCode();
	if (code != static_cast<int>(Protocol::PAR_NUM)) 
	{
		throw new ProtocolViolationException("Receive numeric parameter",
				static_cast<int>(Protocol::PAR_NUM), code);
	}
	return recvInt();
}

/**
 * Receive a string parameter from the server.
 * 
 * @return The parameter value
 * @throws ConnectionClosedException
 *             If the server died
 */
string MessageHandler::recvStringParameter() 
{
	int code = recvCode();
	if (code != static_cast<int>(Protocol::PAR_STRING)) 
	{
		throw new ProtocolViolationException("Receive string parameter",
				static_cast<int>(Protocol::PAR_STRING), code);
	}
	int n = recvInt();
	if (n < 0) 
	{
		throw new ProtocolViolationException("Receive string parameter: Number of characters < 0");
	}
	std::stringstream result;
	for (int i = 1; i <= n; i++)
	{
		char ch = char(conn->read());
		result << ch;
	}
	return result.str();
}
