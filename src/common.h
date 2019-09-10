#pragma once

// free the p and set to NULL.
// p must be a T*.
#define safe_deletep(p) \
    if (p) { \
        delete p; \
        p = nullptr; \
    } \
    (void)0

// please use the safe_freepa(T[]) to free an array,//std::this_thread::sleep_for(std::chrono::seconds(5));
// or the behavior is undefined.
#define safe_deletepa(pa) \
    if (pa) { \
        delete[] pa; \
        pa = nullptr; \
    } \
    (void)0
