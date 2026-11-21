//Require standard library
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Foundation/Foundation.h>
#include <iostream>
#include <UIKit/UIKit.h>
#include <vector>
#import "pthread.h"
#include <array>
#import <os/log.h>
#include <cmath>
#include <deque>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <cerrno>
#include <cctype>
//Imgui library
#import "Esp/CaptainHook.h"
#import "Esp/ImGuiDrawView.h"
#import "IMGUI/imgui.h"
#import "IMGUI/imgui_internal.h"
#import "IMGUI/imgui_impl_metal.h"
#import "IMGUI/zzz.h"
#import "Hosts/NSObject+URL.h"
#include "oxorany/oxorany_include.h"
#import "Helper/Mem.h"
#include "font.h"
#import "Helper/Vector3.h"
#import "Helper/Vector2.h"
#import "Helper/Quaternion.h"
#import "Helper/Monostring.h"
#include "Helper/font.h"
#include "Helper/data.h"
#include "Helper/cicada_icon.h"
ImFont* verdana_smol;
ImFont* pixel_big = {};
ImFont* pixel_smol = {};
#include "Helper/Obfuscate.h"
#import "Helper/Hooks.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <unistd.h>
#include <string.h>

#define kWidth  [UIScreen mainScreen].bounds.size.width
#define kHeight [UIScreen mainScreen].bounds.size.height
#define kScale [UIScreen mainScreen].scale

// Safe window lookup: windows[0] throws NSRangeException when the app has
// no window yet (scene-based apps / early launch) -> that was the crash on
// opening the panel. Returns nil when there is no window at all.
static UIWindow *SafeHostWindow(void)
{
    UIWindow *key = [UIApplication sharedApplication].keyWindow;
    if (key && !key.hidden && key.alpha > 0.01)
        return key;
    for (UIWindow *w in [UIApplication sharedApplication].windows)
        if (w && !w.hidden && w.alpha > 0.01)
            return w;
    return [UIApplication sharedApplication].windows.firstObject;
}

// ===================== floating feature buttons =====================
// Quick-toggle buttons over the game. VISIBILITY: ghir li n-active-hom f
// l'panel (gFltShow — user: "mnin n'active hadi → tla3 lya l'boutonat f
// l'écran"). A plain tap toggles the option on and off, dragging moves the
// button around. COLOR: card = always the OFF look (cyan border, 7amra
// label); ONLY the iOS switch changes color when ON (Fly=7amra, Runn/
// Telekill=mauve, Send=khadra, Auto/Tele Mark=orange/azr9 — user: "color
// changes ONLY on the toggle switch"). Panel = plain checkbox (machi hada
// li brit itbdel lounha).
enum { FLTBTN_TELEKILL = 0, FLTBTN_FLYV2 = 1, FLTBTN_RUNN = 2, FLTBTN_AIMKILL = 3, FLTBTN_STOPTIME = 4, FLTBTN_FLYOP = 5, FLTBTN_FLYJUP = 6, FLTBTN_AUTOMARK = 7, FLTBTN_TELEMARK = 8, FLTBTN_DOWNKILL2 = 9, FLTBTN_GHOST = 10, FLTBTN_UNDERGROUND = 11, FLTBTN_AUTOUNDER = 12, FLTBTN_COUNT = 13 };
static bool gFltShow[FLTBTN_COUNT] = { false, false, false, false, false, false, false, false, false, false, false, false, false }; // panel checkbox latch

struct FltBtnState {
    ImVec2 pos = ImVec2(0, 0);
    ImVec2 baseline = ImVec2(0, 0);   // touch-down point (kills stale-delta jumps)
    ImVec2 grabPos = ImVec2(0, 0);    // window pos at touch-down
    bool inited = false;
    bool moved = false;
    bool pressed = false;
    bool toggled = false;   // instant tap applied, awaiting possible drag-cancel
    float knobX = -1.0f;    // animated knob screen-x, -1 = not placed yet
};
static FltBtnState gFltBtn[FLTBTN_COUNT];
static ImRect gFltBtnRect[FLTBTN_COUNT];   // screen rect while visible (hit-test)

// ===================== floating CICADA menu icon =====================
static FltBtnState gMenuIconBtn;
static ImRect gMenuIconRect = ImRect(0, 0, 0, 0);
static id<MTLTexture> gCicadaIconTex = nil;

static bool MenDeal = true;   // menu visibility (moved up: FFOverlayView needs it)

// Touch hit-testing: while the menu is closed the overlay must give the game
// every touch EXCEPT taps that land on the CICADA menu icon or a floating feature button.
static bool FFPointOverFloating(CGPoint p)
{
    ImVec2 pt((float)p.x, (float)p.y);
    if (gMenuIconRect.Contains(pt))
        return true;
    for (int i = 0; i < FLTBTN_COUNT; i++)
        if (gFltBtnRect[i].Contains(pt))
            return true;
    return false;
}

// ON colors dyal l'floating buttons (user: "wa7da l7mra, wa7da mauve,
// wa7da khadra") — t-tbdlo ghir mnin l'button yweli ON
static const ImVec4 kFltOnCol[FLTBTN_COUNT] = {
    ImVec4(0.78f, 0.35f, 1.00f, 1.00f), // TELEKILL - mauve
    ImVec4(1.00f, 0.25f, 0.25f, 1.00f), // FLYV2    - 7amra
    ImVec4(0.78f, 0.35f, 1.00f, 1.00f), // RUNN     - mauve
    ImVec4(0.25f, 1.00f, 0.45f, 1.00f), // AIMKILL  - khadra (Send + 360)
    ImVec4(1.00f, 0.82f, 0.25f, 1.00f), // STOPTIME - sfer (freeze)
    ImVec4(1.00f, 0.25f, 0.25f, 1.00f), // FLYOP    - 7amra (fly family, snippet)
    ImVec4(0.75f, 1.00f, 0.20f, 1.00f), // FLYJUP   - chartreuse (jump = energy)
    ImVec4(1.00f, 0.55f, 0.05f, 1.00f), // AUTOMARK - orange (CHIBYA)
    ImVec4(0.25f, 0.60f, 1.00f, 1.00f), // TELEMARK - azr9 (teleport)
    // ---- lounat JDADE (user: "DIRE LWN JDADE B L BOTON") — 4 boutonat jdad ----
    ImVec4(0.13f, 0.88f, 0.80f, 1.00f), // DOWNKILL2- ferrouj (teal)
    ImVec4(0.88f, 0.96f, 1.00f, 1.00f), // GHOST    - byd glaci (ice)
    ImVec4(1.00f, 0.38f, 0.15f, 1.00f), // UNDERGROUND - lava orange
    ImVec4(0.00f, 0.92f, 0.72f, 1.00f), // AUTOUNDER   - neon mint
};

// panel design: tab SETTINGS -> ColorEdit4 loun dyal l'panel (user nkhtar)
static float g_panelCol[4] = {0.06f, 0.06f, 0.09f, 0.96f};
static float g_accentCol[4] = {0.00f, 0.88f, 0.96f, 1.00f}; // CICADA Neon Cyan accent
static ImVec4 PanelBright(float k)
{
    return ImVec4(
        g_panelCol[0] + (g_accentCol[0] - g_panelCol[0]) * k,
        g_panelCol[1] + (g_accentCol[1] - g_panelCol[1]) * k,
        g_panelCol[2] + (g_accentCol[2] - g_panelCol[2]) * k,
        1.0f);
}

// ===================== AUTO SAVE SYSTEM (NSUserDefaults) =====================
static bool g_autoSave = true;

struct CicadaSaveData {
    uint32_t magic;
    bool autoSave;
    // ESP
    bool espEnable, lines, box, outline, name, health, distance, circlepos, skeleton, oof, enemycount;
    // Aimbot
    bool aimbot, fovaimglow;
    int aimWhen;
    float aimFov;
    float fovLineColor[4];
    // Features & Floating Buttons
    bool fltShow[FLTBTN_COUNT];
    bool telekill, aimKillSend, fly, flyingtest, flyOp, flyJump, stopTime, teleMark, autoMark, downKillV2, ghost, underGround, autoUnder, resetGuest;
    float flySpeed, flyhei, khoangcach, ugDepth;
    // Theme
    float panelCol[4];
    float accentCol[4];
    // Floating icon + button positions
    float menuIconPos[2];
    bool menuIconInited;
    float fltPos[FLTBTN_COUNT][2];
    bool fltInited[FLTBTN_COUNT];
};

static CicadaSaveData g_lastSavedSnap = {};
static bool g_saveSnapInited = false;
static NSString *const kCicadaSaveKey = @"CicadaAutoSave_v3";
static NSString *const kCicadaAutoSaveFlagKey = @"CicadaAutoSaveEnabled_v3";

static void CaptureSaveSnapshot(CicadaSaveData *d)
{
    memset(d, 0, sizeof(CicadaSaveData));
    d->magic = 0xC1CADA03;
    d->autoSave = g_autoSave;
    d->espEnable = Vars.Enable;
    d->lines = Vars.lines;
    d->box = Vars.Box;
    d->outline = Vars.Outline;
    d->name = Vars.Name;
    d->health = Vars.Health;
    d->distance = Vars.Distance;
    d->circlepos = Vars.circlepos;
    d->skeleton = Vars.skeleton;
    d->oof = Vars.OOF;
    d->enemycount = Vars.enemycount;

    d->aimbot = Vars.Aimbot;
    d->fovaimglow = Vars.fovaimglow;
    d->aimWhen = Vars.AimWhen;
    d->aimFov = Vars.AimFov;
    for (int i = 0; i < 4; ++i) d->fovLineColor[i] = Vars.fovLineColor[i];

    for (int i = 0; i < FLTBTN_COUNT; ++i) {
        d->fltShow[i] = gFltShow[i];
        d->fltPos[i][0] = gFltBtn[i].pos.x;
        d->fltPos[i][1] = gFltBtn[i].pos.y;
        d->fltInited[i] = gFltBtn[i].inited;
    }
    d->telekill = Vars.Telekill;
    d->aimKillSend = Vars.AimKillSend;
    d->fly = Vars.fly;
    d->flyingtest = Vars.Flyingtest;
    d->flyOp = Vars.FlyOp;
    d->flyJump = Vars.FlyJump;
    d->stopTime = Vars.StopTime;
    d->teleMark = Vars.TeleMark;
    d->autoMark = Vars.AutoMark;
    d->downKillV2 = Vars.DownKillV2;
    d->ghost = Vars.Ghost;
    d->underGround = Vars.UnderGround;
    d->autoUnder = Vars.AutoUnder;
    d->resetGuest = Vars.ResetGuest;
    d->flySpeed = Vars.flySpeed;
    d->flyhei = Vars.flyhei;
    d->khoangcach = Vars.khoangcach;
    d->ugDepth = Vars.UgDepth;

    for (int i = 0; i < 4; ++i) {
        d->panelCol[i] = g_panelCol[i];
        d->accentCol[i] = g_accentCol[i];
    }
    d->menuIconPos[0] = gMenuIconBtn.pos.x;
    d->menuIconPos[1] = gMenuIconBtn.pos.y;
    d->menuIconInited = gMenuIconBtn.inited;
}

static void SaveAutoSaveConfigNow(void)
{
    CicadaSaveData cur;
    CaptureSaveSnapshot(&cur);
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    [ud setBool:g_autoSave forKey:kCicadaAutoSaveFlagKey];
    if (g_autoSave) {
        NSData *blob = [NSData dataWithBytes:&cur length:sizeof(CicadaSaveData)];
        [ud setObject:blob forKey:kCicadaSaveKey];
    }
    [ud synchronize];
    g_lastSavedSnap = cur;
    g_saveSnapInited = true;
}

static void LoadAutoSaveConfig(void)
{
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    if ([ud objectForKey:kCicadaAutoSaveFlagKey] != nil) {
        g_autoSave = [ud boolForKey:kCicadaAutoSaveFlagKey];
    }
    if (!g_autoSave) {
        CaptureSaveSnapshot(&g_lastSavedSnap);
        g_saveSnapInited = true;
        return;
    }
    NSData *blob = [ud dataForKey:kCicadaSaveKey];
    if (!blob || [blob length] != sizeof(CicadaSaveData)) {
        CaptureSaveSnapshot(&g_lastSavedSnap);
        g_saveSnapInited = true;
        return;
    }
    const CicadaSaveData *d = (const CicadaSaveData *)[blob bytes];
    if (d->magic != 0xC1CADA03) {
        CaptureSaveSnapshot(&g_lastSavedSnap);
        g_saveSnapInited = true;
        return;
    }
    Vars.Enable = d->espEnable;
    Vars.lines = d->lines;
    Vars.Box = d->box;
    Vars.Outline = d->outline;
    Vars.Name = d->name;
    Vars.Health = d->health;
    Vars.Distance = d->distance;
    Vars.circlepos = d->circlepos;
    Vars.skeleton = d->skeleton;
    Vars.OOF = d->oof;
    Vars.enemycount = d->enemycount;

    Vars.Aimbot = d->aimbot;
    Vars.fovaimglow = d->fovaimglow;
    Vars.AimWhen = d->aimWhen;
    Vars.AimFov = d->aimFov;
    for (int i = 0; i < 4; ++i) Vars.fovLineColor[i] = d->fovLineColor[i];

    for (int i = 0; i < FLTBTN_COUNT; ++i) {
        gFltShow[i] = d->fltShow[i];
        if (d->fltInited[i]) {
            gFltBtn[i].pos = ImVec2(d->fltPos[i][0], d->fltPos[i][1]);
            gFltBtn[i].inited = true;
        }
    }
    Vars.Telekill = d->telekill;
    Vars.AimKillSend = d->aimKillSend;
    Vars.fly = d->fly;
    Vars.Flyingtest = d->flyingtest;
    Vars.FlyOp = d->flyOp;
    Vars.FlyJump = d->flyJump;
    Vars.StopTime = d->stopTime;
    Vars.TeleMark = d->teleMark;
    Vars.AutoMark = d->autoMark;
    Vars.DownKillV2 = d->downKillV2;
    Vars.Ghost = d->ghost;
    Vars.UnderGround = d->underGround;
    Vars.AutoUnder = d->autoUnder;
    Vars.ResetGuest = false; // never auto-reset guest on launch
    Vars.flySpeed = d->flySpeed;
    Vars.flyhei = d->flyhei;
    Vars.khoangcach = d->khoangcach;
    Vars.UgDepth = d->ugDepth;

    for (int i = 0; i < 4; ++i) {
        g_panelCol[i] = d->panelCol[i];
        g_accentCol[i] = d->accentCol[i];
    }
    if (d->menuIconInited) {
        gMenuIconBtn.pos = ImVec2(d->menuIconPos[0], d->menuIconPos[1]);
        gMenuIconBtn.inited = true;
    }
    CaptureSaveSnapshot(&g_lastSavedSnap);
    g_saveSnapInited = true;
}

static void CheckAutoSaveConfig(void)
{
    if (!g_autoSave) return;
    // Wait until user finishes dragging any floating button before saving
    if (gMenuIconBtn.pressed) return;
    for (int i = 0; i < FLTBTN_COUNT; ++i) {
        if (gFltBtn[i].pressed) return;
    }
    CicadaSaveData cur;
    CaptureSaveSnapshot(&cur);
    if (!g_saveSnapInited || memcmp(&cur, &g_lastSavedSnap, sizeof(CicadaSaveData)) != 0) {
        SaveAutoSaveConfigNow();
    }
}

static void ResetDefaultConfig(void)
{
    Vars = Vars_t{};
    for (int i = 0; i < FLTBTN_COUNT; ++i) {
        gFltShow[i] = false;
        gFltBtn[i].inited = false;
    }
    gMenuIconBtn.inited = false;
    g_panelCol[0] = 0.06f; g_panelCol[1] = 0.06f; g_panelCol[2] = 0.09f; g_panelCol[3] = 0.96f;
    g_accentCol[0] = 0.00f; g_accentCol[1] = 0.88f; g_accentCol[2] = 0.96f; g_accentCol[3] = 1.00f;
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    [ud removeObjectForKey:kCicadaSaveKey];
    [ud synchronize];
    CaptureSaveSnapshot(&g_lastSavedSnap);
}

static void DrawMenuIconButton(void)
{
    FltBtnState &b = gMenuIconBtn;
    const float S = 44.0f; // compact small floating icon
    ImGuiIO &io = ImGui::GetIO();
    if (!b.inited) {
        b.inited = true;
        b.pos = ImVec2(18.0f, 72.0f);
    }
    // Clamp inside screen bounds
    if (io.DisplaySize.x > S && io.DisplaySize.y > S) {
        if (b.pos.x < 4.0f) b.pos.x = 4.0f;
        if (b.pos.y < 4.0f) b.pos.y = 4.0f;
        if (b.pos.x > io.DisplaySize.x - S - 4.0f) b.pos.x = io.DisplaySize.x - S - 4.0f;
        if (b.pos.y > io.DisplaySize.y - S - 4.0f) b.pos.y = io.DisplaySize.y - S - 4.0f;
    }

    ImGui::SetNextWindowPos(b.pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(S, S), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, S * 0.5f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

    ImGui::Begin("##cicada_icon_win", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                 ImGuiWindowFlags_NoBackground);

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::InvisibleButton("##cicada_icon_btn", ImVec2(S, S));

    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        if (!b.pressed) {
            b.pressed = true; b.moved = false; b.toggled = false;
            b.baseline = io.MousePos;
            b.grabPos = b.pos;
        } else {
            float dx = io.MousePos.x - b.baseline.x;
            float dy = io.MousePos.y - b.baseline.y;
            if (!b.moved && (fabsf(dx) + fabsf(dy) > 10.0f))
                b.moved = true;
            if (b.moved) {
                b.pos = ImVec2(b.grabPos.x + dx, b.grabPos.y + dy);
            }
        }
    }
    if (b.pressed && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (!b.moved) {
            MenDeal = !MenDeal;
        }
        b.pressed = false; b.moved = false; b.toggled = false;
    }

    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImVec2 wp = ImGui::GetWindowPos();
    ImVec2 center(wp.x + S * 0.5f, wp.y + S * 0.5f);
    float r = S * 0.5f - 2.0f;

    // Outer ambient glow + dark circular badge
    ImU32 accCol = IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), MenDeal ? 245 : 175);
    ImU32 glowCol = IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), MenDeal ? 75 : 35);
    dl->AddCircleFilled(center, r + 1.5f, glowCol, 36);
    dl->AddCircleFilled(center, r, IM_COL32(8, 9, 13, 240), 36);

    // Draw the uploaded CICADA icon texture inside the circular button
    if (gCicadaIconTex != nil) {
        float pad = 6.0f;
        dl->AddImageRounded((ImTextureID)((uintptr_t)gCicadaIconTex),
                            ImVec2(wp.x + pad, wp.y + pad),
                            ImVec2(wp.x + S - pad, wp.y + S - pad),
                            ImVec2(0, 0), ImVec2(1, 1),
                            IM_COL32(255, 255, 255, 255),
                            (S - pad * 2.0f) * 0.5f);
    } else {
        ImFont *fnt = ImGui::GetFont();
        ImVec2 ts = fnt->CalcTextSizeA(15.0f, FLT_MAX, 0.0f, "C");
        dl->AddText(fnt, 15.0f, ImVec2(center.x - ts.x * 0.5f, center.y - ts.y * 0.5f), accCol, "C");
    }

    // Crisp neon ring border
    dl->AddCircle(center, r, accCol, 36, MenDeal ? 2.0f : 1.5f);

    gMenuIconRect = ImRect(wp, ImVec2(wp.x + S, wp.y + S));

    ImGui::End();
    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);
}

static void DrawFloatingButton(int idx, const char *label, bool *v)
{
    // Visible ghir mnin n-active-ha f l'panel (gFltShow). Card look = ALWAYS
    // neutral/OFF (cyan border, dark bg, red label): kolchi loun kaybdel ghir
    // f t-switch dyal iOS mnin toggle ON (user: "color changes ONLY on the
    // toggle switch", "label + iOS toggle switch", size 60x56 (user).
    FltBtnState &b = gFltBtn[idx];
    const float W = 60.0f, H = 56.0f;    // user: "l'button gaddihom 60 56"
    const float swW = 42.0f, swH = 22.0f; // iOS switch track (label above)
    if (!b.inited) {
        b.inited = true;
        ImVec2 ds = ImGui::GetIO().DisplaySize;
        // stack top-right; WRAP into a left column when the stack would run
        // off a short (phone) screen -> all 7 buttons stay tappable
        const float gap = 12.0f, y0 = 64.0f;
        int rows = (int)((ds.y - y0 - H - 6.0f) / (H + gap)) + 1;
        if (rows < 1) rows = 1;
        int col = idx / rows, row = idx % rows;
        b.pos = ImVec2(ds.x - W - 12.0f - col * (W + 8.0f),
                       y0 + row * (H + gap));
    }

    // window look: NEUTRAL always (the OFF style) — the color lives only in
    // the switch drawn below (user: body keeps the neutral/OFF look)
    ImGui::SetNextWindowPos(b.pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(W, H), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.07f, 0.10f, 0.62f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.31f, 0.90f, 1.00f, 1.00f));

    char wname[48];
    snprintf(wname, sizeof(wname), "##fltwin%d", idx);
    ImGui::Begin(wname, nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // --- input FIRST so label/knob update in the SAME frame (instant tap) ---
    ImGui::SetCursorPos(ImVec2(0, 0));
    char bname[48];
    snprintf(bname, sizeof(bname), "##fltbtn%d", idx);
    ImGui::InvisibleButton(bname, ImVec2(W, H));

    ImGuiIO &io = ImGui::GetIO();
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        if (!b.pressed) {
            // instant toggle on touch-down. The first frame only RECORDS the
            // press point: MouseDelta can hold a jump from the stale position
            // of the previous touch (the "buttons run away" bug), so movement
            // is measured from the touch-down point, never from old deltas.
            b.pressed = true; b.moved = false; b.toggled = true;
            b.baseline = io.MousePos;
            b.grabPos = b.pos;
            *v = !*v;
        } else if (!b.moved) {
            float dx = io.MousePos.x - b.baseline.x;
            float dy = io.MousePos.y - b.baseline.y;
            if (fabsf(dx) + fabsf(dy) > 12.0f) b.moved = true;         // drag threshold
            if (b.moved && b.toggled) { *v = !*v; b.toggled = false; }  // drag cancels tap
        }
        if (b.moved) {
            // absolute positioning: window follows the finger from the press
            // point, no incremental drift, works from any spot on the button
            b.pos = ImVec2(b.grabPos.x + (io.MousePos.x - b.baseline.x),
                           b.grabPos.y + (io.MousePos.y - b.baseline.y));
        }
    }
    if (b.pressed && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        b.pressed = false; b.moved = false; b.toggled = false;
    }

    const bool on = *v;   // knob target: right = ON, left = OFF

    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImVec2 wp = ImGui::GetWindowPos();

    // label: neutral red always (look qdim) — top-center, auto-fit to the
    // 60 px card so it never runs under the switch
    const ImVec4 &oc = kFltOnCol[idx];
    const float labelMax = W - 6.0f;
    ImFont *fnt = ImGui::GetFont();
    float fs = ImGui::GetFontSize();
    ImVec2 ts = fnt->CalcTextSizeA(fs, FLT_MAX, 0.0f, label);
    if (ts.x > labelMax && fs > 9.0f)
    {
        fs = fs * labelMax / ts.x;
        ts = fnt->CalcTextSizeA(fs, FLT_MAX, 0.0f, label);
    }
    dl->AddText(fnt, fs, ImVec2(wp.x + (W - ts.x) * 0.5f, wp.y + 6.0f),
                IM_COL32(255, 64, 64, 255), label);

    // iOS switch — bottom-center, the ONLY part that changes color
    float swX = wp.x + (W - swW) * 0.5f;
    float swY = wp.y + H - swH - 7.0f;
    ImU32 track = on ? IM_COL32((int)(oc.x * 255.0f), (int)(oc.y * 255.0f), (int)(oc.z * 255.0f), 255)
                     : IM_COL32(70, 74, 82, 255); // OFF = grey track
    dl->AddRectFilled(ImVec2(swX, swY), ImVec2(swX + swW, swY + swH), track, swH * 0.5f);
    const float kR = swH * 0.5f - 2.0f;               // knob radius
    float targetX = on ? (swX + swW - kR - 2.0f) : (swX + kR + 2.0f);
    if (b.knobX <= 0.0f) b.knobX = targetX;           // first frame: snap
    float k = ImGui::GetIO().DeltaTime * 14.0f;       // smooth slide
    if (k > 1.0f) k = 1.0f;
    b.knobX += (targetX - b.knobX) * k;
    dl->AddCircleFilled(ImVec2(b.knobX, swY + swH * 0.5f), kR,
                        IM_COL32(245, 245, 245, 255), 36);

    gFltBtnRect[idx] = ImRect(wp, ImVec2(wp.x + W, wp.y + H));   // always tappable

    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

// Overlay view that eats only the touches landing on a floating button while
// the menu is hidden; everything else passes straight through to the game.
@interface FFOverlayView : MTKView
@end
@implementation FFOverlayView
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
    if (MenDeal)
        return [super hitTest:point withEvent:event];
    return FFPointOverFloating(point) ? self : nil;
}
@end

@interface ImGuiDrawView () <MTKViewDelegate>
@property (nonatomic, strong) id <MTLDevice> device;
@property (nonatomic, strong) id <MTLCommandQueue> commandQueue;
@end

@implementation ImGuiDrawView
ImFont *_espFont;
ImFont* verdanab;
ImFont* icons;
ImFont* interb;
ImFont* Urbanist;


- (instancetype)initWithNibName:(nullable NSString *)nibNameOrNil bundle:(nullable NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    _device = MTLCreateSystemDefaultDevice();
    _commandQueue = [_device newCommandQueue];

    if (!self.device) abort();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsClassic();
    auto& Style = ImGui::GetStyle();
    Style.WindowPadding = ImVec2(12.0f, 10.0f);
    Style.FramePadding = ImVec2(9.0f, 6.0f);
    Style.ScrollbarRounding = 9.0f;
            // ===== CICADA theme: ultra-sleek obsidian dark panels + neon cyan/violet accents =====
            ImVec4* colors = ImGui::GetStyle().Colors;
        const ImColor cyan(0, 224, 245);            // #00E0F5
        const ImColor cyanDim(0, 224, 245, 150);
        colors[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.058f, 0.085f, 0.96f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.065f, 0.070f, 0.100f, 0.99f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.075f, 0.080f, 0.115f, 0.65f);
        colors[ImGuiCol_Border] = ImVec4(0.00f, 0.88f, 0.96f, 0.55f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.110f, 0.120f, 0.165f, 0.88f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.150f, 0.190f, 0.250f, 0.95f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.180f, 0.250f, 0.320f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.045f, 0.050f, 0.075f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.060f, 0.070f, 0.105f, 1.00f);
        colors[ImGuiCol_CheckMark] = cyan.Value;
        colors[ImGuiCol_SliderGrab] = cyan.Value;
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.95f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.110f, 0.145f, 0.195f, 0.92f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.160f, 0.300f, 0.380f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.200f, 0.420f, 0.520f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.130f, 0.210f, 0.270f, 0.85f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.160f, 0.320f, 0.400f, 0.95f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.200f, 0.440f, 0.540f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.88f, 0.96f, 0.35f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.88f, 0.96f, 0.65f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.88f, 0.96f, 0.90f);
        colors[ImGuiCol_ResizeGrip] = cyanDim.Value;
        colors[ImGuiCol_ResizeGripHovered] = cyan.Value;
        colors[ImGuiCol_ResizeGripActive] = cyan.Value;
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ScrollbarGrab] = cyanDim.Value;
        colors[ImGuiCol_ScrollbarGrabHovered] = cyan.Value;
        colors[ImGuiCol_ScrollbarGrabActive] = cyan.Value;
        colors[ImGuiCol_Tab] = ImVec4(0.085f, 0.095f, 0.135f, 0.92f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.200f, 0.540f, 0.650f, 0.95f);
        colors[ImGuiCol_TabActive] = ImVec4(0.130f, 0.340f, 0.430f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.070f, 0.075f, 0.105f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.110f, 0.180f, 0.230f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.88f, 0.96f, 0.35f);
        colors[ImGuiCol_NavHighlight] = cyan.Value;
        colors[ImGuiCol_Text] = ImVec4(0.92f, 0.95f, 0.98f, 1.00f);

        ImGui::GetStyle().WindowRounding = 14.0f;
        ImGui::GetStyle().FrameRounding = 8.0f;
        ImGui::GetStyle().ChildRounding = 10.0f;
        ImGui::GetStyle().GrabRounding = 7.0f;
        ImGui::GetStyle().ScrollbarRounding = 8.0f;
        ImGui::GetStyle().TabRounding = 7.0f;
        ImGui::GetStyle().WindowBorderSize = 1.5f;
        ImGui::GetStyle().WindowPadding = ImVec2(14.0f, 12.0f);
        ImGui::GetStyle().ItemSpacing = ImVec2(10.0f, 8.0f);
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
    ImFont* font = io.Fonts->AddFontFromMemoryTTF(sansbold, sizeof(sansbold), 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    verdana_smol = io.Fonts->AddFontFromMemoryTTF(verdana, sizeof verdana, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());
    pixel_big = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 128, NULL, io.Fonts->GetGlyphRangesCyrillic());
    pixel_smol = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 10*2, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplMetal_Init(_device);

    // Create Metal texture for the uploaded CICADA icon (32x32 RGBA8)
    if (!gCicadaIconTex && _device) {
        MTLTextureDescriptor *td = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                      width:32
                                                                                     height:32
                                                                                  mipmapped:NO];
        td.usage = MTLTextureUsageShaderRead;
        td.storageMode = MTLStorageModeShared;
        gCicadaIconTex = [_device newTextureWithDescriptor:td];
        if (gCicadaIconTex) {
            [gCicadaIconTex replaceRegion:MTLRegionMake2D(0, 0, 32, 32)
                              mipmapLevel:0
                                withBytes:kCicadaIconRGBA
                              bytesPerRow:32 * 4];
        }
    }

    LoadAutoSaveConfig();

    return self;
}

+ (void)showChange:(BOOL)open
{
    MenDeal = open;
}

- (MTKView *)mtkView
{
    return (MTKView *)self.view;
}

- (void)loadView
{

 

    UIWindow *host = SafeHostWindow();
    CGSize sz = (host && host.rootViewController.view) ? host.rootViewController.view.frame.size : UIScreen.mainScreen.bounds.size;
    if (sz.width < 1 || sz.height < 1)
        sz = UIScreen.mainScreen.bounds.size;
    self.view = [[FFOverlayView alloc] initWithFrame:CGRectMake(0, 0, sz.width, sz.height)];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.mtkView.device = self.device;
    self.mtkView.delegate = self;
    self.mtkView.clearColor = MTLClearColorMake(0, 0, 0, 0);
    self.mtkView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
    self.mtkView.clipsToBounds = YES;

}



#pragma mark - Interaction

- (void)updateIOWithTouchEvent:(UIEvent *)event
{
    UITouch *anyTouch = event.allTouches.anyObject;
    CGPoint touchLocation = [anyTouch locationInView:self.view];
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(touchLocation.x, touchLocation.y);

    BOOL hasActiveTouch = NO;
    for (UITouch *touch in event.allTouches)
    {
        if (touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseCancelled)
        {
            hasActiveTouch = YES;
            break;
        }
    }
    io.MouseDown[0] = hasActiveTouch;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

#pragma mark - MTKViewDelegate

- (void)drawInMTKView:(MTKView*)view
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = view.bounds.size.width;
    io.DisplaySize.y = view.bounds.size.height;

    CGFloat framebufferScale = view.window.screen.nativeScale ?: UIScreen.mainScreen.nativeScale;
    io.DisplayFramebufferScale = ImVec2(framebufferScale, framebufferScale);
    io.DeltaTime = 1 / float(view.preferredFramesPerSecond ?: 60);
    
    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
        
        if (MenDeal == true) 
        {
            [self.view setUserInteractionEnabled:YES];
            
        } 
        else if (MenDeal == false) 
        {
           
            // FFOverlayView.hitTest decides per touch whether the game (nil)
            // or the overlay (self) receives it, so the floating feature
            // buttons stay tappable while the menu is closed.
            [self.view setUserInteractionEnabled:YES];
           

        }

        MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;
        if (renderPassDescriptor != nil)
        {
            id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            [renderEncoder pushDebugGroup:@"ImGui Jane"];

            ImGui_ImplMetal_NewFrame(renderPassDescriptor);
            ImGui::NewFrame();
    UIWindow *menuHost = SafeHostWindow();
    CGSize menuSize = (menuHost && menuHost.rootViewController.view) ? menuHost.rootViewController.view.frame.size : UIScreen.mainScreen.bounds.size;
    CGFloat x = (menuSize.width - 410) / 2;
    CGFloat y = (menuSize.height - 360) / 2;
    disp.width = io.DisplaySize.x;   // was never set -> Arrow ESP bounds were wrong
    disp.height = io.DisplaySize.y;
            // Always draw the small floating CICADA toggle icon
            DrawMenuIconButton();

            // floating buttons: GHIR li n-active-hom f l'panel (gFltShow —
            // user: "mnin n'active hadi → tla3 lya l'boutonat f l'écran");
            // hidden → rect mrqyad = tap kolchi l l'game
            for (int i = 0; i < FLTBTN_COUNT; i++)
            {
                if (!gFltShow[i])
                {
                    gFltBtnRect[i] = ImRect(0, 0, 0, 0); // ma kaykheddmich
                    continue;
                }
                switch (i)
                {
                case FLTBTN_TELEKILL:
                    DrawFloatingButton(i, oxorany("Telekill"), &Vars.Telekill);
                    break;
                case FLTBTN_FLYV2:
                    DrawFloatingButton(i, oxorany("Fly V2"), &Vars.Flyingtest);
                    break;
                case FLTBTN_RUNN:
                    DrawFloatingButton(i, oxorany("Runn"), &Vars.fly);
                    break;
                case FLTBTN_AIMKILL:
                    DrawFloatingButton(i, oxorany("AimKill"), &Vars.AimKillSend);
                    break;
                case FLTBTN_STOPTIME:
                    DrawFloatingButton(i, oxorany("Stop Time"), &Vars.StopTime);
                    break;
                case FLTBTN_FLYOP:
                    DrawFloatingButton(i, oxorany("Fly Op"), &Vars.FlyOp);
                    break;
                case FLTBTN_FLYJUP:
                    DrawFloatingButton(i, oxorany("Fly Jump"), &Vars.FlyJump);
                    break;
                case FLTBTN_AUTOMARK:
                    DrawFloatingButton(i, oxorany("Auto Mark"), &Vars.AutoMark);
                    break;
                case FLTBTN_TELEMARK:
                    DrawFloatingButton(i, oxorany("Tele Mark"), &Vars.TeleMark);
                    break;
                case FLTBTN_DOWNKILL2:
                    DrawFloatingButton(i, oxorany("Down Kill V2"), &Vars.DownKillV2);
                    break;
                case FLTBTN_GHOST:
                    DrawFloatingButton(i, oxorany("Ghost"), &Vars.Ghost);
                    break;
                case FLTBTN_UNDERGROUND:
                    DrawFloatingButton(i, oxorany("Under"), &Vars.UnderGround);
                    break;
                case FLTBTN_AUTOUNDER:
                    DrawFloatingButton(i, oxorany("Auto Under"), &Vars.AutoUnder);
                    break;
                }
            }
     ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(410, 360), ImGuiCond_FirstUseEver);
            if (MenDeal == true)
            {                
                // design: loun dyal l'panel (SETTINGS) + border/tab/accent tinted
                ImVec4 accVec(g_accentCol[0], g_accentCol[1], g_accentCol[2], 1.0f);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(g_panelCol[0], g_panelCol[1], g_panelCol[2], g_panelCol[3]));
                ImGui::PushStyleColor(ImGuiCol_Border, PanelBright(0.45f));
                ImGui::PushStyleColor(ImGuiCol_TabActive, PanelBright(0.28f));
                ImGui::PushStyleColor(ImGuiCol_TabHovered, PanelBright(0.42f));
                ImGui::PushStyleColor(ImGuiCol_CheckMark, accVec);
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, accVec);
                ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, accVec);

                ImGui::Begin(oxorany("  CICADA  |  FreeFire 1.132.X  ##cicada_main"), &MenDeal, ImGuiWindowFlags_NoCollapse);

                // Custom CICADA Banner Header with embedded logo + neon accent divider
                {
                    ImDrawList *wdl = ImGui::GetWindowDrawList();
                    ImVec2 p = ImGui::GetCursorScreenPos();
                    float availW = ImGui::GetContentRegionAvail().x;
                    float bannerH = 38.0f;
                    ImU32 accU32 = IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), 255);
                    ImU32 accBgL = IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), 45);
                    ImU32 accBgR = IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), 8);
                    wdl->AddRectFilledMultiColor(p, ImVec2(p.x + availW, p.y + bannerH), accBgL, accBgR, accBgR, accBgL);
                    wdl->AddRect(p, ImVec2(p.x + availW, p.y + bannerH), IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), 90), 8.0f, 0, 1.0f);
                    wdl->AddRectFilled(p, ImVec2(p.x + 4.0f, p.y + bannerH), accU32, 4.0f);

                    float textX = p.x + 12.0f;
                    if (gCicadaIconTex != nil) {
                        wdl->AddImageRounded((ImTextureID)((uintptr_t)gCicadaIconTex),
                                             ImVec2(p.x + 10.0f, p.y + 5.0f),
                                             ImVec2(p.x + 38.0f, p.y + 33.0f),
                                             ImVec2(0, 0), ImVec2(1, 1),
                                             IM_COL32(255, 255, 255, 255), 6.0f);
                        textX = p.x + 46.0f;
                    }
                    wdl->AddText(ImVec2(textX, p.y + 5.0f), accU32, oxorany("CICADA ENGINE"));
                    wdl->AddText(ImVec2(textX, p.y + 20.0f), IM_COL32(180, 195, 210, 215), oxorany("VIP EDITION  •  v1.132.X"));

                    // Status pill on the right
                    const char *badge = oxorany("ONLINE");
                    ImVec2 bsz = ImGui::CalcTextSize(badge);
                    ImVec2 bp(p.x + availW - bsz.x - 20.0f, p.y + (bannerH - bsz.y) * 0.5f);
                    wdl->AddRectFilled(ImVec2(bp.x - 8.0f, bp.y - 3.0f), ImVec2(bp.x + bsz.x + 8.0f, bp.y + bsz.y + 3.0f), IM_COL32(25, 190, 95, 45), 6.0f);
                    wdl->AddCircleFilled(ImVec2(bp.x - 1.0f, bp.y + bsz.y * 0.5f), 3.0f, IM_COL32(55, 245, 120, 255));
                    wdl->AddText(ImVec2(bp.x + 4.0f, bp.y), IM_COL32(80, 255, 140, 255), badge);

                    ImGui::Dummy(ImVec2(availW, bannerH + 4.0f));
                }

                auto DrawSection = [](const char *title) {
                    ImDrawList *sdl = ImGui::GetWindowDrawList();
                    ImVec2 sp = ImGui::GetCursorScreenPos();
                    float sw = ImGui::GetContentRegionAvail().x;
                    ImU32 acc = IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), 240);
                    ImU32 bg = IM_COL32((int)(g_accentCol[0] * 255), (int)(g_accentCol[1] * 255), (int)(g_accentCol[2] * 255), 28);
                    sdl->AddRectFilled(sp, ImVec2(sp.x + sw, sp.y + 21.0f), bg, 5.0f);
                    sdl->AddRectFilled(sp, ImVec2(sp.x + 3.0f, sp.y + 21.0f), acc, 2.0f);
                    sdl->AddText(ImVec2(sp.x + 10.0f, sp.y + 3.0f), acc, title);
                    ImGui::Dummy(ImVec2(sw, 23.0f));
                };

                if (ImGui::BeginTabBar(oxorany("Tab"), ImGuiTabBarFlags_FittingPolicyResizeDown)) { // resize-down: kolchi tab yban daiman (ma kat7bet)
                    if (ImGui::BeginTabItem(("  ESP  "))) {
                        ImGui::Spacing();
                        DrawSection(oxorany("VISUALS & RADAR"));
                        ImGui::Checkbox(oxorany("Enable Master ESP"), &Vars.Enable);
                        ImGui::Separator();
                        if (ImGui::BeginTable("split", 3))
                        {
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Line ESP"), &Vars.lines);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Box ESP"), &Vars.Box);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Health ESP"), &Vars.Health);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Name ESP"), &Vars.Name);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Bone ESP"), &Vars.skeleton);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Distance"), &Vars.Distance);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Circle ESP"), &Vars.circlepos);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Outline"), &Vars.Outline);
                            ImGui::TableNextColumn();
                            ImGui::Checkbox(oxorany("Arrow ESP"), &Vars.OOF);
                        }
                        ImGui::EndTable();
                        ImGui::Checkbox(oxorany("Enemy Counter HUD"), &Vars.enemycount);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem((" AIMBOT "))) {
                        ImGui::Spacing();
                        DrawSection(oxorany("AIM ASSISTANCE"));
                        ImGui::Checkbox(oxorany("Enable Aimbot"), &Vars.Aimbot);
                        ImGui::Combo(oxorany("Trigger Mode"), &Vars.AimWhen, Vars.dir, 4);
                        ImGui::SliderFloat(oxorany("FOV Radius"), &Vars.AimFov, 0.0f, 500.0f, "%.0f px");
                        ImGui::Spacing();
                        DrawSection(oxorany("FOV CIRCLE STYLE"));
                        ImGui::Checkbox(oxorany("Glow FOV Circle"), &Vars.fovaimglow);
                        if (Vars.fovaimglow) {
                            ImGui::ColorEdit4(oxorany("FOV Color"), Vars.fovLineColor);
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem((" FEATURES "))) {
                        ImGui::Spacing();
                        DrawSection(oxorany("MOVEMENT & FLIGHT"));
                        if (ImGui::Checkbox(oxorany("Button FLY V2"), &Vars.Flyingtest))
                            gFltShow[FLTBTN_FLYV2] = Vars.Flyingtest; // panel = sataf dyal l'button f écran
                        ImGui::SliderFloat(oxorany("Fly Height"), &Vars.flyhei, 0.0f, 1000.0f, "%.f");
                        ImGui::TextDisabled("Zone: %d  |  r: %.0f  |  phase: %d", flyZSt, flyZR, flyZPhase);
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button RUNN"), &Vars.fly)) gFltShow[FLTBTN_RUNN] = Vars.fly;
                        ImGui::SliderFloat(oxorany("Run Speed"), &Vars.flySpeed, 0.0f, 100.0f, "%.f");
                        ImGui::Spacing();
                        DrawSection(oxorany("COMBAT & TELEPORT"));
                        if (ImGui::Checkbox(oxorany("Button Telekill"), &Vars.Telekill)) gFltShow[FLTBTN_TELEKILL] = Vars.Telekill;
                        ImGui::SliderFloat(oxorany("Telekill Dist"), &Vars.khoangcach, 2.0f, 50.0f, "%.f m");
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button AimKill"), &Vars.AimKillSend)) gFltShow[FLTBTN_AIMKILL] = Vars.AimKillSend;
                        ImGui::TextDisabled("Aik: st:%d  n:%d  s:%d  c:%d", aikDbgSt, aikDbgN, aikDbgSent, aikDbgC);
                        ImGui::Spacing();
                        DrawSection(oxorany("QUICK FLOATING BUTTONS"));
                        if (ImGui::Checkbox(oxorany("Button Stop Time"), &Vars.StopTime)) gFltShow[FLTBTN_STOPTIME] = Vars.StopTime;
                        if (ImGui::BeginTable("splitm", 3))
                        {
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Fly Op"), &Vars.FlyOp))
                                gFltShow[FLTBTN_FLYOP] = Vars.FlyOp;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Fly Jump"), &Vars.FlyJump))
                                gFltShow[FLTBTN_FLYJUP] = Vars.FlyJump;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Tele Mark"), &Vars.TeleMark)) gFltShow[FLTBTN_TELEMARK] = Vars.TeleMark;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Auto Mark"), &Vars.AutoMark)) gFltShow[FLTBTN_AUTOMARK] = Vars.AutoMark;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Down V2"), &Vars.DownKillV2)) gFltShow[FLTBTN_DOWNKILL2] = Vars.DownKillV2;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Ghost"), &Vars.Ghost)) gFltShow[FLTBTN_GHOST] = Vars.Ghost;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Under Ground"), &Vars.UnderGround)) gFltShow[FLTBTN_UNDERGROUND] = Vars.UnderGround;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Auto Under"), &Vars.AutoUnder)) gFltShow[FLTBTN_AUTOUNDER] = Vars.AutoUnder;
                        }
                        ImGui::EndTable();
                        ImGui::SliderFloat(oxorany("Under Depth"), &Vars.UgDepth, 1.0f, 6.0f, "%.1f m");
                        ImGui::TextDisabled("Auto Under = starts automatically when round begins (phase 2)");
                        ImGui::TextDisabled("Stop Time = freeze | Down = ta7t l'ard | Ghost = underground");
                        ImGui::TextDisabled("Mark: %d  |  ST: %.5f", g_markDbg, g_stDeltaShow);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem((" SETTINGS "))) {
                        ImGui::Spacing();
                        DrawSection(oxorany("AUTO SAVE & CONFIG"));
                        if (ImGui::Checkbox("Auto Save Settings (All Options & Buttons)", &g_autoSave)) {
                            SaveAutoSaveConfigNow();
                        }
                        if (ImGui::Button("Save Settings Now")) {
                            SaveAutoSaveConfigNow();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Reset Default Settings")) {
                            ResetDefaultConfig();
                        }
                        ImGui::Spacing();
                        DrawSection(oxorany("CICADA THEME CUSTOMIZER"));
                        ImGui::ColorEdit4("Panel Background", g_panelCol);
                        ImGui::ColorEdit4("Accent Neon", g_accentCol);
                        ImGui::Spacing();
                        ImGui::Text("Theme Presets:");
                        struct ThemePreset { const char *name; ImVec4 bg; ImVec4 acc; };
                        static const ThemePreset kPresets[5] = {
                            {"Cicada Cyber", {0.06f, 0.06f, 0.09f, 0.96f}, {0.00f, 0.88f, 0.96f, 1.00f}},
                            {"Amethyst VIP", {0.08f, 0.05f, 0.12f, 0.96f}, {0.75f, 0.32f, 1.00f, 1.00f}},
                            {"Crimson Blood",{0.10f, 0.05f, 0.06f, 0.96f}, {1.00f, 0.22f, 0.35f, 1.00f}},
                            {"Emerald Venom",{0.05f, 0.09f, 0.07f, 0.96f}, {0.15f, 0.95f, 0.55f, 1.00f}},
                            {"Gold Royal",   {0.09f, 0.08f, 0.05f, 0.96f}, {1.00f, 0.78f, 0.18f, 1.00f}},
                        };
                        for (int pi = 0; pi < 5; pi++)
                        {
                            if (pi) ImGui::SameLine();
                            char pid[24];
                            snprintf(pid, sizeof(pid), "##pre%d", pi);
                            if (ImGui::ColorButton(pid, kPresets[pi].acc, ImGuiColorEditFlags_NoBorder, ImVec2(30, 26)))
                            {
                                for (int pc = 0; pc < 4; pc++) {
                                    g_panelCol[pc]  = (&kPresets[pi].bg.x)[pc];
                                    g_accentCol[pc] = (&kPresets[pi].acc.x)[pc];
                                }
                            }
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(("  INFO  "))) {
                        ImGui::Spacing();
                        DrawSection(oxorany("ABOUT CICADA"));
                        ImGui::TextColored(ImVec4(g_accentCol[0], g_accentCol[1], g_accentCol[2], 1.00f), "CICADA VIP PANEL");
                        ImGui::Text("Target: FreeFire 1.132.X");
                        ImGui::Spacing();
                        DrawSection(oxorany("SECURITY & ACCOUNT"));
                        ImGui::TextColored(ImVec4(0.35f, 0.95f, 0.45f, 1.00f), "SAFE FONCTIONS (crash-free):");
                        ImGui::TextDisabled("ESP / Aimbot - client only (safest)");
                        ImGui::TextDisabled("Fly V2 / Fly Op / Fly Jump / Stop Time / Ghost / Tele Mark");
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1.00f, 0.55f, 0.25f, 1.00f), "HIGH RISK (ki9esso l'players):");
                        ImGui::TextDisabled("AimKill / Telekill / Down Kill V2");
                        ImGui::Spacing();
                        ImGui::Checkbox("RESET GUEST (unlock game option + MSDK reset)", &Vars.ResetGuest);
                        if (ImGui::Button("Force Reset Guest Account Now")) {
                            Vars.ResetGuest = true;
                            ExecuteDirectGuestReset();
                        }
                        if (g_rgState == 1)
                            ImGui::TextColored(ImVec4(0.35f, 0.95f, 0.45f, 1.00f), "Reset Guest Active: option 'Reset Guest' unlocked + MSDK ready");
                        else if (g_rgState == -1)
                            ImGui::TextDisabled("MSDK Direct Reset ready (use Force Reset button)");
                        else
                            ImGui::TextDisabled("config: initializing...");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
        ImGui::End();
        ImGui::PopStyleColor(7);
    }
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            get_players();
            aimbot();
            game_sdk->init();
            // ported features — after init() so every function pointer is set
            RunFly();
            RunFlyV2();
            RunFlyOp(io.DeltaTime); // FLY OP (user snippet, dt-based rise)
            RunFlyJump(io.DeltaTime); // FLY JUMP (Elite HighJump: jump held + lift)
            RunStopTime();
            RunDownKillV2();
            RunGhost();
            RunUnderGround();
            RunMarkTP();
            RunTelekill();
            StartAimkill();
            RunResetGuest(); // Reset Guest config (login/lobby — runs always)
            CheckAutoSaveConfig();
            if (Vars.AimFov > 0) {
                Vars.isAimFov = true;
            } else {
                Vars.isAimFov = false;
            }
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();
            ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);
          
            [renderEncoder popDebugGroup];
            [renderEncoder endEncoding];

            [commandBuffer presentDrawable:view.currentDrawable];
        }

        [commandBuffer commit];
}

- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size
{
    
}

@end

