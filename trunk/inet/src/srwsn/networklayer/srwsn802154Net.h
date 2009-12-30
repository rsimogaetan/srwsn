#include "Ieee802154AppPkt_m.h"
#include "Ieee802154NetworkCtrlInfo_m.h"

#ifndef IEEE_SRWSN_802154_Net
#define IEEE_SRWSN_802154_Net

class  srwsn802154Net : public cSimpleModule
{
public:
	virtual void initialize(int);
	virtual void finish();

protected:
	// Message handle functions
	void				handleMessage 			(cMessage*);
	// Send a message to another host
	void sendTo(const char * dest);

	// debugging enabled for this node? Used in the definition of EV
	bool				m_debug;
	bool				isPANCoor;
	const char*		m_moduleName;

	// module gate ID
  	int				mUppergateIn;
 	int				mUppergateOut;
  	int				mLowergateIn;
 	int				mLowergateOut;

	// for statistical data
	double			numForward;

	int stop;
};
#endif

