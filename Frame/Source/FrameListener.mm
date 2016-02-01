/*+
 *
 *  FrameListener.mm
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Define a listener for frame operations. Forward events to the document to 
 *	update the UI.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 -*/

#include "FrameListener.h"
#include "FrameDocument.h"
#include "StrMessage.h"

void
FrameListener::ListenToMessage(DlUInt32 msg, void* data) const
{
	if (msg == MessageListAddOne || msg == MessageListRemoveOne ||
			msg == MessageListAddMultiple || msg == MessageListRemoveMultiple) {
		ListMessage* m = (ListMessage*)data;
		switch (m->id) {
		case NodeListID:
			switch(msg) {
			case MessageListAddOne:
			case MessageListRemoveOne:
				[_frameDocument nodeChanged: (Node*)m->element removed:msg == MessageListRemoveOne];
				break;
			default:
				{
					NodeEnumerator e((NodeList*)m->element);
					[_frameDocument nodesChanged: e removed:msg == MessageListRemoveOne];
				}
				break;
			}
			
			// adding/removing can affect load counts.
			//[_frameDocument updateLoadCounts];

			break;
				
		case ElementListID:
			switch(msg) {
			case MessageListAddOne:
			case MessageListRemoveOne:
				[_frameDocument elemChanged: (Element*)m->element removed:msg == MessageListRemoveOne];
				break;
			default:
				{
					ElementEnumerator e((ElementList*)m->element);
					[_frameDocument elemsChanged: e removed:msg == MessageListRemoveMultiple];
				}
				break;
			}
				
			// adding/removing can affect load counts.
			//[_frameDocument updateLoadCounts];

			break;
				
		case NodeLoadListID:
			switch(msg) {
			case MessageListAddOne:
			case MessageListRemoveOne:
				[_frameDocument nodeLoadChanged: (NodeLoad*)m->element removed:msg == MessageListRemoveOne];
				break;
			default:
				{
					NodeLoadEnumerator e((NodeLoadList*)m->element);
					[_frameDocument nodeLoadsChanged: e removed:msg == MessageListRemoveMultiple];
				}
				break;
			}
			break;
				
		case ElementLoadListID:
			switch(msg) {
			case MessageListAddOne:
			case MessageListRemoveOne:
				[_frameDocument elemLoadChanged: (ElementLoad*)m->element removed:msg == MessageListRemoveOne];
				break;
			default:
				{
					ElementLoadEnumerator e((ElemLoadList*)m->element);
					[_frameDocument elemLoadsChanged: e removed:msg == MessageListRemoveMultiple];
				}
				break;
			}
			break;
				
		case PropertyListID:
			switch(msg) {
			case MessageListAddOne:
			case MessageListRemoveOne:
				[_frameDocument propChanged: (Property*)m->element removed:msg == MessageListRemoveOne];
				break;
			default:
				{
					PropertyEnumerator e((PropertyList*)m->element);
					[_frameDocument propsChanged: e removed:msg == MessageListRemoveMultiple];
				}
				break;
			}
			break;
				
		default:
			break;
		}
	}
}
