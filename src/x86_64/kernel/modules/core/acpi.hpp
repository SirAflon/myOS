#pragma once
#include "dataTypes.hpp"

namespace ACPI {
    struct [[gnu::packed]] Header {
        char signature[4];
        uint32 length;
        uint8 revision;
        uint8 checksum;
        char oem_id[6];
        char oem_table_id[8];
        uint32 oem_revision;
        uint32 creator_id;
        uint32 creator_revision;
    };

    struct [[gnu::packed]] RSDP {
        char signature[8];
        uint8 checksum;
        char oem_id[6];
        uint8 revision;
        uint32 rsdt_address; 
    };

    struct [[gnu::packed]] RSDT {
        Header header;
        uint32 tables[]; // Array of physical addresses to other tables
    };

    struct [[gnu::packed]] GenericAddressStructure {
        uint8  address_space; // 0 = System Memory, 1 = System I/O
        uint8  bit_width;
        uint8  bit_offset;
        uint8  access_size;
        uint64 address;
    };

    struct [[gnu::packed]] FADT {
        Header header;
        uint32 firmware_ctrl;
        uint32 dsdt;
        uint8  reserved;
        uint8  preferred_pm_profile;
        uint16 sci_int;
        uint32 smi_cmd;
        uint8  acpi_enable;
        uint8  acpi_disable;
        uint8  s4bios_req;
        uint8  pstate_cnt;
        uint32 pm1a_evt_blk;
        uint32 pm1b_evt_blk;
        uint32 pm1a_cnt_blk;
        uint32 pm1b_cnt_blk;
        uint32 pm2_cnt_blk;
        uint32 pm_tmr_blk;
        uint32 gpe0_blk;
        uint32 gpe1_blk;
        uint8  pm1_evt_len;
        uint8  pm1_cnt_len;
        uint8  pm2_cnt_len;
        uint8  pm_tmr_len;
        uint8  gpe0_len;
        uint8  gpe1_len;
        uint8  gpe1_base;
        uint8  cstate_cnt;
        uint16 lv2_lat;
        uint16 lv3_lat;
        uint16 flush_size;
        uint16 flush_stride;
        uint8  duty_offset;
        uint8  duty_width;
        uint8  day_alrm;
        uint8  mon_alrm;
        uint8  century;
        uint16 boot_arch_flags;
        uint8  reserved2;
        uint32 flags;               // Bit 10 indicates reset register support
        GenericAddressStructure reset_reg;
        uint8  reset_value;
        uint8  reserved3[3];
    };

    inline RSDP* find_rsdp() {
        for (uintptr addr = 0xE0000; addr < 0x100000; addr += 16) {
            char* ptr = reinterpret_cast<char*>(addr);
            if (ptr[0] == 'R' && ptr[1] == 'S' && ptr[2] == 'D' && 
                ptr[3] == ' ' && ptr[7] == ' ') {
                return reinterpret_cast<RSDP*>(addr);
            }
        }
        return nullptr;
    }

    // Helper to find a specific table by its 4-character signature
    inline Header* find_table(RSDP* rsdp, const char* sig) {
        RSDT* rsdt = reinterpret_cast<RSDT*>((uintptr)rsdp->rsdt_address);
        uint32 entries = static_cast<uint32>((rsdt->header.length - sizeof(Header)) / 4);

        for (uint32 i = 0; i < entries; i++) {
            Header* table = reinterpret_cast<Header*>((uintptr)rsdt->tables[i]);
            if (table->signature[0] == sig[0] && table->signature[1] == sig[1] &&
                table->signature[2] == sig[2] && table->signature[3] == sig[3]) {
                return table;
            }
        }
        return nullptr;
    }
}