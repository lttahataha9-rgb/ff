
#import "PubgLoad.h"
#import <UIKit/UIKit.h>

#import "JHPP.h"
#import "JHDragView.h"
#import "ImGuiLoad.h"
#import "ImGuiDrawView.h"
@interface PubgLoad()
@property (nonatomic, strong) ImGuiDrawView *vna;
@end

@implementation PubgLoad

static PubgLoad *extraInfo;

UIWindow *mainWindow;

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


+ (void)load
{
    [super load];

    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(6 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        mainWindow = [UIApplication sharedApplication].keyWindow;
        extraInfo = [PubgLoad new];
        // Mount the overlay view immediately; the floating CICADA icon inside
        // ImGuiDrawView toggles the menu on/off with a single tap.
        [extraInfo tapIconView2];
    });
}

-(void)initTapGes
{
    

    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] init];
    tap.numberOfTapsRequired = 2;//点击次数
    tap.numberOfTouchesRequired = 3;//手指数
    [[JHPP currentViewController].view addGestureRecognizer:tap];
    [tap addTarget:self action:@selector(tapIconView)];
}

-(void)initTapGes2
{
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] init];
    tap.numberOfTapsRequired = 2;//点击次数
    tap.numberOfTouchesRequired = 2;//手指数
    [[JHPP currentViewController].view addGestureRecognizer:tap];
    [tap addTarget:self action:@selector(tapIconView2)];
}

-(void)tapIconView2
{
 if (!_vna) {
     ImGuiDrawView *vc = [[ImGuiDrawView alloc] init];
     _vna = vc;
 }
 
 [ImGuiDrawView showChange:false];
 UIWindow *host = SafeHostWindow();
 if (host.rootViewController.view) [host.rootViewController.view addSubview:_vna.view];
}

-(void)tapIconView
{
 if (!_vna) {
     ImGuiDrawView *vc = [[ImGuiDrawView alloc] init];
     _vna = vc;
 }
 
 [ImGuiDrawView showChange:true];
 UIWindow *host = SafeHostWindow();
 if (host.rootViewController.view) [host.rootViewController.view addSubview:_vna.view];
}
@end
