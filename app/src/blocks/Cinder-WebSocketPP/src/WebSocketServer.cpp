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

#include "WebSocketServer.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace std;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

WebSocketServer::WebSocketServer()
{
    mServer.clear_access_channels(websocketpp::log::alevel::all);
	mServer.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);
	
	mServer.init_asio();
	
	mServer.set_close_handler( websocketpp::lib::bind( &WebSocketServer::onDisconnect, this, &mServer, ::_1 ) );
	mServer.set_interrupt_handler( websocketpp::lib::bind( &WebSocketServer::onInterrupt, this, &mServer, ::_1 ) );
	mServer.set_open_handler( websocketpp::lib::bind( &WebSocketServer::onConnect, this, &mServer, ::_1 ) );
	mServer.set_ping_handler( websocketpp::lib::bind( &WebSocketServer::onPing, this, &mServer, ::_1, ::_2 ) );
	mServer.set_message_handler( websocketpp::lib::bind( &WebSocketServer::onRead, this, &mServer, ::_1, ::_2 ) );
	mServer.set_fail_handler( websocketpp::lib::bind( &WebSocketServer::onFail, this, &mServer, ::_1 ) );
}

WebSocketServer::~WebSocketServer()
{
	if ( !mServer.stopped() ) {
		cancel();
		mServer.stop();
	}
}


void WebSocketServer::close(){
    try {
        mServer.close(mHandle, websocketpp::close::status::normal, "Connection Closed");
	} catch ( const std::exception& ex ) {
		mSignalError( ex.what() );
    } catch ( websocketpp::lib::error_code err ) {
        mSignalError( err.message() );
    } catch ( ... ) {
        mSignalError( "An unknown exception occurred." );
    }
}

void WebSocketServer::cancel()
{
	try {
		mServer.stop();
	} catch ( const std::exception& ex ) {
		mSignalError( ex.what() );
    } catch ( websocketpp::lib::error_code err ) {
        mSignalError( err.message() );
    } catch ( ... ) {
        mSignalError( "An unknown exception occurred." );
    }
}

void WebSocketServer::listen( uint16_t port )
{
	try {
		mServer.listen( port );
		mServer.start_accept();
	} catch ( const std::exception& ex ) {
		mSignalError( ex.what() );
    } catch ( websocketpp::lib::error_code err ) {
        mSignalError( err.message() );
    } catch ( ... ) {
        mSignalError( "An unknown exception occurred." );
    }
}

void WebSocketServer::ping( const string& msg )
{
	try {
		mServer.get_con_from_hdl( mHandle )->ping( msg );
	} catch( ... ) {
		mSignalError( "Ping failed." );
	}
}

void WebSocketServer::poll()
{
	mServer.poll();
}

void WebSocketServer::write( const std::string& msg )
{
	if ( msg.empty() ) {
		mSignalError( "Cannot send empty message." );
	} else {
		websocketpp::lib::error_code err;
		mServer.send( mHandle, msg, websocketpp::frame::opcode::TEXT, err );
		if ( err ) {
			mSignalError( err.message() );
		} else {
			mSignalWrite();
		}
	}
}

std::string WebSocketServer::getClientAddress() {
    if( mServer.get_con_from_hdl(mHandle) ){
        return mServer.get_con_from_hdl(mHandle)->get_host();
    }else{
        return "";
    }
}

void WebSocketServer::onConnect( Server* client, websocketpp::connection_hdl handle )
{
	mHandle = handle;
	mSignalConnect();
}

void WebSocketServer::onDisconnect( Server* client, websocketpp::connection_hdl handle )
{
	mSignalDisconnect();
}

void WebSocketServer::onFail( Server* client, websocketpp::connection_hdl handle )
{
	mHandle = handle;
	mSignalError( "Transfer failed." );
}

void WebSocketServer::onInterrupt( Server* client, websocketpp::connection_hdl handle )
{
	mHandle = handle;
	mSignalInterrupt();
}

bool WebSocketServer::onPing( Server* client, websocketpp::connection_hdl handle, string msg )
{
	mHandle = handle;
	mSignalPing( msg );
	return true;
}

void WebSocketServer::onRead( Server* client, websocketpp::connection_hdl handle, MessageRef msg )
{
	mHandle = handle;
	mSignalRead( msg->get_payload() );
}
