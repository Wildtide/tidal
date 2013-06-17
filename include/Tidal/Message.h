#pragma once

namespace Tidal
{

	enum MessageId
	{
		msg_FrameBegin	= 0x1000,
		msg_FrameEnd	= 0x1001,
		msg_SysLast		= 0x7fff
	};

	class Message
	{
	public:

		Message(MessageId id) : msgId(id)
		{}

		MessageId msgId;
	};

}
