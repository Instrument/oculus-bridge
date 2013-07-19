/*
 * Copyright (c) 2013, Ban the Rewind, Wieden+Kennedy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ban the Rewind nor the names of its
 * contributors may be used to endorse or promote products
 * derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/client.hpp"

#include "WebSocketConnection.h"

class WebSocketClient : public WebSocketConnection
{
public:
	WebSocketClient();
	~WebSocketClient();

	void	connect( const std::string& uri );
	void	disconnect();
	void	ping( const std::string& msg = "" );
	void	poll();
	void	write( const std::string& msg );
protected:
	typedef websocketpp::client<websocketpp::config::asio_client>	Client;
	typedef websocketpp::config::asio_client::message_type::ptr		MessageRef;
	
	Client	mClient;
	
	void	onConnect( Client* client, websocketpp::connection_hdl handle );
	void	onDisconnect( Client* client, websocketpp::connection_hdl handle );
	void	onFail( Client* client, websocketpp::connection_hdl handle );
	void	onInterrupt( Client* client, websocketpp::connection_hdl handle );
	void	onPing( Client* client, websocketpp::connection_hdl handle, std::string msg );
	void	onRead( Client* client, websocketpp::connection_hdl handle, MessageRef msg );
};
