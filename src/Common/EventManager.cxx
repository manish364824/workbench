/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <QThread>

#include <algorithm>
#include <iostream>
#include <typeinfo>

#define __EVENT_MANAGER_MAIN__
#include "Event.h"
#include "EventManager.h"
#undef __EVENT_MANAGER_MAIN__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventListenerInterface.h"

using namespace caret;
/**
 * \class  caret::EventManager
 * \brief  The event manager.
 *
 * The event manager processes events
 * from senders to receivers.
 *
 * Events are sent by calling this class' sendEvent()
 * method.
 *
 * Objects that wish to receive events must (1) extend
 * publicly EventListenerInterface, (2) implement
 * EventListenerInterface's receiveEvent() method,
 * (3) Call one of two methods in EventManger,
 * addEventListener() or addProcessedEventListener() which
 * are typically called from the object's constructor, and
 * (4) call removeEventFromListener() or removeAllEventsFromListener
 * to cease listening for events which is typciall called
 * from the object's constructor.
 *
 * In most cases addEventListener() is used to request events.
 * addProcessedEventListener() is used when an object wants
 * to be notified of an event but not until after it has been
 * processed by at least one other receiver.  For example,
 * a event for a new window may be sent.  A receiver of the
 * event will create the new window.  Other receivers may
 * want to know AFTER the window has been created in which
 * case these receivers will use addProcessedEventListener().
 */

/**
 * Constructor.
 */
EventManager::EventManager()
{
    m_eventIssuedCounter = 0;
    m_eventBlockingCounter.resize(EventTypeEnum::EVENT_COUNT, 0);
}

/**
 * Destructor.
 */
EventManager::~EventManager()
{
    /*
     * Verify that all listeners were removed.
     */ 
    for (int32_t i = 0; i < EventTypeEnum::EVENT_COUNT; i++) {
        EVENT_LISTENER_CONTAINER el = m_eventListeners[i];
        if (el.empty() == false) {
            EventTypeEnum::Enum enumValue = static_cast<EventTypeEnum::Enum>(i);
            std::cout 
            << "Not all listeners removed for event "
            << EventTypeEnum::toName(enumValue)
            << ", count is: "
            << el.size()
            << std::endl;
        }
    }
    
    /*
     * Verify that all processed listeners were removed.
     */ 
    for (int32_t i = 0; i < EventTypeEnum::EVENT_COUNT; i++) {
        EVENT_LISTENER_CONTAINER el = m_eventProcessedListeners[i];
        if (el.empty() == false) {
            EventTypeEnum::Enum enumValue = static_cast<EventTypeEnum::Enum>(i);
            std::cout 
            << "Not all listeners removed for processed event "
            << EventTypeEnum::toName(enumValue)
            << ", count is: "
            << el.size()
            << std::endl;
        }
    }
}

/**
 * Create the event manager.
 */
void 
EventManager::createEventManager()
{
    CaretAssertMessage((EventManager::s_singletonEventManager == NULL),
                       "Event manager has already been created.");
    
    EventManager::s_singletonEventManager = new EventManager();
}

/**
 * Delete the event manager.
 * This may only be called one time after event manager is created.
 */
void 
EventManager::deleteEventManager()
{
    CaretAssertMessage((EventManager::s_singletonEventManager != NULL), 
                       "Event manager does not exist, cannot delete it.");
    
    delete EventManager::s_singletonEventManager;
    EventManager::s_singletonEventManager = NULL;
}

/**
 * Get the one and only event mangers.
 *
 * @return  Pointer to the event manager.
 */
EventManager* 
EventManager::get()
{
    CaretAssertMessage(EventManager::s_singletonEventManager,
                       "Event manager was not created.\n"
                       "It must be created with EventManager::createEventManager().");
    
    return EventManager::s_singletonEventManager;
}

/**
 * Add a listener for a specific event.
 *
 * @param eventListener
 *     Listener for an event.
 * @param listenForEventType
 *     Type of event that is wanted.
 */
void 
EventManager::addEventListener(EventListenerInterface* eventListener,
                               const EventTypeEnum::Enum listenForEventType)
{
#ifdef CONTAINER_VECTOR
    m_eventListeners[listenForEventType].push_back(eventListener);
#elif CONTAINER_HASH_SET
    m_eventListeners[listenForEventType].insert(eventListener);
#elif CONTAINER_SET
    m_eventListeners[listenForEventType].insert(eventListener);
#else
    INTENTIONAL_COMPILER_ERROR_MISSING_CONTAINER_TYPE
#endif
    
    //std::cout << "Adding listener from class "
    //<< typeid(*eventListener).name()
    //<< " for "
    //<< EventTypeEnum::toName(listenForEventType)
    //<< std::endl;
}

/**
 * Add a listener for a specific event but only receive the
 * event AFTER it has been processed.
 *
 * @param eventListener
 *     Listener for an event.
 * @param listenForEventType
 *     Type of event that is wanted.
 */
void 
EventManager::addProcessedEventListener(EventListenerInterface* eventListener,
                               const EventTypeEnum::Enum listenForEventType)
{
#ifdef CONTAINER_VECTOR
    m_eventProcessedListeners[listenForEventType].push_back(eventListener);
#elif CONTAINER_HASH_SET
    m_eventProcessedListeners[listenForEventType].insert(eventListener);
#elif CONTAINER_SET
    m_eventProcessedListeners[listenForEventType].insert(eventListener);
#else
    INTENTIONAL_COMPILER_ERROR_MISSING_CONTAINER_TYPE
#endif
    
    //std::cout << "Adding listener from class "
    //<< typeid(*eventListener).name()
    //<< " for "
    //<< EventTypeEnum::toName(listenForEventType)
    //<< std::endl;
}

/**
 * Stop listening for an event.
 *
 * @param eventListener
 *     Listener for an event.
 * @param listenForEventType
 *     Type of event that is no longer wanted.
 */
void 
EventManager::removeEventFromListener(EventListenerInterface* eventListener,
                                  const EventTypeEnum::Enum listenForEventType)
{
#ifdef CONTAINER_VECTOR
    /*
     * Remove from NORMAL listeners
     */
    EVENT_LISTENER_CONTAINER& listeners = m_eventListeners[listenForEventType];
    EVENT_LISTENER_CONTAINER_ITERATOR eventIter = std::find(listeners.begin(),
                                                            listeners.end(),
                                                            eventListener);
    if (eventIter != listeners.end()) {
        listeners.erase(eventIter);
    }

    /*
     * Remove from PROCESSED listeners
     * These are issued AFTER all of the NORMAL listeners have been notified
     */
    EVENT_LISTENER_CONTAINER& processedListeners = m_eventProcessedListeners[listenForEventType];
    EVENT_LISTENER_CONTAINER_ITERATOR processedEventIter = std::find(processedListeners.begin(),
                                                                     processedListeners.end(),
                                                                     eventListener);
    if (processedEventIter != processedListeners.end()) {
        processedListeners.erase(processedEventIter);
    }
    
//    EVENT_LISTENER_CONTAINER listeners = m_eventListeners[listenForEventType];
//    
//    /*
//     * Remove the listener by creating a new container
//     * of non-matching listeners.
//     */
//    EVENT_LISTENER_CONTAINER updatedListeners;
//    for (EVENT_LISTENER_CONTAINER_ITERATOR iter = listeners.begin();
//         iter != listeners.end();
//         iter++) {
//        if (*iter == eventListener) {
//            //std::cout << "Removing listener from class "
//            //<< typeid(*eventListener).name()
//            //<< " for "
//            //<< EventTypeEnum::toName(listenForEventType)
//            //<< std::endl;
//        }
//        else {
//            updatedListeners.push_back(*iter);
//        }
//    }
//    
//    if (updatedListeners.size() != listeners.size()) {
//        m_eventListeners[listenForEventType] = updatedListeners;
//    }
//    
//    
//    EVENT_LISTENER_CONTAINER processedListeners = m_eventProcessedListeners[listenForEventType];
//    
//    /*
//     * Remove the listener by creating a new container
//     * of non-matching listeners.
//     */
//    EVENT_LISTENER_CONTAINER updatedProcessedListeners;
//    for (EVENT_LISTENER_CONTAINER_ITERATOR iter = processedListeners.begin();
//         iter != processedListeners.end();
//         iter++) {
//        if (*iter == eventListener) {
//            //std::cout << "Removing listener from class "
//            //<< typeid(*eventListener).name()
//            //<< " for "
//            //<< EventTypeEnum::toName(listenForEventType)
//            //<< std::endl;
//        }
//        else {
//            updatedProcessedListeners.push_back(*iter);
//        }
//    }
//    
//    if (updatedProcessedListeners.size() != processedListeners.size()) {
//        m_eventProcessedListeners[listenForEventType] = updatedProcessedListeners;
//    }
#elif CONTAINER_HASH_SET
    m_eventListeners[listenForEventType].erase(eventListener);
    m_eventProcessedListeners[listenForEventType].erase(eventListener);
#elif CONTAINER_SET
    m_eventListeners[listenForEventType].erase(eventListener);
    m_eventProcessedListeners[listenForEventType].erase(eventListener);
#else
    INTENTIONAL_COMPILER_ERROR_MISSING_CONTAINER_TYPE
#endif
}

/**
 * Stop listening for all events.
 * @param eventListener
 *     Listener for all events.
 */ 
void 
EventManager::removeAllEventsFromListener(EventListenerInterface* eventListener)
{
    for (int32_t i = 0; i < EventTypeEnum::EVENT_COUNT; i++) {
        removeEventFromListener(eventListener, static_cast<EventTypeEnum::Enum>(i));
    }
}

/**
 * Send an event.
 * 
 * @param event
 *    Event that is sent.
 */
void 
EventManager::sendEvent(Event* event)
{   
    EventTypeEnum::Enum eventType = event->getEventType();
    const AString eventNumberString = AString::number(m_eventIssuedCounter);
    const AString eventMessagePrefix = ("Event "
                                        + eventNumberString
                                        + ": "
                                        + event->toString() 
                                        + " from thread: " 
                                        + AString::number((uint64_t)QThread::currentThread())
                                        + " ");
    
    const int32_t eventTypeIndex = static_cast<int32_t>(eventType);
    CaretAssertVectorIndex(m_eventBlockingCounter, eventTypeIndex);
    if (m_eventBlockingCounter[eventTypeIndex] > 0) {
        AString msg = (eventMessagePrefix
                       + " is blocked.  Blocking counter="
                       + AString::number(m_eventBlockingCounter[eventTypeIndex]));
        CaretLogFiner(msg);
    }
    else {
        /*
         * Get listeners for event.
         */
        EVENT_LISTENER_CONTAINER listeners = m_eventListeners[eventType];
        
        const AString eventNumberString = AString::number(m_eventIssuedCounter);
        
        // Too many prints (JWH)
        //AString msg = (eventMessagePrefix + " SENT.");
        //CaretLogFiner(msg);
        //std::cout << msg << std::endl;
        
        /*
         * Send event to each of the listeners.
         */
        for (EVENT_LISTENER_CONTAINER_ITERATOR iter = listeners.begin();
             iter != listeners.end();
             iter++) {
            EventListenerInterface* listener = *iter;
            
            //std::cout << "Sending event from class "
            //<< typeid(*listener).name()
            //<< " for "
            //<< EventTypeEnum::toName(eventType)
            //<< std::endl;
            
            
            listener->receiveEvent(event);
            
            if (event->isError()) {
                CaretLogWarning("Event " + eventNumberString + " had error: " + event->toString() + ": " + event->getErrorMessage());
                break;
            }
        }
        
        /*
         * Verify event was processed.
         */
        if (event->getEventProcessCount() > 0) {
            /*
             * Send event to each of the PROCESSED listeners.
             */
            EVENT_LISTENER_CONTAINER processedListeners = m_eventProcessedListeners[eventType];
            for (EVENT_LISTENER_CONTAINER_ITERATOR iter = processedListeners.begin();
                 iter != processedListeners.end();
                 iter++) {
                EventListenerInterface* listener = *iter;
                
                //std::cout << "Sending event from class "
                //<< typeid(*listener).name()
                //<< " for "
                //<< EventTypeEnum::toName(eventType)
                //<< std::endl;
                
                
                listener->receiveEvent(event);
                
                if (event->isError()) {
                    CaretLogWarning("Event " + eventNumberString + " had error: " + event->toString());
                    break;
                }
            }
        }
        else {
            // Too many prints (JWH) CaretLogFine("Event " + eventNumberString + " not processed: " + event->toString());
        }
    }    
    
    m_eventIssuedCounter++;
}

/**
 * Block an event.  A counter is used to track blocking of each
 * event type.  Each time a request is made to block an event type,
 * the counter is incremented for that event type.  When a request
 * is made to un-block the event, the counter is decremented.  This
 * allows multiple requests for blocking an event to come from 
 * different sections of the source code.  Thus, anytime the
 * blocking counter is greater than zero for an event, the event
 * is blocked.
 * 
 * @param eventType
 *    Type of event to block.
 * @param blockStatus
 *    Blocking status (true increments blocking counter,
 *    false decrements blocking counter.
 */
void 
EventManager::blockEvent(const EventTypeEnum::Enum eventType,
                         const bool blockStatus)
{
    const int32_t eventTypeIndex = static_cast<int32_t>(eventType);
    CaretAssertVectorIndex(m_eventBlockingCounter, eventTypeIndex);
    
    const AString eventName = EventTypeEnum::toName(eventType);
    
    if (blockStatus) {
        m_eventBlockingCounter[eventTypeIndex]++;
        CaretLogFiner("Blocking event "
                      + eventName
                      + " blocking counter is now "
                      + AString::number(m_eventBlockingCounter[eventTypeIndex]));
    }
    else {
        if (m_eventBlockingCounter[eventTypeIndex] > 0) {
            m_eventBlockingCounter[eventTypeIndex]--;
            CaretLogFiner("Unblocking event "
                          + eventName
                          + " blocking counter is now "
                          + AString::number(m_eventBlockingCounter[eventTypeIndex]));
        }
        else {
            const AString message("Trying to unblock event "
                                  + eventName
                                  + " but it is not blocked");
            CaretAssertMessage(0, message);
            CaretLogWarning(message);
        }
    }
}

