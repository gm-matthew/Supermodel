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
 * Thread.h
 * 
 * Header file for threading primitives.
 */
 
#ifndef INCLUDED_THREADS_H
#define INCLUDED_THREADS_H

#include "Types.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

/*
 * CThread
 *
 * Class that represents an O/S thread.
 */
class CThread
{
private:
	std::thread m_thread;
	std::mutex m_mtx;
	std::condition_variable m_cv;

	bool m_signal = false;
	bool m_busy = false;
	bool m_destroy = false;

	void ThreadLoop(std::function<void()> callback);

public:
	/*
	 * GetTicks
	 *
	 * Gets number of millseconds since beginning of program.
	 */
	static UINT32 GetTicks();

	/*
	 * Create()
	 *
	 * Creates a new thread with the specified callback function.
	 * Does nothing if the thread has already been created.
	 */
	void Create(std::function<void()> callback);

	/*
	 * Signal()
	 *
	 * Signals the thread to run the callback function.
	 * Does nothing if the thread is currently busy.
	 */
	void Signal(void);

	/*
	 * Wait()
	 *
	 * Wait until the thread has finished processing.
	 * Returns immediately if the thread is not currently busy.
	 */
	void Wait(void);

	/*
	 * Destroy()
	 *
	 * Waits for the thread to finish processing and then destroys it.
	 */
	void Destroy(void);
};

#endif	// INCLUDED_THREADS_H
