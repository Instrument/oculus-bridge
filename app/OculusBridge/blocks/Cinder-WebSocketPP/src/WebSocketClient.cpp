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

#include "WebSocketClient.h"

using namespace std;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

WebSocketClient::WebSocketClient()
{
	mClient.clear_access_channels( websocketpp::log::alevel::all );
	mClient.clear_error_channels( websocketpp::log::elevel::all );
	
	mClient.init_asio();

	mClient.set_close_handler( websocketpp::lib::bind( &WebSocketClient::onDisconnect, this, &mClient, ::_1 ) );
	mClient.set_interrupt_handler( websocketpp::lib::bind( &WebSocketClient::onInterrupt, this, &mClient, ::_1 ) );
	mClient.set_open_handler( websocketpp::lib::bind( &WebSocketClient::onConnect, this, &mClient, ::_1 ) );
	mClient.set_pong_handler( websocketpp::lib::bind( &WebSocketClient::onPing, this, &mClient, ::_1, ::_2 ) );
	mClient.set_message_handler( websocketpp::lib::bind( &WebSocketClient::onRead, this, &mClient, ::_1, ::_2 ) );
	mClient.set_fail_handler( websocketpp::lib::bind( &WebSocketClient::onFail, this, &mClient, ::_1 ) );
}

WebSocketClient::~WebSocketClient()
{
	if ( !mClient.stopped() ) {
		disconnect();
		mClient.stop();
	}
}

void WebSocketClient::connect( const std::string& uri )
{
	try {
		websocketpp::lib::error_code err;
		Client::connection_ptr conn = mClient.get_connection( uri, err );
		if ( err ) {
			mSignalError( err.message() );
		} else {
			if ( conn ) {
				mClient.connect( conn );
			} else {
				mSignalError( "Unable to resolve address." );
			}
		}
	} catch ( const std::exception& ex ) {
		mSignalError( ex.what() );
    } catch ( websocketpp::lib::error_code err ) {
        mSignalError( err.message() );
    } catch ( ... ) {
        mSignalError( "An unknown exception occurred." );
    }
}

void WebSocketClient::disconnect()
{
	websocketpp::lib::error_code err;
	mClient.close( mHandle, websocketpp::close::status::going_away, "", err );
	if ( err ) {
		mSignalError( err.message() );
	}
}

void WebSocketClient::ping( const string& msg )
{
	try {
		mClient.get_con_from_hdl( mHandle )->ping( msg );
	} catch( ... ) {
		mSignalError( "Ping failed." );
	}
}

void WebSocketClient::poll()
{
	mClient.poll();
}

void WebSocketClient::write( const std::string& msg )
{
	if ( msg.empty() ) {
		mSignalError( "Cannot send empty message." );
	} else {
		websocketpp::lib::error_code err;
		mClient.send( mHandle, msg, websocketpp::frame::opcode::TEXT, err );
		if ( err ) {
			mSignalError( err.message() );
		} else {
			mSignalWrite();
		}
	}
}

void WebSocketClient::onConnect( Client* client, websocketpp::connection_hdl handle ) 
{
	mHandle = handle;
	mSignalConnect();
}

void WebSocketClient::onDisconnect( Client* client, websocketpp::connection_hdl handle ) 
{
	mSignalDisconnect();
}

void WebSocketClient::onFail( Client* client, websocketpp::connection_hdl handle ) 
{
	mHandle = handle;
	mSignalError( "Transfer failed." );
}

void WebSocketClient::onInterrupt( Client* client, websocketpp::connection_hdl handle ) 
{
	mHandle = handle;
	mSignalInterrupt();
}

void WebSocketClient::onPing( Client* client, websocketpp::connection_hdl handle, string msg )
{
	mHandle = handle;
	mSignalPing( msg );
}

void WebSocketClient::onRead( Client* client, websocketpp::connection_hdl handle, MessageRef msg ) 
{
	mHandle = handle;
	mSignalRead( msg->get_payload() );
}
