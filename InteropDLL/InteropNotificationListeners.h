#pragma once
#include "stdafx.h"
#include "../Core/INotificationListener.h"
#include "../Core/NotificationManager.h"
#include "../Core/Console.h"
#include "../Utilities/SimpleLock.h"
#include "InteropNotificationListener.h"

typedef void (__stdcall *NotificationListenerCallback)(ConsoleNotificationType, void*);

class InteropNotificationListeners
{
	SimpleLock _externalNotificationListenerLock;
	vector<shared_ptr<INotificationListener>> _externalNotificationListeners;

public:
	INotificationListener* RegisterNotificationCallback(NotificationListenerCallback callback,
	                                                    shared_ptr<Console> console)
	{
		auto lock = _externalNotificationListenerLock.AcquireSafe();
		auto listener = shared_ptr<INotificationListener>(new InteropNotificationListener(callback));
		_externalNotificationListeners.push_back(listener);
		console->GetNotificationManager()->RegisterNotificationListener(listener);
		return listener.get();
	}

	void UnregisterNotificationCallback(INotificationListener* listener)
	{
		auto lock = _externalNotificationListenerLock.AcquireSafe();
		_externalNotificationListeners.erase(
			std::remove_if(
				_externalNotificationListeners.begin(),
				_externalNotificationListeners.end(),
				[=](shared_ptr<INotificationListener> ptr) { return ptr.get() == listener; }
			),
			_externalNotificationListeners.end()
		);
	}
};
