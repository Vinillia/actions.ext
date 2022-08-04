#ifndef _INCLUDE_OFFSET_MANAGER_H
#define _INCLUDE_OFFSET_MANAGER_H

class OffsetManager
{
public:
    OffsetManager(const char* name);
    OffsetManager();
    ~OffsetManager();

    void AcquireConfig(const char* name); 
    void ReleaseConfig(); 

    const char* OffsetToName(int32_t offset) const;
    const int32_t NameToOffset(const char* name) const;

    const int32_t RequestOffset(const char* name);
    
    template<typename T>
    T* const RequestSignature(const char* name);
    uintptr_t* const RequestSignature(const char* name);

    template<typename T>
    T* const RequestAddress(const char* name);
    uintptr_t* const RequestAddress(const char* name);

    const std::map<std::string, int32_t>& GetRequestedOffsets() const;
    const std::map<std::string, uintptr_t*>& GetRequestedAddresses() const;
    const std::map<std::string, uintptr_t*>& GetRequestedSignatures() const;

    inline const int32_t GetFailedRequestCount() const noexcept;
    inline const bool HaveFailedRequest() const noexcept;

    void Dump() const;

protected:
    void EmitError(const char* name) const;

private:
    IGameConfig* config;
    mutable int32_t errorEmited;

    std::map<std::string, int32_t> offsmap;
    std::map<std::string, uintptr_t*> addrmap;
    std::map<std::string, uintptr_t*> sigmap;
};

inline const int32_t OffsetManager::GetFailedRequestCount() const noexcept
{
    return errorEmited;
}

inline const bool OffsetManager::HaveFailedRequest() const noexcept
{
    return errorEmited > 0;
}

#endif
