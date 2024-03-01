/*
 * Memory DLL loading code
 * Version 0.0.4
 *
 * Copyright (c) 2004-2015 by Joachim Bauch / mail@joachim-bauch.de
 * http://www.joachim-bauch.de
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is MemoryModule.h
 *
 * The Initial Developer of the Original Code is Joachim Bauch.
 *
 * Portions created by Joachim Bauch are Copyright (C) 2004-2015
 * Joachim Bauch. All Rights Reserved.
 *
 */


#include "../include/MasterQian.Meta.h"
#include <Windows.h>
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_System_##name

using HMEMORYMODULE = void*;
using HMEMORYRSRC = void*;

struct ExportNameEntry {
    LPCSTR name;
    WORD idx;
};

using DllEntryProc = BOOL(__stdcall*)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
using ExeEntryProc = int(__stdcall*)();
#define GET_HEADER_DICTIONARY(mod, idx)  &(mod)->headers->OptionalHeader.DataDirectory[idx]

typedef struct POINTER_LIST {
    POINTER_LIST* next;
    void* address;
} POINTER_LIST;

using PMEMORYMODULE = struct MEMORYMODULE {
    PIMAGE_NT_HEADERS headers;
    unsigned char* codeBase;
    HMODULE* modules;
    int numModules;
    bool initialized;
    bool isDLL;
    bool isRelocated;
    ExportNameEntry* nameExportsTable;
    ExeEntryProc exeEntry;
    DWORD pageSize;
    POINTER_LIST* blockedMemory;
}*;

using PSECTIONFINALIZEDATA = struct SECTIONFINALIZEDATA {
    LPVOID address;
    LPVOID alignedAddress;
    size_t size;
    DWORD characteristics;
    BOOL last;
}*;

inline constexpr uintptr_t AlignValueDown(uintptr_t value, uintptr_t alignment) {
    return value & ~(alignment - 1);
}

inline LPVOID AlignAddressDown(LPVOID address, uintptr_t alignment) {
    return (LPVOID)AlignValueDown((uintptr_t)address, alignment);
}

inline size_t AlignValueUp(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

inline void* OffsetPointer(void* data, ptrdiff_t offset) {
    return (void*)((uintptr_t)data + offset);
}

static void FreePointerList(POINTER_LIST* head) {
    POINTER_LIST* node = head;
    while (node) {
        POINTER_LIST* next;
        VirtualFree(node->address, 0, MEM_RELEASE);
        next = node->next;
        free(node);
        node = next;
    }
}

inline BOOL CheckSize(size_t size, size_t expected) {
    if (size < expected) {
        SetLastError(ERROR_INVALID_DATA);
        return false;
    }
    return true;
}

static BOOL CopySections(const unsigned char* data, size_t size, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module) {
    int i, section_size;
    unsigned char* codeBase = module->codeBase;
    unsigned char* dest;
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
    for (i = 0; i < module->headers->FileHeader.NumberOfSections; i++, section++) {
        if (section->SizeOfRawData == 0) {
            section_size = old_headers->OptionalHeader.SectionAlignment;
            if (section_size > 0) {
                dest = (unsigned char*)VirtualAlloc(codeBase + section->VirtualAddress,
                    section_size,
                    MEM_COMMIT,
                    PAGE_READWRITE);
                if (dest == nullptr) {
                    return false;
                }
                dest = codeBase + section->VirtualAddress;
                section->Misc.PhysicalAddress = (DWORD)((uintptr_t)dest & 0xffffffff);
                memset(dest, 0, section_size);
            }
            continue;
        }

        if (!CheckSize(size, section->PointerToRawData + section->SizeOfRawData)) {
            return false;
        }

        dest = (unsigned char*)VirtualAlloc(codeBase + section->VirtualAddress,
            section->SizeOfRawData,
            MEM_COMMIT,
            PAGE_READWRITE);
        if (dest == nullptr) {
            return false;
        }
        dest = codeBase + section->VirtualAddress;
        memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);
        section->Misc.PhysicalAddress = (DWORD)((uintptr_t)dest & 0xffffffff);
    }

    return true;
}

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] = {
    { { PAGE_NOACCESS, PAGE_WRITECOPY }, { PAGE_READONLY, PAGE_READWRITE } },
    { { PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY }, { PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE } }
};

static size_t GetRealSectionSize(PMEMORYMODULE module, PIMAGE_SECTION_HEADER section) {
    DWORD size = section->SizeOfRawData;
    if (size == 0) {
        if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
            size = module->headers->OptionalHeader.SizeOfInitializedData;
        }
        else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
            size = module->headers->OptionalHeader.SizeOfUninitializedData;
        }
    }
    return (size_t)size;
}

static BOOL FinalizeSection(PMEMORYMODULE module, PSECTIONFINALIZEDATA sectionData) {
    DWORD protect, oldProtect;
    BOOL executable;
    BOOL readable;
    BOOL writeable;

    if (sectionData->size == 0) {
        return true;
    }

    if (sectionData->characteristics & IMAGE_SCN_MEM_DISCARDABLE) {
        // section is not needed any more and can safely be freed
        if (sectionData->address == sectionData->alignedAddress &&
            (sectionData->last ||
                module->headers->OptionalHeader.SectionAlignment == module->pageSize ||
                (sectionData->size % module->pageSize) == 0)
            ) {
            VirtualFree(sectionData->address, sectionData->size, MEM_DECOMMIT);
        }
        return true;
    }

    executable = (sectionData->characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
    readable = (sectionData->characteristics & IMAGE_SCN_MEM_READ) != 0;
    writeable = (sectionData->characteristics & IMAGE_SCN_MEM_WRITE) != 0;
    protect = ProtectionFlags[executable][readable][writeable];
    if (sectionData->characteristics & IMAGE_SCN_MEM_NOT_CACHED) {
        protect |= PAGE_NOCACHE;
    }

    if (VirtualProtect(sectionData->address, sectionData->size, protect, &oldProtect) == 0) {
        return false;
    }

    return true;
}

static BOOL FinalizeSections(PMEMORYMODULE module) {
    int i;
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
    uintptr_t imageOffset = ((uintptr_t)module->headers->OptionalHeader.ImageBase & 0xffffffff00000000);
    SECTIONFINALIZEDATA sectionData;
    sectionData.address = (LPVOID)((uintptr_t)section->Misc.PhysicalAddress | imageOffset);
    sectionData.alignedAddress = AlignAddressDown(sectionData.address, module->pageSize);
    sectionData.size = GetRealSectionSize(module, section);
    sectionData.characteristics = section->Characteristics;
    sectionData.last = false;
    section++;

    for (i = 1; i < module->headers->FileHeader.NumberOfSections; i++, section++) {
        LPVOID sectionAddress = (LPVOID)((uintptr_t)section->Misc.PhysicalAddress | imageOffset);
        LPVOID alignedAddress = AlignAddressDown(sectionAddress, module->pageSize);
        size_t sectionSize = GetRealSectionSize(module, section);
        if (sectionData.alignedAddress == alignedAddress || (uintptr_t)sectionData.address + sectionData.size > (uintptr_t) alignedAddress) {
            // Section shares page with previous
            if ((section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0 || (sectionData.characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0) {
                sectionData.characteristics = (sectionData.characteristics | section->Characteristics) & ~IMAGE_SCN_MEM_DISCARDABLE;
            }
            else {
                sectionData.characteristics |= section->Characteristics;
            }
            sectionData.size = (((uintptr_t)sectionAddress) + ((uintptr_t)sectionSize)) - (uintptr_t)sectionData.address;
            continue;
        }

        if (!FinalizeSection(module, &sectionData)) {
            return false;
        }
        sectionData.address = sectionAddress;
        sectionData.alignedAddress = alignedAddress;
        sectionData.size = sectionSize;
        sectionData.characteristics = section->Characteristics;
    }
    sectionData.last = true;
    if (!FinalizeSection(module, &sectionData)) {
        return false;
    }
    return true;
}

static BOOL ExecuteTLS(PMEMORYMODULE module) {
    unsigned char* codeBase = module->codeBase;
    PIMAGE_TLS_DIRECTORY tls;
    PIMAGE_TLS_CALLBACK* callback;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_TLS);
    if (directory->VirtualAddress == 0) {
        return true;
    }

    tls = (PIMAGE_TLS_DIRECTORY)(codeBase + directory->VirtualAddress);
    callback = (PIMAGE_TLS_CALLBACK*)tls->AddressOfCallBacks;
    if (callback) {
        while (*callback) {
            (*callback)((LPVOID)codeBase, DLL_PROCESS_ATTACH, nullptr);
            callback++;
        }
    }
    return true;
}

static BOOL PerformBaseRelocation(PMEMORYMODULE module, ptrdiff_t delta) {
    unsigned char* codeBase = module->codeBase;
    PIMAGE_BASE_RELOCATION relocation;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if (directory->Size == 0) {
        return (delta == 0);
    }

    relocation = (PIMAGE_BASE_RELOCATION)(codeBase + directory->VirtualAddress);
    for (; relocation->VirtualAddress > 0; ) {
        DWORD i;
        unsigned char* dest = codeBase + relocation->VirtualAddress;
        unsigned short* relInfo = (unsigned short*)OffsetPointer(relocation, sizeof(IMAGE_BASE_RELOCATION));
        for (i = 0; i < ((relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2); i++, relInfo++) {
            int type = *relInfo >> 12;
            int offset = *relInfo & 0xfff;
            switch (type) {
            case IMAGE_REL_BASED_ABSOLUTE: break;
            case IMAGE_REL_BASED_HIGHLOW: {
                DWORD* patchAddrHL = (DWORD*)(dest + offset);
                *patchAddrHL += (DWORD)delta;
                break;
            }
            case IMAGE_REL_BASED_DIR64: {
                ULONGLONG* patchAddr64 = (ULONGLONG*)(dest + offset);
                *patchAddr64 += (ULONGLONG)delta;
                break;
            }
            default: break;
            }
        }
        relocation = (PIMAGE_BASE_RELOCATION)OffsetPointer(relocation, relocation->SizeOfBlock);
    }
    return true;
}

static BOOL BuildImportTable(PMEMORYMODULE module) {
    unsigned char* codeBase = module->codeBase;
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    BOOL result = true;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (directory->Size == 0) {
        return true;
    }

    importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(codeBase + directory->VirtualAddress);
    for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++) {
        uintptr_t* thunkRef;
        FARPROC* funcRef;
        HMODULE* tmp;
        HMODULE handle = LoadLibraryA((LPCSTR)(codeBase + importDesc->Name));
        if (handle == nullptr) {
            SetLastError(ERROR_MOD_NOT_FOUND);
            result = false;
            break;
        }

        tmp = (HMODULE*)realloc(module->modules, (module->numModules + 1) * (sizeof(HMODULE)));
        if (tmp == nullptr) {
            FreeLibrary(handle);
            SetLastError(ERROR_OUTOFMEMORY);
            result = false;
            break;
        }
        module->modules = tmp;

        module->modules[module->numModules++] = handle;
        if (importDesc->OriginalFirstThunk) {
            thunkRef = (uintptr_t*)(codeBase + importDesc->OriginalFirstThunk);
            funcRef = (FARPROC*)(codeBase + importDesc->FirstThunk);
        }
        else {
            thunkRef = (uintptr_t*)(codeBase + importDesc->FirstThunk);
            funcRef = (FARPROC*)(codeBase + importDesc->FirstThunk);
        }
        for (; *thunkRef; thunkRef++, funcRef++) {
            if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
                *funcRef = GetProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef));
            }
            else {
                PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)(codeBase + (*thunkRef));
                *funcRef = GetProcAddress(handle, (LPCSTR)&thunkData->Name);
            }
            if (*funcRef == 0) {
                result = false;
                break;
            }
        }

        if (!result) {
            FreeLibrary(handle);
            SetLastError(ERROR_PROC_NOT_FOUND);
            break;
        }
    }

    return result;
}

META_EXPORT_API(void, MemoryFreeLibrary, HMEMORYMODULE mod) {
    PMEMORYMODULE module = (PMEMORYMODULE)mod;
    if (module == nullptr) {
        return;
    }
    if (module->initialized) {
        DllEntryProc DllEntry = (DllEntryProc)(LPVOID)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
        (*DllEntry)((HINSTANCE)module->codeBase, DLL_PROCESS_DETACH, 0);
    }

    free(module->nameExportsTable);
    if (module->modules) {
        int i;
        for (i = 0; i < module->numModules; i++) {
            if (module->modules[i]) {
                FreeLibrary(module->modules[i]);
            }
        }
        free(module->modules);
    }

    if (module->codeBase) {
        VirtualFree(module->codeBase, 0, MEM_RELEASE);
    }

    FreePointerList(module->blockedMemory);
    HeapFree(GetProcessHeap(), 0, module);
}

META_EXPORT_API(HMEMORYMODULE, MemoryLoadLibrary, mqcmem data, mqui64 size) {
    PMEMORYMODULE result = nullptr;
    PIMAGE_DOS_HEADER dos_header;
    PIMAGE_NT_HEADERS old_header;
    unsigned char* code, * headers;
    ptrdiff_t locationDelta;
    SYSTEM_INFO sysInfo;
    PIMAGE_SECTION_HEADER section;
    DWORD i;
    size_t optionalSectionSize;
    size_t lastSectionEnd = 0;
    size_t alignedImageSize;
    POINTER_LIST* blockedMemory = nullptr;

    if (!CheckSize(size, sizeof(IMAGE_DOS_HEADER))) {
        return nullptr;
    }
    dos_header = (PIMAGE_DOS_HEADER)data;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return nullptr;
    }

    if (!CheckSize(size, dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS))) {
        return nullptr;
    }
    old_header = (PIMAGE_NT_HEADERS) & ((const unsigned char*)(data))[dos_header->e_lfanew];
    if (old_header->Signature != IMAGE_NT_SIGNATURE) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return nullptr;
    }

    if (old_header->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return nullptr;
    }

    if (old_header->OptionalHeader.SectionAlignment & 1) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return nullptr;
    }

    section = IMAGE_FIRST_SECTION(old_header);
    optionalSectionSize = old_header->OptionalHeader.SectionAlignment;
    for (i = 0; i < old_header->FileHeader.NumberOfSections; i++, section++) {
        size_t endOfSection;
        if (section->SizeOfRawData == 0) {
            endOfSection = section->VirtualAddress + optionalSectionSize;
        }
        else {
            endOfSection = section->VirtualAddress + section->SizeOfRawData;
        }

        if (endOfSection > lastSectionEnd) {
            lastSectionEnd = endOfSection;
        }
    }

    GetNativeSystemInfo(&sysInfo);
    alignedImageSize = AlignValueUp(old_header->OptionalHeader.SizeOfImage, sysInfo.dwPageSize);
    if (alignedImageSize != AlignValueUp(lastSectionEnd, sysInfo.dwPageSize)) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return nullptr;
    }

    code = (unsigned char*)VirtualAlloc((LPVOID)(old_header->OptionalHeader.ImageBase),
        alignedImageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (code == nullptr) {
        code = (unsigned char*)VirtualAlloc(nullptr,
            alignedImageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (code == nullptr) {
            SetLastError(ERROR_OUTOFMEMORY);
            return nullptr;
        }
    }

    // Memory block may not span 4 GB boundaries.
    while ((((uintptr_t)code) >> 32) < (((uintptr_t)(code + alignedImageSize)) >> 32)) {
        POINTER_LIST* node = (POINTER_LIST*)malloc(sizeof(POINTER_LIST));
        if (!node) {
            VirtualFree(code, 0, MEM_RELEASE);
            FreePointerList(blockedMemory);
            SetLastError(ERROR_OUTOFMEMORY);
            return nullptr;
        }

        node->next = blockedMemory;
        node->address = code;
        blockedMemory = node;

        code = (unsigned char*)VirtualAlloc(nullptr,
            alignedImageSize,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (code == nullptr) {
            FreePointerList(blockedMemory);
            SetLastError(ERROR_OUTOFMEMORY);
            return nullptr;
        }
    }

    result = (PMEMORYMODULE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MEMORYMODULE));
    if (result == nullptr) {
        VirtualFree(code, 0, MEM_RELEASE);
        FreePointerList(blockedMemory);
        SetLastError(ERROR_OUTOFMEMORY);
        return nullptr;
    }

    result->codeBase = code;
    result->isDLL = (old_header->FileHeader.Characteristics & IMAGE_FILE_DLL) != 0;
    result->pageSize = sysInfo.dwPageSize;
    result->blockedMemory = blockedMemory;

    if (!CheckSize(size, old_header->OptionalHeader.SizeOfHeaders)) {
        goto error;
    }

    // commit memory for headers
    headers = (unsigned char*)VirtualAlloc(code,
        old_header->OptionalHeader.SizeOfHeaders, MEM_COMMIT, PAGE_READWRITE);

    if (!headers) {
        goto error;
    }

    // copy PE header to code
    memcpy(headers, dos_header, old_header->OptionalHeader.SizeOfHeaders);
    result->headers = (PIMAGE_NT_HEADERS) & ((const unsigned char*)(headers))[dos_header->e_lfanew];

    // update position
    result->headers->OptionalHeader.ImageBase = (uintptr_t)code;

    // copy sections from DLL file block to new memory location
    if (!CopySections((const unsigned char*)data, size, old_header, result)) {
        goto error;
    }

    // adjust base address of imported data
    locationDelta = (ptrdiff_t)(result->headers->OptionalHeader.ImageBase - old_header->OptionalHeader.ImageBase);
    if (locationDelta != 0) {
        result->isRelocated = PerformBaseRelocation(result, locationDelta);
    }
    else {
        result->isRelocated = true;
    }

    // load required dlls and adjust function table of imports
    if (!BuildImportTable(result)) {
        goto error;
    }

    // mark memory pages depending on section headers and release
    // sections that are marked as "discardable"
    if (!FinalizeSections(result)) {
        goto error;
    }

    // TLS callbacks are executed BEFORE the main loading
    if (!ExecuteTLS(result)) {
        goto error;
    }

    // get entry point of loaded library
    if (result->headers->OptionalHeader.AddressOfEntryPoint != 0) {
        if (result->isDLL) {
            DllEntryProc DllEntry = (DllEntryProc)(LPVOID)(code + result->headers->OptionalHeader.AddressOfEntryPoint);
            // notify library about attaching to process
            BOOL successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);
            if (!successfull) {
                SetLastError(ERROR_DLL_INIT_FAILED);
                goto error;
            }
            result->initialized = true;
        }
        else {
            result->exeEntry = (ExeEntryProc)(LPVOID)(code + result->headers->OptionalHeader.AddressOfEntryPoint);
        }
    }
    else {
        result->exeEntry = nullptr;
    }

    return (HMEMORYMODULE)result;

error:
    MasterQian_System_MemoryFreeLibrary(result);
    return nullptr;
}

META_EXPORT_API(FARPROC, MemoryGetProcAddress, HMEMORYMODULE mod, LPCSTR name) {
    PMEMORYMODULE module = (PMEMORYMODULE)mod;
    unsigned char* codeBase = module->codeBase;
    DWORD idx = 0;
    PIMAGE_EXPORT_DIRECTORY exports;
    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_EXPORT);
    if (directory->Size == 0) {
        // no export table found
        SetLastError(ERROR_PROC_NOT_FOUND);
        return nullptr;
    }

    exports = (PIMAGE_EXPORT_DIRECTORY)(codeBase + directory->VirtualAddress);
    if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0) {
        // DLL doesn't export anything
        SetLastError(ERROR_PROC_NOT_FOUND);
        return nullptr;
    }

    if (HIWORD(name) == 0) {
        if (LOWORD(name) < exports->Base) {
            SetLastError(ERROR_PROC_NOT_FOUND);
            return nullptr;
        }
        idx = LOWORD(name) - exports->Base;
    }
    else if (!exports->NumberOfNames) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return nullptr;
    }
    else {
        const ExportNameEntry* found;

        if (!module->nameExportsTable) {
            DWORD i;
            DWORD* nameRef = (DWORD*)(codeBase + exports->AddressOfNames);
            WORD* ordinal = (WORD*)(codeBase + exports->AddressOfNameOrdinals);
            ExportNameEntry* entry = (ExportNameEntry*)malloc(exports->NumberOfNames * sizeof(ExportNameEntry));
            module->nameExportsTable = entry;
            if (!entry) {
                SetLastError(ERROR_OUTOFMEMORY);
                return nullptr;
            }
            for (i = 0; i < exports->NumberOfNames; i++, nameRef++, ordinal++, entry++) {
                entry->name = (const char*)(codeBase + (*nameRef));
                entry->idx = *ordinal;
            }
            qsort(module->nameExportsTable,
                exports->NumberOfNames,
                sizeof(ExportNameEntry), [] (mqcmem a, mqcmem b) {
                return lstrcmpA(static_cast<ExportNameEntry const*>(a)->name, static_cast<ExportNameEntry const*>(b)->name);
            });
        }

        found = (const ExportNameEntry*)bsearch(&name,
            module->nameExportsTable,
            exports->NumberOfNames,
            sizeof(ExportNameEntry), [ ] (mqcmem a, mqcmem b) {
            return lstrcmpA(*static_cast<LPCSTR const*>(a), static_cast<const ExportNameEntry*>(b)->name);
        });
        if (!found) {
            SetLastError(ERROR_PROC_NOT_FOUND);
            return nullptr;
        }

        idx = found->idx;
    }

    if (idx > exports->NumberOfFunctions) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return nullptr;
    }
    return (FARPROC)(LPVOID)(codeBase + (*(DWORD*)(codeBase + exports->AddressOfFunctions + (idx * 4))));
}

META_EXPORT_API(int, MemoryCallEntryPoint, HMEMORYMODULE mod) {
    PMEMORYMODULE module = (PMEMORYMODULE)mod;

    if (module == nullptr || module->isDLL || module->exeEntry == nullptr || !module->isRelocated) {
        return -1;
    }

    return module->exeEntry();
}

static PIMAGE_RESOURCE_DIRECTORY_ENTRY _MemorySearchResourceEntry(
    void* root,
    PIMAGE_RESOURCE_DIRECTORY resources,
    LPCTSTR key) {
    PIMAGE_RESOURCE_DIRECTORY_ENTRY entries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(resources + 1);
    PIMAGE_RESOURCE_DIRECTORY_ENTRY result = nullptr;
    DWORD start;
    DWORD end;
    DWORD middle;

    if (!IS_INTRESOURCE(key) && key[0] == TEXT('#')) {
        // special case: resource id given as string
        TCHAR* endpos = nullptr;
        long int tmpkey = (WORD)wcstol((TCHAR*)&key[1], &endpos, 10);
        if (tmpkey <= 0xffff && lstrlen(endpos) == 0) {
            key = MAKEINTRESOURCE(tmpkey);
        }
    }

    if (IS_INTRESOURCE(key)) {
        WORD check = (WORD)(uintptr_t)key;
        start = resources->NumberOfNamedEntries;
        end = start + resources->NumberOfIdEntries;

        while (end > start) {
            WORD entryName;
            middle = (start + end) >> 1;
            entryName = (WORD)entries[middle].Name;
            if (check < entryName) {
                end = (end != middle ? middle : middle - 1);
            }
            else if (check > entryName) {
                start = (start != middle ? middle : middle + 1);
            }
            else {
                result = &entries[middle];
                break;
            }
        }
    }
    else {
        LPCWSTR searchKey = key;
        size_t searchKeyLen = lstrlenW(key);
        start = 0;
        end = resources->NumberOfNamedEntries;
        while (end > start) {
            int cmp;
            PIMAGE_RESOURCE_DIR_STRING_U resourceString;
            middle = (start + end) >> 1;
            resourceString = (PIMAGE_RESOURCE_DIR_STRING_U)OffsetPointer(root, entries[middle].Name & 0x7FFFFFFF);
            cmp = lstrcmpiW(searchKey, resourceString->NameString);
            if (cmp == 0) {
                // Handle partial match
                if (searchKeyLen > resourceString->Length) {
                    cmp = 1;
                }
                else if (searchKeyLen < resourceString->Length) {
                    cmp = -1;
                }
            }
            if (cmp < 0) {
                end = (middle != end ? middle : middle - 1);
            }
            else if (cmp > 0) {
                start = (middle != start ? middle : middle + 1);
            }
            else {
                result = &entries[middle];
                break;
            }
        }
    }

    return result;
}

META_EXPORT_API(HMEMORYRSRC, MemoryFindResource, HMEMORYMODULE module, LPCWSTR name, LPCWSTR type) {
    unsigned char* codeBase = ((PMEMORYMODULE)module)->codeBase;
    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY((PMEMORYMODULE)module, IMAGE_DIRECTORY_ENTRY_RESOURCE);
    PIMAGE_RESOURCE_DIRECTORY rootResources;
    PIMAGE_RESOURCE_DIRECTORY nameResources;
    PIMAGE_RESOURCE_DIRECTORY typeResources;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY foundType;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY foundName;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY foundLanguage;
    if (directory->Size == 0) {
        // no resource table found
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return nullptr;
    }

    WORD language = LANGIDFROMLCID(GetThreadLocale());

    rootResources = (PIMAGE_RESOURCE_DIRECTORY)(codeBase + directory->VirtualAddress);
    foundType = _MemorySearchResourceEntry(rootResources, rootResources, type);
    if (foundType == nullptr) {
        SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
        return nullptr;
    }

    typeResources = (PIMAGE_RESOURCE_DIRECTORY)(codeBase + directory->VirtualAddress + (foundType->OffsetToData & 0x7fffffff));
    foundName = _MemorySearchResourceEntry(rootResources, typeResources, name);
    if (foundName == nullptr) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return nullptr;
    }

    nameResources = (PIMAGE_RESOURCE_DIRECTORY)(codeBase + directory->VirtualAddress + (foundName->OffsetToData & 0x7fffffff));
    foundLanguage = _MemorySearchResourceEntry(rootResources, nameResources, (LPCTSTR)(uintptr_t)language);
    if (foundLanguage == nullptr) {
        // requested language not found, use first available
        if (nameResources->NumberOfIdEntries == 0) {
            SetLastError(ERROR_RESOURCE_LANG_NOT_FOUND);
            return nullptr;
        }

        foundLanguage = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(nameResources + 1);
    }

    return (codeBase + directory->VirtualAddress + (foundLanguage->OffsetToData & 0x7fffffff));
}

META_EXPORT_API(DWORD, MemorySizeofResource, HMEMORYMODULE module, HMEMORYRSRC resource) {
    PIMAGE_RESOURCE_DATA_ENTRY entry;
    entry = (PIMAGE_RESOURCE_DATA_ENTRY)resource;
    if (entry == nullptr) {
        return 0;
    }

    return entry->Size;
}

META_EXPORT_API(LPVOID, MemoryLoadResource, HMEMORYMODULE module, HMEMORYRSRC resource) {
    unsigned char* codeBase = ((PMEMORYMODULE)module)->codeBase;
    if (PIMAGE_RESOURCE_DATA_ENTRY entry = (PIMAGE_RESOURCE_DATA_ENTRY)resource) {
        return codeBase + entry->OffsetToData;
    }
    return nullptr;
}