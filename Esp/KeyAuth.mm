#import "KeyAuth.h"
#import <UIKit/UIKit.h>
#include <stdint.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include "../oxorany/oxorany_include.h"

#define OX_NS(s) ([NSString stringWithUTF8String:oxorany(s)])

// Mach-O __TEXT,__text integrity seal (patched in __DATA,__data by seal_integrity.py at build time)
static constexpr uint64_t kIntegritySalt = 0xA55A1234DEADBEEFULL;
struct IntegritySeal {
    uint64_t marker;
    uint64_t hashXorMask;
};
__attribute__((used, section("__DATA,__data")))
static volatile IntegritySeal g_integritySeal = {
    0x4349434144415F53ULL,
    0x0000000000000000ULL
};

// Split obfuscated auth state: (g_authStateA ^ g_authStateB) == kAuthMagic
static constexpr uint64_t kAuthMagic = 0x7A93C1E4B208D56FULL;
static volatile uint64_t g_authStateA = 0;
static volatile uint64_t g_authStateB = 0;

static uint64_t Fnv1a64(const char *data, size_t len)
{
    uint64_t h = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < len; ++i) {
        h ^= (uint8_t)data[i];
        h *= 0x100000001b3ULL;
    }
    return h ? h : 0x9e3779b97f4a7c15ULL;
}

static void MarkSessionVerified(NSString *sessionId, NSString *hwid)
{
    NSString *seedStr = [NSString stringWithFormat:@"%@|%@", sessionId ?: @"", hwid ?: @""];
    const char *utf8 = seedStr.UTF8String ?: "seed";
    uint64_t entropy = Fnv1a64(utf8, strlen(utf8));
    g_authStateA = entropy;
    g_authStateB = entropy ^ kAuthMagic;
}

static bool IsArm64Hooked(const void *fnPtr)
{
    if (!fnPtr) return true;
    const uint32_t *insns = (const uint32_t *)fnPtr;
    uint32_t i0 = insns[0];
    uint32_t i1 = insns[1];
    // Substrate / ElleKit / Frida trampolines:
    // LDR X16/X17, #imm (0x58000010 / 0x58000011) or unconditional B (0x14xxxxxx)
    if ((i0 & 0xFF00001E) == 0x58000010) return true;
    if ((i0 & 0xFC000000) == 0x14000000) return true;
    // ADRP X16/X17 followed by BR X16/X17
    if ((i0 & 0x9F00001E) == 0x90000010 && (i1 & 0xFFFFFFDF) == 0xD61F0200) return true;
    return false;
}

static bool IsDebuggerAttached(void)
{
    struct kinfo_proc info;
    memset(&info, 0, sizeof(info));
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
    size_t size = sizeof(info);
    if (sysctl(mib, 4, &info, &size, NULL, 0) == 0) {
        return (info.kp_proc.p_flag & P_TRACED) != 0;
    }
    return false;
}

bool KA_VerifyRuntimeIntegrity(void)
{
    uint64_t sealed = g_integritySeal.hashXorMask;
    if (sealed == 0 || IsDebuggerAttached()) {
        g_authStateA = 0;
        g_authStateB = 0;
        _exit(0);
        return false;
    }

    if (IsArm64Hooked((const void *)&KA_StartSessionGate) ||
        IsArm64Hooked((const void *)&KA_IsSessionVerified)) {
        g_authStateA = 0;
        g_authStateB = 0;
        _exit(0);
        return false;
    }

    Dl_info info;
    if (!dladdr((const void *)&KA_StartSessionGate, &info) || !info.dli_fbase) {
        g_authStateA = 0;
        g_authStateB = 0;
        _exit(0);
        return false;
    }

    const struct mach_header_64 *mh = (const struct mach_header_64 *)info.dli_fbase;
    unsigned long textSize = 0;
    const uint8_t *textPtr = getsectiondata(mh, "__TEXT", "__text", &textSize);
    if (!textPtr || textSize == 0) {
        g_authStateA = 0;
        g_authStateB = 0;
        _exit(0);
        return false;
    }

    uint64_t actual = Fnv1a64((const char *)textPtr, (size_t)textSize);
    if (actual != (sealed ^ kIntegritySalt)) {
        g_authStateA = 0;
        g_authStateB = 0;
        _exit(0);
        return false;
    }
    return true;
}

bool KA_IsSessionVerified(void)
{
    if (IsArm64Hooked((const void *)&KA_StartSessionGate)) {
        g_authStateA = 0;
        g_authStateB = 0;
        _exit(0);
        return false;
    }
    uint64_t a = g_authStateA;
    uint64_t b = g_authStateB;
    return (a != 0) && (b != 0) && ((a ^ b) == kAuthMagic);
}

// XOR-obfuscated storage for the cached license key in NSUserDefaults
static NSString *ObfuscateKeyForStorage(NSString *plain)
{
    NSData *raw = [plain dataUsingEncoding:NSUTF8StringEncoding];
    if (!raw || raw.length == 0) return nil;
    const uint8_t *bytes = (const uint8_t *)raw.bytes;
    NSMutableData *out = [NSMutableData dataWithLength:raw.length];
    uint8_t *dst = (uint8_t *)out.mutableBytes;
    const uint8_t mask = 0x5A;
    for (NSUInteger i = 0; i < raw.length; ++i) {
        dst[i] = bytes[i] ^ (uint8_t)(mask + (i * 13));
    }
    return [out base64EncodedStringWithOptions:0];
}

static NSString *DeobfuscateStoredKey(NSString *encoded)
{
    if (![encoded isKindOfClass:[NSString class]] || encoded.length == 0) return nil;
    NSData *raw = [[NSData alloc] initWithBase64EncodedString:encoded options:0];
    if (!raw || raw.length == 0 || raw.length > 256) return nil;
    const uint8_t *bytes = (const uint8_t *)raw.bytes;
    NSMutableData *out = [NSMutableData dataWithLength:raw.length];
    uint8_t *dst = (uint8_t *)out.mutableBytes;
    const uint8_t mask = 0x5A;
    for (NSUInteger i = 0; i < raw.length; ++i) {
        dst[i] = bytes[i] ^ (uint8_t)(mask + (i * 13));
    }
    return [[NSString alloc] initWithData:out encoding:NSUTF8StringEncoding];
}

static NSURL *KeyAuthEndpointURL(void)
{
    // Build via NSURLComponents to bypass Hosts/hookURL.m's +[NSURL URLWithString:] swizzle.
    NSURLComponents *comps = [[NSURLComponents alloc] init];
    comps.scheme = OX_NS("https");
    comps.host   = OX_NS("keyauth.win");
    comps.path   = OX_NS("/api/1.2/");
    return comps.URL;
}

static NSURLSession *KeyAuthSession(void)
{
    static NSURLSession *session = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSURLSessionConfiguration *cfg = [NSURLSessionConfiguration ephemeralSessionConfiguration];
        cfg.timeoutIntervalForRequest  = 15.0;
        cfg.timeoutIntervalForResource = 20.0;
        cfg.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
        cfg.connectionProxyDictionary = @{}; // Block Charles/Proxyman/Burp Wi-Fi HTTP proxies
        session = [NSURLSession sessionWithConfiguration:cfg];
    });
    return session;
}

static NSString *FormEncode(NSString *str)
{
    if (!str) return @"";
    NSMutableCharacterSet *allowed = [[NSCharacterSet URLQueryAllowedCharacterSet] mutableCopy];
    [allowed removeCharactersInString:OX_NS("&+=/?#")];
    return [str stringByAddingPercentEncodingWithAllowedCharacters:allowed] ?: @"";
}

static NSString *DeviceHWID(void)
{
    NSString *idfv = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
    return idfv.length > 0 ? idfv : OX_NS("ios-unknown-hwid");
}

static UIWindow *SafeKeyAuthWindow(void)
{
    UIWindow *key = [UIApplication sharedApplication].keyWindow;
    if (key && !key.hidden && key.alpha > 0.01 && key.rootViewController)
        return key;
    for (UIWindow *w in [UIApplication sharedApplication].windows) {
        if (w && !w.hidden && w.alpha > 0.01 && w.rootViewController)
            return w;
    }
    return [UIApplication sharedApplication].windows.firstObject;
}

static UIViewController *TopViewController(void)
{
    UIWindow *window = SafeKeyAuthWindow();
    UIViewController *vc = window.rootViewController;
    while (vc.presentedViewController && !vc.presentedViewController.isBeingDismissed) {
        vc = vc.presentedViewController;
    }
    return vc;
}

static NSString *ReadClipboardKey(void)
{
    UIPasteboard *pb = [UIPasteboard generalPasteboard];
    if (!pb.hasStrings) return nil;
    NSString *raw = pb.string;
    if (![raw isKindOfClass:[NSString class]]) return nil;
    NSString *trimmed = [raw stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    if (trimmed.length == 0 || trimmed.length > 128) return nil;
    return trimmed;
}

static void PostForm(NSDictionary<NSString *, NSString *> *params,
                     void (^completion)(NSDictionary * _Nullable json, NSString * _Nullable errorMsg))
{
    NSURL *url = KeyAuthEndpointURL();
    if (!url) {
        completion(nil, OX_NS("Invalid endpoint."));
        return;
    }

    NSMutableURLRequest *req = [NSMutableURLRequest requestWithURL:url];
    req.HTTPMethod = OX_NS("POST");
    req.timeoutInterval = 15.0;
    [req setValue:OX_NS("application/x-www-form-urlencoded") forHTTPHeaderField:OX_NS("Content-Type")];

    NSMutableArray<NSString *> *pairs = [NSMutableArray arrayWithCapacity:params.count];
    for (NSString *k in params) {
        [pairs addObject:[NSString stringWithFormat:@"%@=%@", FormEncode(k), FormEncode(params[k])]];
    }
    req.HTTPBody = [[pairs componentsJoinedByString:@"&"] dataUsingEncoding:NSUTF8StringEncoding];

    NSURLSessionDataTask *task = [KeyAuthSession() dataTaskWithRequest:req
        completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
            if (error || !data) {
                completion(nil, error.localizedDescription ?: OX_NS("Network error."));
                return;
            }
            if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
                NSInteger code = ((NSHTTPURLResponse *)response).statusCode;
                if (code != 200) {
                    completion(nil, OX_NS("Server error."));
                    return;
                }
            }
            NSError *jsonErr = nil;
            id parsed = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonErr];
            if (jsonErr || ![parsed isKindOfClass:[NSDictionary class]]) {
                completion(nil, OX_NS("Invalid server response."));
                return;
            }
            completion((NSDictionary *)parsed, nil);
        }];
    [task resume];
}

static void ShowDenyAndExit(NSString *message);

static void VerifyLicenseKey(NSString *licenseKey,
                             void (^completion)(BOOL success, NSString *message))
{
    NSString *trimmed = [licenseKey stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    if (trimmed.length == 0) {
        dispatch_async(dispatch_get_main_queue(), ^{
            completion(NO, OX_NS("No key found. Copy your key before opening the game."));
        });
        return;
    }

    NSString *appName = OX_NS("external");
    NSString *ownerId = OX_NS("iko60kkbQV");
    NSString *appVer  = OX_NS("1.0");

    NSDictionary *initParams = @{
        OX_NS("type"):    OX_NS("init"),
        OX_NS("ver"):     appVer,
        OX_NS("name"):    appName,
        OX_NS("ownerid"): ownerId
    };

    PostForm(initParams, ^(NSDictionary * _Nullable initJson, NSString * _Nullable initErr) {
        if (!initJson) {
            dispatch_async(dispatch_get_main_queue(), ^{
                completion(NO, initErr ?: OX_NS("Failed to connect to API."));
            });
            return;
        }

        id initOkVal = initJson[OX_NS("success")];
        BOOL initOk = [initOkVal respondsToSelector:@selector(boolValue)] && [initOkVal boolValue];
        NSString *sessionId = [initJson[OX_NS("sessionid")] isKindOfClass:[NSString class]] ? initJson[OX_NS("sessionid")] : nil;
        if (!initOk || sessionId.length == 0) {
            NSString *msg = [initJson[OX_NS("message")] isKindOfClass:[NSString class]]
                ? initJson[OX_NS("message")]
                : OX_NS("Initialization failed.");
            dispatch_async(dispatch_get_main_queue(), ^{
                completion(NO, msg);
            });
            return;
        }

        NSString *hwid = DeviceHWID();
        NSDictionary *licParams = @{
            OX_NS("type"):      OX_NS("license"),
            OX_NS("key"):       trimmed,
            OX_NS("hwid"):      hwid,
            OX_NS("sessionid"): sessionId,
            OX_NS("name"):      appName,
            OX_NS("ownerid"):   ownerId
        };

        PostForm(licParams, ^(NSDictionary * _Nullable licJson, NSString * _Nullable licErr) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (!licJson) {
                    completion(NO, licErr ?: OX_NS("License check failed."));
                    return;
                }
                id okVal = licJson[OX_NS("success")];
                BOOL ok = [okVal respondsToSelector:@selector(boolValue)] && [okVal boolValue];

                // Verify real KeyAuth info + subscriptions payload to block naive fake JSON spoofs
                NSDictionary *info = [licJson[OX_NS("info")] isKindOfClass:[NSDictionary class]] ? licJson[OX_NS("info")] : nil;
                NSArray *subs = (info && [info[OX_NS("subscriptions")] isKindOfClass:[NSArray class]]) ? info[OX_NS("subscriptions")] : nil;
                id bannedVal = info ? info[OX_NS("banned")] : nil;
                BOOL isBanned = [bannedVal respondsToSelector:@selector(boolValue)] && [bannedVal boolValue];
                BOOL payloadValid = ok && info != nil && subs != nil && subs.count > 0 && !isBanned;

                NSString *msg = [licJson[OX_NS("message")] isKindOfClass:[NSString class]]
                    ? licJson[OX_NS("message")]
                    : (payloadValid ? OX_NS("Logged in!") : OX_NS("Invalid license."));

                if (payloadValid) {
                    MarkSessionVerified(sessionId, hwid);
                    NSString *enc = ObfuscateKeyForStorage(trimmed);
                    if (enc) {
                        [[NSUserDefaults standardUserDefaults] setObject:enc forKey:OX_NS("com.dts.freefireth.cache_cfg_v3")];
                        [[NSUserDefaults standardUserDefaults] synchronize];
                    }
                    // Start 30s live session heartbeat so banning/deleting the user kicks them out immediately
                    static dispatch_source_t s_hbTimer = nil;
                    static NSString *s_hbSession = nil;
                    s_hbSession = [sessionId copy];
                    if (!s_hbTimer) {
                        s_hbTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
                        dispatch_source_set_timer(s_hbTimer, dispatch_time(DISPATCH_TIME_NOW, 30 * NSEC_PER_SEC), 30 * NSEC_PER_SEC, 2 * NSEC_PER_SEC);
                        dispatch_source_set_event_handler(s_hbTimer, ^{
                            if (!KA_IsSessionVerified() || s_hbSession.length == 0) return;
                            NSDictionary *chkParams = @{
                                OX_NS("type"):      OX_NS("check"),
                                OX_NS("sessionid"): s_hbSession,
                                OX_NS("name"):      OX_NS("external"),
                                OX_NS("ownerid"):   OX_NS("iko60kkbQV")
                            };
                            PostForm(chkParams, ^(NSDictionary * _Nullable chkJson, NSString * _Nullable chkErr) {
                                if (!chkJson) return; // ignore transient offline blip
                                id chkOkVal = chkJson[OX_NS("success")];
                                BOOL chkOk = [chkOkVal respondsToSelector:@selector(boolValue)] && [chkOkVal boolValue];
                                if (!chkOk) {
                                    NSString *errMsg = [chkJson[OX_NS("message")] isKindOfClass:[NSString class]]
                                        ? chkJson[OX_NS("message")]
                                        : OX_NS("Session terminated or key banned.");
                                    dispatch_async(dispatch_get_main_queue(), ^{
                                        [[NSUserDefaults standardUserDefaults] removeObjectForKey:OX_NS("com.dts.freefireth.cache_cfg_v3")];
                                        [[NSUserDefaults standardUserDefaults] synchronize];
                                        ShowDenyAndExit(errMsg);
                                    });
                                }
                            });
                        });
                        dispatch_resume(s_hbTimer);
                    }
                }
                completion(payloadValid, msg);
            });
        });
    });
}

static void ShowDenyAndExit(NSString *message)
{
    g_authStateA = 0;
    g_authStateB = 0;

    // Force-exit after 2.0s even if alert presentation is blocked or ignored
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        _exit(0);
    });

    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *presenter = TopViewController();
        if (!presenter || !presenter.view.window || presenter.isBeingDismissed || presenter.isBeingPresented) {
            return;
        }

        NSString *body = [NSString stringWithFormat:@"%@\n\n%@",
                          message ?: OX_NS("Authentication required."),
                          OX_NS("Copy a valid key before opening the game.")];
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:OX_NS("EXTRERNAL X | API")
                                                                       message:body
                                                                preferredStyle:UIAlertControllerStyleAlert];

        UIAlertAction *exitAction = [UIAlertAction actionWithTitle:OX_NS("Exit")
                                                             style:UIAlertActionStyleDestructive
                                                           handler:^(UIAlertAction * _Nonnull action) {
            _exit(0);
        }];

        [alert addAction:exitAction];
        [presenter presentViewController:alert animated:YES completion:nil];
    });
}

static void CheckClipboardWithCompletion(void (^onVerified)(void))
{
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *clipKey = ReadClipboardKey();
        if (clipKey.length == 0) {
            ShowDenyAndExit(OX_NS("No key found in clipboard."));
            return;
        }

        VerifyLicenseKey(clipKey, ^(BOOL success, NSString *message) {
            if (success && KA_IsSessionVerified()) {
                if (onVerified) onVerified();
            } else {
                ShowDenyAndExit(message);
            }
        });
    });
}

void KA_StartSessionGate(void (^onVerified)(void))
{
    if (!KA_VerifyRuntimeIntegrity()) {
        return;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *encKey = [[NSUserDefaults standardUserDefaults] stringForKey:OX_NS("com.dts.freefireth.cache_cfg_v3")];
        NSString *savedKey = DeobfuscateStoredKey(encKey);
        if (savedKey.length > 0) {
            VerifyLicenseKey(savedKey, ^(BOOL success, NSString *message) {
                if (success && KA_IsSessionVerified()) {
                    if (onVerified) onVerified();
                } else {
                    [[NSUserDefaults standardUserDefaults] removeObjectForKey:OX_NS("com.dts.freefireth.cache_cfg_v3")];
                    [[NSUserDefaults standardUserDefaults] synchronize];
                    CheckClipboardWithCompletion(onVerified);
                }
            });
        } else {
            CheckClipboardWithCompletion(onVerified);
        }
    });
}
