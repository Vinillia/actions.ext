// This is the OS abstraction layer.

#ifndef SAFETYHOOK_USE_CXXMODULES
#include <cstdint>
#include <expected.hpp>
#include <functional>
#else
import std.compat;
#endif

namespace safetyhook {

enum class OsError {
    FAILED_TO_ALLOCATE,
    FAILED_TO_PROTECT,
    FAILED_TO_QUERY,
    FAILED_TO_GET_NEXT_THREAD,
    FAILED_TO_GET_THREAD_CONTEXT,
    FAILED_TO_SET_THREAD_CONTEXT,
    FAILED_TO_FREEZE_THREAD,
    FAILED_TO_UNFREEZE_THREAD,
    FAILED_TO_GET_THREAD_ID,
};

struct VmAccess {
    bool read : 1;
    bool write : 1;
    bool execute : 1;

    constexpr VmAccess() : read(true), write(true), execute(true) {};
    constexpr VmAccess(bool pread, bool pwrite, bool pexecute) : read(pread), write(pwrite), execute(pexecute) {};

    constexpr bool operator==(const VmAccess& other) const {
        return read == other.read && write == other.write && execute == other.execute;
    }
};

constexpr VmAccess VM_ACCESS_R(true, false,  false);
constexpr VmAccess VM_ACCESS_RW(true, true,  false);
constexpr VmAccess VM_ACCESS_RX(true, false,  true);
constexpr VmAccess VM_ACCESS_RWX(true, true,  true);

struct VmBasicInfo {
    uint8_t* address;
    size_t size;
    VmAccess access;
    bool is_free;
};

std::expected<uint8_t*, OsError> vm_allocate(uint8_t* address, size_t size, VmAccess access);
void vm_free(uint8_t* address);
std::expected<uint32_t, OsError> vm_protect(uint8_t* address, size_t size, VmAccess access);
std::expected<uint32_t, OsError> vm_protect(uint8_t* address, size_t size, uint32_t access);
std::expected<VmBasicInfo, OsError> vm_query(uint8_t* address);
bool vm_is_readable(uint8_t* address, size_t size);
bool vm_is_writable(uint8_t* address, size_t size);
bool vm_is_executable(uint8_t* address);

struct SystemInfo {
    uint32_t page_size;
    uint32_t allocation_granularity;
    uint8_t* min_address;
    uint8_t* max_address;
};

SystemInfo system_info();

using ThreadContext = void*;

void trap_threads(uint8_t* from, uint8_t* to, size_t len, const std::function<void()>& run_fn);

/// @brief Will modify the context of a thread's IP to point to a new address if its IP is at the old address.
/// @param ctx The thread context to modify.
/// @param old_ip The old IP address.
/// @param new_ip The new IP address.
void fix_ip(ThreadContext ctx, uint8_t* old_ip, uint8_t* new_ip);

} // namespace safetyhook