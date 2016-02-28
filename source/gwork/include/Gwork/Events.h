/*
 *  Gwork
 *  Copyright (c) 2010 Facepunch Studios
 *  Copyright (c) 2013-16 Billy Quith
 *  See license in Gwork.h
 */

#pragma once
#ifndef GWK_EVENTS_H
#define GWK_EVENTS_H

#include <list>
#include "Gwork/Exports.h"
#include "Gwork/Structures.h"
#include "Gwork/ControlList.h"

namespace Gwk
{
    namespace Controls
    {
        class Base;
    }

    namespace Event
    {
        class Caller;

        /**
         *
         *  When adding an event hook you can add a Packet too.
         *  This will be passed in Event::Info when you receive an event.
         *
         */
        struct Packet
        {
            Packet(Gwk::Controls::Base* pControl = NULL)
                :   Control(pControl)
            {
            }

            Gwk::Controls::Base   *Control;
            Gwk::String String;
            int Integer;
            float Float;
            unsigned long long UnsignedLongLong;
        };


        /**
         *
         *  Passed to an event hook
         *
         */
        struct Information
        {
            Information()
            {
                Init();
            }

            Information(Gwk::Controls::Base* pctrl)
            {
                Init();
                Control = pctrl;
            }

            void Init()
            {
                ControlCaller   = NULL;
                Packet          = NULL;
                Control         = NULL;
                Integer         = 0;
            }

            /// This is set by the event caller, it will always be
            /// the control which is calling the event.
            Gwk::Controls::Base* ControlCaller;

            /// This is set by the event hook, e.g. :-
            /// ~~~
            /// onDoSomething.Add( this, &ThisClass::MyFunction, Gwk::Event::Packet("Something") );
            /// ~~~
            Gwk::Event::Packet* Packet;

            //
            /// These are set by the event and may or may not be set.
            //
            Gwk::Controls::Base* Control;

            Gwk::ControlList ControlList;
            Gwk::String String;
            Gwk::Point Point;
            int Integer;

        };


        typedef const Gwk::Event::Information& Info;

        //
        /// A class wanting to receive events must be derived from this.
        //
        class GWK_EXPORT Handler
        {
        public:

            virtual ~Handler();

            void RegisterCaller(Caller*);
            void UnRegisterCaller(Caller*);

        protected:

            Handler();  // Class needs to be derived.

            void CleanLinks();
            std::list<Caller*> m_Callers;

        public:

            typedef void (Handler::*Function)                   (Gwk::Controls::Base* pFromPanel);
            typedef void (Handler::*FunctionBlank)              ();
            typedef void (Handler::*FunctionWithInformation)    (Gwk::Event::Info info);

        };


        /// Event callback management.
        //
        class GWK_EXPORT Caller
        {
        public:

            Caller();
            ~Caller();

            void Call(Controls::Base* pThis);
            void Call(Controls::Base* pThis, Gwk::Event::Info info);

            template <typename T>
            void Add(Event::Handler* ob, T f)
            {
                AddInternal(ob, static_cast<Handler::Function>(f));
            }

            template <typename T>
            void Add(Event::Handler* ob, void (T::*f)(Gwk::Event::Info))
            {
                AddInternal(ob, static_cast<Handler::FunctionWithInformation>(f));
            }

            template <typename T>
            void Add(Event::Handler* ob,
                     void (T::*f)(Gwk::Event::Info),
                     const Gwk::Event::Packet& packet)
            {
                AddInternal(ob, static_cast<Handler::FunctionWithInformation>(f), packet);
            }

            template <typename T>
            void Add(Event::Handler* ob, void (T::*f)())
            {
                AddInternal(ob, static_cast<Handler::FunctionBlank>(f));
            }

            void RemoveHandler(Event::Handler* pObject);

        protected:

            void CleanLinks();
            void AddInternal(Event::Handler* pObject, Handler::Function pFunction);
            void AddInternal(Event::Handler* pObject, Handler::FunctionWithInformation pFunction);
            void AddInternal(Event::Handler* pObject, Handler::FunctionWithInformation pFunction,
                             const Gwk::Event::Packet& packet);
            void AddInternal(Event::Handler* pObject, Handler::FunctionBlank pFunction);

            struct HandlerInstance
            {
                HandlerInstance()
                    :   fnFunction(NULL)
                    ,   fnFunctionInfo(NULL)
                    ,   fnFunctionBlank(NULL)
                    ,   pObject(NULL)
                {
                }

                Handler::Function fnFunction;
                Handler::FunctionWithInformation fnFunctionInfo;
                Handler::FunctionBlank fnFunctionBlank;

                Event::Handler*         pObject;
                Gwk::Event::Packet Packet;
            };


            std::list<HandlerInstance> m_Handlers;
        };


    }

}

#endif // ifndef GWK_EVENTS_H
