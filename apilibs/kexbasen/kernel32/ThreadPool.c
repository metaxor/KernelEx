/*
 *  KernelEx
 *  Copyright (C) 2009-2010, Tihiy
 *
 *  This file is part of KernelEx source code.
 *
 *  KernelEx is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation; version 2 of the License.
 *
 *  KernelEx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <windows.h>
#include <malloc.h>
#include "shlord.h"

/*
 *  The following functions are implemented:
 *  QueueUserWorkItem (with SHLWAPI.SHQueueUserWorkItem)
 *  RegisterWaitForSingleObject
 *  RegisterWaitForSingleObjectEx
 *  UnregisterWait
 *  UnregisterWaitEx
 *  CreateTimerQueue
 *  CreateTimerQueueTimer
 *  DeleteTimerQueue
 *  DeleteTimerQueueEx
 *  DeleteTimerQueueTimer
 *  all functions could be implemented with shlwapi,
 *  but they don't support async de-registration and most flags.
 *  Wine code was completely purged outta here - it's no good.
 */

/*	RegisterWait/TimerQueues are implemented using waitable timers, 
 *  unlike wine/shlwapi/whatever implementations exist.
 */


#define TPS_EXECUTEIO 0x00000001
#define TPS_LONGEXECTIME 0x00000008

typedef BOOL (WINAPI* SHQueueUserWorkItem_API) (
		LPTHREAD_START_ROUTINE pfnCallback,
		LPVOID pContext,
		LONG lPriority,
		PDWORD dwTag,
		PDWORD * pdwId,
		LPCSTR pszModule,
		DWORD dwFlags
);

static SHQueueUserWorkItem_API SHQueueUserWorkItem;


/* MAKE_EXPORT QueueUserWorkItem_new=QueueUserWorkItem */
BOOL WINAPI QueueUserWorkItem_new( LPTHREAD_START_ROUTINE Function, PVOID Context, ULONG Flags)
{
	if (!SHQueueUserWorkItem)
		SHQueueUserWorkItem = (SHQueueUserWorkItem_API)GetShlwapiProc(260);

	DWORD dwFlags = 0;
	if (Flags & WT_EXECUTEINIOTHREAD) dwFlags |= TPS_EXECUTEIO;
	if (Flags & WT_EXECUTELONGFUNCTION) dwFlags |= TPS_LONGEXECTIME;

	return SHQueueUserWorkItem( Function, Context, 0, NULL, NULL, NULL, dwFlags );
}

//256h should be enough for everyone
#define MAX_WAIT_ITEMS (MAXIMUM_WAIT_OBJECTS / 2)
#define MAX_WAIT_THREADS	8

#define WAIT_STATE_ACTIVE 0			//item is in wait thread
#define WAIT_STATE_UNREGISTERED 1	//item is unregistered and queued to be removed from wait thread
#define WAIT_STATE_REMOVED 2		//item is removed from wait thread
#define WAIT_STATE_DELETESYNC 3		//item will be destroyed with removal
#define WAIT_STATE_GAMEOVER 4		//item is destroyed
#define WAIT_STATE_ABANDONED 5		//item expired and removed from wait thread but not yet unregistered

#define NONSENSE	10000

//	RegisterWaitForSingleObject routines.

typedef struct
{
	HANDLE Object;
	HANDLE TimerObject;
	WAITORTIMERCALLBACK Callback;
	PVOID Context;
	ULONG Milliseconds;
	ULONG Flags;
	PVOID waitThread;
	LONG State;
	LONG CallbacksPending;
	HANDLE CompletionEvent;
	HANDLE CallbackEvent;
} WAIT_WORK_ITEM, *PWAIT_WORK_ITEM;

typedef struct 
{
	HANDLE hThread;
	HANDLE WaitEvent;
	BOOL InSyncCallback;
	LONG nItems;
	PWAIT_WORK_ITEM waitItems[MAX_WAIT_ITEMS];	
} WAIT_THREAD, *PWAIT_THREAD;

typedef struct
{
	PWAIT_WORK_ITEM item;
	BOOLEAN TimerOrWaitFired;
} ASYNC_CALL, *PASYNC_CALL;

static PWAIT_THREAD waitthreads[MAX_WAIT_THREADS] = {0};
static CRITICAL_SECTION wait_cs;

BOOL init_threadpool()
{
	InitializeCriticalSection(&wait_cs);
	return TRUE;
}

static VOID ActivateTimer(PWAIT_WORK_ITEM workItem)
{
	if (workItem->Milliseconds != INFINITE)
	{
		LARGE_INTEGER timeout;
		timeout.QuadPart = workItem->Milliseconds*(LONGLONG)-10000;
		SetWaitableTimer(workItem->TimerObject,&timeout,0,NULL,NULL,FALSE);
	}
}

static VOID DestroyWaitItem(PWAIT_WORK_ITEM workItem)
{
	CloseHandle(workItem->CallbackEvent);
	CloseHandle(workItem->TimerObject);
	HeapFree(GetProcessHeap(),0,workItem);
}

static VOID CALLBACK AddWaitItem(ULONG_PTR Arg)
{
	PWAIT_WORK_ITEM workItem = (PWAIT_WORK_ITEM)Arg;
	PWAIT_THREAD waitThread = (PWAIT_THREAD)workItem->waitThread;
	int i;
	for (i=0;i<waitThread->nItems;i++)
	{
		//we can't have one handle several times
		if (waitThread->waitItems[i]->Object == workItem->Object) 
		{
			workItem->waitThread = NULL;
			SetEvent(workItem->CompletionEvent);
			return;
		}
	}	
	waitThread->waitItems[waitThread->nItems] = workItem;
	waitThread->nItems++;
	SetEvent(workItem->CompletionEvent);
	//what if callback will fire before RegisterWait... returns?
	ActivateTimer(workItem);
}

static VOID CALLBACK RemoveWaitItem(ULONG_PTR Arg)
{
	PWAIT_WORK_ITEM workItem = (PWAIT_WORK_ITEM)Arg;
	PWAIT_THREAD waitThread = (PWAIT_THREAD)workItem->waitThread;
	int i, j;
	for (i=0;i<MAX_WAIT_ITEMS;i++)
		if (waitThread->waitItems[i] == workItem)
			break;
	if (i<MAX_WAIT_ITEMS)
	{
		for (j=i;j<waitThread->nItems-1;j++)
		{
			waitThread->waitItems[j]=waitThread->waitItems[j+1];
			waitThread->waitItems[j+1]=NULL;
		}
		waitThread->nItems--;
	}
	//if unregister marked it for delete, kill it here
	if (InterlockedCompareExchange(&workItem->State,WAIT_STATE_GAMEOVER,WAIT_STATE_DELETESYNC) == WAIT_STATE_DELETESYNC)
	{
		if (workItem->CompletionEvent) SetEvent(workItem->CompletionEvent);		
		DestroyWaitItem(workItem);
	}
	else
	{
		InterlockedCompareExchange(&workItem->State,WAIT_STATE_ABANDONED,WAIT_STATE_ACTIVE);
		InterlockedCompareExchange(&workItem->State,WAIT_STATE_REMOVED,WAIT_STATE_UNREGISTERED); //mark it removed		
	}
}

static DWORD CALLBACK AsyncWaiterCall(LPVOID Arg)
{
	PASYNC_CALL waiter = (PASYNC_CALL)Arg;
	PWAIT_WORK_ITEM workItem = waiter->item;
	if (workItem->State == WAIT_STATE_ACTIVE || workItem->State == WAIT_STATE_ABANDONED)
		workItem->Callback(workItem->Context,waiter->TimerOrWaitFired);
	LONG CallbacksPending = InterlockedDecrement(&workItem->CallbacksPending);
	SetEvent(workItem->CallbackEvent);	
	if (CallbacksPending == 0 && workItem->State != WAIT_STATE_ACTIVE)
	{
		//if it's removed, all fine, destroy it here
		if (InterlockedCompareExchange(&workItem->State,WAIT_STATE_GAMEOVER,WAIT_STATE_REMOVED) == WAIT_STATE_REMOVED)
		{
			if (workItem->CompletionEvent) SetEvent(workItem->CompletionEvent);		
			DestroyWaitItem(workItem);
		}
		else //destroy it with removal
			InterlockedCompareExchange(&workItem->State,WAIT_STATE_DELETESYNC,WAIT_STATE_UNREGISTERED);
	}

	HeapFree(GetProcessHeap(),0,Arg);
	return 0;
}

static VOID TerminateWaitThread(PWAIT_THREAD threadinfo)
{
	int i;
	EnterCriticalSection(&wait_cs);
	for (i=0;i<MAX_WAIT_THREADS;i++)
	{
		if (waitthreads[i] == threadinfo)
		{
			waitthreads[i] = NULL;
			break;
		}
	}
	LeaveCriticalSection(&wait_cs);
	//if thread died of failure, mark all wait items abandoned
	for (i=0;i<threadinfo->nItems;i++)
		InterlockedCompareExchange(&threadinfo->waitItems[i]->State,WAIT_STATE_ABANDONED,WAIT_STATE_ACTIVE);
	//make sure there aren't any outstanding APCs
	SleepEx(0,TRUE);
	CloseHandle(threadinfo->hThread);
	CloseHandle(threadinfo->WaitEvent);
	HeapFree(GetProcessHeap(),0,threadinfo);
}

static DWORD WaitThreadWait(PWAIT_THREAD threadinfo)
{
	int i;
	int nCount = threadinfo->nItems * 2;
	HANDLE* handles = (HANDLE*)alloca(sizeof(HANDLE)*nCount);
	for (i=0;i<threadinfo->nItems;i++)
	{
		handles[i*2] = threadinfo->waitItems[i]->Object;
		handles[(i*2)+1] = threadinfo->waitItems[i]->TimerObject;
	}
	return WaitForMultipleObjectsEx(nCount,handles,FALSE,INFINITE,TRUE);
}

static DWORD CALLBACK WaitThreadProc(LPVOID Arg)
{
	PWAIT_THREAD threadinfo = (PWAIT_THREAD)Arg;
	//wait for first APC to come
	SleepEx(INFINITE,TRUE);
	while (TRUE)
	{
		SetEvent(threadinfo->WaitEvent);
		if (!threadinfo->nItems) //no things to do, wait 3 seconds and leave
		{
			if (SleepEx(3000,TRUE)!=WAIT_IO_COMPLETION)
				break;
			else
				continue;
		}
		DWORD status = WaitThreadWait(threadinfo);
		ResetEvent(threadinfo->WaitEvent);
		if (status == WAIT_FAILED) break; //FFFFFUUUU
		if (status >= WAIT_OBJECT_0 && status <= WAIT_OBJECT_0+MAXIMUM_WAIT_OBJECTS )
		{
			status -= WAIT_OBJECT_0;
			PWAIT_WORK_ITEM workItem = threadinfo->waitItems[status/2];
			BOOL TimerFired = (status & 1);
			if (workItem->State != WAIT_STATE_ACTIVE) continue; //don't run if removed
			if (workItem->CallbackEvent == NULL) //sync callback
			{
				SetEvent(threadinfo->WaitEvent);
				threadinfo->InSyncCallback = TRUE;
				workItem->Callback(workItem->Context,TimerFired);
				threadinfo->InSyncCallback = FALSE;
			}
			else
			{
				PASYNC_CALL waiter = (PASYNC_CALL)HeapAlloc(GetProcessHeap(),0,sizeof(*waiter));
				if (waiter)
				{
					waiter->item = workItem;
					waiter->TimerOrWaitFired = TimerFired;
					InterlockedIncrement(&workItem->CallbacksPending);
					QueueUserWorkItem_new(AsyncWaiterCall,waiter,workItem->Flags);
				}
			}
			if (workItem->Flags & WT_EXECUTEONLYONCE || workItem->Milliseconds == 0)
				RemoveWaitItem((ULONG_PTR)workItem);
			else
				ActivateTimer(workItem);
		}
	}
	TerminateWaitThread(threadinfo);
	return 0;
}

static BOOL RegisterWaitItem(PWAIT_WORK_ITEM workItem)
{
	EnterCriticalSection(&wait_cs);
	BOOL success = FALSE;
	int i;
	for (i=0;i<MAX_WAIT_THREADS;i++)
	{
		if (waitthreads[i] == NULL)
		{
			DWORD dummy;
			waitthreads[i] = (PWAIT_THREAD)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*waitthreads[i]));
			if (waitthreads[i] != NULL)
				waitthreads[i]->hThread = CreateThread(NULL, 0, WaitThreadProc, waitthreads[i], 0, &dummy);
			if (waitthreads[i]->hThread == NULL)
			{
				HeapFree(GetProcessHeap(),0,waitthreads[i]);
				waitthreads[i]=NULL;
				break;
			}
			waitthreads[i]->WaitEvent = CreateEvent(NULL,TRUE,TRUE,NULL);
		}
		if (waitthreads[i]->nItems == MAX_WAIT_ITEMS || waitthreads[i]->InSyncCallback) //full or busy, gtfo
			continue;
		else
		{
			//try adding it
			workItem->waitThread = waitthreads[i];
			workItem->CompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
			if ( QueueUserAPC(AddWaitItem,waitthreads[i]->hThread,(ULONG_PTR)workItem) )
			{
				//wait until item is added
				WaitForSingleObject(workItem->CompletionEvent,INFINITE);
				success = (workItem->waitThread != NULL);
			}
			CloseHandle(workItem->CompletionEvent);
			workItem->CompletionEvent = NULL;
			if (success) break;
		}
	}
	LeaveCriticalSection(&wait_cs);
	return success;
}

static BOOL IsValidHandle( HANDLE hObject )
{
	HANDLE newobject = NULL;
	BOOL success = DuplicateHandle(GetCurrentProcess(),hObject,GetCurrentProcess(),&newobject,0,0,DUPLICATE_SAME_ACCESS);
	if (success) CloseHandle(newobject);
	return success;
}

/* MAKE_EXPORT RegisterWaitForSingleObject_new=RegisterWaitForSingleObject */
BOOL WINAPI RegisterWaitForSingleObject_new( 
  PHANDLE phNewWaitObject, HANDLE hObject, WAITORTIMERCALLBACK Callback, 
  PVOID Context, ULONG dwMilliseconds, ULONG dwFlags)
{
	if (!phNewWaitObject || IsBadCodePtr((FARPROC)Callback) || !IsValidHandle(hObject))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	PWAIT_WORK_ITEM newWorkItem = (PWAIT_WORK_ITEM)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*newWorkItem));
	if (!newWorkItem) return FALSE;
	newWorkItem->Object = hObject;
	newWorkItem->Callback = Callback;
	newWorkItem->Context = Context;
	newWorkItem->Milliseconds = dwMilliseconds;
	newWorkItem->Flags = dwFlags;
	if (!(dwFlags & WT_EXECUTEINWAITTHREAD))
		newWorkItem->CallbackEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	newWorkItem->State = WAIT_STATE_ACTIVE;
	if (dwMilliseconds != INFINITE)
		newWorkItem->TimerObject = CreateWaitableTimer(NULL,FALSE,NULL);
	else
		newWorkItem->TimerObject = CreateEvent(NULL,FALSE,FALSE,NULL); //dummy event which never fires

	if (!RegisterWaitItem(newWorkItem))
	{
		DestroyWaitItem(newWorkItem);
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return FALSE;
	}
	*phNewWaitObject = (HANDLE)newWorkItem;
	return TRUE;
}

/* MAKE_EXPORT UnregisterWaitEx_new=UnregisterWaitEx */
BOOL WINAPI UnregisterWaitEx_new( HANDLE WaitHandle, HANDLE CompletionEvent)
{
	PWAIT_WORK_ITEM workItem = (PWAIT_WORK_ITEM)WaitHandle;
	if (!workItem)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	//9x dies if we queue an apc into dead thread so avoid it at all costs
	if ( InterlockedCompareExchange(&workItem->State,WAIT_STATE_UNREGISTERED,WAIT_STATE_ACTIVE)==WAIT_STATE_ACTIVE )
	{
		PWAIT_THREAD waitthread = (PWAIT_THREAD)workItem->waitThread;
		if (!QueueUserAPC(RemoveWaitItem,waitthread->hThread,(ULONG_PTR)workItem))
			return FALSE;
		//make sure thread is in wait state and isn't doing sth with item we delete
		WaitForSingleObject(waitthread->WaitEvent,NONSENSE);
	}
	InterlockedCompareExchange(&workItem->State,WAIT_STATE_REMOVED,WAIT_STATE_ABANDONED);
	BOOL pending = FALSE;
	if (workItem->CallbacksPending)
	{
		pending = TRUE;
		if (CompletionEvent != NULL)
		{
			if (CompletionEvent == INVALID_HANDLE_VALUE)
			{
				//wait for callbacks to finish
				while (workItem->CallbacksPending)
					WaitForSingleObject(workItem->CallbackEvent,INFINITE);
				pending = FALSE;
			}
			else
				InterlockedExchangePointer(&workItem->CompletionEvent, CompletionEvent);
		}
	}
	//all async callbacks are finished for sure?
	if (!pending)
	{
		//yes? and the item is removed from thread?
		if (InterlockedCompareExchange(&workItem->State,WAIT_STATE_GAMEOVER,WAIT_STATE_REMOVED) == WAIT_STATE_REMOVED)
			DestroyWaitItem(workItem);
		else //not removed? okay, it will be destroyed with removal
			InterlockedCompareExchange(&workItem->State,WAIT_STATE_DELETESYNC,WAIT_STATE_UNREGISTERED);
	}
	//otherwise, there are still pending callbacks and item will be destroyed as soon as all callbacks end	
	if (pending)
	{
		SetLastError(ERROR_IO_PENDING);
		return FALSE;
	}
	return TRUE;
}

/* MAKE_EXPORT RegisterWaitForSingleObjectEx_new=RegisterWaitForSingleObjectEx */
HANDLE WINAPI RegisterWaitForSingleObjectEx_new(HANDLE hObject, 
		WAITORTIMERCALLBACK Callback, PVOID Context,
		ULONG dwMilliseconds, ULONG dwFlags)
{
	HANDLE retHandle;
	if ( RegisterWaitForSingleObject_new(&retHandle,hObject,Callback,Context,dwMilliseconds,dwFlags) )
		return retHandle;
	else
		return NULL;
}

/* MAKE_EXPORT UnregisterWait_new=UnregisterWait */
BOOL WINAPI UnregisterWait_new(HANDLE WaitHandle)
{
	return UnregisterWaitEx_new(WaitHandle,NULL);
}


//	Timer Queue routines
#define MAX_TIMERS MAXIMUM_WAIT_OBJECTS

typedef struct
{
	HANDLE TimerObject;
	WAITORTIMERCALLBACK Callback;
	PVOID Parameter;
	ULONG DueTime;
	DWORD Period;
	ULONG Flags;
	PVOID TimerQueue;
	LONG State;
	LONG CallbacksPending;
	HANDLE CompletionEvent;
	HANDLE CallbackEvent;
} TIMER_ITEM, *PTIMER_ITEM;

typedef struct 
{
	HANDLE hThread;
	HANDLE WaitEvent;
	HANDLE CompletionEvent;
	LONG nItems;
	PTIMER_ITEM timers[MAX_TIMERS];	
} TIMER_QUEUE, *PTIMER_QUEUE;

static PTIMER_QUEUE defaultTimerQueue = NULL;

static VOID DestroyTimerItem(PTIMER_ITEM timer)
{
	CloseHandle(timer->CallbackEvent);
	CloseHandle(timer->TimerObject);
	HeapFree(GetProcessHeap(),0,timer);
}

static VOID CALLBACK ChangeTimerItem(PTIMER_ITEM timer)
{
	//(re)activate waitable timer
	LARGE_INTEGER timeout;
	timeout.QuadPart = timer->DueTime * (LONGLONG)-10000;
	SetWaitableTimer(timer->TimerObject,&timeout,(timer->Flags & WT_EXECUTEONLYONCE) ? 0 : timer->Period,NULL,NULL,FALSE);
}

static VOID CALLBACK AddTimerItem(ULONG_PTR Arg)
{
	PTIMER_ITEM timer = (PTIMER_ITEM)Arg;
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)timer->TimerQueue;
	if (timerqueue->nItems == MAX_TIMERS) //too much timers
	{
		timer->TimerQueue = NULL;
		SetEvent(timer->CompletionEvent);
		return;
	}
	timerqueue->timers[timerqueue->nItems] = timer;
	timerqueue->nItems++;
	SetEvent(timer->CompletionEvent);
	ChangeTimerItem(timer);
}

static VOID CALLBACK RemoveTimerItem(ULONG_PTR Arg)
{
	PTIMER_ITEM timer = (PTIMER_ITEM)Arg;
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)timer->TimerQueue;
	int i, j;
	for (i=0;i<MAX_TIMERS;i++)
		if (timerqueue->timers[i] == timer)
			break;
	if (i<MAX_TIMERS)
	{
		for (j=i;j<timerqueue->nItems-1;j++)
		{
			timerqueue->timers[j]=timerqueue->timers[j+1];
			timerqueue->timers[j+1]=NULL;
		}
		timerqueue->nItems--;
	}
	//if unregister marked it for delete, kill it here
	if (InterlockedCompareExchange(&timer->State,WAIT_STATE_GAMEOVER,WAIT_STATE_DELETESYNC) == WAIT_STATE_DELETESYNC)
	{
		if (timer->CompletionEvent) SetEvent(timer->CompletionEvent);		
		DestroyTimerItem(timer);
	}
	else
		InterlockedCompareExchange(&timer->State,WAIT_STATE_REMOVED,WAIT_STATE_UNREGISTERED); //mark it removed		
}

static DWORD CALLBACK AsyncTimerCall(LPVOID Arg)
{
	PTIMER_ITEM timer = PTIMER_ITEM(Arg);
	if (timer->State == WAIT_STATE_ACTIVE)
		timer->Callback(timer->Parameter,TRUE);
	LONG CallbacksPending = InterlockedDecrement(&timer->CallbacksPending);
	SetEvent(timer->CallbackEvent);	
	if (CallbacksPending == 0 && timer->State != WAIT_STATE_ACTIVE)
	{
		//if it's removed, all fine, destroy it here
		if (InterlockedCompareExchange(&timer->State,WAIT_STATE_GAMEOVER,WAIT_STATE_REMOVED) == WAIT_STATE_REMOVED)
		{
			if (timer->CompletionEvent) SetEvent(timer->CompletionEvent);		
			DestroyTimerItem(timer);
		}
		else //destroy it with removal
			InterlockedCompareExchange(&timer->State,WAIT_STATE_DELETESYNC,WAIT_STATE_UNREGISTERED);
	}

	return 0;
}

/* MAKE_EXPORT DeleteTimerQueueTimer_new=DeleteTimerQueueTimer */
BOOL WINAPI DeleteTimerQueueTimer_new( HANDLE TimerQueue, HANDLE Timer, HANDLE CompletionEvent)
{
	//absolutely equal to UnregisterWaitEx
	if (!Timer)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	PTIMER_ITEM timer = (PTIMER_ITEM)Timer;
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)timer->TimerQueue;
	if ( InterlockedCompareExchange(&timer->State,WAIT_STATE_UNREGISTERED,WAIT_STATE_ACTIVE)==WAIT_STATE_ACTIVE )
	{
		if (!QueueUserAPC(RemoveTimerItem,timerqueue->hThread,(ULONG_PTR)timer))
			return FALSE;
		//make sure thread is in wait state and isn't doing sth with item we delete
		WaitForSingleObject(timerqueue->WaitEvent,NONSENSE);
	}
	BOOL pending = FALSE;
	if (timer->CallbacksPending)
	{
		pending = TRUE;
		if (CompletionEvent != NULL)
		{
			if (CompletionEvent == INVALID_HANDLE_VALUE)
			{
				while (timer->CallbacksPending)
					WaitForSingleObject(timer->CallbackEvent,INFINITE);
				pending = FALSE;
			}
			else
				InterlockedExchangePointer(&timer->CompletionEvent, CompletionEvent);
		}
	}
	if (!pending)
	{
		if (InterlockedCompareExchange(&timer->State,WAIT_STATE_GAMEOVER,WAIT_STATE_REMOVED) == WAIT_STATE_REMOVED)
			DestroyTimerItem(timer);
		else
			InterlockedCompareExchange(&timer->State,WAIT_STATE_DELETESYNC,WAIT_STATE_UNREGISTERED);
	}
	if (pending)
	{
		SetLastError(ERROR_IO_PENDING);
		return FALSE;
	}
	return TRUE;
}

static VOID CALLBACK TerminateTimerQueue(ULONG_PTR Arg)
{
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)Arg;
	CloseHandle(timerqueue->hThread);
	CloseHandle(timerqueue->WaitEvent);
	HeapFree(GetProcessHeap(),0,timerqueue);
	ExitThread(0);
}

static VOID CALLBACK FinishTimerQueue(ULONG_PTR Arg)
{
	int i;
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)Arg;
	EnterCriticalSection(&wait_cs);
	if (defaultTimerQueue == timerqueue)
		defaultTimerQueue = NULL;
	LeaveCriticalSection(&wait_cs);
	//delete all timers, waiting
	for (i=timerqueue->nItems-1;i>=0;i--)
		DeleteTimerQueueTimer_new( timerqueue, timerqueue->timers[i], INVALID_HANDLE_VALUE );
	if (timerqueue->CompletionEvent)
		SetEvent(timerqueue->CompletionEvent);
	//queue object termination
	QueueUserAPC(TerminateTimerQueue,GetCurrentThread(),Arg);
}

static DWORD TimerQueueWait(PTIMER_QUEUE timerqueue)
{
	int i;
	HANDLE* handles = (HANDLE*)alloca(sizeof(HANDLE)*timerqueue->nItems);
	for (i=0;i<timerqueue->nItems;i++)
		handles[i] = timerqueue->timers[i]->TimerObject;

	return WaitForMultipleObjectsEx(timerqueue->nItems,handles,FALSE,INFINITE,TRUE);
}

static DWORD CALLBACK TimerQueueProc(LPVOID Arg)
{
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)Arg;
	while (TRUE)
	{
		SetEvent(timerqueue->WaitEvent);
		if (!timerqueue->nItems) //wait for items
		{
			SleepEx(INFINITE,TRUE);
			continue;
		}
		DWORD status = TimerQueueWait(timerqueue);
		ResetEvent(timerqueue->WaitEvent);
		if (status == WAIT_FAILED) break; //uberfail
		if (status >= WAIT_OBJECT_0 && status <= WAIT_OBJECT_0+MAXIMUM_WAIT_OBJECTS )
		{
			PTIMER_ITEM timer = timerqueue->timers[status-WAIT_OBJECT_0];
			if (timer->State != WAIT_STATE_ACTIVE) continue;
			if (timer->CallbackEvent == NULL) //sync callback
			{
				SetEvent(timerqueue->WaitEvent);
				timer->Callback(timer->Parameter,TRUE);
			}
			else
			{
				InterlockedIncrement(&timer->CallbacksPending);
				QueueUserWorkItem_new(AsyncTimerCall,timer,timer->Flags);
			}
		}
	}
	FinishTimerQueue((ULONG_PTR)timerqueue);
	SleepEx(0,TRUE);
	return 0;
}

/* MAKE_EXPORT CreateTimerQueue_new=CreateTimerQueue */
HANDLE WINAPI CreateTimerQueue_new(VOID)
{
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*timerqueue));
	if (timerqueue != NULL)
	{
		DWORD dummy;
		timerqueue->WaitEvent = CreateEvent(NULL,TRUE,TRUE,NULL);
		timerqueue->hThread = CreateThread(NULL,0,TimerQueueProc,timerqueue,0,&dummy);
		if (!timerqueue->hThread)
		{
			HeapFree(GetProcessHeap(),HEAP_ZERO_MEMORY,timerqueue);
			timerqueue = NULL;
		}
	}
	return (HANDLE)timerqueue;
}

/* MAKE_EXPORT CreateTimerQueueTimer_new=CreateTimerQueueTimer */
BOOL WINAPI CreateTimerQueueTimer_new( PHANDLE phNewTimer, HANDLE TimerQueue, WAITORTIMERCALLBACK Callback,
								  PVOID Parameter, DWORD DueTime, DWORD Period, ULONG Flags)
{
	if (!phNewTimer || IsBadCodePtr((FARPROC)Callback))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	*phNewTimer = NULL;
	if (TimerQueue == NULL)	
	{
		EnterCriticalSection(&wait_cs);
		if (defaultTimerQueue == NULL)
			defaultTimerQueue = (PTIMER_QUEUE)CreateTimerQueue_new();
		TimerQueue = defaultTimerQueue;
		LeaveCriticalSection(&wait_cs);
		if (TimerQueue == NULL)	return FALSE;
	}
	PTIMER_ITEM timer = (PTIMER_ITEM)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*timer));
	timer->TimerQueue = TimerQueue;
	timer->Callback = Callback;
	timer->Parameter = Parameter;
	timer->DueTime = DueTime;
	timer->Period = Period;
	timer->Flags = Flags;
	timer->State = WAIT_STATE_ACTIVE;
	timer->TimerObject = CreateWaitableTimer(NULL,FALSE,NULL);
	if (!(Flags & WT_EXECUTEINTIMERTHREAD))
		timer->CallbackEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	timer->CompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);	
	BOOL success = (BOOL)QueueUserAPC(AddTimerItem,((PTIMER_QUEUE)TimerQueue)->hThread,(ULONG_PTR)timer);
	if (success)
	{
		WaitForSingleObject(timer->CompletionEvent,INFINITE);
		CloseHandle(timer->CompletionEvent);
		timer->CompletionEvent = NULL;
		success = (timer->TimerObject != NULL);
		if (success) *phNewTimer = timer;
	}
	return success;
}

/* MAKE_EXPORT DeleteTimerQueueEx_new=DeleteTimerQueueEx */
BOOL WINAPI DeleteTimerQueueEx_new( HANDLE TimerQueue, HANDLE CompletionEvent)
{
	PTIMER_QUEUE timerqueue = (PTIMER_QUEUE)TimerQueue;
	if (!timerqueue)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	if (CompletionEvent != NULL)
	{
		if (CompletionEvent == INVALID_HANDLE_VALUE)
			timerqueue->CompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		else
			timerqueue->CompletionEvent = CompletionEvent;
	}
	QueueUserAPC(FinishTimerQueue,timerqueue->hThread,(ULONG_PTR)timerqueue);
	if (CompletionEvent == INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(timerqueue->CompletionEvent,INFINITE);
		CloseHandle(timerqueue->CompletionEvent);
	}
	return TRUE;
}

/* MAKE_EXPORT DeleteTimerQueue_new=DeleteTimerQueue */
BOOL WINAPI DeleteTimerQueue_new( HANDLE TimerQueue)
{
	return DeleteTimerQueueEx_new(TimerQueue,NULL);
}

/* MAKE_EXPORT ChangeTimerQueueTimer_new=ChangeTimerQueueTimer */
BOOL WINAPI ChangeTimerQueueTimer_new( HANDLE TimerQueue, HANDLE Timer, ULONG DueTime, ULONG Period)
{
	PTIMER_ITEM timer = (PTIMER_ITEM)Timer;
	if (!timer || timer->State != WAIT_STATE_ACTIVE)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	timer->DueTime = DueTime;
	timer->Period = Period;	
	return (BOOL)QueueUserAPC((PAPCFUNC)ChangeTimerItem,((PTIMER_QUEUE)timer->TimerQueue)->hThread,(ULONG_PTR)timer);
}

//some undocumented APIs too...

/* MAKE_EXPORT CancelTimerQueueTimer_new=CancelTimerQueueTimer */
BOOL WINAPI CancelTimerQueueTimer_new( HANDLE TimerQueue, HANDLE Timer)
{
	PTIMER_ITEM timer = (PTIMER_ITEM)Timer;
	if (!timer || timer->State != WAIT_STATE_ACTIVE)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	return CancelWaitableTimer(timer->TimerObject);
}

/* MAKE_EXPORT SetTimerQueueTimer_new=SetTimerQueueTimer */
HANDLE WINAPI SetTimerQueueTimer_new( HANDLE TimerQueue, WAITORTIMERCALLBACK Callback, PVOID Parameter, DWORD DueTime, DWORD Period, BOOL PreferIo )
{
	HANDLE newHandle = NULL;
	CreateTimerQueueTimer_new(&newHandle,TimerQueue,Callback,Parameter,DueTime,Period,PreferIo ? WT_EXECUTEINIOTHREAD : WT_EXECUTEDEFAULT);
	return newHandle;
}
