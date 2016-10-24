#!/bin/bash

function fail()
{
    echo "$@"
    exit 1
}

LCPROG="$(mktemp)"

gcc -o "$LCPROG" -O2 -x c - <<EOF || fail "can't compile lcprog"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE (1 * 1024 * 1024)

static int is_all_space(char *data)
{
    while (*data && isspace(*data)) data++;
    return !*data;
}

static const char *fdlist[] = {
    "nemu",
    "kernel",
    "game",
    "testcase",
    "lib-common",
};
static const int fdcnt = sizeof(fdlist) / sizeof(fdlist[0]);

int main()
{
    static char buf[MAXLINE];

    int cnt[fdcnt];
    int space[fdcnt];
    int fail = 0;
    int other = 0;
    memset(cnt, 0, sizeof(int) * fdcnt);
    memset(space, 0, sizeof(int) * fdcnt);

    while (fgets(buf, sizeof(buf), stdin)) {
        int len = strlen(buf);
        if (len == 0 || buf[len - 1] != '\n') { fail++; continue; }
        buf[len - 1] = '\0';

        char *fpath, *fdelim; // file path, pathdelimiter
        char *data;

        fpath = strchr(buf, ':');
        if (!fpath) { fail++; continue; }
        *fpath++ = '\0';

        data = strchr(fpath, ':');
        if (!data) { fail++; continue; }
        *data++ = '\0';
        
        fdelim = strchr(fpath, '/');
        if (!fdelim) { continue; }

        //printf("fdir = [%.*s], fpath = [%s], data = [%s], space = %d\n", (int)(fdelim - fpath), fpath, fpath, data, is_all_space(data));

        int fdirlen = fdelim - fpath;
        int i;
        for (i = 0; i < fdcnt; i++) {
            if (strncmp(fpath, fdlist[i], fdirlen) == 0 && fdlist[i][fdirlen] == '\0') {
                cnt[i]++;
                if (is_all_space(data)) space[i]++;
                break;
            }
        }
        if (i >= fdcnt) other++;
    }
    
    printf("%d %d", fail, other);
    int i;
    for (i = 0; i < fdcnt; i++) printf("  %d %d", cnt[i], space[i]);
    printf("\n");
    
    return 0;
}
EOF

ALL_COMMIT="$(git rev-list --reverse --all)"
TOT_COMMIT=$(echo "$ALL_COMMIT" | wc -l)

echo "# line count data generated at `date '+%F %T %s'`"
echo "#"
echo "# data format:"
echo "#   utc-unixtimestamp fail other"
echo "#   nemu-total nemu-space"
echo "#   kernel-total kernel-space"
echo "#   game-total game-space"
echo "#   testcase-total testcase-space"
echo "#   lib-common-total lib-common-space"

echo "#"

CUR_COMMIT=0
for i in $ALL_COMMIT; do
    CUR_COMMIT=$[$CUR_COMMIT+1]
    echo "progress: commit $i $CUR_COMMIT/$TOT_COMMIT" >&2
    echo -n "$(git show -s --format='%at' $i) "
    git grep -IE '' $i | "$LCPROG" || fail "can't run lcprog"
done

rm "$LCPROG"

exit 0
