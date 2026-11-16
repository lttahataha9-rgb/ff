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
// button around. COLOR: OFF = look qdim (cyan border, 7amra label, byd knob);
// ON → lounha ybdel (Fly=7amra, Runn/Telekill=mauve, Send=khadra — user:
// "mnin ndire liha ON itbdel lounha"). Panel = plain checkbox (machi hada
// li brit itbdel lounha).
enum { FLTBTN_TELEKILL = 0, FLTBTN_FLYV2 = 1, FLTBTN_RUNN = 2, FLTBTN_AIMKILL = 3, FLTBTN_COUNT = 4 };
static bool gFltShow[FLTBTN_COUNT] = { false, false, false, false }; // panel checkbox latch

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
    ImVec4(0.78f, 0.35f, 1.00f, 1.00f), // TELEKILL — mauve
    ImVec4(1.00f, 0.25f, 0.25f, 1.00f), // FLYV2    — 7amra
    ImVec4(0.78f, 0.35f, 1.00f, 1.00f), // RUNN     — mauve
    ImVec4(0.25f, 1.00f, 0.45f, 1.00f), // AIMKILL  — khadra
};

static void DrawFloatingButton(int idx, const char *label, bool *v)
{
    // Visible ghir mnin n-active-ha f l'panel (gFltShow). OFF = look qdim
    // (border cyan, label 7amra, knob byd); ON → kolchi ybdel l'loun dyalha
    // (kFltOnCol) — user: "mnin ndire liha ON itbdel lounha".
    FltBtnState &b = gFltBtn[idx];
    const float W = 50.0f, H = 46.0f;   // user exact: "50 46" (qbel 80x74 → 104x96)
    if (!b.inited) {
        b.inited = true;
        ImVec2 ds = ImGui::GetIO().DisplaySize;
        b.pos = ImVec2(ds.x - W - 12.0f, 64.0f + idx * (H + 12.0f));   // higher up
    }

    // window look: OFF = qdim (cyan), ON = loun dyalha (bg/border — onPre)
    ImGui::SetNextWindowPos(b.pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(W, H), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    const ImVec4 &oc = kFltOnCol[idx];
    const bool onPre = *v; // style pushed before Begin → 1-frame lag (ghir 16ms)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, onPre
        ? ImVec4(0.05f + oc.x * 0.10f, 0.07f + oc.y * 0.10f, 0.10f + oc.z * 0.10f, 0.72f)
        : ImVec4(0.05f, 0.07f, 0.10f, 0.62f));
    ImGui::PushStyleColor(ImGuiCol_Border, onPre ? oc : ImVec4(0.31f, 0.90f, 1.00f, 1.00f));

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

    // label: OFF = 7amra (look qdim), ON = loun dyal l'button — auto-fit font
    // bach ma ykhrejch mn l'button sghir (50x46)
    ImFont *fnt = ImGui::GetFont();
    float fs = ImGui::GetFontSize();
    ImVec2 ts = fnt->CalcTextSizeA(fs, FLT_MAX, 0.0f, label);
    if (ts.x > W - 6.0f && fs > 9.0f)
    {
        fs = fs * (W - 6.0f) / ts.x;
        ts = fnt->CalcTextSizeA(fs, FLT_MAX, 0.0f, label);
    }
    ImU32 col32 = on ? IM_COL32((int)(oc.x * 255.0f), (int)(oc.y * 255.0f), (int)(oc.z * 255.0f), 255)
                     : IM_COL32(255, 64, 64, 255);
    dl->AddText(fnt, fs, ImVec2(wp.x + (W - ts.x) * 0.5f, wp.y + 4.0f), col32, label);

    // knob SLIDES left (OFF) / right (ON) — scaled l button sghir (50x46)
    const float margin = 4.0f, r = 9.0f;
    float targetX = on ? (wp.x + W - margin - r) : (wp.x + margin + r);
    if (b.knobX <= 0.0f) b.knobX = targetX;              // first frame: snap
    float k = ImGui::GetIO().DeltaTime * 14.0f;          // smooth slide
    if (k > 1.0f) k = 1.0f;
    b.knobX += (targetX - b.knobX) * k;
    dl->AddCircleFilled(ImVec2(b.knobX, wp.y + H * 0.66f), r,
                        on ? col32 : IM_COL32(245, 245, 245, 255), 36);

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
    Style.WindowPadding = ImVec2(8.0f, 8.0f);
    Style.FramePadding = ImVec2(9.0f, 7.0f);
    Style.ScrollbarRounding = 9.0f;
            // ===== 4xCRYPTED theme: dark panels + cyan accents =====
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
    CGFloat y = (menuSize.height - 260) / 2;
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
                case FLTBTN_AIMKILL:
                    DrawFloatingButton(i, oxorany("Send"), &Vars.AimKillSend);
                    break;
                }
            }
     ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(365, 270), ImGuiCond_FirstUseEver);
            if (MenDeal == true)
            {                
                ImGui::Begin(oxorany("  4xCRYPTED  |  FreeFire 1.132.X"), &MenDeal);
                if (ImGui::BeginTabBar(oxorany("Tab"),ImGuiTabBarFlags_FittingPolicyScroll)) {
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
                        if (ImGui::Checkbox(oxorany("Button FLY V2"), &Vars.Flyingtest)) gFltShow[FLTBTN_FLYV2] = Vars.Flyingtest; // panel = sataf dyal l'button f écran
                        ImGui::SliderFloat(oxorany("Fly Height"), &Vars.flyhei, 0.0f, 1000.0f, "%.f");
                        ImGui::Text("Zone: %d r:%.0f p:%d", flyZSt, flyZR, flyZPhase); // st7=arc rise, st6=p ma zal Fight, st5=scene null
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button RUNN"), &Vars.fly)) gFltShow[FLTBTN_RUNN] = Vars.fly;
                        ImGui::SliderFloat(oxorany("Runn?"), &Vars.flySpeed, 0.0f, 100.0f, "%.f");
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("Button Telekill"), &Vars.Telekill)) gFltShow[FLTBTN_TELEKILL] = Vars.Telekill;
                        ImGui::SliderFloat(oxorany("DistanceTelekill"), &Vars.khoangcach, 2.0f, 50.0f, "%.f");
                        ImGui::Spacing();
                        if (ImGui::Checkbox(oxorany("AimKill Send"), &Vars.AimKillSend)) gFltShow[FLTBTN_AIMKILL] = Vars.AimKillSend;
                        ImGui::Text("Aik: st:%d n:%d s:%d", aikDbgSt, aikDbgN, aikDbgSent); // st9=sent, s>0=packets (zexisyy st1-12)
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(("INFO"))) {
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.31f, 0.90f, 1.00f, 1.00f), "4xCRYPTED");
                        ImGui::Text("FreeFire 1.132.X");
                        ImGui::TextDisabled("Telekill / Fly V2 / Ninja Run");
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
        ImGui::End();
    }
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            get_players();
            draw_watermark();
            aimbot();
            game_sdk->init();
            // ported features — after init() so every function pointer is set
            RunFly();
            RunFlyV2();
            RunTelekill();
            StartAimkill();
            // zexisyy: msg 108 (reload) — same frame as Aimkill, after local+enemy
            {
                void *aikM108 = game_sdk->Curent_Match();
                void *aikL108 = aikM108 ? game_sdk->GetLocalPlayer(aikM108) : nullptr;
                void *aikE108 = GetClosestEnemy();
                if (Vars.AimKillSend && aikL108 && aikE108)
                    Send108(aikL108, aikE108);
            }
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

