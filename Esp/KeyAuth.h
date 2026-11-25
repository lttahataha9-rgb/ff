#pragma once
#import <Foundation/Foundation.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Hidden C-linkage gate: verifies saved/clipboard key with KeyAuth and
/// invokes `onVerified` on the main queue only after cryptographic state is set.
__attribute__((visibility("hidden")))
void KA_StartSessionGate(void (^onVerified)(void));

/// Returns true only when the split obfuscated session token matches and no hook is present.
__attribute__((visibility("hidden")))
bool KA_IsSessionVerified(void);

/// Verifies Mach-O __TEXT,__text integrity seal and anti-hook prologues.
/// Terminates the process if tampering is detected.
__attribute__((visibility("hidden")))
bool KA_VerifyRuntimeIntegrity(void);

#ifdef __cplusplus
}
#endif
