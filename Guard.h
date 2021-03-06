/*
 *  Copyright (C) 2011 Stephen F. Booth <me@sbooth.org>
 *  All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - Neither the name of Stephen F. Booth nor the names of its 
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Mutex.h"

// ========================================
// A wrapper around a pthread mutex and condition variable
// ========================================
class Guard : public Mutex
{
public:
	Guard();
	virtual ~Guard();

	// Wait() and WaitUntil() will throw std::runtime_error if the mutex isn't locked
	// WaitUntil() returns true if the request timed out, false otherwise

	void Wait();
	bool WaitUntil(struct timespec absoluteTime);

	void Signal();
	void Broadcast();

protected:
	Guard(const Guard& guard);
	Guard& operator=(const Guard& guard);

	pthread_cond_t mCondition;

public:
	class Locker
	{
	public:
		Locker(Guard& guard);
		~Locker();

		inline void Wait()										{ mGuard.Wait(); }
		inline bool WaitUntil(struct timespec absoluteTime)		{ return mGuard.WaitUntil(absoluteTime); }

		inline void Signal()									{ mGuard.Signal(); }
		inline void Broadcast()									{ mGuard.Broadcast(); }

	private:
		Guard& mGuard;
		bool mReleaseLock;	
	};
};
