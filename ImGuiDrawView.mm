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
// Quick-toggle buttons over the game, styled like the cyan toggle from the
// screenshot: red label on top, white knob, cyan rounded border. A button
// shows only while its feature is ON; a plain tap switches it off, dragging
// moves the button around.
enum { FLTBTN_TELEKILL = 0, FLTBTN_FLYV2 = 1, FLTBTN_RUNN = 2, FLTBTN_COUNT = 3 };

struct FltBtnState {
    ImVec2 pos = ImVec2(0, 0);
    bool inited = false;
    bool moved = false;
    bool pressed = false;
    float travel = 0.0f;
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

static void DrawFloatingButton(int idx, const char *label, bool *v)
{
    if (!*v) { gFltBtnRect[idx] = ImRect(); return; }

    FltBtnState &b = gFltBtn[idx];
    const float W = 104.0f, H = 96.0f;
    if (!b.inited) {
        b.inited = true;
        ImVec2 ds = ImGui::GetIO().DisplaySize;
        b.pos = ImVec2(ds.x - W - 14.0f, 130.0f + idx * (H + 14.0f));
    }

    ImGui::SetNextWindowPos(b.pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(W, H), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.5f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.07f, 0.10f, 0.62f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.31f, 0.90f, 1.00f, 1.00f));

    char wname[48];
    snprintf(wname, sizeof(wname), "##fltwin%d", idx);
    ImGui::Begin(wname, nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImVec2 wp = ImGui::GetWindowPos();

    // red label, top center (like "Ghost" in the screenshot)
    ImVec2 ts = ImGui::CalcTextSize(label);
    dl->AddText(ImVec2(wp.x + (W - ts.x) * 0.5f, wp.y + 10.0f),
                IM_COL32(255, 64, 64, 255), label);

    // white knob
    dl->AddCircleFilled(ImVec2(wp.x + W * 0.5f, wp.y + H * 0.63f), 15.0f,
                        IM_COL32(245, 245, 245, 255), 36);

    ImGui::SetCursorPos(ImVec2(0, 0));
    char bname[48];
    snprintf(bname, sizeof(bname), "##fltbtn%d", idx);
    ImGui::InvisibleButton(bname, ImVec2(W, H));

    ImGuiIO &io = ImGui::GetIO();
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        if (!b.pressed) { b.pressed = true; b.moved = false; b.travel = 0.0f; }
        b.travel += fabsf(io.MouseDelta.x) + fabsf(io.MouseDelta.y);
        if (b.travel > 6.0f) b.moved = true;
        if (b.moved) b.pos += io.MouseDelta;
    }
    if (b.pressed && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (!b.moved) *v = false;    // plain tap = switch the feature off
        b.pressed = false; b.moved = false; b.travel = 0.0f;
    }

    gFltBtnRect[idx] = ImRect(wp, ImVec2(wp.x + W, wp.y + H));

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
            ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.17f, 0.17f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_ScrollbarGrab] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_ScrollbarGrabActive] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_SliderGrab] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_SliderGrabActive] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_Button] = ImVec4(0.24f, 0.24f, 0.24f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.73f, 0.73f, 0.73f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.65f, 0.65f, 0.65f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.72f, 0.72f, 0.72f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_ResizeGripHovered] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_ResizeGripActive] = ImColor(163, 122, 195).Value;
        colors[ImGuiCol_Tab] = ImVec4(0.17f, 0.17f, 0.17f, 0.86f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.11f, 0.11f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.59f, 0.11f, 0.11f, 0.35f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImColor(28, 28, 30);
        ImGui::GetStyle().Colors[ImGuiCol_Border] = ImColor(36, 36, 38);
        ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = ImColor(36, 36, 38);

        ImGui::GetStyle().WindowRounding = 8 / 1.5f;
        ImGui::GetStyle().FrameRounding = 4 / 1.5f;
        ImGui::GetStyle().ChildRounding = 6 / 1.5f;
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
     ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(365, 270), ImGuiCond_FirstUseEver);
            if (MenDeal == true)
            {                
                ImGui::Begin(oxorany("  @MODDER - FreeFire 1.132.X"), &MenDeal);
                if (ImGui::BeginTabBar(oxorany("Tab"),ImGuiTabBarFlags_FittingPolicyScroll)) {
                    if (ImGui::BeginTabItem(("Modder - ESP"))) {
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
                    if (ImGui::BeginTabItem(("Modder - Aimbot"))) {
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
                    if (ImGui::BeginTabItem(("Modder - Igg"))) {
                        ImGui::Spacing();
                        ImGui::Checkbox(oxorany("Button FLY V2"), &Vars.Flyingtest);
                        ImGui::SliderFloat(oxorany("Fly Height"), &Vars.flyhei, 0.0f, 80.0f, "%.f");
                        ImGui::Spacing();
                        ImGui::Checkbox(oxorany("Button RUNN"), &Vars.fly);
                        ImGui::SliderFloat(oxorany("Runn?"), &Vars.flySpeed, 0.0f, 100.0f, "%.f");
                        ImGui::Spacing();
                        ImGui::Checkbox(oxorany("Button Telekill"), &Vars.Telekill);
                        ImGui::SliderFloat(oxorany("DistanceTelekill"), &Vars.khoangcach, 2.0f, 50.0f, "%.f");
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(("Modder - Info"))) {


                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
        ImGui::End();
    }
            // floating quick-toggle buttons (visible while the feature is on)
            DrawFloatingButton(FLTBTN_TELEKILL, oxorany("Telekill"), &Vars.Telekill);
            DrawFloatingButton(FLTBTN_FLYV2, oxorany("Fly V2"), &Vars.Flyingtest);
            DrawFloatingButton(FLTBTN_RUNN, oxorany("Ninja Run"), &Vars.fly);
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            get_players();
            draw_watermark();
            aimbot();
            game_sdk->init();
            // ported features — after init() so every function pointer is set
            RunFly();
            RunFlyV2();
            RunTelekill();
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

