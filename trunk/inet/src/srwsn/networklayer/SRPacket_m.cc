//
// Generated file, do not edit! Created by opp_msgc 4.0 from srwsn/networklayer/SRPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "SRPacket_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("SRQuery");
    if (!e) enums.getInstance()->add(e = new cEnum("SRQuery"));
    e->insert(Q_REQUEST, "Q_REQUEST");
    e->insert(Q_REPLY, "Q_REPLY");
);

EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("SRMsg");
    if (!e) enums.getInstance()->add(e = new cEnum("SRMsg"));
    e->insert(MSG_DISCOVERY, "MSG_DISCOVERY");
    e->insert(MSG_NORMAL, "MSG_NORMAL");
    e->insert(MSG_ALERT, "MSG_ALERT");
);

Register_Class(SRPacket);

SRPacket::SRPacket(const char *name, int kind) : cPacket(name,kind)
{
    this->msgType_var = 0;
    this->queryType_var = 0;
    this->queryId_var = 0;
    this->Id_var = 0;
    this->amIAlertGenerator_var = 0;
    this->alertTimeStamp_var = 0;
}

SRPacket::SRPacket(const SRPacket& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

SRPacket::~SRPacket()
{
}

SRPacket& SRPacket::operator=(const SRPacket& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->msgType_var = other.msgType_var;
    this->queryType_var = other.queryType_var;
    this->queryId_var = other.queryId_var;
    this->srcMACAddress_var = other.srcMACAddress_var;
    this->destMACAddress_var = other.destMACAddress_var;
    this->Id_var = other.Id_var;
    this->amIAlertGenerator_var = other.amIAlertGenerator_var;
    this->alertTimeStamp_var = other.alertTimeStamp_var;
    this->bloom_var = other.bloom_var;
    return *this;
}

void SRPacket::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->msgType_var);
    doPacking(b,this->queryType_var);
    doPacking(b,this->queryId_var);
    doPacking(b,this->srcMACAddress_var);
    doPacking(b,this->destMACAddress_var);
    doPacking(b,this->Id_var);
    doPacking(b,this->amIAlertGenerator_var);
    doPacking(b,this->alertTimeStamp_var);
    doPacking(b,this->bloom_var);
}

void SRPacket::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->msgType_var);
    doUnpacking(b,this->queryType_var);
    doUnpacking(b,this->queryId_var);
    doUnpacking(b,this->srcMACAddress_var);
    doUnpacking(b,this->destMACAddress_var);
    doUnpacking(b,this->Id_var);
    doUnpacking(b,this->amIAlertGenerator_var);
    doUnpacking(b,this->alertTimeStamp_var);
    doUnpacking(b,this->bloom_var);
}

int SRPacket::getMsgType() const
{
    return msgType_var;
}

void SRPacket::setMsgType(int msgType_var)
{
    this->msgType_var = msgType_var;
}

int SRPacket::getQueryType() const
{
    return queryType_var;
}

void SRPacket::setQueryType(int queryType_var)
{
    this->queryType_var = queryType_var;
}

int SRPacket::getQueryId() const
{
    return queryId_var;
}

void SRPacket::setQueryId(int queryId_var)
{
    this->queryId_var = queryId_var;
}

MACAddress& SRPacket::getSrcMACAddress()
{
    return srcMACAddress_var;
}

void SRPacket::setSrcMACAddress(const MACAddress& srcMACAddress_var)
{
    this->srcMACAddress_var = srcMACAddress_var;
}

MACAddress& SRPacket::getDestMACAddress()
{
    return destMACAddress_var;
}

void SRPacket::setDestMACAddress(const MACAddress& destMACAddress_var)
{
    this->destMACAddress_var = destMACAddress_var;
}

uint16_t SRPacket::getId() const
{
    return Id_var;
}

void SRPacket::setId(uint16_t Id_var)
{
    this->Id_var = Id_var;
}

bool SRPacket::getAmIAlertGenerator() const
{
    return amIAlertGenerator_var;
}

void SRPacket::setAmIAlertGenerator(bool amIAlertGenerator_var)
{
    this->amIAlertGenerator_var = amIAlertGenerator_var;
}

uint16_t SRPacket::getAlertTimeStamp() const
{
    return alertTimeStamp_var;
}

void SRPacket::setAlertTimeStamp(uint16_t alertTimeStamp_var)
{
    this->alertTimeStamp_var = alertTimeStamp_var;
}

BloomFilter& SRPacket::getBloom()
{
    return bloom_var;
}

void SRPacket::setBloom(const BloomFilter& bloom_var)
{
    this->bloom_var = bloom_var;
}

class SRPacketDescriptor : public cClassDescriptor
{
  public:
    SRPacketDescriptor();
    virtual ~SRPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual bool getFieldAsString(void *object, int field, int i, char *resultbuf, int bufsize) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(SRPacketDescriptor);

SRPacketDescriptor::SRPacketDescriptor() : cClassDescriptor("SRPacket", "cPacket")
{
}

SRPacketDescriptor::~SRPacketDescriptor()
{
}

bool SRPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<SRPacket *>(obj)!=NULL;
}

const char *SRPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int SRPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount(object) : 9;
}

unsigned int SRPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0: return FD_ISEDITABLE;
        case 1: return FD_ISEDITABLE;
        case 2: return FD_ISEDITABLE;
        case 3: return FD_ISCOMPOUND;
        case 4: return FD_ISCOMPOUND;
        case 5: return FD_ISEDITABLE;
        case 6: return FD_ISEDITABLE;
        case 7: return FD_ISEDITABLE;
        case 8: return FD_ISCOMPOUND;
        default: return 0;
    }
}

const char *SRPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0: return "msgType";
        case 1: return "queryType";
        case 2: return "queryId";
        case 3: return "srcMACAddress";
        case 4: return "destMACAddress";
        case 5: return "Id";
        case 6: return "amIAlertGenerator";
        case 7: return "alertTimeStamp";
        case 8: return "bloom";
        default: return NULL;
    }
}

const char *SRPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0: return "int";
        case 1: return "int";
        case 2: return "int";
        case 3: return "MACAddress";
        case 4: return "MACAddress";
        case 5: return "uint16_t";
        case 6: return "bool";
        case 7: return "uint16_t";
        case 8: return "BloomFilter";
        default: return NULL;
    }
}

const char *SRPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "SRMsg";
            return NULL;
        case 1:
            if (!strcmp(propertyname,"enum")) return "SRQuery";
            return NULL;
        case 2:
            if (!strcmp(propertyname,"enum")) return "SensoType";
            return NULL;
        default: return NULL;
    }
}

int SRPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    SRPacket *pp = (SRPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

bool SRPacketDescriptor::getFieldAsString(void *object, int field, int i, char *resultbuf, int bufsize) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i,resultbuf,bufsize);
        field -= basedesc->getFieldCount(object);
    }
    SRPacket *pp = (SRPacket *)object; (void)pp;
    switch (field) {
        case 0: long2string(pp->getMsgType(),resultbuf,bufsize); return true;
        case 1: long2string(pp->getQueryType(),resultbuf,bufsize); return true;
        case 2: long2string(pp->getQueryId(),resultbuf,bufsize); return true;
        case 3: {std::stringstream out; out << pp->getSrcMACAddress(); opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        case 4: {std::stringstream out; out << pp->getDestMACAddress(); opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        case 5: ulong2string(pp->getId(),resultbuf,bufsize); return true;
        case 6: bool2string(pp->getAmIAlertGenerator(),resultbuf,bufsize); return true;
        case 7: ulong2string(pp->getAlertTimeStamp(),resultbuf,bufsize); return true;
        case 8: {std::stringstream out; out << pp->getBloom(); opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        default: return false;
    }
}

bool SRPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    SRPacket *pp = (SRPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgType(string2long(value)); return true;
        case 1: pp->setQueryType(string2long(value)); return true;
        case 2: pp->setQueryId(string2long(value)); return true;
        case 5: pp->setId(string2ulong(value)); return true;
        case 6: pp->setAmIAlertGenerator(string2bool(value)); return true;
        case 7: pp->setAlertTimeStamp(string2ulong(value)); return true;
        default: return false;
    }
}

const char *SRPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 3: return "MACAddress"; break;
        case 4: return "MACAddress"; break;
        case 8: return "BloomFilter"; break;
        default: return NULL;
    }
}

void *SRPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    SRPacket *pp = (SRPacket *)object; (void)pp;
    switch (field) {
        case 3: return (void *)(&pp->getSrcMACAddress()); break;
        case 4: return (void *)(&pp->getDestMACAddress()); break;
        case 8: return (void *)(&pp->getBloom()); break;
        default: return NULL;
    }
}


