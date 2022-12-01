/**
 ** Supermodel
 ** A Sega Model 3 Arcade Emulator.
 ** Copyright 2003-2022 by The Supermodel Team
 **
 ** This file is part of Supermodel.
 **
 ** Supermodel is free software: you can redistribute it and/or modify it under
 ** the terms of the GNU General Public License as published by the Free
 ** Software Foundation, either version 3 of the License, or (at your option)
 ** any later version.
 **
 ** Supermodel is distributed in the hope that it will be useful, but WITHOUT
 ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 ** FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 ** more details.
 **
 ** You should have received a copy of the GNU General Public License along
 ** with Supermodel.  If not, see <http://www.gnu.org/licenses/>.
 **/

/*
 * Thread.cpp
 *
 * SDL-based implementation of threading primitives.
 */

#include "Thread.h"

#include "SDLIncludes.h"

void CThread::ThreadLoop(std::function<void()> callback)
{
	while (true)
	{
		// Wait until Signal() or Destroy() is called
		std::unique_lock<std::mutex> lk(m_mtx);
		m_cv.wait(lk, [this] {return m_signal || m_destroy; });

		// If Destroy() called, exit the loop
		if (m_destroy)
			break;

		// Let the main thread know we are busy
		m_busy = true;

		lk.unlock();

		// Run the callback function!
		callback();

		lk.lock();

		// Finished; no longer busy, reset signal
		m_busy = false;
		m_signal = false;

		lk.unlock();

		// Notify the main thread that we are done
		m_cv.notify_one();
	}
}

UINT32 CThread::GetTicks()
{
	return SDL_GetTicks();
}

void CThread::Create(std::function<void()> callback)
{
	// Create the thread if it hasn't already been created
	if (!m_thread.joinable())
		m_thread = std::thread(&CThread::ThreadLoop, this, callback);
}

void CThread::Signal(void)
{
	{
		std::lock_guard<std::mutex> lk(m_mtx);

		// If the thread is busy, do nothing
		if (m_busy)
			return;

		// Signal the thread to run the callback function
		m_signal = true;
	}

	m_cv.notify_one();
}

void CThread::Wait(void)
{
	// Wait for the thread to return from the callback function
	std::unique_lock<std::mutex> lk(m_mtx);
	m_cv.wait(lk, [this] {return !m_busy; });
}

void CThread::Destroy(void)
{
	{
		std::unique_lock<std::mutex> lk(m_mtx);
		m_cv.wait(lk, [this] {return !m_busy; });

		m_destroy = true;
	}

	m_cv.notify_one();

	// Wait for the thread to end
	if (m_thread.joinable())
		m_thread.join();
}