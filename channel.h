#define IEOL "\r\n"

struct _chanmode
{
	long mode[2];
};
typedef struct _chanmode IrcChannelMode;


struct _channame
{
	char	*name;
};
typedef struct _channame IRC(ChannelName);

struct _chanhandle
{
	IRC(ChannelName)	*channelName;
	time_t			created;
	time_t			lastTime;
	IrcChannelMode		mode;
	LIST_HEAD(,_chanuser)	*users;
};
typedef struct _chanhandle IrcChannel;
