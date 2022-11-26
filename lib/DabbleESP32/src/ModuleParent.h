

#ifndef ModuleParent_h
#define ModuleParent_h


/* Output functions ID. */
#define QUERY_SELECTED		0xFF
#define SELECT_MODULE		0xFE	
#define DESELECT_MODULE		0xFD


/* Input functions ID. */
#define CHECK_SELECTED		0xFF

class DabbleClass;

class ModuleParent
{
protected:
	ModuleParent(byte);
	virtual void processFrame(void);
	virtual void processData(void){};
	void enteringACallback();
	void exitingACallback();
	bool isInACallback();
	DabbleClass & getDabbleInstance();
public:
	/* Functions will be inherited by all Modules. */
	void select(void);
	void deselect(void);
	void setOnSelected(void(*)(void));
	byte getModuleId(void);
private:
	bool isCallBackSet;
	byte ModuleID;
	void (*selectedCallBack)(void);
	static DabbleClass * DabbleInstance;
	static bool DabbleInstanceAvailable;
	static void setDabbleInstance(DabbleClass &);
	static void unSetDabbleInstance();

friend class DabbleClass;
};

#endif