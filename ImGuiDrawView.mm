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
// RIOSSM port (user: "THYD 360 W SEND W AUTO MARK" + "TZID AUTO MARK 00 W KILL 360"
// + "TRD LYA L AUTO MARK KIF MA KAN"): Send / 360 mchaw, AUTO MARK 00 + KILL 360
// zado, w Auto Mark l-9dima RJA3T (user: "Rj33-ha m3a l-hop (7 -> 15)").
enum { FLTBTN_TELEKILL = 0, FLTBTN_FLYV2 = 1, FLTBTN_RUNN = 2, FLTBTN_STOPTIME = 3, FLTBTN_FLYOP = 4, FLTBTN_JUFLY = 5, FLTBTN_TELEMARK = 6, FLTBTN_AUTOMARK = 7, FLTBTN_DOWNCOVER = 8, FLTBTN_GHOST = 9, FLTBTN_FLYAUTO = 10, FLTBTN_AIMKILL = 11, FLTBTN_AUTOSWAP = 12, FLTBTN_COUNT = 13 };
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

static bool MenDeal = true;   // menu visibility (moved up: FFOverlayView needs it)

// Touch hit-testing: while the menu is closed the overlay must give the game
// every touch EXCEPT taps that land on a floating feature button.
static bool FFPointOverFloating(CGPoint p)
{
    ImVec2 pt((float)p.x, (float)p.y);
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
    ImVec4(1.00f, 0.82f, 0.25f, 1.00f), // STOPTIME - sfer (freeze)
    ImVec4(1.00f, 0.25f, 0.25f, 1.00f), // FLYOP    - 7amra (fly family, snippet)
    ImVec4(0.75f, 1.00f, 0.20f, 1.00f), // JUFLY    - chartreuse (jump = energy)
    ImVec4(0.25f, 0.60f, 1.00f, 1.00f), // TELEMARK - azr9 (teleport)
    ImVec4(1.00f, 0.55f, 0.05f, 1.00f), // AUTOMARK - orange (kif kan l-9dim)
    ImVec4(0.15f, 0.90f, 0.40f, 1.00f), // DOWNCOVER - khadra (cover)
    ImVec4(0.88f, 0.96f, 1.00f, 1.00f), // GHOST    - byd glaci (ice)
    ImVec4(1.00f, 0.35f, 0.70f, 1.00f), // FLYAUTO  - wardi (pink, fly family — RIOSSM FLYAUTO)
    // ---- loun RIOSSM ----
    ImVec4(0.00f, 0.85f, 1.00f, 1.00f), // AIMKILL    - cyan (zexisyy 106 + 108)
    ImVec4(1.00f, 0.75f, 0.10f, 1.00f), // AUTOSWAP  - asfar (weapon swap, Elite port)
};

// panel design: tab SETTINGS -> ColorEdit4 loun dyal l'panel (user nkhtar)
static float g_panelCol[4] = {0.11f, 0.12f, 0.16f, 0.95f};
// GLASS variant (user: "tbdl lya chkl l panel" -> glass/rounded): nafs l-tint
// dyal l-panel walakin b alpha m9ellel bach l-bg yban "semi-transparent"
// (glass) 3la wara l'game.
static ImVec4 GlassCol(float k, float a)
{
    return ImVec4(
        g_panelCol[0] + (1.0f - g_panelCol[0]) * k,
        g_panelCol[1] + (1.0f - g_panelCol[1]) * k,
        g_panelCol[2] + (1.0f - g_panelCol[2]) * k,
        a);
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
    // ===== AUTO SAVE -> CRASH FIX (user: "mnin dakchi kytsava w kan-restarti
    // kaytra7 crash ktra") =====
    // ImGui's AUTO SAVE = io.IniFilename = "imgui.ini" (l-default, imgui.cpp
    // L1408). Kay-khdem b tariqten automatiques:
    //   - NewFrame() -> UpdateSettings(): kay-QRA l-file f l-frame l-awal
    //     (imgui.cpp L14441 LoadIniSettingsFromDisk) — hadi f RESTART.
    //   - 5 s ba3d kol modification (window move / resize / tab reorder /
    //     table resize) kay-KTEB-h   (imgui.cpp L14452 SaveIniSettingsToDisk)
    //     — hadi f l-PLAY, hwa "dkchi kytsava".
    // F ESign/iOS l-file kay-ysala7 (write interrupted mn l-app kill / partial
    // write) w l-parse dyalo f restart kay-tri9 -> CRASH qbel mn ma l-panel
    // yban kamel. n-3tqlo KAMEL:
    //   io.IniFilename = NULL -> LA save LA load (ImGui manual mode;
    //   WantSaveIniSettings kay-b9a true w kan-tgehdo, harmless).
    // l-file l-qdim n-hiyedo bach ma-yqrah-ch abadan walou.
    io.IniFilename = NULL;
    ::unlink("imgui.ini"); // <unistd.h> (L48) — n-hiyed l-file l-qdim

   ImGui::StyleColorsClassic();
    auto& Style = ImGui::GetStyle();
    Style.WindowPadding = ImVec2(8.0f, 8.0f);
    Style.FramePadding = ImVec2(9.0f, 7.0f);
    Style.ScrollbarRounding = 9.0f;
            // ===== base theme: dark panels + cyan accents =====
            ImVec4* colors = ImGui::GetStyle().Colors;
        const ImColor cyan(79, 228, 255);            // #4FE4FF
        const ImColor cyanDim(79, 228, 255, 150);
        colors[ImGuiCol_WindowBg] = ImVec4(0.045f, 0.050f, 0.062f, 0.97f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.055f, 0.060f, 0.075f, 0.99f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.060f, 0.065f, 0.080f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.31f, 0.90f, 1.00f, 0.55f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.110f, 0.120f, 0.150f, 0.85f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.150f, 0.180f, 0.220f, 0.90f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.180f, 0.230f, 0.280f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.060f, 0.070f, 0.090f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.075f, 0.090f, 0.115f, 1.00f);
        colors[ImGuiCol_CheckMark] = cyan.Value;
        colors[ImGuiCol_SliderGrab] = cyan.Value;
        colors[ImGuiCol_SliderGrabActive] = cyan.Value;
        colors[ImGuiCol_Button] = ImVec4(0.110f, 0.140f, 0.170f, 0.90f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.160f, 0.280f, 0.330f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.200f, 0.400f, 0.470f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.130f, 0.200f, 0.240f, 0.80f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.160f, 0.300f, 0.360f, 0.90f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.200f, 0.420f, 0.500f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.31f, 0.90f, 1.00f, 0.35f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.31f, 0.90f, 1.00f, 0.60f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.31f, 0.90f, 1.00f, 0.85f);
        colors[ImGuiCol_ResizeGrip] = cyanDim.Value;
        colors[ImGuiCol_ResizeGripHovered] = cyan.Value;
        colors[ImGuiCol_ResizeGripActive] = cyan.Value;
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ScrollbarGrab] = cyanDim.Value;
        colors[ImGuiCol_ScrollbarGrabHovered] = cyanDim.Value;
        colors[ImGuiCol_ScrollbarGrabActive] = cyan.Value;
        colors[ImGuiCol_Tab] = ImVec4(0.090f, 0.100f, 0.125f, 0.90f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.250f, 0.560f, 0.650f, 0.90f);
        colors[ImGuiCol_TabActive] = ImVec4(0.140f, 0.300f, 0.360f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.070f, 0.075f, 0.090f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.110f, 0.150f, 0.180f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.31f, 0.90f, 1.00f, 0.35f);
        colors[ImGuiCol_NavHighlight] = cyan.Value;
        colors[ImGuiCol_Text] = ImVec4(0.86f, 0.90f, 0.94f, 1.00f);

        ImGui::GetStyle().WindowRounding = 12.0f;
        ImGui::GetStyle().FrameRounding = 7.0f;
        ImGui::GetStyle().ChildRounding = 10.0f;
        ImGui::GetStyle().GrabRounding = 6.0f;
        ImGui::GetStyle().ScrollbarRounding = 8.0f;
        ImGui::GetStyle().TabRounding = 6.0f;
        ImGui::GetStyle().WindowBorderSize = 1.5f;
        ImGui::GetStyle().WindowPadding = ImVec2(12.0f, 10.0f);
        ImGui::GetStyle().ItemSpacing = ImVec2(10.0f, 8.0f);
    ImFont* font = io.Fonts->AddFontFromMemoryTTF(sansbold, sizeof(sansbold), 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    verdana_smol = io.Fonts->AddFontFromMemoryTTF(verdana, sizeof verdana, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());
    pixel_big = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 128, NULL, io.Fonts->GetGlyphRangesCyrillic());
    pixel_smol = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 10*2, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplMetal_Init(_device);

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
    CGFloat x = (menuSize.width - 380) / 2;
    CGFloat y = (menuSize.height - 340) / 2;
    disp.width = io.DisplaySize.x;   // was never set -> Arrow ESP bounds were wrong
    disp.height = io.DisplaySize.y;
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
                case FLTBTN_AIMKILL: // zexisyy Aimkill Send (msg 106 + 108)
                    DrawFloatingButton(i, oxorany("AIMKILL"), &Vars.AimKill);
                    break;
                case FLTBTN_STOPTIME:
                    DrawFloatingButton(i, oxorany("Stop Time"), &Vars.StopTime);
                    break;
                case FLTBTN_FLYOP:
                    DrawFloatingButton(i, oxorany("Fly Op"), &Vars.FlyOp);
                    break;
                case FLTBTN_JUFLY: // JU FLY option REMOVED (user) — slot kept for index alignment
                    break;
                case FLTBTN_TELEMARK:
                    DrawFloatingButton(i, oxorany("Tele Mark"), &Vars.TeleMark);
                    break;
                case FLTBTN_AUTOMARK: // restored (user "Rj33-ha m3a l-hop")
                    DrawFloatingButton(i, oxorany("Auto Mark"), &Vars.AutoMark);
                    break;
                case FLTBTN_DOWNCOVER:
                    DrawFloatingButton(i, oxorany("DOWN COVER"), &Vars.DownCover);
                    break;
                case FLTBTN_GHOST:
                    DrawFloatingButton(i, oxorany("Ghost"), &Vars.Ghost);
                    break;
                case FLTBTN_FLYAUTO:
                    DrawFloatingButton(i, oxorany("FLYAUTO"), &Vars.FlyAuto); // RIOSSM label
                    break;
                case FLTBTN_AUTOSWAP:
                    DrawFloatingButton(i, oxorany("AUTO SWAP"), &Vars.AutoSwap);
                    break;
                }
            }
     ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(365, 340), ImGuiCond_FirstUseEver);
            if (MenDeal == true)
            {                
                // ===== GLASS / ROUNDED panel (user: "tbdl lya chkl l panel") =====
                // rounded corners + semi-transparent tinted bg (glass) + pill
                // tabs + rounded frames/sliders.  Loun l-base = g_panelCol
                // (sala mn SETTINGS) -> l-panel 3ando design jdid w ma fihach
                // "4xCRYPTED" / "FreeFire" f l-titre.
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 9.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 8.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 10.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 11.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 7.0f));
                ImGui::PushStyleColor(ImGuiCol_WindowBg, GlassCol(0.00f, 0.84f));      // glass
                ImGui::PushStyleColor(ImGuiCol_ChildBg, GlassCol(0.06f, 0.42f));       // inner glass
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.00f, 1.00f, 1.00f, 0.18f)); // soft white edge
                ImGui::PushStyleColor(ImGuiCol_TitleBg, GlassCol(0.04f, 0.90f));
                ImGui::PushStyleColor(ImGuiCol_TitleBgActive, GlassCol(0.12f, 0.95f));
                ImGui::PushStyleColor(ImGuiCol_Tab, GlassCol(0.10f, 0.62f));
                ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.31f, 0.90f, 1.00f, 0.45f));
                ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.31f, 0.90f, 1.00f, 0.30f));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.00f, 1.00f, 1.00f, 0.10f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.31f, 0.90f, 1.00f, 0.20f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.31f, 0.90f, 1.00f, 0.30f));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.31f, 0.90f, 1.00f, 0.24f));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.31f, 0.90f, 1.00f, 0.34f));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.31f, 0.90f, 1.00f, 0.44f));
                ImGui::PushStyleColor(ImGuiCol_PopupBg, GlassCol(0.05f, 0.97f));
                ImGui::Begin(oxorany("  PANEL  "), &MenDeal);
                if (ImGui::BeginTabBar(oxorany("Tab"), ImGuiTabBarFlags_FittingPolicyResizeDown)) { // resize-down: kolchi tab yban daiman (ma kat7bet)
                    if (ImGui::BeginTabItem(("ESP"))) {
                    ImGui::Checkbox(oxorany("ESP master"), &Vars.Enable);
                        if (ImGui::BeginTable("split", 4))
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
                    ImGui::Checkbox(oxorany("Distance ESP"), &Vars.Distance);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(oxorany("Circle ESP"), &Vars.circlepos);
                    ImGui::TableNextColumn();
                    ImGui::Checkbox(oxorany("Outline ESP"), &Vars.Outline);
                        }
                        ImGui::EndTable();
                        ImGui::Checkbox(oxorany("Arrow ESP"), &Vars.OOF);ImGui::SameLine();
                        ImGui::Checkbox(oxorany("Enemy Count"), &Vars.enemycount);
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(("AIMBOT"))) {
                            ImGui::Spacing();
                            ImGui::Checkbox(oxorany("Aimbot"), &Vars.Aimbot);
                            ImGui::Combo(oxorany("Trigger"), &Vars.AimWhen, Vars.dir, 4);
                            ImGui::SliderFloat(oxorany("Radio FOV"), &Vars.AimFov, 0.0f, 500.0f);
                            ImGui::Checkbox(oxorany("Glow FOV"), &Vars.fovaimglow);
                            if (Vars.fovaimglow) {
                                ImGui::ColorEdit4(oxorany("Color FOV"), Vars.fovLineColor);
                            }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(("FEATURES"))) {
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button FLY V2"), &Vars.Flyingtest))
                            gFltShow[FLTBTN_FLYV2] = Vars.Flyingtest; // panel = sataf dyal l'button f écran
                        ImGui::SliderFloat(oxorany("Fly Height"), &Vars.flyhei, 0.0f, 1000.0f, "%.f");
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button RUNN"), &Vars.fly)) gFltShow[FLTBTN_RUNN] = Vars.fly;
                        ImGui::SliderFloat(oxorany("Runn?"), &Vars.flySpeed, 0.0f, 100.0f, "%.f");
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button Telekill"), &Vars.Telekill)) gFltShow[FLTBTN_TELEKILL] = Vars.Telekill;
                        ImGui::SliderFloat(oxorany("DistanceTelekill"), &Vars.khoangcach, 2.0f, 50.0f, "%.f");
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("AIMKILL"), &Vars.AimKill)) gFltShow[FLTBTN_AIMKILL] = Vars.AimKill; // zexisyy msg 106 + 108 (RIOSSM 360 removed)
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button Stop Time"), &Vars.StopTime)) gFltShow[FLTBTN_STOPTIME] = Vars.StopTime; // own option (user: bouhdo)
                        if (ImGui::BeginTable("splitm", 4))
                        {
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Fly Op"), &Vars.FlyOp))
                                gFltShow[FLTBTN_FLYOP] = Vars.FlyOp; // panel = latch dyal l'button f ecran
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Tele Mark"), &Vars.TeleMark)) gFltShow[FLTBTN_TELEMARK] = Vars.TeleMark;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Auto Mark"), &Vars.AutoMark)) gFltShow[FLTBTN_AUTOMARK] = Vars.AutoMark;
                        }
                        ImGui::EndTable();
                        // ported round 3 (user: "thyd Down Kill V2 w zid DOWN COVER") + GHOST
                        if (ImGui::BeginTable("splitn", 2))
                        {
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Button DOWN COVER"), &Vars.DownCover)) gFltShow[FLTBTN_DOWNCOVER] = Vars.DownCover;
                            ImGui::TableNextColumn();
                            if (ImGui::Checkbox(oxorany("Button Ghost"), &Vars.Ghost)) gFltShow[FLTBTN_GHOST] = Vars.Ghost;
                        }
                        ImGui::EndTable();
                        // ---- FLY AUTO (RIOSSM 261 "flyair") — labels dyalo ----
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button FLYAUTO"), &Vars.FlyAuto)) gFltShow[FLTBTN_FLYAUTO] = Vars.FlyAuto;
                        ImGui::SliderFloat(oxorany("SPEED AUTO"), &Vars.FlyAutoSpeed, 0.0f, 0.5f, "%.2f v"); // RIOSSM: 0..0.5, default 0.110
                        ImGui::SliderFloat(oxorany("DELAY AUTO"), &Vars.FlyAutoDelay, 0.0f, 10.0f, "%.1f v"); // RIOSSM: 0..10 s, default 1.8 -> auto-off
                        ImGui::Spacing();
                        // AUTO SWAP (Elite port — SWAP_WEAPON.txt): slots 1<->2 kol 12 ms
                        if (ImGui::Checkbox(oxorany("Button AUTO SWAP"), &Vars.AutoSwap)) gFltShow[FLTBTN_AUTOSWAP] = Vars.AutoSwap;
                        ImGui::Spacing();
                        ImGui::TextDisabled("Stop Time = freeze / DOWN COVER = nozol+flip / Ghost = underground");
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(("SETTINGS"))) {
                        ImGui::Spacing();
                        ImGui::Text("Loun dyal l'panel (design):");
                        ImGui::ColorEdit4("Panel Color", g_panelCol);
                        ImGui::Text("Presets:");
                        static const ImVec4 kPresets[5] = {
                            {0.11f, 0.12f, 0.16f, 0.95f}, // dark navy (default)
                            {0.85f, 0.92f, 1.00f, 0.95f}, // ice / byd
                            {0.10f, 0.55f, 0.75f, 0.95f}, // cyan
                            {0.15f, 0.55f, 0.25f, 0.95f}, // green
                            {0.65f, 0.15f, 0.20f, 0.95f}, // red
                        };
                        for (int pi = 0; pi < 5; pi++)
                        {
                            if (pi) ImGui::SameLine();
                            char pid[16];
                            snprintf(pid, sizeof(pid), "##pre%d", pi);
                            if (ImGui::ColorButton(pid, kPresets[pi], ImGuiColorEditFlags_NoBorder, ImVec2(26, 26)))
                            {
                                for (int pc = 0; pc < 4; pc++)
                                    g_panelCol[pc] = (&kPresets[pi].x)[pc];
                            }
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(("INFO"))) {
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.35f, 0.95f, 0.45f, 1.00f), "SAFE FONCTIONS (crash-free):");
                        ImGui::TextDisabled("ESP / Aimbot - client only (safest)");
                        ImGui::TextDisabled("Fly V2 / Fly Op / Stop Time / Ghost / Tele Mark / Auto Mark");
                        ImGui::TextDisabled("FLY AUTO = SPEED AUTO rise, DELAY AUTO s then auto-off + anti-fall (ex-JU FLY method)");
                        ImGui::TextDisabled("offsets fail-closed: chet -> ghir matkhdemch");
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(1.00f, 0.55f, 0.25f, 1.00f), "BAN RISK (ki9esso l'players):");
                        ImGui::TextDisabled("AIMKILL / Telekill / Down Cover");
                        ImGui::TextDisabled("AIMKILL = msg 106 + 108, damage = dm l-3douwwa");
                        ImGui::TextDisabled("+15 m hop: Tele Mark / Auto Mark / Telekill");
                        ImGui::Spacing();
                        ImGui::Checkbox("RESET GUEST (unlock game option)", &Vars.ResetGuest);
                        if (g_rgState == 1)
                            ImGui::TextDisabled("config OK: ON -> option 'Delete Guest'");
                        else if (g_rgState == -1)
                            ImGui::TextDisabled("config unavailable (il2cpp API)");
                        else
                            ImGui::TextDisabled("config: ... (retry)");
                        ImGui::TextDisabled("f l'game -> CONFIRM -> OFF 3awd");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
        ImGui::End();
        ImGui::PopStyleColor(15); // GLASS color stack (see PushStyleColor above)
        ImGui::PopStyleVar(8);    // GLASS rounding/padding stack
    }
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            get_players();
            draw_watermark();
            aimbot();
            game_sdk->init();
            // ported features — after init() so every function pointer is set
            RunFly();
            RunFlyV2();
            RunFlyOp(io.DeltaTime); // FLY OP (user snippet, dt-based rise)
            RunFlyAuto(io.DeltaTime); // FLY AUTO (RIOSSM flyair): rise + DELAY auto-off
            RunGroundedCheat(); // JU FLY (user snippet: juflyYy) -> isGrounded = true
            RunStopTime();
            RunDownCover();
            RunGhost();
            RunMarkTP();
            RunAutoMark00(io.DeltaTime); // AUTO MARK 00 (RIOSSM autounderground): delay -> approach
            RunAutoSwap(); // AUTO SWAP (Elite port): weapon slots 1<->2 every ~12 ms
            RunTelekill();
            StartAimkill(); // AIMKILL (zexisyy msg 106 + 108) — gated on Vars.AimKill
            RunResetGuest(); // Reset Guest config (login/lobby — runs always)
            // zexisyy msg108 (Send) REMOVED — user: "THYD ... W SEND"
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

