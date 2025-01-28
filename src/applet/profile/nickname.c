#include "nickname.h"
#include "main.h"

#include <euicc/base64.h>
#include <euicc/es10c.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int applet_main(int argc, char **argv) {
    int ret;
    const char *iccid;
    const char *new_name = NULL;
    char *new_name_decode = NULL;
    int name_bytes_len = 0;

    if (argc < 2) {
        printf("Usage: %s [iccid] [new_name] [name_bytes_size]\n", argv[0]);
        printf("\t[new_name]: optional\n");
        printf("\t[name_bytes_len]: optional, If provided, the nickname will be decoded using base64.\n");
        return -1;
    }

    iccid = argv[1];
    if (argc > 3) {
        new_name_decode = (char *)malloc((size_t)euicc_base64_decode_len(argv[2]));
        if (new_name_decode) {
            int len = euicc_base64_decode((unsigned char *)new_name_decode, argv[2]);
            if (len > 0 && len == atoi(argv[3])) {
                new_name = (const char *)new_name_decode;
                name_bytes_len = len;
            }
        }
        if (!new_name) {
            jprint_error("es10c_set_nickname", "base64 decode new_name failed");
            return -1;
        }
    } else if (argc > 2) {
        new_name = argv[2];
        name_bytes_len = (int)strlen(new_name);
    } else {
        new_name = "";
    }

    if ((ret = es10c_set_nickname(&euicc_ctx, iccid, new_name, name_bytes_len))) {
        const char *reason;
        switch (ret) {
        case 1:
            reason = "iccid not found";
            break;
        default:
            reason = "unknown";
            break;
        }
        jprint_error("es10c_set_nickname", reason);
        return -1;
    }

    free(new_name_decode);

    jprint_success(NULL);

    return 0;
}

struct applet_entry applet_profile_nickname = {
    .name = "nickname",
    .main = applet_main,
};
