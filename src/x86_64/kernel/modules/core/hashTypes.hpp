#pragma once
#include "dataTypes.hpp"
enum hashTypes{
    FNV1a,
    DJB2,
    DJB2a,
    Murmur3,
    xxHash,
    CRC32,
    CRC64,
    SipHash,
    Adler32
};
namespace Hash {
    // =========================
    //  FNV-1a (64-bit)
    // =========================
    inline uint64 FNV1a(const char* str) {
        uint64 hash = 1469598103934665603ULL;
        while (*str) {
            hash ^= (uint64)(uint8)(*str++);
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    inline uint64 FNV1a(const void* data, uint64 len) {
        const uint8* bytes = (const uint8*)data;
        uint64 hash = 1469598103934665603ULL;
        for (uint64 i = 0; i < len; ++i) {
            hash ^= (uint64)bytes[i];
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    // =========================
    //  DJB2 / DJB2a (64-bit)
    // =========================
    inline uint64 DJB2(const char* str) {
        uint64 hash = 5381;
        int c;
        while ((c = (uint8)*str++)) {
            hash = ((hash << 5) + hash) + (uint64)c; // hash * 33 + c
        }
        return hash;
    }

    inline uint64 DJB2a(const char* str) {
        uint64 hash = 5381;
        int c;
        while ((c = (uint8)*str++)) {
            hash = ((hash << 5) + hash) ^ (uint64)c; // hash * 33 ^ c
        }
        return hash;
    }

    // =========================
    //  MurmurHash3 (64-bit, x64_64 style, simplified)
    // =========================
    inline uint64 Murmur3(const void* key, uint64 len, uint64 seed = 0) {
        const uint8* data = (const uint8*)key;
        const uint64 nblocks = len / 16;

        uint64 h1 = seed;
        uint64 h2 = seed;

        const uint64 c1 = 0x87c37b91114253d5ULL;
        const uint64 c2 = 0x4cf5ad432745937fULL;

        // body
        const uint64* blocks = (const uint64*)(data);
        for (uint64 i = 0; i < nblocks; ++i) {
            uint64 k1 = blocks[i * 2 + 0];
            uint64 k2 = blocks[i * 2 + 1];

            k1 *= c1; k1 = (k1 << 31) | (k1 >> (64 - 31)); k1 *= c2; h1 ^= k1;
            h1 = (h1 << 27) | (h1 >> (64 - 27)); h1 += h2; h1 = h1 * 5 + 0x52dce729;

            k2 *= c2; k2 = (k2 << 33) | (k2 >> (64 - 33)); k2 *= c1; h2 ^= k2;
            h2 = (h2 << 31) | (h2 >> (64 - 31)); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
        }

        // tail
        const uint8* tail = (const uint8*)(data + nblocks * 16);
        uint64 k1 = 0;
        uint64 k2 = 0;

        switch (len & 15) {
        case 15: k2 ^= (uint64)tail[14] << 48;
        [[fallthrough]];
        case 14: k2 ^= (uint64)tail[13] << 40;
        [[fallthrough]];
        case 13: k2 ^= (uint64)tail[12] << 32;
        [[fallthrough]];
        case 12: k2 ^= (uint64)tail[11] << 24;
        [[fallthrough]];
        case 11: k2 ^= (uint64)tail[10] << 16;
        [[fallthrough]];
        case 10: k2 ^= (uint64)tail[9]  << 8;
        [[fallthrough]];
        case 9:  k2 ^= (uint64)tail[8]  << 0;
                 k2 *= c2; k2 = (k2 << 33) | (k2 >> (64 - 33)); k2 *= c1; h2 ^= k2;
                 [[fallthrough]];
        case 8:  k1 ^= (uint64)tail[7]  << 56;
        [[fallthrough]];
        case 7:  k1 ^= (uint64)tail[6]  << 48;
        [[fallthrough]];
        case 6:  k1 ^= (uint64)tail[5]  << 40;
        [[fallthrough]];
        case 5:  k1 ^= (uint64)tail[4]  << 32;
        [[fallthrough]];
        case 4:  k1 ^= (uint64)tail[3]  << 24;
        [[fallthrough]];
        case 3:  k1 ^= (uint64)tail[2]  << 16;
        [[fallthrough]];
        case 2:  k1 ^= (uint64)tail[1]  << 8;
        [[fallthrough]];
        case 1:  k1 ^= (uint64)tail[0]  << 0;
                 k1 *= c1; k1 = (k1 << 31) | (k1 >> (64 - 31)); k1 *= c2; h1 ^= k1;
        }

        // finalization
        h1 ^= len;
        h2 ^= len;

        h1 += h2;
        h2 += h1;

        // fmix64
        auto fmix64 = [](uint64 k) -> uint64 {
            k ^= k >> 33;
            k *= 0xff51afd7ed558ccdULL;
            k ^= k >> 33;
            k *= 0xc4ceb9fe1a85ec53ULL;
            k ^= k >> 33;
            return k;
        };

        h1 = fmix64(h1);
        h2 = fmix64(h2);

        h1 += h2;
        // h2 += h1; // if you want 128-bit, but we return 64-bit

        return h1;
    }

    inline uint64 Murmur3(const char* str, uint64 seed = 0) {
        // compute length manually (no libc)
        uint64 len = 0;
        while (str[len] != '\0') len++;
        return Murmur3((const void*)str, len, seed);
    }

    // =========================
    //  xxHash64 (simplified)
    // =========================
    inline uint64 xxHash64(const void* input, uint64 len, uint64 seed = 0) {
        const uint8* p = (const uint8*)input;
        const uint8* const end = p + len;

        const uint64 PRIME1 = 11400714785074694791ULL;
        const uint64 PRIME2 = 14029467366897019727ULL;
        const uint64 PRIME3 =  1609587929392839161ULL;
        const uint64 PRIME4 =  9650029242287828579ULL;
        const uint64 PRIME5 =  2870177450012600261ULL;

        uint64 h;

        if (len >= 32) {
            const uint8* const limit = end - 32;
            uint64 v1 = seed + PRIME1 + PRIME2;
            uint64 v2 = seed + PRIME2;
            uint64 v3 = seed + 0;
            uint64 v4 = seed - PRIME1;

            do {
                uint64 k1 = *(const uint64*)p; p += 8;
                v1 += k1 * PRIME2;
                v1 = (v1 << 31) | (v1 >> (64 - 31));
                v1 *= PRIME1;

                uint64 k2 = *(const uint64*)p; p += 8;
                v2 += k2 * PRIME2;
                v2 = (v2 << 31) | (v2 >> (64 - 31));
                v2 *= PRIME1;

                uint64 k3 = *(const uint64*)p; p += 8;
                v3 += k3 * PRIME2;
                v3 = (v3 << 31) | (v3 >> (64 - 31));
                v3 *= PRIME1;

                uint64 k4 = *(const uint64*)p; p += 8;
                v4 += k4 * PRIME2;
                v4 = (v4 << 31) | (v4 >> (64 - 31));
                v4 *= PRIME1;
            } while (p <= limit);

            h = ((v1 << 1) | (v1 >> 63)) +
                ((v2 << 7) | (v2 >> 57)) +
                ((v3 << 12) | (v3 >> 52)) +
                ((v4 << 18) | (v4 >> 46));
        } else {
            h = seed + PRIME5;
        }

        h += len;

        while ((p + 8) <= end) {
            uint64 k1 = *(const uint64*)p; p += 8;
            k1 *= PRIME2;
            k1 = (k1 << 31) | (k1 >> (64 - 31));
            k1 *= PRIME1;
            h ^= k1;
            h = ((h << 27) | (h >> (64 - 27))) * PRIME1 + PRIME4;
        }

        while (p < end) {
            uint8 k1 = *p++;
            h ^= (uint64)k1 * PRIME5;
            h = ((h << 11) | (h >> (64 - 11))) * PRIME1;
        }

        // final mix
        h ^= h >> 33;
        h *= PRIME2;
        h ^= h >> 29;
        h *= PRIME3;
        h ^= h >> 32;

        return h;
    }

    inline uint64 xxHash64(const char* str, uint64 seed = 0) {
        uint64 len = 0;
        while (str[len] != '\0') len++;
        return xxHash64((const void*)str, len, seed);
    }

    // =========================
    //  CRC32 (simple, bitwise)
    // =========================
    inline uint32 CRC32(const void* data, uint64 len) {
        const uint8* bytes = (const uint8*)data;
        uint32 crc = 0xFFFFFFFFU;

        for (uint64 i = 0; i < len; ++i) {
            crc ^= bytes[i];
            for (int j = 0; j < 8; ++j) {
                uint32 mask = -(crc & 1U);
                crc = (crc >> 1) ^ (0xEDB88320U & mask);
            }
        }

        return ~crc;
    }

    inline uint32 CRC32(const char* str) {
        uint64 len = 0;
        while (str[len] != '\0') len++;
        return CRC32((const void*)str, len);
    }

    // =========================
    //  CRC64 (ECMA polynomial)
    // =========================
    inline uint64 CRC64(const void* data, uint64 len) {
        const uint8* bytes = (const uint8*)data;
        uint64 crc = 0xFFFFFFFFFFFFFFFFULL;
        const uint64 poly = 0xC96C5795D7870F42ULL;

        for (uint64 i = 0; i < len; ++i) {
            crc ^= (uint64)bytes[i];
            for (int j = 0; j < 8; ++j) {
                uint64 mask = -(crc & 1ULL);
                crc = (crc >> 1) ^ (poly & mask);
            }
        }

        return ~crc;
    }

    inline uint64 CRC64(const char* str) {
        uint64 len = 0;
        while (str[len] != '\0') len++;
        return CRC64((const void*)str, len);
    }

    // =========================
    //  SipHash-2-4 (64-bit)
    // =========================
    inline uint64 SipHash24(const void* data, uint64 len, uint64 k0, uint64 k1) {
        const uint8* bytes = (const uint8*)data;

        uint64 v0 = 0x736f6d6570736575ULL ^ k0;
        uint64 v1 = 0x646f72616e646f6dULL ^ k1;
        uint64 v2 = 0x6c7967656e657261ULL ^ k0;
        uint64 v3 = 0x7465646279746573ULL ^ k1;

        auto ROTL = [](uint64 x, int b) -> uint64 {
            return (x << b) | (x >> (64 - b));
        };

        auto SIPROUND = [&]() {
            v0 += v1; v1 = ROTL(v1, 13); v1 ^= v0; v0 = ROTL(v0, 32);
            v2 += v3; v3 = ROTL(v3, 16); v3 ^= v2;
            v0 += v3; v3 = ROTL(v3, 21); v3 ^= v0;
            v2 += v1; v1 = ROTL(v1, 17); v1 ^= v2; v2 = ROTL(v2, 32);
        };

        uint64 b = ((uint64)len) << 56;

        uint64 i = 0;
        while (i + 8 <= len) {
            uint64 m = 0;
            m |= (uint64)bytes[i + 0] << 0;
            m |= (uint64)bytes[i + 1] << 8;
            m |= (uint64)bytes[i + 2] << 16;
            m |= (uint64)bytes[i + 3] << 24;
            m |= (uint64)bytes[i + 4] << 32;
            m |= (uint64)bytes[i + 5] << 40;
            m |= (uint64)bytes[i + 6] << 48;
            m |= (uint64)bytes[i + 7] << 56;
            i += 8;

            v3 ^= m;
            SIPROUND(); SIPROUND();
            v0 ^= m;
        }

        uint64 m = 0;
        uint64 left = len - i;
        if (left >= 1) m |= (uint64)bytes[i + 0] << 0;
        if (left >= 2) m |= (uint64)bytes[i + 1] << 8;
        if (left >= 3) m |= (uint64)bytes[i + 2] << 16;
        if (left >= 4) m |= (uint64)bytes[i + 3] << 24;
        if (left >= 5) m |= (uint64)bytes[i + 4] << 32;
        if (left >= 6) m |= (uint64)bytes[i + 5] << 40;
        if (left >= 7) m |= (uint64)bytes[i + 6] << 48;

        b |= m;

        v3 ^= b;
        SIPROUND(); SIPROUND();
        v0 ^= b;

        v2 ^= 0xFF;
        SIPROUND(); SIPROUND(); SIPROUND(); SIPROUND();

        return v0 ^ v1 ^ v2 ^ v3;
    }

    inline uint64 SipHash24(const char* str, uint64 k0, uint64 k1) {
        uint64 len = 0;
        while (str[len] != '\0') len++;
        return SipHash24((const void*)str, len, k0, k1);
    }

    inline uint32 Adler32(const char* str) {
        uint32 a = 1;
        uint32 b = 0;
        const uint32 MOD_ADLER = 65521;
        while (*str) {
            a = (a + (uint8)(*str)) % MOD_ADLER;
            b = (b + a) % MOD_ADLER;
            str++;
        }
        return (b << 16) | a;
    }
    inline uint32 Adler32(const char* str,uint32 MOD_ADLER) {
        uint32 a = 1;
        uint32 b = 0;
        while (*str) {
            a = (a + (uint8)(*str)) % MOD_ADLER;
            b = (b + a) % MOD_ADLER;
            str++;
        }
        return (b << 16) | a;
    }
    inline uint64 hash(hashTypes type,const char* str){
        switch(type){
            case hashTypes::FNV1a:
                return FNV1a(str);
            case hashTypes::DJB2:
                return DJB2(str);
            case hashTypes::DJB2a:
                return DJB2a(str);
            case hashTypes::Murmur3:
                return Murmur3(str);
            case hashTypes::xxHash:
                return xxHash64(str);
            case hashTypes::CRC32:
                return (uint64)CRC32(str);
            case hashTypes::CRC64:
                return CRC64(str);
            case hashTypes::SipHash:
                return SipHash24(str,0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL);
            case hashTypes::Adler32:
                return (uint64)Adler32(str);
            default:
                return 0;
        }
    }
}
