//
// Generated file, do not edit! Created by opp_msgc 4.0 from srwsn/networklayer/SRPacket.msg.
//

#ifndef _SRPACKET_M_H_
#define _SRPACKET_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0400
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{ ... }} section:

#include "MACAddress.h"
#include <stdint.h>  


#define SR_HEADER_BYTES   28
// end cplusplus



/**
 * Enum generated from <tt>srwsn/networklayer/SRPacket.msg</tt> by opp_msgc.
 * <pre>
 * enum SROpcode
 * {
 * 
 *     SR_REQUEST = 1;      
 *     SR_REPLY = 2;        
 *     SR_INFO = 3;        
 * }
 * </pre>
 */
enum SROpcode {
    SR_REQUEST = 1,
    SR_REPLY = 2,
    SR_INFO = 3
};

/**
 * Class generated from <tt>srwsn/networklayer/SRPacket.msg</tt> by opp_msgc.
 * <pre>
 * packet SRPacket
 * {
 *     int opcode enum(SROpcode);
 *     MACAddress srcMACAddress;
 *     MACAddress destMACAddress;
 *     uint16_t Id;
 * }
 * </pre>
 */
class SRPacket : public cPacket
{
  protected:
    int opcode_var;
    MACAddress srcMACAddress_var;
    MACAddress destMACAddress_var;
    uint16_t Id_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const SRPacket&);

  public:
    SRPacket(const char *name=NULL, int kind=0);
    SRPacket(const SRPacket& other);
    virtual ~SRPacket();
    SRPacket& operator=(const SRPacket& other);
    virtual SRPacket *dup() const {return new SRPacket(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getOpcode() const;
    virtual void setOpcode(int opcode_var);
    virtual MACAddress& getSrcMACAddress();
    virtual const MACAddress& getSrcMACAddress() const {return const_cast<SRPacket*>(this)->getSrcMACAddress();}
    virtual void setSrcMACAddress(const MACAddress& srcMACAddress_var);
    virtual MACAddress& getDestMACAddress();
    virtual const MACAddress& getDestMACAddress() const {return const_cast<SRPacket*>(this)->getDestMACAddress();}
    virtual void setDestMACAddress(const MACAddress& destMACAddress_var);
    virtual uint16_t getId() const;
    virtual void setId(uint16_t Id_var);
};

inline void doPacking(cCommBuffer *b, SRPacket& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, SRPacket& obj) {obj.parsimUnpack(b);}


#endif // _SRPACKET_M_H_