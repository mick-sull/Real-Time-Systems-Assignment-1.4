#define MESS_SZ 30
struct MemData;

struct MemData
	{
	int MessageNumber;
	int PID[5];
	};

#define MEMORY_SIZE sizeof(struct MemData)

