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

#include "boost/signals2.hpp"
#include "websocketpp/common/connection_hdl.hpp"

class WebSocketConnection
{
public:
	virtual void	ping( const std::string& msg ) = 0;
	virtual void	poll() = 0;
	virtual void	write( const std::string& msg ) = 0;
	
	template<typename T, typename Y>
	inline uint32_t	addConnectCallback( T callback, Y *callbackObject )
	{
		uint32_t id = mCallbacks.empty() ? 0 : mCallbacks.rbegin()->first + 1;
		mCallbacks.insert( std::make_pair( id, CallbackRef( new Callback( mSignalConnect.connect( std::bind( callback, callbackObject ) ) ) ) ) );
		return id;
	}
	
	template<typename T, typename Y>
	inline uint32_t	addDisconnectCallback( T callback, Y *callbackObject )
	{
		uint32_t id = mCallbacks.empty() ? 0 : mCallbacks.rbegin()->first + 1;
		mCallbacks.insert( std::make_pair( id, CallbackRef( new Callback( mSignalDisconnect.connect( std::bind( callback, callbackObject ) ) ) ) ) );
		return id;
	}
	
	template<typename T, typename Y>
	inline uint32_t	addErrorCallback( T callback, Y *callbackObject )
	{
		uint32_t id = mCallbacks.empty() ? 0 : mCallbacks.rbegin()->first + 1;
		mCallbacks.insert( std::make_pair( id, CallbackRef( new Callback( mSignalError.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) ) ) ) ) );
		return id;
	}
	
	template<typename T, typename Y>
	inline uint32_t	addInterruptCallback( T callback, Y *callbackObject )
	{
		uint32_t id = mCallbacks.empty() ? 0 : mCallbacks.rbegin()->first + 1;
		mCallbacks.insert( std::make_pair( id, CallbackRef( new Callback( mSignalInterrupt.connect( std::bind( callback, callbackObject ) ) ) ) ) );
		return id;
	}
	
	template<typename T, typename Y>
	inline uint32_t	addPingCallback( T callback, Y *callbackObject )
	{
		uint32_t id = mCallbacks.empty() ? 0 : mCallbacks.rbegin()->first + 1;
		mCallbacks.insert( std::make_pair( id, CallbackRef( new Callback( mSignalPing.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) ) ) ) ) );
		return id;
	}
	
	template<typename T, typename Y>
	inline uint32_t	addReadCallback( T callback, Y *callbackObject )
	{
		uint32_t id = mCallbacks.empty() ? 0 : mCallbacks.rbegin()->first + 1;
		mCallbacks.insert( std::make_pair( id, CallbackRef( new Callback( mSignalRead.connect( std::bind( callback, callbackObject, std::placeholders::_1 ) ) ) ) ) );
		return id;
	}
	
	void			removeCallback( uint32_t id );
protected:
	typedef boost::signals2::connection				Callback;
	typedef std::shared_ptr<Callback>				CallbackRef;
	typedef std::map<uint32_t, CallbackRef>			CallbackList;
	
	websocketpp::connection_hdl						mHandle;
	
	CallbackList									mCallbacks;
	boost::signals2::signal<void ()>				mSignalConnect;
	boost::signals2::signal<void ()>				mSignalDisconnect;
	boost::signals2::signal<void ( std::string )>	mSignalError;
	boost::signals2::signal<void ()>				mSignalInterrupt;
	boost::signals2::signal<void ( std::string )>	mSignalPing;
	boost::signals2::signal<void ( std::string )>	mSignalRead;
	boost::signals2::signal<void ()>				mSignalWrite;
};
