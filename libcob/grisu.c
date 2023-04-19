
#include <assert.h>
#include <string.h>
#include <math.h>
#include "grisu.h"


typedef struct diy_fp_t {
    unsigned long long f;
    int e;
} diy_fp_t;

#define D_1_LOG2_10 0.30102999566398114 //  1 / lg(10)

static int k_comp(int e, int alpha, int gamma) {
    return ceil((alpha - e + 63) * D_1_LOG2_10);
}

static diy_fp_t minus(diy_fp_t x, diy_fp_t y) {
    diy_fp_t r = { x.f - y.f,  x.e };
    return r;
}

/*
static diy_fp_t minus(diy_fp_t x, diy_fp_t y) {
  assert(x.e == y.e);
  assert(x.f >= y.f);
  diy_fp_t r = {.f = x.f - y.f, .e = x.e};
  return r;
}
*/

static diy_fp_t multiply(diy_fp_t x, diy_fp_t y) {
    unsigned long long a, b, c, d, ac, bc, ad, bd, tmp;
    diy_fp_t r; unsigned long long M32 = 0xFFFFFFFF;
    a = x.f >> 32; b = x.f & M32;
    c = y.f >> 32; d = y.f & M32;
    ac = a * c; bc = b * c; ad = a * d; bd = b * d;
    tmp = (bd >> 32) + (ad & M32) + (bc & M32);
    tmp += 1U << 31; /// mult_round
    r.f = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
    r.e = x.e + y.e + 64;
    return r;
}

#define DIY_SIGNIFICAND_SIZE 64
static const unsigned long long powers_ten[] = { 0xbf29dcaba82fdeaeLL, 0xeef453d6923bd65aLL, 0x9558b4661b6565f8LL, 0xbaaee17fa23ebf76LL, 0xe95a99df8ace6f54LL, 0x91d8a02bb6c10594LL, 0xb64ec836a47146faLL, 0xe3e27a444d8d98b8LL, 0x8e6d8c6ab0787f73LL, 0xb208ef855c969f50LL, 0xde8b2b66b3bc4724LL, 0x8b16fb203055ac76LL, 0xaddcb9e83c6b1794LL, 0xd953e8624b85dd79LL, 0x87d4713d6f33aa6cLL, 0xa9c98d8ccb009506LL, 0xd43bf0effdc0ba48LL, 0x84a57695fe98746dLL, 0xa5ced43b7e3e9188LL, 0xcf42894a5dce35eaLL, 0x818995ce7aa0e1b2LL, 0xa1ebfb4219491a1fLL, 0xca66fa129f9b60a7LL, 0xfd00b897478238d1LL, 0x9e20735e8cb16382LL, 0xc5a890362fddbc63LL, 0xf712b443bbd52b7cLL, 0x9a6bb0aa55653b2dLL, 0xc1069cd4eabe89f9LL, 0xf148440a256e2c77LL, 0x96cd2a865764dbcaLL, 0xbc807527ed3e12bdLL, 0xeba09271e88d976cLL, 0x93445b8731587ea3LL, 0xb8157268fdae9e4cLL, 0xe61acf033d1a45dfLL, 0x8fd0c16206306bacLL, 0xb3c4f1ba87bc8697LL, 0xe0b62e2929aba83cLL, 0x8c71dcd9ba0b4926LL, 0xaf8e5410288e1b6fLL, 0xdb71e91432b1a24bLL, 0x892731ac9faf056fLL, 0xab70fe17c79ac6caLL, 0xd64d3d9db981787dLL, 0x85f0468293f0eb4eLL, 0xa76c582338ed2622LL, 0xd1476e2c07286faaLL, 0x82cca4db847945caLL, 0xa37fce126597973dLL, 0xcc5fc196fefd7d0cLL, 0xff77b1fcbebcdc4fLL, 0x9faacf3df73609b1LL, 0xc795830d75038c1eLL, 0xf97ae3d0d2446f25LL, 0x9becce62836ac577LL, 0xc2e801fb244576d5LL, 0xf3a20279ed56d48aLL, 0x9845418c345644d7LL, 0xbe5691ef416bd60cLL, 0xedec366b11c6cb8fLL, 0x94b3a202eb1c3f39LL, 0xb9e08a83a5e34f08LL, 0xe858ad248f5c22caLL, 0x91376c36d99995beLL, 0xb58547448ffffb2eLL, 0xe2e69915b3fff9f9LL, 0x8dd01fad907ffc3cLL, 0xb1442798f49ffb4bLL, 0xdd95317f31c7fa1dLL, 0x8a7d3eef7f1cfc52LL, 0xad1c8eab5ee43b67LL, 0xd863b256369d4a41LL, 0x873e4f75e2224e68LL, 0xa90de3535aaae202LL, 0xd3515c2831559a83LL, 0x8412d9991ed58092LL, 0xa5178fff668ae0b6LL, 0xce5d73ff402d98e4LL, 0x80fa687f881c7f8eLL, 0xa139029f6a239f72LL, 0xc987434744ac874fLL, 0xfbe9141915d7a922LL, 0x9d71ac8fada6c9b5LL, 0xc4ce17b399107c23LL, 0xf6019da07f549b2bLL, 0x99c102844f94e0fbLL, 0xc0314325637a193aLL, 0xf03d93eebc589f88LL, 0x96267c7535b763b5LL, 0xbbb01b9283253ca3LL, 0xea9c227723ee8bcbLL, 0x92a1958a7675175fLL, 0xb749faed14125d37LL, 0xe51c79a85916f485LL, 0x8f31cc0937ae58d3LL, 0xb2fe3f0b8599ef08LL, 0xdfbdcece67006ac9LL, 0x8bd6a141006042beLL, 0xaecc49914078536dLL, 0xda7f5bf590966849LL, 0x888f99797a5e012dLL, 0xaab37fd7d8f58179LL, 0xd5605fcdcf32e1d7LL, 0x855c3be0a17fcd26LL, 0xa6b34ad8c9dfc070LL, 0xd0601d8efc57b08cLL, 0x823c12795db6ce57LL, 0xa2cb1717b52481edLL, 0xcb7ddcdda26da269LL, 0xfe5d54150b090b03LL, 0x9efa548d26e5a6e2LL, 0xc6b8e9b0709f109aLL, 0xf867241c8cc6d4c1LL, 0x9b407691d7fc44f8LL, 0xc21094364dfb5637LL, 0xf294b943e17a2bc4LL, 0x979cf3ca6cec5b5bLL, 0xbd8430bd08277231LL, 0xece53cec4a314ebeLL, 0x940f4613ae5ed137LL, 0xb913179899f68584LL, 0xe757dd7ec07426e5LL, 0x9096ea6f3848984fLL, 0xb4bca50b065abe63LL, 0xe1ebce4dc7f16dfcLL, 0x8d3360f09cf6e4bdLL, 0xb080392cc4349dedLL, 0xdca04777f541c568LL, 0x89e42caaf9491b61LL, 0xac5d37d5b79b6239LL, 0xd77485cb25823ac7LL, 0x86a8d39ef77164bdLL, 0xa8530886b54dbdecLL, 0xd267caa862a12d67LL, 0x8380dea93da4bc60LL, 0xa46116538d0deb78LL, 0xcd795be870516656LL, 0x806bd9714632dff6LL, 0xa086cfcd97bf97f4LL, 0xc8a883c0fdaf7df0LL, 0xfad2a4b13d1b5d6cLL, 0x9cc3a6eec6311a64LL, 0xc3f490aa77bd60fdLL, 0xf4f1b4d515acb93cLL, 0x991711052d8bf3c5LL, 0xbf5cd54678eef0b7LL, 0xef340a98172aace5LL, 0x9580869f0e7aac0fLL, 0xbae0a846d2195713LL, 0xe998d258869facd7LL, 0x91ff83775423cc06LL, 0xb67f6455292cbf08LL, 0xe41f3d6a7377eecaLL, 0x8e938662882af53eLL, 0xb23867fb2a35b28eLL, 0xdec681f9f4c31f31LL, 0x8b3c113c38f9f37fLL, 0xae0b158b4738705fLL, 0xd98ddaee19068c76LL, 0x87f8a8d4cfa417caLL, 0xa9f6d30a038d1dbcLL, 0xd47487cc8470652bLL, 0x84c8d4dfd2c63f3bLL, 0xa5fb0a17c777cf0aLL, 0xcf79cc9db955c2ccLL, 0x81ac1fe293d599c0LL, 0xa21727db38cb0030LL, 0xca9cf1d206fdc03cLL, 0xfd442e4688bd304bLL, 0x9e4a9cec15763e2fLL, 0xc5dd44271ad3cdbaLL, 0xf7549530e188c129LL, 0x9a94dd3e8cf578baLL, 0xc13a148e3032d6e8LL, 0xf18899b1bc3f8ca2LL, 0x96f5600f15a7b7e5LL, 0xbcb2b812db11a5deLL, 0xebdf661791d60f56LL, 0x936b9fcebb25c996LL, 0xb84687c269ef3bfbLL, 0xe65829b3046b0afaLL, 0x8ff71a0fe2c2e6dcLL, 0xb3f4e093db73a093LL, 0xe0f218b8d25088b8LL, 0x8c974f7383725573LL, 0xafbd2350644eead0LL, 0xdbac6c247d62a584LL, 0x894bc396ce5da772LL, 0xab9eb47c81f5114fLL, 0xd686619ba27255a3LL, 0x8613fd0145877586LL, 0xa798fc4196e952e7LL, 0xd17f3b51fca3a7a1LL, 0x82ef85133de648c5LL, 0xa3ab66580d5fdaf6LL, 0xcc963fee10b7d1b3LL, 0xffbbcfe994e5c620LL, 0x9fd561f1fd0f9bd4LL, 0xc7caba6e7c5382c9LL, 0xf9bd690a1b68637bLL, 0x9c1661a651213e2dLL, 0xc31bfa0fe5698db8LL, 0xf3e2f893dec3f126LL, 0x986ddb5c6b3a76b8LL, 0xbe89523386091466LL, 0xee2ba6c0678b597fLL, 0x94db483840b717f0LL, 0xba121a4650e4ddecLL, 0xe896a0d7e51e1566LL, 0x915e2486ef32cd60LL, 0xb5b5ada8aaff80b8LL, 0xe3231912d5bf60e6LL, 0x8df5efabc5979c90LL, 0xb1736b96b6fd83b4LL, 0xddd0467c64bce4a1LL, 0x8aa22c0dbef60ee4LL, 0xad4ab7112eb3929eLL, 0xd89d64d57a607745LL, 0x87625f056c7c4a8bLL, 0xa93af6c6c79b5d2eLL, 0xd389b47879823479LL, 0x843610cb4bf160ccLL, 0xa54394fe1eedb8ffLL, 0xce947a3da6a9273eLL, 0x811ccc668829b887LL, 0xa163ff802a3426a9LL, 0xc9bcff6034c13053LL, 0xfc2c3f3841f17c68LL, 0x9d9ba7832936edc1LL, 0xc5029163f384a931LL, 0xf64335bcf065d37dLL, 0x99ea0196163fa42eLL, 0xc06481fb9bcf8d3aLL, 0xf07da27a82c37088LL, 0x964e858c91ba2655LL, 0xbbe226efb628afebLL, 0xeadab0aba3b2dbe5LL, 0x92c8ae6b464fc96fLL, 0xb77ada0617e3bbcbLL, 0xe55990879ddcaabeLL, 0x8f57fa54c2a9eab7LL, 0xb32df8e9f3546564LL, 0xdff9772470297ebdLL, 0x8bfbea76c619ef36LL, 0xaefae51477a06b04LL, 0xdab99e59958885c5LL, 0x88b402f7fd75539bLL, 0xaae103b5fcd2a882LL, 0xd59944a37c0752a2LL, 0x857fcae62d8493a5LL, 0xa6dfbd9fb8e5b88fLL, 0xd097ad07a71f26b2LL, 0x825ecc24c8737830LL, 0xa2f67f2dfa90563bLL, 0xcbb41ef979346bcaLL, 0xfea126b7d78186bdLL, 0x9f24b832e6b0f436LL, 0xc6ede63fa05d3144LL, 0xf8a95fcf88747d94LL, 0x9b69dbe1b548ce7dLL, 0xc24452da229b021cLL, 0xf2d56790ab41c2a3LL, 0x97c560ba6b0919a6LL, 0xbdb6b8e905cb600fLL, 0xed246723473e3813LL, 0x9436c0760c86e30cLL, 0xb94470938fa89bcfLL, 0xe7958cb87392c2c3LL, 0x90bd77f3483bb9baLL, 0xb4ecd5f01a4aa828LL, 0xe2280b6c20dd5232LL, 0x8d590723948a535fLL, 0xb0af48ec79ace837LL, 0xdcdb1b2798182245LL, 0x8a08f0f8bf0f156bLL, 0xac8b2d36eed2dac6LL, 0xd7adf884aa879177LL, 0x86ccbb52ea94baebLL, 0xa87fea27a539e9a5LL, 0xd29fe4b18e88640fLL, 0x83a3eeeef9153e89LL, 0xa48ceaaab75a8e2bLL, 0xcdb02555653131b6LL, 0x808e17555f3ebf12LL, 0xa0b19d2ab70e6ed6LL, 0xc8de047564d20a8cLL, 0xfb158592be068d2fLL, 0x9ced737bb6c4183dLL, 0xc428d05aa4751e4dLL, 0xf53304714d9265e0LL, 0x993fe2c6d07b7facLL, 0xbf8fdb78849a5f97LL, 0xef73d256a5c0f77dLL, 0x95a8637627989aaeLL, 0xbb127c53b17ec159LL, 0xe9d71b689dde71b0LL, 0x9226712162ab070eLL, 0xb6b00d69bb55c8d1LL, 0xe45c10c42a2b3b06LL, 0x8eb98a7a9a5b04e3LL, 0xb267ed1940f1c61cLL, 0xdf01e85f912e37a3LL, 0x8b61313bbabce2c6LL, 0xae397d8aa96c1b78LL, 0xd9c7dced53c72256LL, 0x881cea14545c7575LL, 0xaa242499697392d3LL, 0xd4ad2dbfc3d07788LL, 0x84ec3c97da624ab5LL, 0xa6274bbdd0fadd62LL, 0xcfb11ead453994baLL, 0x81ceb32c4b43fcf5LL, 0xa2425ff75e14fc32LL, 0xcad2f7f5359a3b3eLL, 0xfd87b5f28300ca0eLL, 0x9e74d1b791e07e48LL, 0xc612062576589ddbLL, 0xf79687aed3eec551LL, 0x9abe14cd44753b53LL, 0xc16d9a0095928a27LL, 0xf1c90080baf72cb1LL, 0x971da05074da7befLL, 0xbce5086492111aebLL, 0xec1e4a7db69561a5LL, 0x9392ee8e921d5d07LL, 0xb877aa3236a4b449LL, 0xe69594bec44de15bLL, 0x901d7cf73ab0acd9LL, 0xb424dc35095cd80fLL, 0xe12e13424bb40e13LL, 0x8cbccc096f5088ccLL, 0xafebff0bcb24aaffLL, 0xdbe6fecebdedd5bfLL, 0x89705f4136b4a597LL, 0xabcc77118461cefdLL, 0xd6bf94d5e57a42bcLL, 0x8637bd05af6c69b6LL, 0xa7c5ac471b478423LL, 0xd1b71758e219652cLL, 0x83126e978d4fdf3bLL, 0xa3d70a3d70a3d70aLL, 0xcccccccccccccccdLL, 0x8000000000000000LL, 0xa000000000000000LL, 0xc800000000000000LL, 0xfa00000000000000LL, 0x9c40000000000000LL, 0xc350000000000000LL, 0xf424000000000000LL, 0x9896800000000000LL, 0xbebc200000000000LL, 0xee6b280000000000LL, 0x9502f90000000000LL, 0xba43b74000000000LL, 0xe8d4a51000000000LL, 0x9184e72a00000000LL, 0xb5e620f480000000LL, 0xe35fa931a0000000LL, 0x8e1bc9bf04000000LL, 0xb1a2bc2ec5000000LL, 0xde0b6b3a76400000LL, 0x8ac7230489e80000LL, 0xad78ebc5ac620000LL, 0xd8d726b7177a8000LL, 0x878678326eac9000LL, 0xa968163f0a57b400LL, 0xd3c21bcecceda100LL, 0x84595161401484a0LL, 0xa56fa5b99019a5c8LL, 0xcecb8f27f4200f3aLL, 0x813f3978f8940984LL, 0xa18f07d736b90be5LL, 0xc9f2c9cd04674edfLL, 0xfc6f7c4045812296LL, 0x9dc5ada82b70b59eLL, 0xc5371912364ce305LL, 0xf684df56c3e01bc7LL, 0x9a130b963a6c115cLL, 0xc097ce7bc90715b3LL, 0xf0bdc21abb48db20LL, 0x96769950b50d88f4LL, 0xbc143fa4e250eb31LL, 0xeb194f8e1ae525fdLL, 0x92efd1b8d0cf37beLL, 0xb7abc627050305aeLL, 0xe596b7b0c643c719LL, 0x8f7e32ce7bea5c70LL, 0xb35dbf821ae4f38cLL, 0xe0352f62a19e306fLL, 0x8c213d9da502de45LL, 0xaf298d050e4395d7LL, 0xdaf3f04651d47b4cLL, 0x88d8762bf324cd10LL, 0xab0e93b6efee0054LL, 0xd5d238a4abe98068LL, 0x85a36366eb71f041LL, 0xa70c3c40a64e6c52LL, 0xd0cf4b50cfe20766LL, 0x82818f1281ed44a0LL, 0xa321f2d7226895c8LL, 0xcbea6f8ceb02bb3aLL, 0xfee50b7025c36a08LL, 0x9f4f2726179a2245LL, 0xc722f0ef9d80aad6LL, 0xf8ebad2b84e0d58cLL, 0x9b934c3b330c8577LL, 0xc2781f49ffcfa6d5LL, 0xf316271c7fc3908bLL, 0x97edd871cfda3a57LL, 0xbde94e8e43d0c8ecLL, 0xed63a231d4c4fb27LL, 0x945e455f24fb1cf9LL, 0xb975d6b6ee39e437LL, 0xe7d34c64a9c85d44LL, 0x90e40fbeea1d3a4bLL, 0xb51d13aea4a488ddLL, 0xe264589a4dcdab15LL, 0x8d7eb76070a08aedLL, 0xb0de65388cc8ada8LL, 0xdd15fe86affad912LL, 0x8a2dbf142dfcc7abLL, 0xacb92ed9397bf996LL, 0xd7e77a8f87daf7fcLL, 0x86f0ac99b4e8dafdLL, 0xa8acd7c0222311bdLL, 0xd2d80db02aabd62cLL, 0x83c7088e1aab65dbLL, 0xa4b8cab1a1563f52LL, 0xcde6fd5e09abcf27LL, 0x80b05e5ac60b6178LL, 0xa0dc75f1778e39d6LL, 0xc913936dd571c84cLL, 0xfb5878494ace3a5fLL, 0x9d174b2dcec0e47bLL, 0xc45d1df942711d9aLL, 0xf5746577930d6501LL, 0x9968bf6abbe85f20LL, 0xbfc2ef456ae276e9LL, 0xefb3ab16c59b14a3LL, 0x95d04aee3b80ece6LL, 0xbb445da9ca61281fLL, 0xea1575143cf97227LL, 0x924d692ca61be758LL, 0xb6e0c377cfa2e12eLL, 0xe498f455c38b997aLL, 0x8edf98b59a373fecLL, 0xb2977ee300c50fe7LL, 0xdf3d5e9bc0f653e1LL, 0x8b865b215899f46dLL, 0xae67f1e9aec07188LL, 0xda01ee641a708deaLL, 0x884134fe908658b2LL, 0xaa51823e34a7eedfLL, 0xd4e5e2cdc1d1ea96LL, 0x850fadc09923329eLL, 0xa6539930bf6bff46LL, 0xcfe87f7cef46ff17LL, 0x81f14fae158c5f6eLL, 0xa26da3999aef774aLL, 0xcb090c8001ab551cLL, 0xfdcb4fa002162a63LL, 0x9e9f11c4014dda7eLL, 0xc646d63501a1511eLL, 0xf7d88bc24209a565LL, 0x9ae757596946075fLL, 0xc1a12d2fc3978937LL, 0xf209787bb47d6b85LL, 0x9745eb4d50ce6333LL, 0xbd176620a501fc00LL, 0xec5d3fa8ce427b00LL, 0x93ba47c980e98ce0LL, 0xb8a8d9bbe123f018LL, 0xe6d3102ad96cec1eLL, 0x9043ea1ac7e41393LL, 0xb454e4a179dd1877LL, 0xe16a1dc9d8545e95LL, 0x8ce2529e2734bb1dLL, 0xb01ae745b101e9e4LL, 0xdc21a1171d42645dLL, 0x899504ae72497ebaLL, 0xabfa45da0edbde69LL, 0xd6f8d7509292d603LL, 0x865b86925b9bc5c2LL, 0xa7f26836f282b733LL, 0xd1ef0244af2364ffLL, 0x8335616aed761f1fLL, 0xa402b9c5a8d3a6e7LL, 0xcd036837130890a1LL, 0x802221226be55a65LL, 0xa02aa96b06deb0feLL, 0xc83553c5c8965d3dLL, 0xfa42a8b73abbf48dLL, 0x9c69a97284b578d8LL, 0xc38413cf25e2d70eLL, 0xf46518c2ef5b8cd1LL, 0x98bf2f79d5993803LL, 0xbeeefb584aff8604LL, 0xeeaaba2e5dbf6785LL, 0x952ab45cfa97a0b3LL, 0xba756174393d88e0LL, 0xe912b9d1478ceb17LL, 0x91abb422ccb812efLL, 0xb616a12b7fe617aaLL, 0xe39c49765fdf9d95LL, 0x8e41ade9fbebc27dLL, 0xb1d219647ae6b31cLL, 0xde469fbd99a05fe3LL, 0x8aec23d680043beeLL, 0xada72ccc20054aeaLL, 0xd910f7ff28069da4LL, 0x87aa9aff79042287LL, 0xa99541bf57452b28LL, 0xd3fa922f2d1675f2LL, 0x847c9b5d7c2e09b7LL, 0xa59bc234db398c25LL, 0xcf02b2c21207ef2fLL, 0x8161afb94b44f57dLL, 0xa1ba1ba79e1632dcLL, 0xca28a291859bbf93LL, 0xfcb2cb35e702af78LL, 0x9defbf01b061adabLL, 0xc56baec21c7a1916LL, 0xf6c69a72a3989f5cLL, 0x9a3c2087a63f6399LL, 0xc0cb28a98fcf3c80LL, 0xf0fdf2d3f3c30b9fLL, 0x969eb7c47859e744LL, 0xbc4665b596706115LL, 0xeb57ff22fc0c795aLL, 0x9316ff75dd87cbd8LL, 0xb7dcbf5354e9beceLL, 0xe5d3ef282a242e82LL, 0x8fa475791a569d11LL, 0xb38d92d760ec4455LL, 0xe070f78d3927556bLL, 0x8c469ab843b89563LL, 0xaf58416654a6babbLL, 0xdb2e51bfe9d0696aLL, 0x88fcf317f22241e2LL, 0xab3c2fddeeaad25bLL, 0xd60b3bd56a5586f2LL, 0x85c7056562757457LL, 0xa738c6bebb12d16dLL, 0xd106f86e69d785c8LL, 0x82a45b450226b39dLL, 0xa34d721642b06084LL, 0xcc20ce9bd35c78a5LL, 0xff290242c83396ceLL, 0x9f79a169bd203e41LL, 0xc75809c42c684dd1LL, 0xf92e0c3537826146LL, 0x9bbcc7a142b17cccLL, 0xc2abf989935ddbfeLL, 0xf356f7ebf83552feLL, 0x98165af37b2153dfLL, 0xbe1bf1b059e9a8d6LL, 0xeda2ee1c7064130cLL, 0x9485d4d1c63e8be8LL, 0xb9a74a0637ce2ee1LL, 0xe8111c87c5c1ba9aLL, 0x910ab1d4db9914a0LL, 0xb54d5e4a127f59c8LL, 0xe2a0b5dc971f303aLL, 0x8da471a9de737e24LL, 0xb10d8e1456105dadLL, 0xdd50f1996b947519LL, 0x8a5296ffe33cc930LL, 0xace73cbfdc0bfb7bLL, 0xd8210befd30efa5aLL, 0x8714a775e3e95c78LL, 0xa8d9d1535ce3b396LL, 0xd31045a8341ca07cLL, 0x83ea2b892091e44eLL, 0xa4e4b66b68b65d61LL, 0xce1de40642e3f4b9LL, 0x80d2ae83e9ce78f4LL, 0xa1075a24e4421731LL, 0xc94930ae1d529cfdLL, 0xfb9b7cd9a4a7443cLL, 0x9d412e0806e88aa6LL, 0xc491798a08a2ad4fLL, 0xf5b5d7ec8acb58a3LL, 0x9991a6f3d6bf1766LL, 0xbff610b0cc6edd3fLL, 0xeff394dcff8a948fLL, 0x95f83d0a1fb69cd9LL, 0xbb764c4ca7a44410LL, 0xea53df5fd18d5514LL, 0x92746b9be2f8552cLL, 0xb7118682dbb66a77LL, 0xe4d5e82392a40515LL, 0x8f05b1163ba6832dLL, 0xb2c71d5bca9023f8LL, 0xdf78e4b2bd342cf7LL, 0x8bab8eefb6409c1aLL, 0xae9672aba3d0c321LL, 0xda3c0f568cc4f3e9LL, 0x8865899617fb1871LL, 0xaa7eebfb9df9de8eLL, 0xd51ea6fa85785631LL, 0x8533285c936b35dfLL, 0xa67ff273b8460357LL, 0xd01fef10a657842cLL, 0x8213f56a67f6b29cLL, 0xa298f2c501f45f43LL, 0xcb3f2f7642717713LL, 0xfe0efb53d30dd4d8LL, 0x9ec95d1463e8a507LL, 0xc67bb4597ce2ce49LL, 0xf81aa16fdc1b81dbLL, 0x9b10a4e5e9913129LL, 0xc1d4ce1f63f57d73LL, 0xf24a01a73cf2dcd0LL, 0x976e41088617ca02LL, 0xbd49d14aa79dbc82LL, 0xec9c459d51852ba3LL, 0x93e1ab8252f33b46LL, 0xb8da1662e7b00a17LL, 0xe7109bfba19c0c9dLL, 0x906a617d450187e2LL, 0xb484f9dc9641e9dbLL, 0xe1a63853bbd26451LL, 0x8d07e33455637eb3LL, 0xb049dc016abc5e60LL, 0xdc5c5301c56b75f7LL, 0x89b9b3e11b6329bbLL, 0xac2820d9623bf429LL, 0xd732290fbacaf134LL, 0x867f59a9d4bed6c0LL, 0xa81f301449ee8c70LL, 0xd226fc195c6a2f8cLL, 0x83585d8fd9c25db8LL, 0xa42e74f3d032f526LL, 0xcd3a1230c43fb26fLL, 0x80444b5e7aa7cf85LL, 0xa0555e361951c367LL, 0xc86ab5c39fa63441LL, 0xfa856334878fc151LL, 0x9c935e00d4b9d8d2LL, 0xc3b8358109e84f07LL, 0xf4a642e14c6262c9LL, 0x98e7e9cccfbd7dbeLL, 0xbf21e44003acdd2dLL, 0xeeea5d5004981478LL, 0x95527a5202df0ccbLL, 0xbaa718e68396cffeLL, 0xe950df20247c83fdLL, 0x91d28b7416cdd27eLL, 0xb6472e511c81471eLL, 0xe3d8f9e563a198e5LL, 0x8e679c2f5e44ff8fLL, 0xb201833b35d63f73LL, 0xde81e40a034bcf50LL, 0x8b112e86420f6192LL, 0xadd57a27d29339f6LL, 0xd94ad8b1c7380874LL, 0x87cec76f1c830549LL, 0xa9c2794ae3a3c69bLL, 0xd433179d9c8cb841LL, 0x849feec281d7f329LL, 0xa5c7ea73224deff3LL, 0xcf39e50feae16bf0LL, 0x81842f29f2cce376LL, 0xa1e53af46f801c53LL, 0xca5e89b18b602368LL, 0xfcf62c1dee382c42LL, 0x9e19db92b4e31ba9LL, 0xc5a05277621be294LL, 0xf70867153aa2db39LL, 0x9a65406d44a5c903LL, 0xc0fe908895cf3b44LL, 0xf13e34aabb430a15LL, 0x96c6e0eab509e64dLL, 0xbc789925624c5fe1LL, 0xeb96bf6ebadf77d9LL, 0x933e37a534cbaae8LL, 0xb80dc58e81fe95a1LL, 0xe61136f2227e3b0aLL, 0x8fcac257558ee4e6LL, 0xb3bd72ed2af29e20LL, 0xe0accfa875af45a8LL, 0x8c6c01c9498d8b89LL, 0xaf87023b9bf0ee6bLL, 0xdb68c2ca82ed2a06LL, 0x892179be91d43a44LL, 0xab69d82e364948d4LL };
static const int powers_ten_e[] = { -1203, -1200, -1196, -1193, -1190, -1186, -1183, -1180, -1176, -1173, -1170, -1166, -1163, -1160, -1156, -1153, -1150, -1146, -1143, -1140, -1136, -1133, -1130, -1127, -1123, -1120, -1117, -1113, -1110, -1107, -1103, -1100, -1097, -1093, -1090, -1087, -1083, -1080, -1077, -1073, -1070, -1067, -1063, -1060, -1057, -1053, -1050, -1047, -1043, -1040, -1037, -1034, -1030, -1027, -1024, -1020, -1017, -1014, -1010, -1007, -1004, -1000, -997, -994, -990, -987, -984, -980, -977, -974, -970, -967, -964, -960, -957, -954, -950, -947, -944, -940, -937, -934, -931, -927, -924, -921, -917, -914, -911, -907, -904, -901, -897, -894, -891, -887, -884, -881, -877, -874, -871, -867, -864, -861, -857, -854, -851, -847, -844, -841, -838, -834, -831, -828, -824, -821, -818, -814, -811, -808, -804, -801, -798, -794, -791, -788, -784, -781, -778, -774, -771, -768, -764, -761, -758, -754, -751, -748, -744, -741, -738, -735, -731, -728, -725, -721, -718, -715, -711, -708, -705, -701, -698, -695, -691, -688, -685, -681, -678, -675, -671, -668, -665, -661, -658, -655, -651, -648, -645, -642, -638, -635, -632, -628, -625, -622, -618, -615, -612, -608, -605, -602, -598, -595, -592, -588, -585, -582, -578, -575, -572, -568, -565, -562, -558, -555, -552, -549, -545, -542, -539, -535, -532, -529, -525, -522, -519, -515, -512, -509, -505, -502, -499, -495, -492, -489, -485, -482, -479, -475, -472, -469, -465, -462, -459, -455, -452, -449, -446, -442, -439, -436, -432, -429, -426, -422, -419, -416, -412, -409, -406, -402, -399, -396, -392, -389, -386, -382, -379, -376, -372, -369, -366, -362, -359, -356, -353, -349, -346, -343, -339, -336, -333, -329, -326, -323, -319, -316, -313, -309, -306, -303, -299, -296, -293, -289, -286, -283, -279, -276, -273, -269, -266, -263, -259, -256, -253, -250, -246, -243, -240, -236, -233, -230, -226, -223, -220, -216, -213, -210, -206, -203, -200, -196, -193, -190, -186, -183, -180, -176, -173, -170, -166, -163, -160, -157, -153, -150, -147, -143, -140, -137, -133, -130, -127, -123, -120, -117, -113, -110, -107, -103, -100, -97, -93, -90, -87, -83, -80, -77, -73, -70, -67, -63, -60, -57, -54, -50, -47, -44, -40, -37, -34, -30, -27, -24, -20, -17, -14, -10, -7, -4, 0, 3, 6, 10, 13, 16, 20, 23, 26, 30, 33, 36, 39, 43, 46, 49, 53, 56, 59, 63, 66, 69, 73, 76, 79, 83, 86, 89, 93, 96, 99, 103, 106, 109, 113, 116, 119, 123, 126, 129, 132, 136, 139, 142, 146, 149, 152, 156, 159, 162, 166, 169, 172, 176, 179, 182, 186, 189, 192, 196, 199, 202, 206, 209, 212, 216, 219, 222, 226, 229, 232, 235, 239, 242, 245, 249, 252, 255, 259, 262, 265, 269, 272, 275, 279, 282, 285, 289, 292, 295, 299, 302, 305, 309, 312, 315, 319, 322, 325, 328, 332, 335, 338, 342, 345, 348, 352, 355, 358, 362, 365, 368, 372, 375, 378, 382, 385, 388, 392, 395, 398, 402, 405, 408, 412, 415, 418, 422, 425, 428, 431, 435, 438, 441, 445, 448, 451, 455, 458, 461, 465, 468, 471, 475, 478, 481, 485, 488, 491, 495, 498, 501, 505, 508, 511, 515, 518, 521, 524, 528, 531, 534, 538, 541, 544, 548, 551, 554, 558, 561, 564, 568, 571, 574, 578, 581, 584, 588, 591, 594, 598, 601, 604, 608, 611, 614, 617, 621, 624, 627, 631, 634, 637, 641, 644, 647, 651, 654, 657, 661, 664, 667, 671, 674, 677, 681, 684, 687, 691, 694, 697, 701, 704, 707, 711, 714, 717, 720, 724, 727, 730, 734, 737, 740, 744, 747, 750, 754, 757, 760, 764, 767, 770, 774, 777, 780, 784, 787, 790, 794, 797, 800, 804, 807, 810, 813, 817, 820, 823, 827, 830, 833, 837, 840, 843, 847, 850, 853, 857, 860, 863, 867, 870, 873, 877, 880, 883, 887, 890, 893, 897, 900, 903, 907, 910, 913, 916, 920, 923, 926, 930, 933, 936, 940, 943, 946, 950, 953, 956, 960, 963, 966, 970, 973, 976, 980, 983, 986, 990, 993, 996, 1000, 1003, 1006, 1009, 1013, 1016, 1019, 1023, 1026, 1029, 1033, 1036, 1039, 1043, 1046, 1049, 1053, 1056, 1059, 1063, 1066, 1069, 1073, 1076 };
static diy_fp_t cached_power(int k) {
    diy_fp_t res;
    int index = 343 + k;
    res.f = powers_ten[index];
    res.e = powers_ten_e[index];
    return res;
}
typedef union {
    double d;
    unsigned long long n;
} converter_t;

static unsigned long long double_to_uint64(double d) { converter_t tmp; tmp.d = d; return tmp.n; }
static double uint64_to_double(unsigned long long d64) { converter_t tmp; tmp.n = d64; return tmp.d; }

#define DP_SIGNIFICAND_SIZE 52
#define DP_EXPONENT_BIAS (0x3FF + DP_SIGNIFICAND_SIZE)
#define DP_MIN_EXPONENT (-DP_EXPONENT_BIAS)
#define DP_EXPONENT_MASK 0x7FF0000000000000
#define DP_SIGNIFICAND_MASK 0x000FFFFFFFFFFFFF
#define DP_HIDDEN_BIT    0x0010000000000000

static diy_fp_t normalize_diy_fp(diy_fp_t in) {
    diy_fp_t res = in;
    /* Normalize now */
    /* the original number could have been a denormal. */
    while ( !(res.f & DP_HIDDEN_BIT) ) {
        res.f <<= 1;
        res.e--;
    }
    /* do the final shifts in one go. Don't forget the hidden bit (the '-1') */
    res.f <<= (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 1);
    res.e = res.e - (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 1);
    return res;
}

static diy_fp_t double2diy_fp(double d) {
    unsigned long long d64 = double_to_uint64(d);
    int biased_e = (d64 & DP_EXPONENT_MASK) >> DP_SIGNIFICAND_SIZE;
    unsigned long long significand = (d64 & DP_SIGNIFICAND_MASK);
    diy_fp_t res;
    if ( biased_e != 0 ) {
        res.f = significand + DP_HIDDEN_BIT;
        res.e = biased_e - DP_EXPONENT_BIAS;
    } else {
        res.f = significand;
        res.e = DP_MIN_EXPONENT + 1;
    }
    return res;
}

static diy_fp_t normalize_boundary(diy_fp_t in) {
    diy_fp_t res = in;
    /* Normalize now */
    /* the original number could have been a denormal. */
    while ( !(res.f & (DP_HIDDEN_BIT << 1)) ) {
        res.f <<= 1;
        res.e--;
    }
    /* do the final shifts in one go. Don't forget the hidden bit (the '-1') */
    res.f <<= (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 2);
    res.e = res.e - (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 2);
    return res;
}

static void normalized_boundaries(double d, diy_fp_t *out_m_minus, diy_fp_t *out_m_plus) {
    diy_fp_t v = double2diy_fp(d);
    diy_fp_t pl, mi;
    int significand_is_zero = v.f == DP_HIDDEN_BIT;
    pl.f  = (v.f << 1) + 1; pl.e  = v.e - 1;
    pl = normalize_boundary(pl);
    if ( significand_is_zero ) {
        mi.f = (v.f << 2) - 1;
        mi.e = v.e - 2;
    } else {
        mi.f = (v.f << 1) - 1;
        mi.e = v.e - 1;
    }
    mi.f <<= mi.e - pl.e;
    mi.e = pl.e;
    *out_m_plus = pl;
    *out_m_minus = mi;
}

#define TEN9 1000000000

static void grisu_round(char *buffer, int len,
                        unsigned long long delta, unsigned long long rest,
                        unsigned long long ten_kappa, unsigned long long wp_w) {
    while ( rest < wp_w &&
            delta - rest >= ten_kappa &&
            (rest + ten_kappa < wp_w || /// closer
             wp_w - rest > rest + ten_kappa - wp_w) ) {
        buffer[len - 1]--; rest += ten_kappa;
    }
}

static void digit_gen(diy_fp_t W, diy_fp_t Mp, diy_fp_t delta,
                      char *buffer, int *len, int *K) {
    unsigned int div; 
    int d, kappa; 
    diy_fp_t one, wp_w;
    unsigned int p1 ;
    unsigned long long p2; 
    unsigned long long unit;
    wp_w = minus(Mp, W);
    one.f = ((unsigned long long)1) << -Mp.e; one.e = Mp.e;
    p1 = Mp.f >> -one.e; /// Mp_cut
    p2 = Mp.f & (one.f - 1);
    *len = 0; kappa = 10; div = TEN9;
    while ( kappa > 0 ) {
        unsigned long long tmp;
        d = p1 / div;
        if ( d || *len ) buffer[(*len)++] = '0' + d; /// Mp_inv1
        p1 %= div; kappa--;
        tmp = (((unsigned long long)p1) << -one.e) + p2;
        if ( tmp <= delta.f ) { /// Mp_delta
            *K += kappa;
            grisu_round(buffer, *len, delta.f, tmp, ((unsigned long long)div) << -one.e, wp_w.f);
            return;
        }
        div /= 10;
    }
    unit = 1;
    while ( 1 ) {
        p2 *= 10; delta.f *= 10; unit *= 10;
        d = p2 >> -one.e;
        if ( d || *len ) buffer[(*len)++] = '0' + d;
        p2 &= one.f - 1; kappa--;
        if ( p2 < delta.f ) {
            *K += kappa;
            grisu_round(buffer, *len, delta.f, p2, one.f, wp_w.f * unit);
            return;
        }
    }
}

void grisu2(double v, char *buffer, int *length, int *K) {
    diy_fp_t w_m, w_p, w, c_mk, W, Wp, Wm;
    int q = 64, alpha = -59, gamma = -56;
    int mk;
    diy_fp_t delta;
    if ( v != 0 ) {
        normalized_boundaries(v, &w_m, &w_p);
        w = normalize_diy_fp(double2diy_fp(v));
        mk = k_comp(w_p.e + q, alpha, gamma);
        c_mk = cached_power(mk);
        W  = multiply(w,   c_mk);
        Wp = multiply(w_p, c_mk);
        Wm = multiply(w_m, c_mk);
        Wm.f++; Wp.f--;
        delta = minus(Wp, Wm);
        *K = -mk;
        digit_gen(W, Wp, delta, buffer, length, K);
    } else{
        *length =1;
        *buffer='0';
        *K=0;
    }

}




