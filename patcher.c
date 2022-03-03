// Based on https://h3adsh0tzz.com/2020/01/macho-file-format

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/loader.h>

int getversioncode(int x, int y, int z) {
    return (x << 16) + ((y & 0xff) << 8) + (z & 0xff);
}

int main(int argc, char** argv) {
    char *buffer;

    if (argc == 1) {
        printf("Usage: %s /path/to/ngrok\n", argv[0]);
        exit(1);
    }

    char *raddr;
    
    FILE *ptr;
    ptr = fopen(argv[1], "rb");
    if (ptr == NULL) {
        printf("Error reading file %s: %s\n", argv[1], strerror(errno));
        exit(-1);
    }

    fseek(ptr, 0, SEEK_END);
    size_t size = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    buffer = malloc(size);
    fread(buffer, size, 1, ptr);
    fclose(ptr);
    printf("Loaded %zu bytes\n", size);

    FILE *ptrout;
    ptrout = fopen(argv[1], "wb");
    if (ptrout == NULL) {
        printf("Error writing file %s: %s\n", argv[1], strerror(errno));
        exit(-1);
    }

    struct mach_header_64* m_header = (struct mach_header_64*) buffer;

    if (m_header->magic != MH_MAGIC_64) {
        printf("Unknown magic number: %x\n", m_header->magic);
        exit(-1);
    }

    uint32_t offset = sizeof(struct mach_header_64);
    raddr = buffer + offset;

int ngrokHack;

    for (int i = 0; i < m_header->ncmds; i++) {
        struct load_command *lc = (struct load_command*) raddr;
        if (lc->cmd == LC_SEGMENT_64) {
            struct segment_command_64 *segCmd = (struct segment_command_64 *)lc;
            if (!strncmp(segCmd->segname, "__DWARF", 7)) {
                if ((segCmd->filesize > segCmd->vmsize) && ((segCmd->vmsize != 0) || ((segCmd->flags & SG_NORELOC) == 0)) ) {
                    // https://github.com/apple-opensource/dyld/blob/1128192c016372ae94793d88530bc5978c1fce93/src/ImageLoaderMachO.cpp#L203
                    printf("Found __DWARF with vmsize < filesize. Patching\n");
                    segCmd->flags |= SG_NORELOC;
                }
            }
        } else if (lc->cmd == LC_RPATH && lc->cmdsize == 32) {
            printf("Found space for LC_BUILD_VERSION, adding the load command\n");

            memset(raddr, 0, 32);

            struct build_version_command *lc_buildver = (struct build_version_command *)lc;
            lc_buildver->cmd = LC_BUILD_VERSION;
            lc_buildver->cmdsize = 32;
            lc_buildver->minos = getversioncode(12, 0, 0);
            lc_buildver->ntools = 1;
            lc_buildver->platform = 0x2; // set to iOS
            lc_buildver->sdk = getversioncode(14, 0, 0);
        }

        raddr += lc->cmdsize;
    }

    fwrite(buffer, size, 1, ptrout);
    fclose(ptrout);
    
    free(buffer);

    return 0;
}

