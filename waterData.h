#define MESS_SZe 30
struct waterData
	{
	int MessageNumbers;
	int tankLevelHight;
	int tankValve;
	int tankTemp;
	int waterCollectionTemp;
	int solarPanelTilt;
	int pumpSpeed;
	int airTemp;

	};

#define MEMORY_WATER_SIZE sizeof(struct waterData)

