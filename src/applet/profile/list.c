#include "list.h"
#include "main.h"

#include <euicc/base64.h>
#include <euicc/es10c.h>
#include <euicc/tostr.h>
#include <lpac/utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *handleProfileNicknameBase64(__attribute__((unused)) cJSON *jprofile, struct es10c_profile_info_list *profile) {
    const char *profileNickname = profile->profileNickname;
    const int profileNicknameLength = profile->profileNicknameLength;
    char *b64Nickname = NULL;
    if (profileNickname) {
        size_t size = (size_t)euicc_base64_encode_len(profileNicknameLength);
        b64Nickname = (char *)malloc(size);
        if (b64Nickname) {
            memset(b64Nickname, 0, size);
            euicc_base64_encode(b64Nickname, (const unsigned char *)profileNickname, profileNicknameLength);
        }
    }
    return b64Nickname;
}

static int applet_main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
    _cleanup_es10c_profile_info_list_ struct es10c_profile_info_list *profiles;
    struct es10c_profile_info_list *rptr;
    cJSON *jdata = NULL;

    if (es10c_get_profiles_info(&euicc_ctx, &profiles)) {
        jprint_error("es10c_get_profiles_info", NULL);
        return -1;
    }

    jdata = cJSON_CreateArray();
    rptr = profiles;

    while (rptr) {
        cJSON *jprofile = NULL;
        char *b64Nickname = handleProfileNicknameBase64(jprofile, rptr);

        jprofile = cJSON_CreateObject();
        cJSON_AddStringOrNullToObject(jprofile, "iccid", rptr->iccid);
        cJSON_AddStringOrNullToObject(jprofile, "isdpAid", rptr->isdpAid);
        cJSON_AddStringOrNullToObject(jprofile, "profileState", euicc_profilestate2str(rptr->profileState));
        cJSON_AddStringOrNullToObject(jprofile, "profileNickname", rptr->profileNickname);
        cJSON_AddStringOrNullToObject(jprofile, "profileNicknameB64", b64Nickname);
        cJSON_AddStringOrNullToObject(jprofile, "serviceProviderName", rptr->serviceProviderName);
        cJSON_AddStringOrNullToObject(jprofile, "profileName", rptr->profileName);
        cJSON_AddStringOrNullToObject(jprofile, "iconType", euicc_icontype2str(rptr->iconType));
        cJSON_AddStringOrNullToObject(jprofile, "icon", rptr->icon);
        cJSON_AddStringOrNullToObject(jprofile, "profileClass", euicc_profileclass2str(rptr->profileClass));
        cJSON_AddItemToArray(jdata, jprofile);

        free(b64Nickname);
        rptr = rptr->next;
    }

    jprint_success(jdata);

    return 0;
}

struct applet_entry applet_profile_list = {
    .name = "list",
    .main = applet_main,
};
