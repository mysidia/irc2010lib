struct _chanmode
{
	long mode[2];
};
typedef struct _chanmode IrcChannelMode;

struct _chanhandle
{
	char		*channelName;
	time_t		created;
	time_t		lastTime;
	IrcChannelMode	mode;
	LIST_HEAD(,_chanuser)	*users;
};
typedef struct _chanhandle IrcChannel;
