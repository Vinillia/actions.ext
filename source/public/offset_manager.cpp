#include <map>

#include "extension.h"
#include "utils.h"
#include "offset_manager.h"

OffsetManager::OffsetManager() 
{
    errorEmited = 0;
    config = NULL;
}

OffsetManager::OffsetManager(const char* name) : OffsetManager()
{
    AcquireConfig(name);
}

OffsetManager::~OffsetManager()
{
    if (config)
        gameconfs->CloseGameConfigFile(config);
}

void OffsetManager::AcquireConfig(const char* name)
{
    char szError[255];
    if (!gameconfs->LoadGameConfigFile(name, &config, szError, sizeof(szError)))
    {
        LOGERROR("Failed to load game config file: %s", szError);
    }
}

void OffsetManager::ReleaseConfig()
{
    if (config)
    {
        gameconfs->CloseGameConfigFile(config);
    }
}

const int32_t OffsetManager::RequestOffset(const char* name)
{
    int32_t offset;
    if (offsmap.count(name))
        return offsmap[name];
    if (config->GetOffset(name, (int*)&offset))
    {
        offsmap[name] = offset;
        return offset;
    }
    EmitError(name);
    return -1;
}

uintptr_t* const OffsetManager::RequestAddress(const char* name)
{
    uintptr_t* addr;
    if (addrmap.count(name))
        return addrmap[name];
    if (config->GetAddress(name, (void**)&addr))
    {
        addrmap[name] = addr;
        return addr;
    }
    EmitError(name);
    return nullptr;
}

template<typename T>
T* const OffsetManager::RequestAddress(const char* name)
{
    T* addr;
    if (addrmap.count(name))
        return addrmap[name];
    if (config->GetAddress(name, (void**)&addr))
    {
        addrmap[name] = addr;
        return addr;
    }
    EmitError(name);
    return (T*)nullptr;
}

uintptr_t* const OffsetManager::RequestSignature(const char* name)
{
    uintptr_t* memSig;
    if (sigmap.count(name))
        return sigmap[name];
    if (config->GetMemSig(name, (void**)&memSig))
    {
        sigmap[name] = memSig;
        return memSig;
    }
    EmitError(name);
    return nullptr;
}

template<typename T>
T* const OffsetManager::RequestSignature(const char* name)
{
    T* memSig;
    if (sigmap.count(name))
        return sigmap[name];
    if (config->GetMemSig(name, (void**)&memSig))
    {
        sigmap[name] = memSig;
        return memSig;
    }
    EmitError(name);
    return (T*)nullptr;
}

void OffsetManager::EmitError(const char* name) const
{
    // assert(false);
    errorEmited++;
    LOGERROR("Failed respond to the request \"%s\"", name);
}

void OffsetManager::Dump() const
{
    LOG("/----------------------------------/");

    int32_t i;

    if (offsmap.size() > 0)
    {
        LOG("Dumping offsets:");
        i = 0;

        for(auto iter = offsmap.cbegin(); iter != offsmap.cend(); iter++)
        {
            LOG("%i. %s: %i", ++i, iter->first.c_str(), iter->second);
        }
    }
    
    if (addrmap.size() > 0)
    {
        LOG("/----------------------------------/");
        LOG("Dumping addresses:");
        i = 0;

        for(auto iter = addrmap.cbegin(); iter != addrmap.cend(); iter++)
        {
            LOG("%i. %s: %X", ++i, iter->first.c_str(), iter->second);
        }
    }
    
    if (sigmap.size() > 0)
    {
        LOG("/----------------------------------/");
        LOG("Dumping signatures:");
        i = 0;

        for(auto iter = sigmap.cbegin(); iter != sigmap.cend(); iter++)
        {
            LOG("%i. %s: %X", ++i, iter->first.c_str(), iter->second);
        }
    }

    LOG("/----------------------------------/");
}

const std::map<std::string, int32_t>& OffsetManager::GetRequestedOffsets() const
{
    return offsmap;
}

const std::map<std::string, uintptr_t*>& OffsetManager::GetRequestedAddresses() const
{
    return addrmap;
}

const std::map<std::string, uintptr_t*>& OffsetManager::GetRequestedSignatures() const
{
    return sigmap;
}

const char* OffsetManager::OffsetToName(int32_t offset) const
{
    for(auto iter = offsmap.cbegin(); iter != offsmap.cend(); iter++)
    {
        if (iter->second == offset)
            return iter->first.c_str();
    }

    return NULL;
}

const int32_t OffsetManager::NameToOffset(const char* name) const
{
    auto f = offsmap.find(name);

    if (f != offsmap.end())
        return f->second;

    return -1;
}