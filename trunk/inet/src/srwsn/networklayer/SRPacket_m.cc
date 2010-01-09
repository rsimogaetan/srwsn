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
    cEnum *e = cEnum::find("SROpcode");
    if (!e) enums.getInstance()->add(e = new cEnum("SROpcode"));
    e->insert(SR_REQUEST, "SR_REQUEST");
    e->insert(SR_REPLY, "SR_REPLY");
    e->insert(SR_INFO, "SR_INFO");
);

Register_Class(SRPacket);

SRPacket::SRPacket(const char *name, int kind) : cPacket(name,kind)
{
    this->opcode_var = 0;
    this->Id_var = 0;
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
    this->opcode_var = other.opcode_var;
    this->srcMACAddress_var = other.srcMACAddress_var;
    this->destMACAddress_var = other.destMACAddress_var;
    this->Id_var = other.Id_var;
    return *this;
}

void SRPacket::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->opcode_var);
    doPacking(b,this->srcMACAddress_var);
    doPacking(b,this->destMACAddress_var);
    doPacking(b,this->Id_var);
}

void SRPacket::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->opcode_var);
    doUnpacking(b,this->srcMACAddress_var);
    doUnpacking(b,this->destMACAddress_var);
    doUnpacking(b,this->Id_var);
}

int SRPacket::getOpcode() const
{
    return opcode_var;
}

void SRPacket::setOpcode(int opcode_var)
{
    this->opcode_var = opcode_var;
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
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
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
        case 1: return FD_ISCOMPOUND;
        case 2: return FD_ISCOMPOUND;
        case 3: return FD_ISEDITABLE;
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
        case 0: return "opcode";
        case 1: return "srcMACAddress";
        case 2: return "destMACAddress";
        case 3: return "Id";
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
        case 1: return "MACAddress";
        case 2: return "MACAddress";
        case 3: return "uint16_t";
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
            if (!strcmp(propertyname,"enum")) return "SROpcode";
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
        case 0: long2string(pp->getOpcode(),resultbuf,bufsize); return true;
        case 1: {std::stringstream out; out << pp->getSrcMACAddress(); opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        case 2: {std::stringstream out; out << pp->getDestMACAddress(); opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        case 3: ulong2string(pp->getId(),resultbuf,bufsize); return true;
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
        case 0: pp->setOpcode(string2long(value)); return true;
        case 3: pp->setId(string2ulong(value)); return true;
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
        case 1: return "MACAddress"; break;
        case 2: return "MACAddress"; break;
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
        case 1: return (void *)(&pp->getSrcMACAddress()); break;
        case 2: return (void *)(&pp->getDestMACAddress()); break;
        default: return NULL;
    }
}


