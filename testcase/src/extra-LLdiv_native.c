#include "trap.h"

typedef unsigned long long ULL;
typedef long long LL;

void ULLdivULL(ULL a, ULL b, ULL *qp, ULL *rp)
{
    if (qp) *qp = a / b;
    if (rp) *rp = a % b;
}

void LLdivLL(LL a, LL b, LL *qp, LL *rp)
{
    if (qp) *qp = a / b;
    if (rp) *rp = a % b;
}

LL data[] = {
    0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
    -1, -2, -3, -5, -7, -11, -13, -17, -19, -23, -29,

    20, 30, 50,
    -20, -30, -50,

    0x7ffffffffffffffa, 0x7ffffffffffffffb,
    0x7ffffffffffffffc, 0x7ffffffffffffffd,
    0x7ffffffffffffffe, 0x7fffffffffffffff,
    0x8000000000000000, 0x8000000000000001,
    0x8000000000000002, 0x8000000000000003,
    0x8000000000000004, 0x8000000000000005,

    0x6631a3c5992d428b, 0x3a6ac5ed24cb0563,
    0x2fa099392715ddf4, 0x434c1488ec806d82,
    0xf8a9e478877a4107, 0x17f95918e3c2dad4,
    0x9dc1dadd05fe03c5, 0x4660525725384aa2,
    0x91a4716950c8f9b7, 0x7d85a9cbf5371f84,
    0x0f49b3ddf4f1e168, 0xbae686f31cfb42e0,
    0xe3665b91bc16d67e, 0xd808a3ba85fdacbd,
    0x2865e0d75a01f456, 0x292d75ea6fb0e5e8,
    0x36fd4f379934ed15, 0xff8f2d6ea26d7db4,
    0xf9035c37627d8250, 0x87d18d6b2d3cf3ca,
};

int data_size = sizeof(data) / sizeof(data[0]);

#define ANS_SIZE 211
LL ans[ANS_SIZE];
int ansp = 0;

LL sample[] = {
    0x0, 0x0, /* 0x0 div 0x87d18d6b2d3cf3ca */
    0x0, 0x1, /* 0x1 div 0xf9035c37627d8250 */
    0x0, 0x2, /* 0x2 div 0xff8f2d6ea26d7db4 */
    0x0, 0x3, /* 0x3 div 0x36fd4f379934ed15 */
    0x0, 0x5, /* 0x5 div 0x292d75ea6fb0e5e8 */
    0x0, 0x7, /* 0x7 div 0x2865e0d75a01f456 */
    0x0, 0xb, /* 0xb div 0xd808a3ba85fdacbd */
    0x0, 0xd, /* 0xd div 0xe3665b91bc16d67e */
    0x0, 0x11, /* 0x11 div 0xbae686f31cfb42e0 */
    0x0, 0x13, /* 0x13 div 0xf49b3ddf4f1e168 */
    0x0, 0x17, /* 0x17 div 0x7d85a9cbf5371f84 */
    0x0, 0x1c, /* 0x1c div 0x91a4716950c8f9b7 */
    0x0, 0xfffffffffffffffe, /* 0xfffffffffffffffe div 0x4660525725384aa2 */
    0x0, 0xfffffffffffffffd, /* 0xfffffffffffffffd div 0x9dc1dadd05fe03c5 */
    0x0, 0xfffffffffffffffb, /* 0xfffffffffffffffb div 0x17f95918e3c2dad4 */
    0x0, 0xfffffffffffffff9, /* 0xfffffffffffffff9 div 0xf8a9e478877a4107 */
    0x0, 0xfffffffffffffff5, /* 0xfffffffffffffff5 div 0x434c1488ec806d82 */
    0x0, 0xfffffffffffffff3, /* 0xfffffffffffffff3 div 0x2fa099392715ddf4 */
    0x0, 0xffffffffffffffef, /* 0xffffffffffffffef div 0x3a6ac5ed24cb0563 */
    0x0, 0xffffffffffffffed, /* 0xffffffffffffffed div 0x6631a3c5992d428b */
    0x0, 0xffffffffffffffe9, /* 0xffffffffffffffe9 div 0x8000000000000005 */
    0x0, 0xffffffffffffffe3, /* 0xffffffffffffffe3 div 0x8000000000000004 */
    0x0, 0x14, /* 0x14 div 0x8000000000000003 */
    0x0, 0x1e, /* 0x1e div 0x8000000000000002 */
    0x0, 0x32, /* 0x32 div 0x8000000000000001 */
    0x0, 0xffffffffffffffec, /* 0xffffffffffffffec div 0x8000000000000000 */
    0x0, 0xffffffffffffffe2, /* 0xffffffffffffffe2 div 0x7fffffffffffffff */
    0x0, 0xffffffffffffffce, /* 0xffffffffffffffce div 0x7ffffffffffffffe */
    0x0, 0x7ffffffffffffffa, /* 0x7ffffffffffffffa div 0x7ffffffffffffffd */
    0x0, 0x7ffffffffffffffb, /* 0x7ffffffffffffffb div 0x7ffffffffffffffc */
    0x1, 0x1, /* 0x7ffffffffffffffc div 0x7ffffffffffffffb */
    0x1, 0x3, /* 0x7ffffffffffffffd div 0x7ffffffffffffffa */
    0xfd70a3d70a3d70a4, 0x6, /* 0x7ffffffffffffffe div 0xffffffffffffffce */
    0xfbbbbbbbbbbbbbbc, 0x7, /* 0x7fffffffffffffff div 0xffffffffffffffe2 */
    0x666666666666666, 0xfffffffffffffff8, /* 0x8000000000000000 div 0xffffffffffffffec */
    0xfd70a3d70a3d70a4, 0xfffffffffffffff9, /* 0x8000000000000001 div 0x32 */
    0xfbbbbbbbbbbbbbbc, 0xfffffffffffffffa, /* 0x8000000000000002 div 0x1e */
    0xf99999999999999a, 0xfffffffffffffffb, /* 0x8000000000000003 div 0x14 */
    0x469ee58469ee584, 0xfffffffffffffff8, /* 0x8000000000000004 div 0xffffffffffffffe3 */
    0x590b21642c8590a, 0xffffffffffffffeb, /* 0x8000000000000005 div 0xffffffffffffffe9 */
    0xfa9f1253ea76e18d, 0x2, /* 0x6631a3c5992d428b div 0xffffffffffffffed */
    0xfc904eb5d0a8c373, 0x6, /* 0x3a6ac5ed24cb0563 div 0xffffffffffffffef */
    0xfc561b992460c78b, 0x3, /* 0x2fa099392715ddf4 div 0xfffffffffffffff3 */
    0xf9e1cf967622dec6, 0x4, /* 0x434c1488ec806d82 div 0xfffffffffffffff5 */
    0x10c4d135a5c3fda, 0xfffffffffffffffd, /* 0xf8a9e478877a4107 div 0xfffffffffffffff9 */
    0xfb3487c7d272a109, 0x1, /* 0x17f95918e3c2dad4 div 0xfffffffffffffffb */
    0x20bf61b65355febe, 0xffffffffffffffff, /* 0x9dc1dadd05fe03c5 div 0xfffffffffffffffd */
    0xdccfd6d46d63daaf, 0x0, /* 0x4660525725384aa2 div 0xfffffffffffffffe */
    0xfc0f040ce774e459, 0xfffffffffffffffb, /* 0x91a4716950c8f9b7 div 0x1c */
    0x5751da4b19e3905, 0x11, /* 0x7d85a9cbf5371f84 div 0x17 */
    0xcdfbfe35501956, 0x6, /* 0xf49b3ddf4f1e168 div 0x13 */
    0xfbef7159984b03f0, 0xfffffffffffffff0, /* 0xbae686f31cfb42e0 div 0x11 */
    0xfdcccbf7849f4b94, 0xfffffffffffffffa, /* 0xe3665b91bc16d67e div 0xd */
    0xfc5de056c65ce129, 0xfffffffffffffffa, /* 0xd808a3ba85fdacbd div 0xb */
    0x5c569435600477a, 0x0, /* 0x2865e0d75a01f456 div 0x7 */
    0x83c4ac87cbcfac8, 0x0, /* 0x292d75ea6fb0e5e8 div 0x5 */
    0x12546fbd3311a45c, 0x1, /* 0x36fd4f379934ed15 div 0x3 */
    0xffc796b75136beda, 0x0, /* 0xff8f2d6ea26d7db4 div 0x2 */
    0xf9035c37627d8250, 0x0, /* 0xf9035c37627d8250 div 0x1 */
    0x0, 0x0, /* 0x0 div 0x87d18d6b2d3cf3ca */
    0x0, 0x1, /* 0x1 div 0xf9035c37627d8250 */
    0x0, 0x2, /* 0x2 div 0xff8f2d6ea26d7db4 */
    0x0, 0x3, /* 0x3 div 0x36fd4f379934ed15 */
    0x0, 0x5, /* 0x5 div 0x292d75ea6fb0e5e8 */
    0x0, 0x7, /* 0x7 div 0x2865e0d75a01f456 */
    0x0, 0xb, /* 0xb div 0xd808a3ba85fdacbd */
    0x0, 0xd, /* 0xd div 0xe3665b91bc16d67e */
    0x0, 0x11, /* 0x11 div 0xbae686f31cfb42e0 */
    0x0, 0x13, /* 0x13 div 0xf49b3ddf4f1e168 */
    0x0, 0x17, /* 0x17 div 0x7d85a9cbf5371f84 */
    0x0, 0x1c, /* 0x1c div 0x91a4716950c8f9b7 */
    0x3, 0x2cdf08fa90572018, /* 0xfffffffffffffffe div 0x4660525725384aa2 */
    0x1, 0x623e2522fa01fc38, /* 0xfffffffffffffffd div 0x9dc1dadd05fe03c5 */
    0xa, 0x104285071a6373b3, /* 0xfffffffffffffffb div 0x17f95918e3c2dad4 */
    0x1, 0x7561b877885bef2, /* 0xfffffffffffffff9 div 0xf8a9e478877a4107 */
    0x3, 0x361bc2653a7eb76f, /* 0xfffffffffffffff5 div 0x434c1488ec806d82 */
    0x5, 0x11dd01e23c92aa2f, /* 0xfffffffffffffff3 div 0x2fa099392715ddf4 */
    0x4, 0x1654e84b6cd3ea63, /* 0xffffffffffffffef div 0x3a6ac5ed24cb0563 */
    0x2, 0x339cb874cda57ad7, /* 0xffffffffffffffed div 0x6631a3c5992d428b */
    0x1, 0x7fffffffffffffe4, /* 0xffffffffffffffe9 div 0x8000000000000005 */
    0x1, 0x7fffffffffffffdf, /* 0xffffffffffffffe3 div 0x8000000000000004 */
    0x0, 0x14, /* 0x14 div 0x8000000000000003 */
    0x0, 0x1e, /* 0x1e div 0x8000000000000002 */
    0x0, 0x32, /* 0x32 div 0x8000000000000001 */
    0x1, 0x7fffffffffffffec, /* 0xffffffffffffffec div 0x8000000000000000 */
    0x1, 0x7fffffffffffffe3, /* 0xffffffffffffffe2 div 0x7fffffffffffffff */
    0x1, 0x7fffffffffffffd0, /* 0xffffffffffffffce div 0x7ffffffffffffffe */
    0x0, 0x7ffffffffffffffa, /* 0x7ffffffffffffffa div 0x7ffffffffffffffd */
    0x0, 0x7ffffffffffffffb, /* 0x7ffffffffffffffb div 0x7ffffffffffffffc */
    0x1, 0x1, /* 0x7ffffffffffffffc div 0x7ffffffffffffffb */
    0x1, 0x3, /* 0x7ffffffffffffffd div 0x7ffffffffffffffa */
    0x0, 0x7ffffffffffffffe, /* 0x7ffffffffffffffe div 0xffffffffffffffce */
    0x0, 0x7fffffffffffffff, /* 0x7fffffffffffffff div 0xffffffffffffffe2 */
    0x0, 0x8000000000000000, /* 0x8000000000000000 div 0xffffffffffffffec */
    0x28f5c28f5c28f5c, 0x9, /* 0x8000000000000001 div 0x32 */
    0x444444444444444, 0xa, /* 0x8000000000000002 div 0x1e */
    0x666666666666666, 0xb, /* 0x8000000000000003 div 0x14 */
    0x0, 0x8000000000000004, /* 0x8000000000000004 div 0xffffffffffffffe3 */
    0x0, 0x8000000000000005, /* 0x8000000000000005 div 0xffffffffffffffe9 */
    0x0, 0x6631a3c5992d428b, /* 0x6631a3c5992d428b div 0xffffffffffffffed */
    0x0, 0x3a6ac5ed24cb0563, /* 0x3a6ac5ed24cb0563 div 0xffffffffffffffef */
    0x0, 0x2fa099392715ddf4, /* 0x2fa099392715ddf4 div 0xfffffffffffffff3 */
    0x0, 0x434c1488ec806d82, /* 0x434c1488ec806d82 div 0xfffffffffffffff5 */
    0x0, 0xf8a9e478877a4107, /* 0xf8a9e478877a4107 div 0xfffffffffffffff9 */
    0x0, 0x17f95918e3c2dad4, /* 0x17f95918e3c2dad4 div 0xfffffffffffffffb */
    0x0, 0x9dc1dadd05fe03c5, /* 0x9dc1dadd05fe03c5 div 0xfffffffffffffffd */
    0x0, 0x4660525725384aa2, /* 0x4660525725384aa2 div 0xfffffffffffffffe */
    0x53396560c072d7d, 0xb, /* 0x91a4716950c8f9b7 div 0x1c */
    0x5751da4b19e3905, 0x11, /* 0x7d85a9cbf5371f84 div 0x17 */
    0xcdfbfe35501956, 0x6, /* 0xf49b3ddf4f1e168 div 0x13 */
    0xafe8068a75a12fe, 0x2, /* 0xbae686f31cfb42e0 div 0x11 */
    0x117e070b35da5f44, 0xa, /* 0xe3665b91bc16d67e div 0xd */
    0x13a3b1cb237426f9, 0xa, /* 0xd808a3ba85fdacbd div 0xb */
    0x5c569435600477a, 0x0, /* 0x2865e0d75a01f456 div 0x7 */
    0x83c4ac87cbcfac8, 0x0, /* 0x292d75ea6fb0e5e8 div 0x5 */
    0x12546fbd3311a45c, 0x1, /* 0x36fd4f379934ed15 div 0x3 */
    0x7fc796b75136beda, 0x0, /* 0xff8f2d6ea26d7db4 div 0x2 */
    0xf9035c37627d8250, 0x0, /* 0xf9035c37627d8250 div 0x1 */
};
int samplep = 0;

LL right[] = {
    0x53ffe0312b9b29be, 0x6f3a48198da0fe24, 
    0xa7ca6f5e6f1721b, 0x83de531ef263bd61, 
    0x7dc393cc008aefa7, 0x267fcda84807fb0f, 
    0xd499b7b7edefbbe, 0x94303bc0b7d77aef, 
    0xd456b029c88de525, 0x2ec3f81f334a489c, 
    0xc043c59d800b27f6, 0x13716da395e74d3c, 
    0x9b9f5f3fba9bbb28, 0x39615cdd1f5f5182, 
    0xcff1dc5fe5a17c27, 0xd14717c1d419a10b, 
    0xd09eba0b132b47e6, 0xe95164dcaebf0ef0, 
    0x9326a4ebebc17dc, 0x6506de61ed7b67fc, 
    0x6229b45ae9cbf1aa, 0x9526c33f4a0aeff, 
    0x5d1069b73e890267, 0x31b22a8eea55a56, 
    0xbbdab9cefb510fe2, 0x5943692266608ab1, 
    0x407575022e71bab7, 0x8e69ddbf1459dd46, 
    0x2aa6b59c3b570807, 0x476cc2b63b8b326d, 
    0xfd36957930c1d126, 0xf9cbe8526ec0da98, 
    0x83fd3128f62e8a90, 0x8a2f98cab26be86c, 
    0x2f1fc760710fe74b, 0x6339bf69ea58ff17, 
    0x60aeaa299d7f35b2, 0x1464d00e6ef3bec4, 
    0x692ff507ccbcfb0c, 0xa5ade7f961873277, 
    0x60b9605a3ef7c9f2, 0x3883274908fe9ad0, 
    0xc52912c7f544bc15, 0x9a883b141e251075, 
    0x7d8de3ff77e693c, 0x1e7f4464e325f278, 
    0x5075bdde498a1852, 0xa893602942eacd56, 
    0xe9e9c4530346a9d9, 0x3ebcd4051a34e4af, 
    0x3c01201dfb774e8f, 0x6544a8890e8199e4, 
    0x2ea89c42a9a314d3, 0xd1b96406f7d52895, 
    0x7f19c2e1dffcf543, 0x1afe4c8de4faf85d, 
    0x7b8ac8f6e8366133, 0xf3c3251bff62342f, 
    0xb0cfa0040bcf2d0d, 0xa399da1aa81ebfb7, 
    0xf92bdb6815754835, 0x4115297beec54298, 
    0xb6e9f402c0a1f2f9, 0x7ae08b4ba66c133e, 
    0x9bbb8f3ca87015f1, 0xe77629391e38f9ad, 
    0x4254dd664a2b7028, 0x6afd1080ec9e42b, 
    0xfedebd6bb2569cc4, 0xc49a8d55c1bfb2e3, 
    0x5568fbe159b7549, 0xd75c39e571955ecb, 
    0x59d3458d3c5eb39b, 0x98eb07573517d0fd, 
    0x39b58bcc164ed1f2, 0x28b54287f70afc5, 
    0x50f14f63b522fcbd, 0x678a17d93550daa1, 
    0x114905facd386dc, 0xc96bc4174b2aabde, 
    0x95986730e50dbb6d, 0x5f8cc81b1cf86645, 
    0x4203bc9dd7fff7d2, 0xe479abc1b4336d8c, 
    0xab958809773577a9, 0xfb8bdbdca1bf6c0d, 
    0xb342b88a09933c66, 0x29b1eac21b995f65, 
    0x7385bafd2a81776, 0x2a0fbb0b5cf9b5bc, 
    0xb1b2ee04502ab956, 0x5f8c6abf1760124a, 
    0x462ac48497f3c45e, 0x9371b4f9b4955754, 
    0xffc71663d156a454, 0x6dd3bcfbbdfd4ac1, 
    0xc1286eff95a9cdde, 0x7c8f0ffbd38accfc, 
    0xcd9f76ee0bb0bdfb, 0x12a334c484cf75e1, 
    0x3fb4d9f5fefcb1cd, 0x1669e1fed5e5edfc, 
    0xd890382256ca588a, 0x99d766ee511eb8fa, 
    0x20c2049d5a33ee31, 0xfee6aef2f8ce0bb7, 
    0xee23f553076b8037, 0x6db098ce1819ef57, 
    0x8f42e40f2b343348, 0x473bf0259903903e, 
    0xd2bb7af668e8fac8, 0x793ef45520652a7d, 
    0xc14a512c0852d061, 0xb2dbb9d2207462d4, 
    0xf8d399f663ac000b, 0x2a2efd4deae43779, 
    0x52ae76092f3d7a2c, 0xa6effc2fd37b48f9, 
    0xae8cce269b47353, 0x1115f5808c647fb7, 
    0x945ef49ec971b1c8, 0x53b6b9d1766b4d63, 
    0xd1fe9597375e425, 0x23ef8140038eb303, 
    0x6785af412eab3755, 0xc2649589bc5f79de, 
    0xe6665ce77135b236, 0x44e6c678f51df771, 
    0xcc3ea4aaadae4af8, 0x37b1dc128455c036, 
    0x8d5d5405d03a2396, 0xb61a969151c68b26, 
    0xc7ae0ec74f62eba0, 0xaf3ee0feef59cc6d, 
    0xbc2e45e9ea537200, 0x5b6b7ff85610bdd8, 
    0xd5877161f96e172d, 0x832fd65693245201, 
    0xeae5c6cb159ecb62, 0x7d41e9d95063173d, 
    0xd13196d5e0162609, 0x7ceaaf3a36ae4a85, 
    0xf4e19eb4472c78fc, 0x822720af0c942eb9, 
    0x35fdd3b85fc0c86e, 0xabca831a04941ee4, 
    0xaa6c8dc21f7522b3, 0x4a88081de118709b, 
    0x49c0d0718cc5f462, 0x956362dfffcc18a3, 
    0x98c6459422175710, 0xfe08fabcdd0a3aae, 
    0xd6d3c3da4faed081, 0x689d1fdbec9a132d, 
    0x8af63c7aef0818c0, 0xe6d95eec051041b7, 
    0x2f55ec23acea736e, 0x3a6565a871d95604, 
    0x13793db1422ad86f, 0xb551e2912efb6ed8, 
    0x529d505a2ed5bc28, 0xe19dcdb08d7c7546, 
    0x2bb33f94eef47655, 0x7ac03333e0d5d5fa, 
    0x7659993a2da7514e, 0x4e4e9e18bbce3eef, 
    0xadbffbd89db8a042, 0xcd93eaa13c39b524, 
    0xcbb4820b69b63267, 0x483b281c0ae5f8bf, 
    0xfb571111f37fbc78, 0xd4afffdbf9ce81dc, 
    0x4daa1789a924a63a, 0xc1019a8615158450, 
    0x7c5b9593335489f4, 0x7b027f4c4b7c035d, 
    0x59cc6264ea37a261, 0xfdeb76da55f7807a, 
    0xd765e8efa0f638fc, 0xe575d32da39b1c99, 
    0xaf1d493d1eec2962, 0xfba349966b4e3e95, 
    0x6db49ae38da1ff00, 0x72243f49579f3f12, 
    0x79a4a511b7f660d7, 0x741a943bdcb4d22e, 
    0xa240fb5dd5e18cee, 0x7e655ba315b34331, 
    0xde654436b21d5e44, 0x614dbb61a6bfd6b1, 
    0xed7a948b9e603af2, 0x97e254c8e1c4d4e, 
    0x4d1eafb3a4dc10f3, 0xdf348f2fb1143433, 
    0x7d6223f4fc33ee44, 0xfa723fde0fe01973, 
    0xf4b45461360166fd, 0x393193326f1f63b7, 
    0xb15a53bc5437bc01, 0xf636147c5e2895d1, 
    0xe5c6c1bdc762f895, 0x363e4f69bfe7a297, 
    0x3d46a314459fa30e, 0xf7d3203f6011ceb4, 
    0x19ceabb818a07e52, 0xb35bf241079a32a, 
    0x1a095f9e9dcae581, 0x4b8cf6f8a7194339, 
    0x5460ef81da9cbb6b, 0x94db0be9de4d6c9d, 
    0x6e5fa41386aaf73e, 
};

#define MAKE_TEST_FUNC(TYPE, FMT) \
void test_ ## TYPE() \
{ \
    TYPE a, b; \
    TYPE q, r; \
    TYPE sample_q, sample_r; \
    int i, j; \
    for (i = 0; i < data_size; i++) { \
        j = data_size - i - 1; \
        a = data[i]; \
        b = data[j]; \
        if (b == 0) continue; \
        TYPE ## div ## TYPE (a, b, &q, &r); \
        sample_q = sample[samplep++]; \
        sample_r = sample[samplep++]; \
        /*printf("0x%llx, 0x%llx, /" "* 0x%llx div 0x%llx *" "/\n", q, r, a, b);*/ \
        nemu_assert(sample_q == q); \
        nemu_assert(sample_r == r); \
    } \
    for (i = 0; i < data_size; i++) \
        for (j = 0; j < data_size; j++) { \
            a = data[i]; \
            b = data[j]; \
            if (b == 0) continue; \
            TYPE ## div ## TYPE (a, b, &q, &r); \
            ans[ansp++] ^= q; \
            ans[ansp++] ^= r; \
            ansp %= ANS_SIZE; \
        } \
}

MAKE_TEST_FUNC(LL, "%lld")
MAKE_TEST_FUNC(ULL, "%llu")

int naive_memcmp(void *p1, void *p2, int n)
{
    unsigned char *s1 = p1, *s2 = p2;
    int i;
    for (i = 0; i < n; i++)
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
    return 0;
}

int main()
{
    test_LL();
    test_ULL();
    /*int i;
    for (i = 0; i < ANS_SIZE; i++) {
        printf("0x%llx, ", ans[i]);
        if (i % 2) printf("\n");
    }*/
    nemu_assert(sizeof(ans) == sizeof(right));
    nemu_assert(naive_memcmp(ans, right, sizeof(ans)) == 0);
    HIT_GOOD_TRAP;
    return 0;
}

