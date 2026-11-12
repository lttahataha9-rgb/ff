#import "ImGuiLoad.h"
#import "ImGuiDrawView.h"

// Safe window lookup: windows[0] can throw NSRangeException when the app
// has no window yet -> that was the crash when opening the panel.
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
@interface ImGuiLoad()
@property (nonatomic, strong) ImGuiDrawView *vna;
@end
@implementation ImGuiLoad

+ (instancetype)share
{
    static ImGuiLoad *tool;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        tool = [[ImGuiLoad alloc] init];
    });
    return tool;
}

- (void)show
{
    if (!_vna) {
        ImGuiDrawView *vc = [[ImGuiDrawView alloc] init];
        _vna = vc;
    }


    [ImGuiDrawView showChange:true];
    UIWindow *host = SafeHostWindow();
    if (host.rootViewController.view) [host.rootViewController.view addSubview:_vna.view];
}

- (void)hide
{
    if (!_vna) {
        ImGuiDrawView *vc = [[ImGuiDrawView alloc] init];
        _vna = vc;
    }
    
    [ImGuiDrawView showChange:false];
    UIWindow *host = SafeHostWindow();
    if (host.rootViewController.view) [host.rootViewController.view addSubview:_vna.view];
}

@end
