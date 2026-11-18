#import "vinhtran.hpp"
#import "loading.hxx"
#include <fstream>
#define FMT_HEADER_ONLY
#include "fmt/core.h"

struct Vars_t
{
    bool Enable = {};
    bool Aimbot = {};
    float AimFov = {};
    int AimCheck = {};
    int AimType = {};
    int AimWhen = {};
    bool isAimFov = {};
    const char *dir[4] = {"always when", "when shootes", "when scopes", "when shoots i scopes"};
    bool lines = {};
    bool Box = {};
    bool Outline = {};
    bool Name = {};
    bool Health = {};
    bool Distance = {};
    bool fovaimglow = {};
    bool circlepos = {};
    bool skeleton = {};
    bool OOF = {};
    bool enemycount = {};
    float fovLineColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    ImVec4 boxColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float AimSpeed = 100000000.0f;

    // ---- ported from X96 2Ripper: Telekill / Fly V2 / Ninja Run ----
    bool Telekill = {};
    bool AimKillSend = {}; // SEND button: crafts the game's own C2S damage packet (Android EliteXzp port)
    bool fly = {};        // RUNN (ninja run)
    bool Flyingtest = {}; // Fly V2
    float flySpeed = 0.75f;
    float flyhei = 300.0f; // FlyV2 sky altitude (video ref = f l'jouj; 0 = ground zone center)
    float khoangcach = 10.0f;
    // ---- ported from Android EliteXzp (the "Elite" project) ----
    bool FlyUltra = {};   // Fly Hack V2: endless climb +0.2/tick ("li kantla3o bih")
    bool StopTime = {};   // Speed Timer freeze: m_FixedDeltaTime -> 0.00001 ("stop time")
    bool TeleMark = {};   // Tele Mark: one-shot TP to my own map mark
    bool AutoMark = {};   // Auto TP to the mark at Fight start (phase==2) + 1.5 s pin
    float fuSpeed = 0.20f; // Fly Ultra: chhal ytla3 f wa7d tick (slider, default Elite 0.20)
    float fuMaxM = 7.0f;   // Fly Ultra: chhal mn mitro ytla3 (slider; l9as l'cap -> auto TP l'mark)
    // ---- ported from Android EliteXzp round 2 (user: "DOWN KILL + V2, FLY FIX, GHOST") ----
    bool DownKill = {};    // DOWN KILL: -1.8 m/f on me + all enemies in 200 m
    bool DownKillV2 = {};  // DOWN KILL V2: smooth -0.4/f descent to -2.5 m, restore off
    bool FixFly = {};      // FIX FLY (Elite Speed Timer V2): m_FixedDeltaTime curve
    int fixFlyVal = 1;     // FIX FLY slider 1..20 (Elite seekbar 5992)
    bool Ghost = {};       // GHOST (Elite Ghost Hack V4): underground + WaitForForceSync
} Vars;

class game_sdk_t
{
public:
    void init();
    int (*GetHp)(void *player);
    void *(*Curent_Match)();
    void *(*GetLocalPlayer)(void *Game);
    void *(*GetHeadPositions)(void *player);
    Vector3 (*get_position)(void *player);
    void *(*Component_GetTransform)(void *player);
    void *(*get_camera)();
    Vector3 (*WorldToViewpoint)(void*, Vector3, int);
    bool (*get_isVisible)(void *player);
    bool (*get_isLocalTeam)(void *player);
    bool (*get_IsDieing)(void *player);
    int (*get_MaxHP)(void *player);
    Vector3 (*GetForward)(void *player);
    void (*set_aim)(void *, Quaternion look);
    bool (*get_IsSighting)(void *player);
    bool (*get_IsFiring)(void *player);
    monoString *(*name)(void *player);
    void *(*_GetHeadPositions)(void *);
    void *(*_newHipMods)(void *);
    void *(*_GetLeftAnkleTF)(void *);
    void *(*_GetRightAnkleTF)(void *);
    void *(*_GetLeftToeTF)(void *);
    void *(*_GetRightToeTF)(void *);
    void *(*_getLeftHandTF)(void *);
    void *(*_getRightHandTF)(void *);
    void *(*_getLeftForeArmTF)(void *);
    void *(*_getRightForeArmTF)(void *);
};

game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init()
{
    // ======================== [تم التحديث: GetHp] ========================
    // القديم: 0x54A4CA4
    // الجديد: 0x56AA7C8
    this->GetHp = (int (*)(void *))getRealOffset(oxo("0x56AA7C8"));

    // ======================== [تم التحديث: Curent_Match] ========================
    // القديم: 0x5997610
    // الجديد: 0x591B898
    this->Curent_Match = (void *(*)())getRealOffset(oxo("0x591B898"));

    // ======================== [تم التحديث: GetLocalPlayer] ========================
    // القديم: 0x304E1A4
    // الجديد: 0x320FF7C
    this->GetLocalPlayer = (void *(*)(void *))getRealOffset(oxo("0x320FF7C"));

    // ======================== [تم التحديث: GetHeadPositions] ========================
    // القديم: 0x54C43D4
    // الجديد: 0x56CA948
    this->GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x56CA948"));

    // ======================== [تم التحديث: get_position] ========================
    // القديم: 0x929B730
    // الجديد: 0x8D6C1EC
    this->get_position = (Vector3(*)(void *))getRealOffset(oxo("0x8D6C1EC"));

    // ======================== [تم التحديث: Component_GetTransform] ========================
    // القديم: 0x9289418
    // الجديد: 0x8D59C2C
    this->Component_GetTransform = (void *(*)(void *))getRealOffset(oxo("0x8D59C2C"));

    // ======================== [تم التحديث: get_camera] ========================
    // القديم: 0x922F64C
    // الجديد: 0x8CFF0B4
    this->get_camera = (void *(*)())getRealOffset(oxo("0x8CFF0B4"));

    // ======================== [تم التحديث: WorldToViewpoint] ========================
    // القديم: 0x922EFCC
    // الجديد: 0x8CFEA34
    this->WorldToViewpoint = (Vector3(*)(void*, Vector3, int))getRealOffset(oxo("0x8CFEA34"));

    // ======================== [تم التحديث: get_isVisible] ========================
    // القديم: 0x543737C
    // الجديد: 0x563B10C
    this->get_isVisible = (bool (*)(void *))getRealOffset(oxo("0x563B10C"));

    // ======================== [تم التحديث: get_isLocalTeam] ========================
    // القديم: 0x5450FE4
    // الجديد: 0x5655F1C
    this->get_isLocalTeam = (bool (*)(void *))getRealOffset(oxo("0x5655F1C"));

    // ======================== [تم التحديث: get_IsDieing] ========================
    // القديم: 0x5418A3C
    // الجديد: 0x561C768
    this->get_IsDieing = (bool (*)(void *))getRealOffset(oxo("0x561C768"));

    // ======================== [تم التحديث: get_MaxHP] ========================
    // القديم: 0x54A4DB4
    // الجديد: 0x5613110
    this->get_MaxHP = (int (*)(void *))getRealOffset(oxo("0x5613110"));

    // ======================== [تم التحديث: GetForward] ========================
    // القديم: 0x929C128
    // الجديد: 0x8D6CBE4
    this->GetForward = (Vector3(*)(void *))getRealOffset(oxo("0x8D6CBE4"));

    // ======================== [تم التحديث: set_aim] ========================
    // القديم: 0x5433004
    // الجديد: 0x5636F80
    this->set_aim = (void (*)(void *, Quaternion))getRealOffset(oxo("0x5636F80"));

    // ======================== [تم التحديث: get_IsSighting] ========================
    // القديم: 0x5425F84
    // الجديد: 0x5629c84
    this->get_IsSighting = (bool (*)(void *))getRealOffset(oxo("0x5629c84"));

    // ======================== [تم التحديث: get_IsFiring] ========================
    // القديم: 0x541B560
    // الجديد: 0x56200F0
    this->get_IsFiring = (bool (*)(void *))getRealOffset(oxo("0x56200F0"));

    // ======================== [تم التحديث: name] ========================
    // القديم: 0x542D3A4
    // الجديد: 0x5630F10
    this->name = (monoString * (*)(void *player)) getRealOffset(oxo("0x5630F10"));

    // ======================== [تم التحديث: _GetHeadPositions] ========================
    // القديم: 0x54C43D4
    // الجديد: 0x56CA948
    this->_GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x56CA948"));

    // ======================== [تم التحديث: _newHipMods] ========================
    // القديم: 0x54C4584
    // الجديد: 0x56CAAF8
    this->_newHipMods = (void *(*)(void *))getRealOffset(oxo("0x56CAAF8"));

    // ======================== [تم التحديث: _GetLeftAnkleTF] ========================
    // القديم: 0x54C49D4
    // الجديد: 0x56CAF48
    this->_GetLeftAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x56CAF48"));

    // ======================== [تم التحديث: _GetRightAnkleTF] ========================
    // القديم: 0x54C4AE0
    // الجديد: 0x56CB054
    this->_GetRightAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x56CB054"));

    // ======================== [تم التحديث: _GetLeftToeTF] ========================
    // القديم: 0x54C4BEC
    // الجديد: 0x56CB160
    this->_GetLeftToeTF = (void *(*)(void *))getRealOffset(oxo("0x56CB160"));

    // ======================== [تم التحديث: _GetRightToeTF] ========================
    // القديم: 0x54C4CF8
    // الجديد: 0x56CB280
    this->_GetRightToeTF = (void *(*)(void *))getRealOffset(oxo("0x56CB280"));

    // ======================== [تم التحديث: _getLeftHandTF] ========================
    // القديم: 0x54320D8
    // الجديد: 0x5635F18
    this->_getLeftHandTF = (void *(*)(void *))getRealOffset(oxo("0x5635F18"));

    // ======================== [تم التحديث: _getRightHandTF] ========================
    // القديم: 0x54321DC
    // الجديد: 0x563601C
    this->_getRightHandTF = (void *(*)(void *))getRealOffset(oxo("0x563601C"));

    // ======================== [تم التحديث: _getLeftForeArmTF] ========================
    // القديم: 0x54322E0
    // الجديد: 0x5636120
    this->_getLeftForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x5636120"));

    // ======================== [تم التحديث: _getRightForeArmTF] ========================
    // القديم: 0x54323E4
    // الجديد: 0x5636224
    this->_getRightForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x5636224"));
}

namespace Camera$$WorldToScreen
{
ImVec2 Regular(Vector3 pos) {
    auto cam = game_sdk->get_camera();
    if (!cam) return {0,0};

    Vector3 worldPoint = game_sdk->WorldToViewpoint(cam,pos, 2);
    Vector3 location;

    int ScreenWidth = ImGui::GetIO().DisplaySize.x;
    int ScreenHeight = ImGui::GetIO().DisplaySize.y;

    location.x = ScreenWidth * worldPoint.x;
    location.y = ScreenHeight - worldPoint.y * ScreenHeight;
    location.z  = worldPoint.z;

    return {location.x, location.y};
}

ImVec2 Checker(Vector3 pos, bool &checker) {
    auto cam = game_sdk->get_camera();
    if (!cam) return {0, 0};
   
    Vector3 worldPoint = game_sdk->WorldToViewpoint(cam,pos, 4);
    Vector3 location;
 
    int ScreenWidth = ImGui::GetIO().DisplaySize.x;
    int ScreenHeight = ImGui::GetIO().DisplaySize.y;
 
    location.x = ScreenWidth * worldPoint.x;
    location.y = ScreenHeight - worldPoint.y * ScreenHeight;
    location.z = worldPoint.z;
 
    checker = location.z > 1;
 
    return {location.x, location.y};
}
}

Vector3 GetBonePosition(void *player, void *(*transformGetter)(void *)) {
    if (!player || !transformGetter)
        return Vector3();
    void *transform = transformGetter(player);
    return transform ? game_sdk->get_position(game_sdk->Component_GetTransform(transform)) : Vector3();
}

Vector3 GetHitboxPosition(void* player, int hitbox) {
    if (!player) return Vector3::zero();
    
    switch (hitbox) {
        case 0: return GetBonePosition(player, game_sdk->GetHeadPositions);
        case 1: {
            Vector3 headPos = GetBonePosition(player, game_sdk->GetHeadPositions);
            return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.1f, headPos.z);
        }
        case 2: {
            Vector3 headPos = GetBonePosition(player, game_sdk->GetHeadPositions);
            return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.2f, headPos.z);
        }
        default: return GetBonePosition(player, game_sdk->GetHeadPositions);
    }
}

Vector3 getPosition(void *player) {
    return game_sdk->get_position(game_sdk->Component_GetTransform(player));
}

Vector3 GetHeadPosition(void *player) {
    return game_sdk->get_position(game_sdk->GetHeadPositions(player));
}

static Vector3 CameraMain(void *player) {
    // ======================== [تم التحديث: CameraMain Offset] ========================
    // القديم: 0x388
    // الجديد: 0x3e8
    return game_sdk->get_position(*(void **)((uint64_t)player + oxo("0x3e8")));//public Transform MainCameraTransform;
}

Quaternion GetRotationToTheLocation(Vector3 Target, float Height, Vector3 MyEnemy) {
    Vector3 direction = (Target + Vector3(0, Height, 0)) - MyEnemy;
    return Quaternion::LookRotation(direction, Vector3(0, 1, 0));
}

Quaternion GetCurrentRotation(void* player) {
    void* transform = game_sdk->Component_GetTransform(player);
    if (!transform) return Quaternion();
    return Quaternion::LookRotation(game_sdk->GetForward(transform), Vector3(0, 1, 0));
}

#include "Helper/Ext.h"

class tanghinh {
public:
    static Vector3 Transform_GetPosition(void *player) {
       Vector3 out = Vector3::zero();
        // ======================== [تم التحديث: Transform_GetPosition] ========================
        // القديم: 0x929B794
        // الجديد: 0x8d6c250
        void (*_Transform_GetPosition)(void *transform, Vector3 *out) = (void (*)(void *, Vector3 *))getRealOffset(oxo("0x8d6c250"));//private void get_position_Injected(out Vector3 ret) { }
        _Transform_GetPosition(player, &out);
        return out;
    }

    static void *Player_GetHeadCollider(void *player)
    {
        // ======================== [تم التحديث: Player_GetHeadCollider] ========================
        // القديم: 0x54310FC
        // الجديد: 0x5634f3c
        void *(*_Player_GetHeadCollider)(void *players) = (void *(*)(void *))getRealOffset(oxo("0x5634f3c"));//public virtual Collider get_HeadCollider() { }
        return _Player_GetHeadCollider(player);
    }

    static bool Physics_Raycast(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider)
    {
        // ======================== [تم التحديث: Physics_Raycast] ========================
        // القديم: 0x605C004
        // الجديد: 0x6396de4
        bool (*_Physics_Raycast)(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider) = (bool (*)(Vector3, Vector3, unsigned int, void *))getRealOffset(oxo("0x6396de4"));//public class HPFKOGPDBBE // TypeDefIndex: 33080 public static bool FOHHPOKDOND(Vector3 OENCHMFCMMO, Vector3 MHAIPDKIHNL, uint CBCKBCPFOAI, ref GMPGMPFNMFP NKEPDPFIOFJ) { }
        return _Physics_Raycast(camLocation, headLocation, LayerID, collider);
    }

    static bool isVisible(void *enemy) {
        if (enemy != NULL) {
            void *hitObj = NULL;
            auto Camera = Transform_GetPosition(game_sdk->Component_GetTransform(game_sdk->get_camera()));
            auto Target = Transform_GetPosition(game_sdk->Component_GetTransform(Player_GetHeadCollider(enemy)));
            return !Physics_Raycast(Camera, Target, 12, &hitObj);
        }
        return false;
    }
};


void DrawSkeleton(void *player, ImDrawList *drawList)
{
    if (!player || !drawList)
        return;
    bool isPlayerVisible = tanghinh::isVisible(player);
    Vector3 headPos = GetBonePosition(player, game_sdk->_GetHeadPositions);
    Vector3 hipPos = GetBonePosition(player, game_sdk->_newHipMods);
    Vector3 leftAnklePos = GetBonePosition(player, game_sdk->_GetLeftAnkleTF);
    Vector3 rightAnklePos = GetBonePosition(player, game_sdk->_GetRightAnkleTF);
    Vector3 leftToePos = GetBonePosition(player, game_sdk->_GetLeftToeTF);
    Vector3 rightToePos = GetBonePosition(player, game_sdk->_GetRightToeTF);
    Vector3 leftHandPos = GetBonePosition(player, game_sdk->_getLeftHandTF);
    Vector3 rightHandPos = GetBonePosition(player, game_sdk->_getRightHandTF);
    Vector3 leftForeArmPos = GetBonePosition(player, game_sdk->_getLeftForeArmTF);
    Vector3 rightForeArmPos = GetBonePosition(player, game_sdk->_getRightForeArmTF);

    // Chuyển đổi vị trí xương sang tọa độ màn hình
    bool visible;
    ImVec2 headScreen = Camera$$WorldToScreen::Checker(headPos, visible);
    if (!visible)
        return;

    ImVec2 hipScreen = Camera$$WorldToScreen::Regular(hipPos);
    ImVec2 leftAnkleScreen = Camera$$WorldToScreen::Regular(leftAnklePos);
    ImVec2 rightAnkleScreen = Camera$$WorldToScreen::Regular(rightAnklePos);
    ImVec2 leftToeScreen = Camera$$WorldToScreen::Regular(leftToePos);
    ImVec2 rightToeScreen = Camera$$WorldToScreen::Regular(rightToePos);
    ImVec2 leftHandScreen = Camera$$WorldToScreen::Regular(leftHandPos);
    ImVec2 rightHandScreen = Camera$$WorldToScreen::Regular(rightHandPos);
    ImVec2 leftForeArmScreen = Camera$$WorldToScreen::Regular(leftForeArmPos);
    ImVec2 rightForeArmScreen = Camera$$WorldToScreen::Regular(rightForeArmPos);
    ImColor boneColor = isPlayerVisible ? ImColor(0, 255, 0) : ImColor(255, 255, 255);
    float thickness = 1.0f;

    // Vẽ đầu
    drawList->AddCircle(headScreen, 2.0f, boneColor, 12, thickness);
    // Vẽ thân
    drawList->AddLine(headScreen, hipScreen, boneColor, thickness);
    // Vẽ tay
    drawList->AddLine(headScreen, leftForeArmScreen, boneColor, thickness);
    drawList->AddLine(headScreen, rightForeArmScreen, boneColor, thickness);
    drawList->AddLine(leftForeArmScreen, leftHandScreen, boneColor, thickness);
    drawList->AddLine(rightForeArmScreen, rightHandScreen, boneColor, thickness);
    // Vẽ chân
    drawList->AddLine(hipScreen, leftAnkleScreen, boneColor, thickness);
    drawList->AddLine(hipScreen, rightAnkleScreen, boneColor, thickness);
    drawList->AddLine(leftAnkleScreen, leftToeScreen, boneColor, thickness);
    drawList->AddLine(rightAnkleScreen, rightToeScreen, boneColor, thickness);
}

bool isFov(Vector3 vec1, Vector3 vec2, int radius)
{
    int x = vec1.x;
    int y = vec1.y;
    int x0 = vec2.x;
    int y0 = vec2.y;
    if ((pow(x - x0, 2) + pow(y - y0, 2)) <= pow(radius, 2))
    {
        return true;
    }
    return false;
}

void *GetClosestEnemy()
{
    try
    {
        float shortestDistance = 9999.0f;
        void *closestEnemy = NULL;
        void *get_MatchGame = game_sdk->Curent_Match();
        if (!get_MatchGame)
            return NULL;
        void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
        if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
            return NULL;
        if (!Vars.Enable)
            return NULL;
        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)get_MatchGame + oxo("0x148"));
        if (!players )
            return NULL;
        for (int u = 0; u < players->getSize(); u++)
        {
            void *Player = players->getValues()[u];
            if (!Player)
                continue;
            if (Player == LocalPlayer)
                continue;
            if (!game_sdk->get_MaxHP(Player))
                continue;
            if (game_sdk->get_IsDieing(Player))
                continue;
            if (!game_sdk->get_isVisible(Player))
                continue;
            if (game_sdk->get_isLocalTeam(Player))
                continue;
            Vector3 PlayerPos = getPosition(Player);
            Vector3 LocalPlayerPos = getPosition(LocalPlayer);
            ImVec2 screenPos = Camera$$WorldToScreen::Regular(PlayerPos);
            bool isFov1 = isFov(Vector3(screenPos.x, screenPos.y), Vector3(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), Vars.AimFov);
            float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
            if (distance < 200)
            {
                Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
                float angle = Vector3::Angle(targetDir, game_sdk->GetForward(game_sdk->Component_GetTransform(game_sdk->get_camera()))) * 100.0f;
                if (angle <= Vars.AimFov && isFov1 && angle < shortestDistance)
                {
                    if (tanghinh::isVisible(Player))
                    {
                        shortestDistance = angle;
                        closestEnemy = Player;
                    }
                }
            }
        }
        return closestEnemy;
    }
    catch (...)
    {
        return NULL;
    }
}

// ==== AimKill target — INSTANT ("twali 00 second f l9etlat l'enemy") ========
// GetClosestEnemy kat7taj FOV circle + camera angle + Vars.Enable → kaykhli
// l'packet ytsenna l'crosshair ywsel 3la l'enemy (w AimFov=0 → hata kill).
// Hadi ghir closest by DISTANCE, bla ta gates — packet f nafs l'frame.
// Visibility: fully visible targets ONLY (cover port removed - user asked to
// strip it from the aimkill): hidden enemy = st=5, no kill, no packet - exactly
// the zexisyy behavior (server raycast must see him).
void *GetClosestEnemyAik()
{
    try
    {
        float shortestVisDistance = 9999.0f;
        void *closestVisEnemy = NULL;
        void *get_MatchGame = game_sdk->Curent_Match();
        if (!get_MatchGame)
            return NULL;
        void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
        if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
            return NULL;
        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)get_MatchGame + oxo("0x148"));
        if (!players)
            return NULL;
        for (int u = 0; u < players->getSize(); u++)
        {
            void *Player = players->getValues()[u];
            if (!Player)
                continue;
            if (Player == LocalPlayer)
                continue;
            if (!game_sdk->get_MaxHP(Player))
                continue;
            if (game_sdk->get_IsDieing(Player))
                continue;
            if (game_sdk->get_isLocalTeam(Player))
                continue;
            if (game_sdk->GetHp(Player) <= 0)
                continue;
            float distance = Vector3::Distance(getPosition(Player), getPosition(LocalPlayer));
            // cover/LOS gates REMOVED (user: "hyd mnha l'cover" + "0 second"):
            // closest enemy ANY time, even through cover
            if (distance < shortestVisDistance)
            {
                shortestVisDistance = distance;
                closestVisEnemy = Player;
            }
        }
        return closestVisEnemy;
    }
    catch (...)
    {
        return NULL;
    }
}

void ProcessAimbot()
{
    if (!Vars.Aimbot)
        return;
    void *CurrentMatch = game_sdk->Curent_Match();
    if (!CurrentMatch)
        return;
    void *LocalPlayer = game_sdk->GetLocalPlayer(CurrentMatch);
    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
        return;
    if (game_sdk->get_IsDieing(LocalPlayer))
        return;
    void *closestEnemy = GetClosestEnemy();
    if (!closestEnemy || !game_sdk->Component_GetTransform(closestEnemy))
        return;
    if (game_sdk->get_IsDieing(closestEnemy))
        return;
    if (!game_sdk->get_isVisible(closestEnemy))
        return;
    Vector3 EnemyLocation = GetHeadPosition(closestEnemy);
    if (EnemyLocation == Vector3::zero())
        return;
    Vector3 PlayerLocation = CameraMain(LocalPlayer);
  if (PlayerLocation == Vector3::zero())
        return;
    Quaternion PlayerLook = GetRotationToTheLocation(EnemyLocation, 0.1f, PlayerLocation);

    bool IsScopeOn = game_sdk->get_IsSighting(LocalPlayer);
    bool IsFiring = game_sdk->get_IsFiring(LocalPlayer);

    bool shouldAim =
        (Vars.AimWhen == 0) ||
        (Vars.AimWhen == 1 && IsFiring) ||
        (Vars.AimWhen == 2 && IsScopeOn);

    if (shouldAim)
    {
        game_sdk->set_aim(LocalPlayer, PlayerLook);
    }
}
void get_players()
{
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list)
        return;
    if (!Vars.Enable)
        return;

    try
    {
        ProcessAimbot();

        void *current_Match = game_sdk->Curent_Match();
        if (!current_Match)
            return;

        void *local_player = game_sdk->GetLocalPlayer(current_Match);
        if (!local_player)
            return;

        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)current_Match + 0x148);
        if (!players )
            return;

        void *camera = game_sdk->get_camera();
        if (!camera)
            return;

        for (int u = 0; u < players->getSize(); u++)
        {
            void *closestEnemy = players->getValues()[u];
            if (!closestEnemy)
                continue;
            if (!game_sdk->Component_GetTransform(closestEnemy))
                continue;
            if (closestEnemy == local_player)
                continue;
            if (!game_sdk->get_MaxHP(closestEnemy))
                continue;
            if (game_sdk->get_IsDieing(closestEnemy))
                continue;
            if (!game_sdk->get_isVisible(closestEnemy))
                continue;
            if (game_sdk->get_isLocalTeam(closestEnemy))
                continue;

            Vector3 pos = getPosition(closestEnemy);
            Vector3 pos2 = getPosition(local_player);
            float distance = Vector3::Distance(pos, pos2);
            if (distance > 200.0f)
                continue;
            ImColor line_color = ImColor(255, 255, 255);
            bool w2sc;
            ImVec2 top_pos = Camera$$WorldToScreen::Regular(pos + Vector3(0, 1.6, 0));
            ImVec2 bot_pos = Camera$$WorldToScreen::Regular(pos);
            ImVec2 pos_3 = Camera$$WorldToScreen::Checker(pos, w2sc);
            auto pmtXtop = top_pos.x;
            auto pmtXbottom = bot_pos.x;
            if (top_pos.x > bot_pos.x)
            {
                pmtXtop = bot_pos.x;
                pmtXbottom = top_pos.x;
            }
            Camera$$WorldToScreen::Checker(pos + Vector3(0, 0.75f, 0), w2sc);
            float calculatedPosition = fabs((top_pos.y - bot_pos.y) * (0.0092f / 0.019f) / 2);

            ImRect rect(
                ImVec2(pmtXtop - calculatedPosition, top_pos.y),
                ImVec2(pmtXbottom + calculatedPosition, bot_pos.y));
            void *camTF = game_sdk->Component_GetTransform(game_sdk->get_camera());
            Vector3 viewpos = camTF ? game_sdk->get_position(camTF) : Vector3::zero();
            if (w2sc)
            {
                if (Vars.lines)
                {
                    if (game_sdk->get_IsDieing(closestEnemy))
                    {
                        draw_list->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0), ImVec2(rect.GetCenter().x, rect.Min.y), ImColor(255, 0, 0));
                    }
                    else
                    {
                        draw_list->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0), ImVec2(rect.GetCenter().x, rect.Min.y), line_color);
                    }
                }
                if (Vars.Box)
                {
                    if (game_sdk->get_IsDieing(closestEnemy))
                    {
                        draw_list->AddRect(rect.Min, rect.Max, ImColor(255, 0, 0));
                    }
                    else
                    {
                        draw_list->AddRect(rect.Min, rect.Max, ImColor(255, 255, 255));
                    }
                    
                    if (Vars.Outline)
                    {
                        draw_list->AddRect(rect.Min - ImVec2(1, 1), rect.Max + ImVec2(1, 1), ImColor(0, 0, 0), 0.65, 0, 1);
                        draw_list->AddRect(rect.Min + ImVec2(1, 1), rect.Max - ImVec2(1, 1), ImColor(0, 0, 0), 0.65, 0, 1);
                    }
                }
                if (Vars.Name)
                {
                    auto pname = game_sdk->name(closestEnemy);
                    std::string names = "null";
                    if (pname)
                        names = pname->toCPPString();
                    std::transform(names.begin(), names.end(), names.begin(), ::tolower);
                    auto playername = names;
                    std::string name = names;
                    ImVec2 text_size = verdana_smol->CalcTextSizeA(8, FLT_MAX, 0, names.c_str());
                    ImVec2 name_pos = {
                        rect.Min.x + (rect.GetWidth() / 2) - text_size.x / 2,
                        rect.Min.y - 2 - text_size.y};
                    AddText(verdana_smol, 8, false, Vars.Outline, name_pos, ImColor(255, 255, 255), name);
                }
                if (Vars.Health)
                {
                    auto health = game_sdk->GetHp(closestEnemy);
                    auto maxhealth = game_sdk->get_MaxHP(closestEnemy);
                    float health_multiplier = (float)health / (float)maxhealth;
                    float health_bar_pos = rect.Min.x - 4;
                    draw_list->AddLine({health_bar_pos, rect.Min.y - 1}, {health_bar_pos, rect.Max.y}, ImColor(0, 0, 0, 100), 3);
                    draw_list->AddLine({health_bar_pos - 0.5f, rect.Max.y}, {health_bar_pos - 0.5f, rect.Max.y - (rect.GetHeight() + 1) * health_multiplier}, ImColor(0, 255, 0), 3);
                    if (Vars.Outline)
                        draw_list->AddRect({health_bar_pos - 2, rect.Min.y - 1}, {health_bar_pos + 2, rect.Max.y + 1}, ImColor(0, 0, 0));
                    std::string hpstr = fmt::format(oxorany("{}HP"), static_cast<int>(health));
                    ImVec2 text_size_hp = pixel_smol->CalcTextSizeA(8, FLT_MAX, 0, hpstr.c_str());
                    ImVec2 text_pos = {
                        rect.Min.x + (rect.GetWidth() / 2) - text_size_hp.x / 2,
                        rect.Max.y};
                    AddText(pixel_smol, 8, false, true, text_pos, ImColor(0, 255, 0), hpstr.c_str());
                }
                if (Vars.Distance)
                {
                    std::string distancestr = fmt::format(oxorany("{}M"), static_cast<int>(distance));
                    ImVec2 distance_pos = {
                        rect.Max.x + 4,
                        rect.Min.y};
                    AddText(pixel_smol, 8, false, true, distance_pos, ImColor(255, 255, 255), distancestr.c_str());
                }
                if (Vars.circlepos)
                {
                    Draw3DCircle(pos, 1.0f, 0.5f, ImColor(255, 0, 0), 36, false, 0.5f);
                }
                if (Vars.skeleton)
                {
                    DrawSkeleton(closestEnemy, draw_list);
                }
            }
            if (Vars.OOF)
            {
                if ((pos_3.x < 0 || pos_3.x > disp.width) || (pos_3.y < 0 || pos_3.y > disp.height) || !w2sc)
                {
                    constexpr int maxpixels = 150;
                    int pixels = maxpixels;
                    if (w2sc)
                    {
                        if (pos_3.x < 0)
                            pixels = clamp((int)-pos_3.x, 0, (int)maxpixels);
                        if (pos_3.y < 0)
                            pixels = clamp((int)-pos_3.y, 0, (int)maxpixels);

                        if (pos_3.x > disp.width)
                            pixels = clamp((int)pos_3.x - (int)disp.width, 0, (int)maxpixels);
                        if (pos_3.y > disp.height)
                            pixels = clamp((int)pos_3.y - (int)disp.height, 0, (int)maxpixels);
                    }

                    float opacity = (float)pixels / (float)maxpixels;

                    float size = 3.5f;
                    Vector3 viewdir = game_sdk->GetForward(game_sdk->Component_GetTransform(game_sdk->get_camera()));
                    Vector3 targetdir = Vector3::Normalized(pos - viewpos);

                    float viewangle = atan2(viewdir.z, viewdir.x) * Rad2Deg;
                    float targetangle = atan2(targetdir.z, targetdir.x) * Rad2Deg;

                    if (viewangle < 0)
                        viewangle += 360;
                    if (targetangle < 0)
                        targetangle += 360;

                    float angle = targetangle - viewangle;

                    while (angle < 0)
                        angle += 360;
                    while (angle > 360)
                        angle -= 360;

                    angle = 360 - angle;
                    angle -= 90;
                    OtFovV1(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2, 90 + distance * 2,
                            angle - size,
                            angle + size,
                            ImColor(1.f, 1.f, 1.f, 1.f * opacity), 1);
                }
            }
        }
    }
    catch (...)
    {
        return;
    }
}
void aimbot()
{
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
    if (!Vars.Aimbot)
        return;
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list)
        return;
    void *Match = game_sdk->Curent_Match();
    if (!Match)
        return;
    if (Vars.isAimFov)
    {
        if (Vars.fovaimglow)
            drawcircleglow(draw_list, center, Vars.AimFov, ImColor(Vars.fovLineColor[0], Vars.fovLineColor[1], Vars.fovLineColor[2], Vars.fovLineColor[3]), 999, 1, 12);
        else
            draw_list->AddCircle(center, Vars.AimFov, ImColor(Vars.fovLineColor[0], Vars.fovLineColor[1], Vars.fovLineColor[2], Vars.fovLineColor[3]), 100);
    }
    void *LocalPlayer = game_sdk->GetLocalPlayer(Match);
    if (!LocalPlayer)
        return;
    void *playertarget = GetClosestEnemy();
    if (!playertarget)
        return;
    ImVec2 EnemyLocation = Camera$$WorldToScreen::Regular(GetHeadPosition(playertarget));
    drawlineglow(draw_list, ImVec2(center.x, center.y), EnemyLocation, ImColor(255, 255, 255), 1, 3);
}
// GameFacade.CurrentInGameUIScene() = 0x5918DB4 — null f lobby/FE, non-null f l'match
static void *GetInGameUIScene()
{
    static void *(*fn)() = (void *(*)())getRealOffset(oxo("0x5918DB4"));
    return fn ? fn() : nullptr;
}

void draw_watermark()
{
    // user (correction): "machi MT x CHEAT — LMT x CHEAT, w tkoun f blast
    // dik [Penny]iOS (top-right, byna f l foto li sft lik), w tla3 mnin
    // ndkhol l game [mnin dkholt l'app — lobby zada]"
    std::string claw = oxorany("LMT x CHEAT");
    ImVec2 text_size = verdana_smol->calc_size(16, claw);
    ImVec2 text_pos(
        ImGui::GetIO().DisplaySize.x - text_size.x - 10.0f, // RIGHT (f blast dik RiOS)
        44.0f); // kif blast "PennyiOS & EreenModz" f l foto
    AddText(verdana_smol, 16, false, false, text_pos + ImVec2(1, 1), ImColor(0, 0, 0, 150), claw);
    static float hue = 0.0f;
    hue += ImGui::GetIO().DeltaTime * 0.1f;
    if (hue > 1.0f)
        hue = 0.0f;
    ImColor rainbow = ImColor::HSV(hue, 0.8f, 0.8f);
    AddText(verdana_smol, 16, false, false, text_pos, rainbow, claw);
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddLine(
        ImVec2(text_pos.x, text_pos.y + text_size.y + 2),
        ImVec2(text_pos.x + text_size.x, text_pos.y + text_size.y + 2),
        rainbow,
        2.0f);
}

// ============================================================
//  Ported from X96 2Ripper (PUBG): Telekill / Fly V2 / Ninja Run
// ============================================================
// --- FF Transform::set_position offset ---------------------------------------
// Taken from the dump.cs of THIS build (validated: its get_position RVA
// 0x8D6C1EC is exactly the working offset already used in game_sdk->init()).
// 0x8D6C2B4 = UnityEngine.Transform::set_position(Vector3 value) — called
// BY VALUE (HFA), the same proven pattern as get_position.
// Fail-closed: an offset failing the image check below keeps the features off.
#include <dlfcn.h> // dladdr — validates the set_position offset before use

static const char *FF_SET_POSITION_OFFSET = "0x8D6C2B4";

// Plain 3-float struct: always an HFA -> passed in v0/v1/v2, exactly like
// Unity's Vector3 (removes any ABI doubt about ff's union-based Vector3).
struct FFVec3 { float x, y, z; };

static void Transform_SetPosition(void *tf, Vector3 pos)
{
    if (!tf)
        return;

    static void *fn = nullptr;
    static bool tried = false;
    if (!tried)
    {
        tried = true;
        if (FF_SET_POSITION_OFFSET && strcmp(FF_SET_POSITION_OFFSET, "0x0") != 0)
        {
            uintptr_t off = strtoull(FF_SET_POSITION_OFFSET, NULL, 0);
            if (off)
            {
                void *cand = (void *)getRealOffset(off);
                // Only trust an address living in the SAME image as the
                // known-good get_position offset — this catches offsets
                // copy-pasted from another dump / another binary.
                Dl_info goodInfo{};
                Dl_info candInfo{};
                if (cand &&
                    dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) &&
                    dladdr(cand, &candInfo) &&
                    goodInfo.dli_fbase && candInfo.dli_fbase &&
                    goodInfo.dli_fbase == candInfo.dli_fbase)
                {
                    fn = cand;
                }
            }
        }
    }
    if (!fn)
        return; // fail-closed: no (valid) offset -> no call -> no crash

    ((void (*)(void *, FFVec3))fn)(tf, FFVec3{pos.x, pos.y, pos.z});
}

// Fly V2 state (the height captured when you switch it on)
void *FlyalturaEXpose = nullptr;
float FlyalturaEXposeX = 0.0f;
float FlyalturaEXposeY = 0.0f;
float FlyalturaEXposeZ = 0.0f;

// ---- Ninja Run (RUNN): run forward like the camera, same height ----
void RunFly()
{
    if (!Vars.fly)
        return;

    void *match = game_sdk->Curent_Match();
    if (!match)
        return;

    void *local_player = game_sdk->GetLocalPlayer(match);
    if (!local_player)
        return;
    if (game_sdk->get_MaxHP(local_player) <= 0)
        return; // lobby / stale match: not a live player

    // Null-guard everything: transform / camera are null while dead or in the lobby
    void *localTF = game_sdk->Component_GetTransform(local_player);
    if (!localTF)
        return;

    void *camera = game_sdk->get_camera();
    if (!camera)
        return;

    void *cameraTF = game_sdk->Component_GetTransform(camera);
    if (!cameraTF)
        return;

    float speed = clamp(Vars.flySpeed, 0.0f, 6.0f);

    Vector3 mountPlayer = game_sdk->get_position(localTF);

    Vector3 forwardMove = game_sdk->GetForward(cameraTF) * speed;
    Vector3 newPosition = mountPlayer + forwardMove;

    newPosition.y = mountPlayer.y;

    Transform_SetPosition(localTF, newPosition);
}

// ---- Fly V2 - BLAST WAHD DIRECT + METHOD L9DIMA (user: "fixi l'sky
//     teleport... brito kima gelt lik... ydini wast l map l'amnina" +
//     answered "Blast wahd direct + bla snap-back") ----
// 1) blast: 3la dor FlyV2 -> teleport WA7D instant f wast l'zone l'amna
//    f l'jouj (st=1); zone null/ghda2a -> settle f blasti (st=2/3/4)
//    - bla rise-then-drop (2 jumps), bla snap-back ("ma bghit auto")
// 2) classic fly: pin ghir Y (exposeY + flyhei), X/Z = FREE (kaymchi 7er
//    f sma) w slider Fly Height = y-tali3 / y-red ("yredni l'map f sma")
// FlyV2 state (debug UI line f FEATURES tab)
static int flyZSt = 0;        // 0 off / 1 zone center / 2 SafeZone null / 3 ZoneDef null / 4 radius ghda2a (sky fallback)
                             // 5 no in-game scene / 6 phase != Fight
static float flyZR = 0.0f;    // OuterRadius (debug)
static int flyZPhase = 0;     // ECSMatchPhase raw (0 Waiting /1 Prepare /2 Fight /3 Post)
static bool flyTPDone = false; // blast wahd sal — ba3dha = FREE hover (bla snap-back)

void RunFlyV2()
{
    if (!Vars.Flyingtest)
    {
        FlyalturaEXposeX = 0.0f;
        FlyalturaEXposeY = 0.0f;
        FlyalturaEXposeZ = 0.0f;
        FlyalturaEXpose = nullptr;
        flyZSt = 0;
        flyZR = 0.0f;
        flyZPhase = 0;
        flyTPDone = false; // activation jdida = blast jdide (wa7d l'wast zone)
        return;
    }

    void *match = game_sdk->Curent_Match();
    if (!match)
        return;

    void *localPlayer = game_sdk->GetLocalPlayer(match);
    if (!localPlayer)
        return;
    if (game_sdk->get_MaxHP(localPlayer) <= 0)
        return; // lobby / stale match: not a live player

    void *tf = game_sdk->Component_GetTransform(localPlayer);
    if (!tf)
        return;

    Vector3 pos = game_sdk->get_position(tf);

    // >>> PHASE GATE — Android pattern (ApplyRotationZone: csPhase != 2 → return;
    //     AutoEnableTPOnPhase2): ECSMatchPhase (OHJMMEGFLLN): 0 Waiting (spawn
    //     island + tayara) / 1 Prepare / 2 FIGHT (= mora ma tnzel) / 3 Post...
    //     → teleport/pin GHIR f Fight — hazz l'mochkil "9bel ma tnzel kydini
    //     wst l map" w lobby zada (scene = null honak → st=5 fail-closed)
    //     GameFacade.CurrentInGameUIScene() = 0x5918DB4 (static) — GetInGameUIScene()
    //     UIInGameScene.m_CSMatchPhase @ 0xD18
    void *scene = GetInGameUIScene();
    if (!scene)
    {
        flyZSt = 5;
        return;
    }
    uint32_t phase = *(uint32_t *)((uintptr_t)scene + oxo("0xD18"));
    flyZPhase = (int)phase;
    if (phase != 2) // Waiting/Prepare: spawn island, tayara, dive → ma kaytsenna TPs
    {
        flyZSt = 6;
        return;
    }

    // >>> BLAST WAHD DIRECT (user: "fixi l'sky teleport... brito kima gelt
    //     lik... ydini wast l map l'amnina" + answered "Blast wahd direct +
    //     bla snap-back"): 3la dor FlyV2 → teleport WA7D instant f wast
    //     l'zone l'amna (f l'jouj), bla rise-then-drop (2 jumps = glitch),
    //     w ba3dha ghir free fly — bla y-rddekni automatic (snap-back TM7).
    //     zone null/ghda2a → settle f blasti (fail-closed, st=2/3/4).
    //     chain dump-verified (iOS dump.cs):
    //       match(JMAGGLCNGIG) + 0x210 = SafeZone OBCJNEDKDOC
    //       SafeZone + 0xB8          = ZoneDefinition BNINENNKABI
    //       ZoneDefinition + 0x14    = OuterCenter (Vector3), +0x20 = OuterRadius
    if (!flyTPDone) // BLAST WAHD — teleport instant l'wast l'zone (st=1)
    {
        void *safeZone = *(void **)((uintptr_t)match + oxo("0x210"));
        void *zoneDef = safeZone ? *(void **)((uintptr_t)safeZone + oxo("0xB8")) : nullptr;
        Vector3 target;
        float anchorY;
        if (zoneDef)
        {
            Vector3 zoneC = *(Vector3 *)((uintptr_t)zoneDef + oxo("0x14"));
            float zoneR = *(float *)((uintptr_t)zoneDef + oxo("0x20"));
            flyZR = zoneR;
            if (zoneR >= 10.0f)
            {
                target.x = zoneC.x;
                target.y = zoneC.y + Vars.flyhei;
                target.z = zoneC.z;
                anchorY = zoneC.y; // hover base = ground dyal l'zone
                flyZSt = 1;
            }
            else
            {
                flyZSt = 4;
                target.x = pos.x;
                target.y = pos.y + Vars.flyhei;
                target.z = pos.z;
                anchorY = pos.y;
            }
        }
        else
        {
            flyZSt = safeZone ? 3 : 2;
            target.x = pos.x;
            target.y = pos.y + Vars.flyhei;
            target.z = pos.z;
            anchorY = pos.y;
        }
        Transform_SetPosition(tf, target); // BLAST WAHD — nafs l'wa7d, bla 2 jumps
        // METHOD L9DIMA anchor: hover base = l'blasa li wesselt lih
        // (machi pos+flyhei KOLA FRAME = +300/frame = ROCKET "kytala3ni")
        FlyalturaEXposeX = target.x;
        FlyalturaEXposeY = anchorY;
        FlyalturaEXposeZ = target.z;
        FlyalturaEXpose = localPlayer;
        flyTPDone = true;
        return; // ba3dha: STEP 3 free fly (bla snap-back — "ma bghit auto")
    }

    // >>> TELEMETRY ONLY — kay-update st/r dyal l'panel kola frame w ma
    //     y-9elleb 3la l'position (snap-back TM7 — user: "ma bghit auto")
    {
        void *safeZone = *(void **)((uintptr_t)match + oxo("0x210"));
        void *zoneDef = safeZone ? *(void **)((uintptr_t)safeZone + oxo("0xB8")) : nullptr;
        if (zoneDef)
        {
            flyZR = *(float *)((uintptr_t)zoneDef + oxo("0x20"));
            flyZSt = (flyZR >= 10.0f) ? 1 : 4;
        }
        else
        {
            flyZSt = safeZone ? 3 : 2;
        }
    }

    // >>> STEP 3 — METHOD L9DIMA (classic fly): pin ghir Y = exposeY + flyhei,
    //     X/Z = pos dyalek daba = FREE — kaymchi 7er f sma, w l'slider
    //     Fly Height (0-1000) = y-tali3 / y-red 3la l'map lli la3bin fiha
    if ((FlyalturaEXposeX == 0.0f && FlyalturaEXposeY == 0.0f && FlyalturaEXposeZ == 0.0f) ||
        FlyalturaEXpose == nullptr)
    {
        if ((FlyalturaEXposeX == 0.0f && FlyalturaEXposeZ == 0.0f) ||
            FlyalturaEXpose == nullptr)
        {
            FlyalturaEXposeX = pos.x;
            FlyalturaEXposeZ = pos.z;
            FlyalturaEXpose = localPlayer;
        }
        FlyalturaEXposeY = pos.y;
    }

    Transform_SetPosition(tf, Vector3(pos.x, FlyalturaEXposeY + Vars.flyhei, pos.z));
}

// ---- closest alive enemy (used by Telekill) ----
void *GetClosestEnemyForTelekill()
{
    float shortestDistance = 3000.0f;
    void *closestEnemy = NULL;

    void *get_MatchGame = game_sdk->Curent_Match();
    if (!get_MatchGame)
        return NULL;

    void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
        return NULL;

    Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)get_MatchGame + oxo("0x148"));
    if (!players)
        return NULL;

    Vector3 LocalPlayerPos = game_sdk->get_position(game_sdk->Component_GetTransform(LocalPlayer));

    for (int u = 0; u < players->getSize(); u++)
    {
        void *Player = players->getValues()[u];
        if (!Player)
            continue;
        if (Player == LocalPlayer)
            continue;
        if (!game_sdk->get_MaxHP(Player))
            continue;
        if (game_sdk->get_isLocalTeam(Player))
            continue;
        if (game_sdk->get_IsDieing(Player))
            continue;

        void *playerTF = game_sdk->Component_GetTransform(Player);
        if (!playerTF)
            continue;

        Vector3 PlayerPos = game_sdk->get_position(playerTF);
        float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
        if (distance >= 3000)
            continue;

        if (distance < shortestDistance)
        {
            shortestDistance = distance;
            closestEnemy = Player;
        }
    }
    return closestEnemy;
}

// ---- Telekill: teleport yourself next to the closest enemy ----
void RunTelekill()
{
    if (!Vars.Telekill)
        return;

    void *closestEnemyForTelekill = GetClosestEnemyForTelekill();
    if (!closestEnemyForTelekill)
        return;

    void *match = game_sdk->Curent_Match();
    if (!match)
        return;

    void *local_player = game_sdk->GetLocalPlayer(match);
    if (!local_player)
        return;
    if (game_sdk->get_MaxHP(local_player) <= 0)
        return; // lobby / stale match: not a live player

    void *_MountTF = game_sdk->Component_GetTransform(closestEnemyForTelekill);
    void *_LocalTF = game_sdk->Component_GetTransform(local_player);
    if (!_MountTF || !_LocalTF)
        return;

    Vector3 MountTF = game_sdk->get_position(_MountTF);
    Vector3 forward = game_sdk->GetForward(_MountTF);
    Vector3 targetPos = MountTF;

    if (Vars.khoangcach < 10)
    {
        float dist = (10 - Vars.khoangcach) * 0.5f;
        targetPos.x += forward.x * dist;
        targetPos.z += forward.z * dist;
    }
    else if (Vars.khoangcach > 10)
    {
        float dist = (Vars.khoangcach - 10) * 0.5f;
        targetPos.x -= forward.x * dist;
        targetPos.z -= forward.z * dist;
    }

    Transform_SetPosition(_LocalTF, targetPos);
}

// ===========================================================================
// AimKill Send — ported from the Android EliteXzp project (StartAimKillSend /
// AimkillSendStrike). Builds the game's own C2S_RUDP_TakeDamage_Req packet
// (opcode 106) from the LOCAL player's real weapon + hit-object scratch, so
// the server accepts it as a normal hit. Every RVA below comes straight from
// the iOS 1.132.X dump.cs — fail-closed: if the dladdr image check or any
// pointer fails, the feature stays silently off (no guessing, no crash).
//
// Deliberately skipped (extra Android runtime lookups / modes):
//   - ComputeAndStoreHashExact (EnableHitTagHackCheck — server-gated)
//   - CheckParametros validate shuffle (EnableShuffleParams statics)
//   - Syns_SwapWeapon_Impl opcode-108 sync (needs il2cpp object_new; the
//     dump has no factory method for message EOBFCLNNIJH)
//   - AimMethodPull / 360v2 aim-focus / anti-invisible / football extras
// ===========================================================================

// BEADLMGGGGL is a 24-byte value struct in the dump (u32 @0x10, u32 @0x14,
// u8 @0x18, u8 @0x19, u64 @0x20). AAPCS64 returns it through a hidden
// pointer (x8) and passes >16B composites by reference — an identical C
// layout makes clang emit exactly the il2cpp ABI.
struct AikPlayerId {
    uint32_t a; // 0x10
    uint32_t b; // 0x14
    uint8_t  c; // 0x18
    uint8_t  d; // 0x19
    uint64_t v; // 0x20
}; // size 0x18

static struct AikFns_t {
    void        *(*GetWeaponOnHand)(void *);            // Player.GetWeaponOnHand
    void        *(*HeadCollider)(void *);               // Player.get_HeadCollider
    void        *(*GameObjectOf)(void *);               // Component.get_gameObject
    void        *(*GetAiming)(void *, void *);          // Player.JGIEOGCGFEP -> interface ptr
    AikPlayerId (*GetPlayerID)(void *);                 // Player.get_PlayerID -> struct
    uint32_t    (*IdToU32)(AikPlayerId);                // BEADLMGGGGL.DEIPJDIGKIP(struct)->uint
    int         (*GetDamage)(void *);                   // HBIBDMMOOOK.KBIFKMJODNA
    uint32_t    (*GetWeaponID)(void *);                 // IEFJGCGOMID.MJCENIGKEPH
    float       (*GetRange)(void *);                    // HBIBDMMOOOK.OEOBPHDFJMO
    void        *(*TimeService)();                      // GameFacade.CurrentGameSimulationTimer
    void        *(*MakeDamageMsg)(void *);              // PhyXShield.GLEMOCGDGHM -> EBEDJLFHHHP
    void        *(*MakeVecMsg)(Vector3);                // message.LMCBHLCPAJD.DJNIOABIFCA
    void        *(*CheckParams)(void *, void *, void *, bool, void *); // HBIBDMMOOOK.NPLMKLBKJPE
    bool        (*Send)(uint32_t, void *, uint8_t, bool);              // GameFacade.Send
    void        (*StartFiring)(void *, void *);         // PlayerNetwork.StartFiring
    void        (*StopFire)(void *, void *);            // PlayerNetwork.StopFire
    void        (*SwapWeapon)(void *, int32_t, bool, void *);          // PlayerNetwork.SwapWeapon
    void        (*ResetSwapTime)(void *);               // Player.ResetSwapWeaponTime
    float       (*Realtime)();                          // Time.get_realtimeSinceStartup
    bool        (*IsLocalTeammate)(void *);             // GameFacade.IsLocalTeammate (static)
} aik;

static bool AikInit()
{
    static int state = 0; // 0 untried / 1 ok / -1 failed (fail-closed)
    if (state != 0)
        return state == 1;

    // sanity: the anchor RVA must resolve inside a loaded image (same dladdr
    // pattern as FF_SET_POSITION_OFFSET) — a bad slide disables the feature,
    // never the game.
    Dl_info info;
    if (!dladdr((void *)getRealOffset(0x5926D78), &info) || !info.dli_fname)
    {
        state = -1;
        return false;
    }

    aik.GetWeaponOnHand = (void *(*)(void *))getRealOffset(oxo("0x5616118"));
    aik.HeadCollider = (void *(*)(void *))getRealOffset(oxo("0x5634F3C"));
    aik.GameObjectOf = (void *(*)(void *))getRealOffset(oxo("0x8D59C7C"));
    aik.GetAiming = (void *(*)(void *, void *))getRealOffset(oxo("0x56A7F5C"));
    aik.GetPlayerID = (AikPlayerId (*)(void *))getRealOffset(oxo("0x55FE154"));
    aik.IdToU32 = (uint32_t (*)(AikPlayerId))getRealOffset(oxo("0x649046C"));
    aik.GetDamage = (int (*)(void *))getRealOffset(oxo("0x5BF4148"));
    aik.GetWeaponID = (uint32_t (*)(void *))getRealOffset(oxo("0x5B0B8EC"));
    aik.GetRange = (float (*)(void *))getRealOffset(oxo("0x5BED0A8"));
    aik.TimeService = (void *(*)())getRealOffset(oxo("0x5919E58"));
    aik.MakeDamageMsg = (void *(*)(void *))getRealOffset(oxo("0x607E1DC"));
    aik.MakeVecMsg = (void *(*)(Vector3))getRealOffset(oxo("0x3715AC4"));
    aik.CheckParams = (void *(*)(void *, void *, void *, bool, void *))getRealOffset(oxo("0x5C262DC"));
    aik.Send = (bool (*)(uint32_t, void *, uint8_t, bool))getRealOffset(oxo("0x5926D78"));
    aik.StartFiring = (void (*)(void *, void *))getRealOffset(oxo("0x5A5B868"));
    aik.StopFire = (void (*)(void *, void *))getRealOffset(oxo("0x5A5C464"));
    aik.SwapWeapon = (void (*)(void *, int32_t, bool, void *))getRealOffset(oxo("0x5A5C7E8"));
    aik.ResetSwapTime = (void (*)(void *))getRealOffset(oxo("0x5636DFC"));
    aik.Realtime = (float (*)())getRealOffset(oxo("0x8D667EC"));
    aik.IsLocalTeammate = (bool (*)(void *))getRealOffset(oxo("0x591C5B4"));

    if (!aik.GetWeaponOnHand || !aik.HeadCollider || !aik.GameObjectOf ||
        !aik.GetAiming || !aik.GetPlayerID || !aik.IdToU32 || !aik.GetDamage ||
        !aik.GetWeaponID || !aik.GetRange || !aik.TimeService || !aik.MakeDamageMsg ||
        !aik.MakeVecMsg || !aik.CheckParams || !aik.Send || !aik.StartFiring ||
        !aik.StopFire || !aik.SwapWeapon || !aik.ResetSwapTime || !aik.Realtime ||
        !aik.IsLocalTeammate)
    {
        state = -1;
        return false;
    }
    state = 1;
    return true;
}

// null-checked head position (the plain GetHeadPosition helper doesn't guard
// a missing head transform)
static bool AikHeadPos(void *p, Vector3 *out)
{
    if (!p)
        return false;
    void *tf = game_sdk->GetHeadPositions(p);
    if (!tf)
        return false;
    *out = game_sdk->get_position(tf);
    return true;
}

// Android IsValidGun — weapon-id blacklist (fists / throwables / vehicles)
static bool AikIsValidGun(void *weapon)
{
    if (!weapon)
        return false;
    int32_t weaponId = (int32_t)aik.GetWeaponID(weapon);
    if (weaponId <= 1 ||
        weaponId == 16 || weaponId == 17 || weaponId == 27 || weaponId == 34 || weaponId == 51 || weaponId == 53 ||
        (weaponId >= 600 && weaponId < 700) ||
        (weaponId >= 1000 && weaponId < 1100) ||
        (weaponId >= 1200 && weaponId < 1300) ||
        (weaponId >= 1400 && weaponId < 1500) ||
        (weaponId >= 6000 && weaponId < 6100) ||
        (weaponId >= 9400 && weaponId < 9500))
    {
        return false;
    }
    return true;
}

// isEnemyInRangeWeapon — st=8 fix: the OLD zexisyy RAW check restored
// (Distance <= the weapon's raw range): no Android unit guess (r<=2 *250)
// and no 120 m default — that rule refused shots the old build accepted.
static bool AikInRange(void *local, void *enemy, void *weapon)
{
    Vector3 h1, h2;
    if (!AikHeadPos(local, &h1) || !AikHeadPos(enemy, &h2))
        return false;
    float range = aik.GetRange(weapon);
    if (range <= 0.0f)
        return false; // old: Distance > 0 range = always refuse
    return Vector3::Distance(h1, h2) <= range;
}

// --- AimKill Send v2 (gaps from Android port) --------------------------------
// Telemetry: st = last StartAimkill gate (1 ran / 2 no-match / 3 not-live /
// 4 no-enemy / 5 enemy-fail / 6 no-gun / 7 not-a-gun / 8 out-of-range /
// 9 SENT / 10 no-pool-or-head / 11 AikInit fail / 12 no-checkParams /
// 13 no-damage-msg / 14 strike gate / 15 no-TimeService), n = 1 when a
// target is acquired, s = packets sent (Elite AimkillSendStrike path).
// Shown in the FEATURES tab.
static int aikDbgSt = 0, aikDbgN = 0, aikDbgSent = 0;
// cover state (SmartCoverKill, Elite port): 0 none / 1 attempt / 2 pulled
static int aikDbgC = 0;

static bool aikStrEq(const char *a, const char *b)
{
    if (!a || !b)
        return false;
    while (*a && *a == *b)
    {
        a++;
        b++;
    }
    return *a == *b;
}

// Android ComputeAndStoreHashExact (EliteMain L3644-3663) — gated by
// EnableHitTagHackCheck @ GameModeSetting+0xD50 (Android 0xB68, iOS offset
// from dump). Writes hash int at weapon+0x40 = CDMMCIIFHGM.HOKKGGGLJIE
// (protected uint @0x40, COW.GamePlay — nefs l'slot dyal Android attackerController[16]).
// Hash chain: GGGBOOOFFOE() static byte[] (elements @0x18 f il2cpp64) ← tick
// @+0, seed @+4; HAEEBMELLEG() static HMACSHA256 → HashAlgorithm.ComputeHash
// → BitConverter.ToInt32(bytes,0) → store.
// tick = TimeService.m_TickCount @+0x20 (Android +0x18 = their tick field).
// Hash semantics: iOS EAJDPCJDMFG = float(uint) → int-cast = identity,
// Android's int(hash) therefore == raw tick/seed mix → ghir nstore-ih direct.
static void AikStoreHitHash(void *weapon, void *target, uint32_t tick)
{
    static int st = 0; // 0 untried / 1 ok / -1 resolve failed (fail-closed)
    static void *(*CurrentGameModeSetting)() = nullptr;
    static void *(*GetHitBytes)() = nullptr;
    static void *(*GetHitHmac)() = nullptr;
    static void *(*HashCompute)(void *, void *) = nullptr;
    static int (*HashToInt)(void *, int) = nullptr;
    if (st == 0)
    {
        CurrentGameModeSetting = (void *(*)())getRealOffset(oxo("0x59144D0")); // CurrentGameModeSetting
        GetHitBytes = (void *(*)())getRealOffset(oxo("0x61C55C0"));             // KMLICIEHMHC.GGGBOOOFFOE byte[]
        GetHitHmac = (void *(*)())getRealOffset(oxo("0x61C56A8"));              // KMLICIEHMHC.HAEEBMELLEG HMACSHA256
        HashCompute = (void *(*)(void *, void *))getRealOffset(oxo("0x8993C1C")); // HashAlgorithm.ComputeHash(byte[])
        HashToInt = (int (*)(void *, int))getRealOffset(oxo("0x8A92320"));     // BitConverter.ToInt32(byte[],0)
        if (!CurrentGameModeSetting || !GetHitBytes || !GetHitHmac ||
            !HashCompute || !HashToInt)
        {
            st = -1;
            return;
        }
        st = 1;
    }
    if (st != 1 || !weapon || !target)
        return;
    void *gms = CurrentGameModeSetting();
    if (!gms)
        return;
    if (!*(uint8_t *)((uintptr_t)gms + 0xD50))
        return; // EnableHitTagHackCheck OFF → no-op (server wields hash only when on)
    void *arr = GetHitBytes();
    if (!arr)
        return;
    void *hmac = GetHitHmac();
    if (!hmac)
        return;
    uint32_t seed = aik.IdToU32(aik.GetPlayerID(target));
    // il2cpp64 Il2CppArray header = 32 bytes (0x20): obj 16 + bounds 8 + max_length 8
    // (Android 32-bit = 0x10 — nefs l'masdar: obj8+bounds4+len4)
    uint8_t *data = (uint8_t *)((uintptr_t)arr + 0x20);
    *(uint32_t *)(data + 0) = tick;
    *(uint32_t *)(data + 4) = seed;
    void *hb = HashCompute(hmac, arr);
    if (!hb)
        return;
    *(uint32_t *)((uintptr_t)weapon + 0x40) = (uint32_t)HashToInt(hb, 0);
}

// Android CheckParametros shuffle-validate (EliteMain L2836-2845):
// GameVarDef.EnableShuffleParams static bool @ statics+0x44E8 (dump L277618);
// NCHOCBKLNEJ.Validate(weapon, params, seedEnc) = 0x5C2B3F0.
// statics base via il2cpp API (dlsym) — fail-closed: la kan API machi
// exported f l'binary → skip (no guessing, Android same order).
static void AikShuffleValidate(void *weapon, void *params, uint32_t seedEnc)
{
    static int st = 0; // 0 untried / 1 ok / -1 no API (fail-closed)
    static void *(*fDomGet)() = nullptr;
    static void **(*fAsmList)(void *, size_t *) = nullptr;
    static void *(*fAsmImg)(void *) = nullptr;
    static const char *(*fImgName)(void *) = nullptr;
    static void *(*fClsName)(void *, const char *, const char *) = nullptr;
    static void *(*fStatics)(void *) = nullptr;
    static void (*fValidate)(void *, void *, uint32_t) = nullptr;
    if (st == 0)
    {
        fDomGet = (void *(*)())dlsym(RTLD_DEFAULT, "il2cpp_domain_get");
        fAsmList = (void **(*)(void *, size_t *))dlsym(RTLD_DEFAULT, "il2cpp_domain_get_assemblies");
        fAsmImg = (void *(*)(void *))dlsym(RTLD_DEFAULT, "il2cpp_assembly_get_image");
        fImgName = (const char *(*)(void *))dlsym(RTLD_DEFAULT, "il2cpp_image_get_name");
        fClsName = (void *(*)(void *, const char *, const char *))dlsym(RTLD_DEFAULT, "il2cpp_class_from_name");
        fStatics = (void *(*)(void *))dlsym(RTLD_DEFAULT, "il2cpp_class_get_static_fields");
        fValidate = (void (*)(void *, void *, uint32_t))getRealOffset(oxo("0x5C2B3F0")); // NCHOCBKLNEJ.Validate
        st = (fDomGet && fAsmList && fAsmImg && fImgName && fClsName && fStatics && fValidate) ? 1 : -1;
        if (st == -1)
            return;
    }
    if (st != 1 || !weapon || !params)
        return;
    size_t nAsm = 0;
    void **asms = fAsmList(fDomGet(), &nAsm);
    if (!asms || !nAsm)
        return;
    void *img = nullptr;
    for (size_t i = 0; i < nAsm && !img; i++)
    {
        void *a = fAsmImg(asms[i]);
        if (a && aikStrEq(fImgName(a), "Assembly-CSharp.dll"))
            img = a;
    }
    if (!img)
        return;
    void *klass = fClsName(img, "COW", "GameVarDef");
    if (!klass)
        return;
    void *statics = fStatics(klass);
    if (!statics)
        return;
    if (!*(uint8_t *)((uintptr_t)statics + 0x44E8))
        return; // EnableShuffleParams OFF → skip
    fValidate(weapon, params, seedEnc);
}

// AimkillSendStrike — fill the local player's hit-object scratch, then build
// and send the packet. Restores the proven-old zexisyy behaviors (st=11 fix):
// weapon+0x5F0 reset, baseDamage from weapon+0xA8 (<100 -> 999), full
// AimTarget pool fill (the old build only wrote 0x18/0x20 here), local
// damage-info writes, StartFiring before CheckParams, CheckParams 2nd arg =
// the ENEMY (NOT the GetAiming result — the old code resolved the aiming
// entity and never gated on it), and WeaponDoSomething/Validate (0x5C2B3F0)
// called UNCONDITIONALLY — the Android shuffle-validate gate is fail-closed
// on iOS (il2cpp dlsym), so it never ran and the strike stayed refused.
// Packet field values stay Elite-verbatim; the hit hash still stores BEFORE
// CheckParametros (Android order L2827->2828).
// st set here: 7 bad baseDamage / 12 no-checkParams / 13 no-damage-msg /
// 14 strike gate (dead / no head) / 15 no-TimeService.

static bool Send108(void *local, void *enemy); // zexisyy part 2 (defined below)

static bool AikStrike(void *local, void *weapon, void *hitInfo, Vector3 firePos, void *target)
{
    if (!local || !weapon || !hitInfo || !target) { aikDbgSt = 14; return false; }
    if (game_sdk->get_IsDieing(target) || game_sdk->GetHp(target) <= 0) { aikDbgSt = 14; return false; }
    // cover/LOS gate REMOVED (user: "hyd mnha l'cover ... 0 second") — strike
    // through cover, no raycast (st14 now = dead / no head only)

    void *headCollider = aik.HeadCollider(target);
    if (!headCollider) { aikDbgSt = 14; return false; }
    void *headGameObject = aik.GameObjectOf(headCollider);
    if (!headGameObject) { aikDbgSt = 14; return false; }

    // CGKJLKPMGDJ: JHAINOBIFMA @0x18 (GameObject), JINDNOFABKB @0x20 (collider)
    *(void **)((uintptr_t)hitInfo + 0x18) = headGameObject;
    *(void **)((uintptr_t)hitInfo + 0x20) = headCollider;

    // old zexisyy: clear the weapon fire-state field before CheckParams
    *(float *)((uintptr_t)weapon + 0x5F0) = 0.0f;

    // old zexisyy: baseDamage straight off the weapon (+0xA8); 0 = not a gun
    // (old st=7), <100 -> 999 so the packet lands as a kill
    int32_t baseDamage = *(int32_t *)((uintptr_t)weapon + 0xA8);
    if (baseDamage == 0) { aikDbgSt = 7; return false; }
    if (baseDamage < 100) baseDamage = 999;
    uint32_t weaponID = aik.GetWeaponID(weapon);

    // old zexisyy: full hit-object pool fill from the enemy's AimTarget —
    // HitLocation/RayDir/StartPos/Distance/Flags feed the CheckParametros
    // list, and writing only 0x18/0x20 left that list empty.
    Vector3 enemyHead;
    if (!AikHeadPos(target, &enemyHead))
        enemyHead = firePos;
    float dist = Vector3::Distance(firePos, enemyHead);
    Vector3 direction = {0, 0, 0};
    if (dist > 0.0001f)
        direction = (enemyHead - firePos) * (1.0f / dist);
    void *aimTarget = *(void **)((uintptr_t)target + 0x738); // Player.AimTarget
    if (aimTarget)
    {
        char *pool = (char *)hitInfo;
        *(void **)(pool + 0x18) = aik.GameObjectOf(aimTarget);
        *(void **)(pool + 0x20) = aimTarget;
        *(Vector3 *)(pool + 0x28) = enemyHead;   // HitLocation
        *(Vector3 *)(pool + 0x34) = direction;   // RayDir
        *(Vector3 *)(pool + 0x40) = direction;   // RayDir2
        *(Vector3 *)(pool + 0x4C) = firePos;     // StartPos
        *(int32_t *)(pool + 0x58) = baseDamage;  // Damage
        *(float *)(pool + 0x5C) = dist;          // Distance
        *(int32_t *)(pool + 0x60) = 1;           // Flags60
        *(int32_t *)(pool + 0x64) = 1;           // Flags64
        *(bool *)(pool + 0x70) = true;           // Flag70
        *(bool *)(pool + 0x71) = true;           // Flag71
    }

    // old zexisyy: local damage-info writes (HitPart / Weapon / DamageValue /
    // AttackerID) — derived from the local player's attributes chain EVERY
    // call (old runtime behavior: no cache -> no stale ptr across matches)
    void *dmgInfo = nullptr;
    {
        void *attrs = *(void **)((uintptr_t)local + 0x768);    // Player.Attributes
        if (attrs)
        {
            void *dmod = *(void **)((uintptr_t)attrs + 0x318); // DamageModule
            if (dmod)
                dmgInfo = *(void **)((uintptr_t)dmod + 0x10);  // DamageInfo
        }
    }
    if (dmgInfo)
    {
        *(int32_t *)((uintptr_t)dmgInfo + 0x14) = 1;                    // HitPart = head
        *(void **)((uintptr_t)dmgInfo + 0x40) = weapon;                 // Weapon
        *(int32_t *)((uintptr_t)dmgInfo + 0x10) = baseDamage;           // DamageValue
        *(AikPlayerId *)((uintptr_t)dmgInfo + 0x28) =
            *(AikPlayerId *)((uintptr_t)local + 0x408);                 // AttackerID = local PlayerID
    }

    void *timeService = aik.TimeService();
    if (!timeService) { aikDbgSt = 15; return false; }
    void *msg = aik.MakeDamageMsg(local); // -> EBEDJLFHHHP
    if (!msg) { aikDbgSt = 13; return false; }

    // Android order: ComputeAndStoreHashExact BEFORE CheckParametros (L2827→2828)
    uint32_t enemyId = aik.IdToU32(aik.GetPlayerID(target));
    AikStoreHitHash(weapon, target, *(uint32_t *)((uintptr_t)timeService + 0x20));

    aik.StartFiring(local, weapon);

    // old: the aiming entity is resolved but NEVER gates the strike (aimkill
    // works off-aim), and CheckParams takes the ENEMY as its 2nd argument
    void *aiming = aik.GetAiming(local, hitInfo); // Player.JGIEOGCGFEP
    (void)aiming;
    void *checkParams = aik.CheckParams(weapon, target, headCollider, false, (void *)0);
    if (!checkParams)
    {
        aik.StopFire(local, weapon);
        aikDbgSt = 12;
        return false;
    }
    // old zexisyy: WeaponDoSomething/Validate called UNCONDITIONALLY (the
    // Android EnableShuffleParams gate never fired on iOS -> strike refused)
    static void (*wValidate)(void *, void *, uint32_t) =
        (void (*)(void *, void *, uint32_t))getRealOffset(oxo("0x5C2B3F0"));
    if (wValidate)
        wValidate(weapon, checkParams, enemyId); // seedEnc = IdToU32(enemyID)

    // EBEDJLFHHHP fields — same type order as the Android message struct:
    // attacker/hp/zero/damage|part/victim/weapon/uniq/part/tick/...
    uint32_t localId = aik.IdToU32(aik.GetPlayerID(local));
    *(uint32_t *)((uintptr_t)msg + 0x14) = enemyId;                              // NMGDAKFKJLH attacker id
    *(uint16_t *)((uintptr_t)msg + 0x18) = (uint16_t)game_sdk->GetHp(target);    // GJCLHNBCABF hp
    *(uint16_t *)((uintptr_t)msg + 0x1A) = 0;                                    // IMKLAILPLKC
    *(uint32_t *)((uintptr_t)msg + 0x1C) = (uint32_t)baseDamage | (1u << 24);    // AEOHKCOACHD damage|part<<24
    *(uint32_t *)((uintptr_t)msg + 0x20) = localId;                              // ECJBNJPOPIM local id
    *(int32_t *)((uintptr_t)msg + 0x24) = (int32_t)weaponID;                     // NEGABPENICF weapon id
    *(uint32_t *)((uintptr_t)msg + 0x28) = *(uint32_t *)((uintptr_t)weapon + 0x10); // NANKDHFLOFI uniq (IEFJGCGOMID.ODFDGMOCJLL)
    *(uint8_t *)((uintptr_t)msg + 0x2C) = 1;                                     // PIIJBAFDLML part = head
    *(uint32_t *)((uintptr_t)msg + 0x30) = *(uint32_t *)((uintptr_t)timeService + 0x20); // AMFEHGKDLOH simTick (TimeService.m_TickCount)
    Vector3 hitPos;
    if (!AikHeadPos(target, &hitPos))
        hitPos = firePos;
    *(void **)((uintptr_t)msg + 0x38) = aik.MakeVecMsg(firePos);                 // HOOMBLGOEPL firePos
    *(void **)((uintptr_t)msg + 0x40) = aik.MakeVecMsg(hitPos);                  // KECMKGHBLDO hitPos
    *(void **)((uintptr_t)msg + 0x48) = checkParams;                             // EEAJDJFLPMN CheckParametros
    *(uint8_t *)((uintptr_t)msg + 0x62) = 0;                                     // FDJNCEIGHGB
    *(uint8_t *)((uintptr_t)msg + 0x63) = 1;                                     // PJAOHJCJKLE
    *(float *)((uintptr_t)msg + 0x70) = aik.Realtime();                          // KHBDOIKPPFI realtime
    *(uint8_t *)((uintptr_t)msg + 0x74) = 0;                                     // ACJANANIPIM

    aik.Send(106, msg, 2, 0); // opcode 106 = C2S_RUDP_TakeDamage_Req
    aikDbgSent++;

    aik.StopFire(local, weapon);
    // swap weapon IN the strike (user: "zide fiha swap weapon f l'Aimkill") =
    // Elite order L2868-2870: StopFire -> Syns_SwapWeapon_Impl(108) -> reset.
    // Our Send108 IS that opcode-108 change-inventory-on-hand packet.
    Send108(local, target);
    aik.ResetSwapTime(local);
    return true;
}

// AimkillRefreshAfterKill — re-arm weapon slots after a confirmed kill
// (Android does SwapWeapon 1/2 + reset; the 108 sync is skipped)
static void AikRefreshAfterKill(void *local)
{
    if (!local)
        return;
    aik.SwapWeapon(local, 1, false, (void *)0);
    aik.SwapWeapon(local, 2, false, (void *)0);
    aik.ResetSwapTime(local);
}

// =============================================================================
// zexisyy (c) 2026 — StartAimkill (message 106) + Send108 (message 108)
// NOTICE (verbatim): "This is the main Aimkill Send used by everyone. The same
// code runs on iOS and Android. The offsets here match the current dump. For
// another build, or for Android, replace only the offsets from your dump.
// Do not rewrite the function."
// Spot-checked against ffdump/dump.cs: every RVA below exists in this dump and
// class EOBFCLNNIJH (namespace "message") has fields +0x14/+0x18 with ctor
// 0x277189C. Fail-closed: nothing here is guessed for other builds.
// Adaptations only (no logic changes): Vars.AimKillSend = their AimKill,
// aikDbgSt/aikDbgN/aikDbgSent lines for the FEATURES panel, g_dmg99 as a
// local static cache (their project global), and UnitySym/FindGameClass
// implemented over dlsym(RTLD_DEFAULT) — same il2cpp chain as AikShuffleValidate.
// =============================================================================
#include "Monostring.h" // monoList<T> — TakeDamage_Req_o2::CheckParams field

// ---- offsets: StartAimkill (zexisyy part 1, verbatim) ----
#define OFF_Zexis_TakeDamageMsgId                 106
#define OFF_Zexis_GameFacade_Send                 0x5926D78
#define OFF_Zexis_GetWeaponOnHand                 0x5616118
#define OFF_Zexis_GetItemDataID                   0x5B0B8EC
#define OFF_Zexis_Weapon_Field5F0                 0x5F0
#define OFF_Zexis_Weapon_BaseDamageField          0xA8
#define OFF_Zexis_Weapon_UniqueNetID              0x10
#define OFF_Zexis_Weapon_Range                    0x5BED0A8
#define OFF_Zexis_Player_ObjectPool               0xE50
#define OFF_Zexis_Player_PlayerID                 0x408
#define OFF_Zexis_Player_Attributes               0x768
#define OFF_Zexis_Player_DamageModule             0x318
#define OFF_Zexis_Player_DamageInfo               0x10
#define OFF_Zexis_Player_AimTarget                0x738
#define OFF_Zexis_DamageInfo_DamageValue          0x10
#define OFF_Zexis_DamageInfo_HitPart              0x14
#define OFF_Zexis_DamageInfo_AttackerID           0x28
#define OFF_Zexis_DamageInfo_Weapon               0x40
#define OFF_Zexis_Pool_HitGameObject              0x18
#define OFF_Zexis_Pool_HitCollider                0x20
#define OFF_Zexis_Pool_HitLocation                0x28
#define OFF_Zexis_Pool_RayDir                     0x34
#define OFF_Zexis_Pool_RayDir2                    0x40
#define OFF_Zexis_Pool_StartPos                   0x4C
#define OFF_Zexis_Pool_Damage                     0x58
#define OFF_Zexis_Pool_Distance                   0x5C
#define OFF_Zexis_Pool_Flags60                    0x60
#define OFF_Zexis_Pool_Flags64                    0x64
#define OFF_Zexis_Pool_Flag70                     0x70
#define OFF_Zexis_Pool_Flag71                     0x71
#define OFF_Zexis_GetHeadCollider                 0x5634F3C
#define OFF_Zexis_GetGameObject                   0x8D59C7C
#define OFF_Zexis_GetAimingEntity                 0x56A7F5C
#define OFF_Zexis_BuildCheckParams                0x5C262DC
#define OFF_Zexis_WeaponDoSomething               0x5C2B3F0
#define OFF_Zexis_StartFiring                     0x5A5B868
#define OFF_Zexis_StopFire                        0x5A5C464
#define OFF_Zexis_CurrentGameSimulationTimer      0x5919E58
#define OFF_Zexis_SimTimer_TickCount              0x20
#define OFF_Zexis_CreateDamageReq                 0x607E1DC
#define OFF_Zexis_GetPlayerID                     0x55FE154
#define OFF_Zexis_PlayerIDToUint                  0x649046C
#define OFF_Zexis_PackVector3                     0x3715AC4
#define OFF_Zexis_RealtimeSinceStartup            0x8D667EC

// ---- offsets: Send108 (zexisyy part 2) — the rest duplicate part 1 above ----
#define OFF_Zexis_ReloadMsgId                     108
#define OFF_Zexis_Msg108_Ctor                     0x277189C   /* void message_EOBFCLNNIJH__ctor(void *self) */
#define OFF_Zexis_Msg108_Id                       0x14        /* uint32_t PlayerID */
#define OFF_Zexis_Msg108_Unique                   0x18        /* uint32_t WeaponUniqueID */

// ---- packet structs (zexisyy verbatim; our Vector3 from Vector3.h instead) ----
struct COW_GamePlay_IHAAMHPPLMG_o {
    uint32_t m_Value;
    uint32_t m_ID;
    uint8_t  m_TeamID;
    uint8_t  m_ShortID;
    uint64_t m_IDMask;
};

struct message_PVector3IntPos_o {
    void *klass;
    void *monitor;
    bool UDPClientMessageBase_m_GetFromPool;
    int32_t X;
    int32_t Y;
    int32_t Z;
};

struct message_C2S_RUDP_TakeDamage_Req_o2 {
    void *klass;
    void *monitor;
    bool UDPClientMessageBase_m_GetFromPool;
    uint32_t AttackableEntityID;
    uint16_t RealDamageValue;
    uint16_t ShieldDamageValue;
    uint32_t DamageValue;
    uint32_t DamagerID;
    int32_t  WeaponDataID;
    uint32_t WeaponUniqueID;
    uint8_t  HitBodyPart;
    uint32_t TickCount;
    message_PVector3IntPos_o *FirePos;
    message_PVector3IntPos_o *TargetPos;
    monoList<float *> *CheckParams;
    uint32_t ExtraData;
    float ArmorRatio;
    uint64_t ExtraFlag;
    int8_t SpecialHitType;
    bool ForceNoHeadShot;
    bool IsPlayerBackArea;
    monoList<int *> *DamageData;
    float BaseTime;
    bool ManualReload;
};

struct message_EOBFCLNNIJH_o {
    void *klass;
    void *monitor;
    bool UDPClientMessageBase_m_GetFromPool;
    uint32_t PlayerID;          /* +0x14 */
    uint32_t WeaponUniqueID;    /* +0x18 */
};

// ---- il2cpp declarations (zexisyy part 2 verbatim) ----
typedef void *(*il2cpp_domain_get_t)();
typedef void **(*il2cpp_domain_get_assemblies_t)(void *domain, size_t *size);
typedef void *(*il2cpp_assembly_get_image_t)(void *assembly);
typedef const char *(*il2cpp_image_get_name_t)(void *image);
typedef void *(*il2cpp_class_from_name_t)(void *image, const char *nameSpace, const char *name);
typedef void *(*il2cpp_object_new_t)(void *klass);

typedef bool (*GameFacade_Send_t)(uint32_t messageID, void *msg, uint8_t sendOption, bool cacheMsgAnyWay);
typedef void (*Msg108_Ctor_t)(void *self);
typedef void *(*GetWeaponOnHand_t)(void *player);
typedef int (*GetItemDataID_t)(void *weapon);
typedef COW_GamePlay_IHAAMHPPLMG_o (*GetPlayerID_t)(void *player);
typedef uint32_t (*PlayerIDToUint_t)(COW_GamePlay_IHAAMHPPLMG_o id);

// UnitySym / FindGameClass — zexisyy il2cpp_api.h behavior, portable body:
// resolve il2cpp symbols from the already-loaded UnityFramework image via
// dlsym(RTLD_DEFAULT) (same chain AikShuffleValidate already uses). Fail-closed.
static void *UnitySym(const char *name) {
    return (void *)dlsym(RTLD_DEFAULT, name);
}

static void *FindGameClass(const char *nameSpace, const char *name) {
    static il2cpp_domain_get_t fDomGet = nullptr;
    static il2cpp_domain_get_assemblies_t fDomAsm = nullptr;
    static il2cpp_assembly_get_image_t fAsmImg = nullptr;
    static il2cpp_class_from_name_t fClsName = nullptr;
    static bool fTried = false;
    if (!fTried) {
        fTried = true;
        fDomGet = (il2cpp_domain_get_t)dlsym(RTLD_DEFAULT, "il2cpp_domain_get");
        fDomAsm = (il2cpp_domain_get_assemblies_t)dlsym(RTLD_DEFAULT, "il2cpp_domain_get_assemblies");
        fAsmImg = (il2cpp_assembly_get_image_t)dlsym(RTLD_DEFAULT, "il2cpp_assembly_get_image");
        fClsName = (il2cpp_class_from_name_t)dlsym(RTLD_DEFAULT, "il2cpp_class_from_name");
    }
    if (!fDomGet || !fDomAsm || !fAsmImg || !fClsName) return nullptr;
    void *domain = fDomGet();
    if (!domain) return nullptr;
    size_t n = 0;
    void **assemblies = fDomAsm(domain, &n);
    if (!assemblies) return nullptr;
    for (size_t i = 0; i < n; i++) {
        if (!assemblies[i]) continue;
        void *image = fAsmImg(assemblies[i]);
        if (!image) continue;
        void *klass = fClsName(image, nameSpace, name);
        if (klass) return klass;
    }
    return nullptr;
}

// zexisyy keeps this as a project global; here it is a static cache so
// StartAimkill derives the damage-info object once (null -> attributes chain).
static void *g_dmg99 = nullptr;

// ============================================================
//  Ported from Android EliteXzp (the "Elite" project)
//  Fly Ultra / Stop Time / Mark TP / AimKill cover
//  Every offset dump-verified vs THIS build's dump.cs (fail-closed).
// ============================================================
#include <chrono>
#include <math.h>

// ---- Fly Ultra (Elite: Fly Hack V2 "montee automatique sans saut") ----------
// RELATIVE climb: pos.y + 0.2 per Elite tick (~35 ms) from the position the
// server actually has us at every frame. The old absolute g_fuY fought the
// server's position correction (rose ~2 m then vibrated). Small step = "itl3
// b chwya". Cap = activation Y + Vars.fuMaxM (slider) then AUTO TP to my
// mark at the cap ("ywsel l'cap -> teleport l'mark"). X/Z stay free. OFF /
// lobby / dead -> re-arm from the current height.
// mark helpers are defined further down — the cap-TP below needs them
static Vector3 MarkedLoc();
static bool TeleportToMark(Vector3 target);

static bool g_fuArmed = false;
static bool g_fuTped = false; // cap reached + auto-TP done (one climb per toggle)
static float g_fuBase = 0.0f; // activation Y (cap reference)
void RunFlyUltra(float dt)
{
    if (!Vars.FlyUltra)
    {
        g_fuArmed = false;
        g_fuTped = false;
        return;
    }
    if (Vars.Flyingtest)
    {
        g_fuArmed = false;
        g_fuTped = false;
        return; // FlyV2 owns Y (panel unchecks Fly Ultra too - no 2 flies fighting)
    }

    void *match = game_sdk->Curent_Match();
    if (!match)
        return;
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local)
        return;
    if (game_sdk->get_MaxHP(local) <= 0)
    {
        g_fuArmed = false;
        g_fuTped = false;
        return; // lobby / dead (new life -> re-arm a fresh climb)
    }
    void *tf = game_sdk->Component_GetTransform(local);
    if (!tf)
        return;
    Vector3 pos = game_sdk->get_position(tf);
    if (pos == Vector3::zero())
        return;
    if (g_fuTped)
        return; // climb + cap-TP already done this toggle (idle at the mark)
    if (!g_fuArmed)
    {
        g_fuBase = pos.y;
        g_fuArmed = true;
    }
    // 2 sliders (user: "2 khtouta: chhal ytl3 f wa7d tick w chhal mn mitro")
    float spd = Vars.fuSpeed;
    if (spd < 0.01f) spd = 0.01f;
    float maxM = Vars.fuMaxM;
    if (maxM < 1.0f) maxM = 1.0f;
    // relative step: climb from where the server actually has us (post-correction)
    float next = pos.y + spd * (dt / 0.035f); // Elite ApplyFlyHackV2: +0.20f per ~35 ms tick
    float fuCap = g_fuBase + maxM;            // cap = activation Y + fuMaxM metres
    if (next >= fuCap)
    {
        // cap reached -> AUTO TP exactly 7 m above my mark (TeleportToMark).
        // No mark = fail-closed: hold at the cap until one exists.
        Vector3 mk = MarkedLoc();
        if (mk != Vector3::zero() && TeleportToMark(mk))
        {
            g_fuTped = true; // one climb + TP per toggle
            g_fuArmed = false;
            return; // ma n-overwrite-ich l'TP b l'pos l'9dim
        }
        next = fuCap; // hold at the cap
    }
    pos.y = next;
    Transform_SetPosition(tf, pos);
}

// ---- Stop Time (Elite: Speed Timer freeze) — SEPARATE again ---------------
// "ywe9ef l'wa9t": TimeService.m_FixedDeltaTime -> 0.00001 (near freeze),
// OFF -> restore the saved value (Elite restores 0.033).
// Dump-verified: GameFacade.CurrentGameSimulationTimer() = RVA 0x5919E58
// returns TimeService; TimeService.m_FixedDeltaTime @ +0x2C.
// USER: "t7iyed l'Stop Time w dir l'button rasso bouhdo, w Fly Ultra bouhdo"
// -> Stop Time = OWN checkbox/button, NO LONGER armed by Fly Ultra.
static bool g_stPatched = false;
static float g_stOrig = 0.033f;
static float g_stDeltaShow = 0.0f; // FEATURES tab telemetry
void RunStopTime()
{
    // iOS hardening (entry crash fix): the game's static CurrentGame may still
    // be null at app entry -> calling into il2cpp then = NRE crash. So: NO
    // game-RVA call at all unless Stop Time is on (or a restore is pending)
    // AND a live local player proves we are inside a match.
    if (!Vars.StopTime && !g_stPatched)
        return;
    void *match = game_sdk->Curent_Match();
    if (!match)
        return;
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
        return; // lobby / dead: ma n-touchiwch l'game timers
    // resolve + validate the RVA only once we are really playing: same dladdr
    // same-image check as Transform_SetPosition (wrong-dump offset = stay off)
    static void *(*simTimerFn)() = nullptr;
    static bool simTried = false;
    if (!simTried)
    {
        simTried = true;
        void *cand = (void *)getRealOffset(OFF_Zexis_CurrentGameSimulationTimer);
        Dl_info goodInfo{};
        Dl_info candInfo{};
        if (cand && dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) &&
            dladdr(cand, &candInfo) && goodInfo.dli_fbase && candInfo.dli_fbase &&
            goodInfo.dli_fbase == candInfo.dli_fbase)
            simTimerFn = (void *(*)())cand;
    }
    if (!simTimerFn)
        return;
    void *ts = simTimerFn();
    if (!ts)
        return;
    float *pDelta = (float *)((uintptr_t)ts + oxo("0x2C"));
    if (Vars.StopTime)
    {
        float cur = *pDelta;
        g_stDeltaShow = cur;
        if (cur <= 0.001f)
            return; // already frozen (previous enable)
        if (!g_stPatched)
        {
            g_stOrig = cur;
            g_stPatched = true;
        }
        *pDelta = 0.00001f;
        g_stDeltaShow = 0.00001f;
    }
    else
    {
        if (g_stPatched)
        {
            if (*pDelta != g_stOrig)
                *pDelta = g_stOrig;
            g_stPatched = false;
        }
        g_stDeltaShow = *pDelta;
    }
}

// ---- DOWN KILL / DOWN KILL V2 (Elite DownKillLoop / DownKillV2Loop) ----------
// V1 (user: "DOWN KILL + V2 kolhom"): kol frame rasi -1.8 m (ta7t l'ard) w
// l'a3da2 li f 200 m zado -1.8 m (nfs l'order dyal Elite: dist qbel l'sub).
// V2: nozol b shewwia -0.4 m/f l -2.5 m men groundY l9itah men l'awl, pin;
// OFF -> 6 frames restore groundY + 0.5 (Elite dkv2ExitHold).
// Lobby guard = get_MaxHP <= 0 (nfs l'pattern dyal l'run functions).
static void DownKillPullEnemies(void *local, Vector3 myPos)
{
    void *m = game_sdk->Curent_Match();
    if (!m)
        return;
    Dictionary<uint8_t *, void **> *players =
        *(Dictionary<uint8_t *, void **> **)((long)m + oxo("0x148"));
    if (!players)
        return;
    for (int u = 0; u < players->getSize(); u++)
    {
        void *e = players->getValues()[u];
        if (!e || e == local)
            continue;
        if (game_sdk->get_MaxHP(e) <= 0 || game_sdk->get_IsDieing(e))
            continue;
        if (game_sdk->get_isLocalTeam(e))
            continue;
        void *etf = game_sdk->Component_GetTransform(e);
        if (!etf)
            continue;
        Vector3 ep = game_sdk->get_position(etf);
        if (ep == Vector3::zero())
            continue;
        if (Vector3::Distance(ep, myPos) > 200.0f)
            continue;
        ep.y -= 1.8f;
        Transform_SetPosition(etf, ep);
    }
}

void RunDownKill()
{
    if (!Vars.DownKill)
        return;
    void *m = game_sdk->Curent_Match();
    if (!m)
        return;
    void *local = game_sdk->GetLocalPlayer(m);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
        return;
    void *tf = game_sdk->Component_GetTransform(local);
    if (!tf)
        return;
    Vector3 p = game_sdk->get_position(tf);
    if (p == Vector3::zero())
        return;
    p.y -= 1.8f;
    Transform_SetPosition(tf, p);
    DownKillPullEnemies(local, p);
}

static bool g_dkv2Run = false;
static float g_dkv2GroundY = 0.0f;
static float g_dkv2CurY = 0.0f;
static int g_dkv2Exit = 0;
void RunDownKillV2()
{
    if (!Vars.DownKillV2)
    {
        if (g_dkv2Run)
        {
            g_dkv2Run = false;
            g_dkv2Exit = 6;
        }
        if (g_dkv2Exit > 0)
        {
            g_dkv2Exit--;
            void *m = game_sdk->Curent_Match();
            void *local = m ? game_sdk->GetLocalPlayer(m) : nullptr;
            void *tf = (local && game_sdk->get_MaxHP(local) > 0)
                           ? game_sdk->Component_GetTransform(local)
                           : nullptr;
            if (tf)
            {
                Vector3 p = game_sdk->get_position(tf);
                if (p != Vector3::zero())
                {
                    p.y = g_dkv2GroundY + 0.5f;
                    Transform_SetPosition(tf, p);
                }
            }
        }
        return;
    }
    void *m = game_sdk->Curent_Match();
    if (!m)
        return;
    void *local = game_sdk->GetLocalPlayer(m);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
    {
        g_dkv2Run = false;
        return;
    }
    void *tf = game_sdk->Component_GetTransform(local);
    if (!tf)
        return;
    Vector3 p = game_sdk->get_position(tf);
    if (p == Vector3::zero())
        return;
    if (!g_dkv2Run)
    {
        g_dkv2Run = true;
        g_dkv2GroundY = p.y;
        g_dkv2CurY = p.y;
    }
    float targetY = g_dkv2GroundY - 2.5f;
    if (g_dkv2CurY > targetY)
    {
        g_dkv2CurY -= 0.40f;
        if (g_dkv2CurY < targetY)
            g_dkv2CurY = targetY;
    }
    p.y = g_dkv2CurY;
    Transform_SetPosition(tf, p);
    DownKillPullEnemies(local, p);
}

// ---- FIX FLY (Elite Speed Timer V2, floating label "FIX FLY") ----------------
// Nefs l'campo dyal Stop Time (TimeService.m_FixedDeltaTime @ +0x2C) walakin b
// value slider 1..20 -> anchor curve (Elite 5991/5992 FastFireMaxTimer):
// v<=1 -> 0.033, 2 -> 0.035, 3 -> 0.038, 5 -> 0.045, 8 -> 0.058, 10 -> 0.070,
// 13 -> 0.095, 16 -> 0.130, 20 -> 0.200 (linear lerp between anchors).
// OFF -> restore l'orig. Stop Time wins (RunFixFly kaynaddi QBEL RunStopTime w
// kayrje3 l'orig ila StopTime 3ndha — freeze owns the field).
// Entry hardening nafs dyal RunStopTime: ZERO game-RVA call ila ma kaynsh
// local player f match (entry crash fix).
static bool g_fxPatched = false;
static float g_fxOrig = 0.033f;
static float g_fxShow = 0.0f; // FEATURES tab telemetry
void RunFixFly()
{
    bool want = Vars.FixFly && !Vars.StopTime;
    if (!want && !g_fxPatched)
        return;
    void *m = game_sdk->Curent_Match();
    if (!m)
        return;
    void *local = game_sdk->GetLocalPlayer(m);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
        return;
    static void *(*simTimerFn)() = nullptr;
    static bool simTried = false;
    if (!simTried)
    {
        simTried = true;
        void *cand = (void *)getRealOffset(OFF_Zexis_CurrentGameSimulationTimer);
        Dl_info goodInfo{};
        Dl_info candInfo{};
        if (cand && dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) &&
            dladdr(cand, &candInfo) && goodInfo.dli_fbase && candInfo.dli_fbase &&
            goodInfo.dli_fbase == candInfo.dli_fbase)
            simTimerFn = (void *(*)())cand;
    }
    if (!simTimerFn)
        return;
    void *ts = simTimerFn();
    if (!ts)
        return;
    float *pDelta = (float *)((uintptr_t)ts + oxo("0x2C"));
    if (want)
    {
        if (!g_fxPatched)
        {
            g_fxOrig = *pDelta;
            g_fxPatched = true;
        }
        float v = (float)Vars.fixFlyVal;
        float delta;
        if (v <= 0.0f)
            delta = 0.00001f;
        else if (v <= 1.0f)
            delta = 0.033f;
        else
        {
            static const float ax[9] = {1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 10.0f, 13.0f, 16.0f, 20.0f};
            static const float ay[9] = {0.033f, 0.035f, 0.038f, 0.045f, 0.058f, 0.070f, 0.095f, 0.130f, 0.200f};
            int idx = 8;
            for (int i = 0; i < 9; i++)
            {
                if (v <= ax[i])
                {
                    idx = i;
                    break;
                }
            }
            int i0 = idx - 1;
            if (i0 < 0)
                i0 = 0;
            float t = (ax[idx] != ax[i0]) ? (v - ax[i0]) / (ax[idx] - ax[i0]) : 0.0f;
            if (t < 0.0f)
                t = 0.0f;
            if (t > 1.0f)
                t = 1.0f;
            delta = ay[i0] + (ay[idx] - ay[i0]) * t;
        }
        *pDelta = delta;
        g_fxShow = delta;
    }
    else
    {
        if (g_fxPatched)
        {
            if (*pDelta != g_fxOrig)
                *pDelta = g_fxOrig;
            g_fxPatched = false;
        }
        g_fxShow = *pDelta;
    }
}

// ---- GHOST (Elite Ghost Hack V4, floating label "GHOST HACK") ----------------
// iOS dump-verified (1.132.X):
//   Player.MainCameraTransform        0x3E8
//   Player.IIPBIDIBJDK (root node)    0x6C8  (ITransformNode)
//   Player.GBJCJJNPPEC (physx)        0x1D48 (GFLBFKKDAMK)
//     -> MovingDirection 0x28 / FallingDirection 0x34 / VSpeed 0x40 / HSpeed 0x44
//       (field order nefs dyal Android: 2 Vector3 + 2 float)
//   GCommon.TransformNode.get_transform 0x6B4964C
//   Player.get_WaitForForceSync        0x5636F20 (Elite hook_get_WaitForForceSync
//     -> if GHOSTMODE_ENABLED() return true; dladdr-validated MSHookFunction)
// Android-only raw offsets (GhostHackFlag 0x548 / GhostPosition 0x12DC /
// net-shadow 0x18C8 + sub-offsets / MoveXYZ chain) don't match the iOS dump
// layout (0x548 iOS = uint GetCount) -> SKIPPED (fail-closed: dump-verified ghir).
static bool g_ghRun = false;
static float g_ghGroundY = 0.0f;
static float g_ghCamOff = 1.6f;
static bool (*g_origWaitSync)(void *) = nullptr;
static bool Hook_WaitForForceSync(void *p)
{
    if (Vars.Ghost)
        return true; // Elite: GHOSTMODE_ENABLED() -> true (kol l'players)
    if (g_origWaitSync)
        return g_origWaitSync(p);
    return false;
}
static void InstallGhostHook()
{
    static bool tried = false;
    if (tried)
        return;
    tried = true;
    void *cand = (void *)getRealOffset(0x5636F20);
    Dl_info goodInfo{};
    Dl_info candInfo{};
    if (cand && dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) &&
        dladdr(cand, &candInfo) && goodInfo.dli_fbase && candInfo.dli_fbase &&
        goodInfo.dli_fbase == candInfo.dli_fbase)
        MSHookFunction(cand, (void *)Hook_WaitForForceSync, (void **)&g_origWaitSync);
}
// root: Player.IIPBIDIBJDK (ITransformNode) -> GCommon.TransformNode.get_transform
static void *GhostRootTf(void *player)
{
    static void *(*getTf)(void *) = nullptr;
    static bool tried = false;
    if (!tried)
    {
        tried = true;
        void *cand = (void *)getRealOffset(oxo("0x6B4964C"));
        Dl_info goodInfo{};
        Dl_info candInfo{};
        if (cand && dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) &&
            dladdr(cand, &candInfo) && goodInfo.dli_fbase && candInfo.dli_fbase &&
            goodInfo.dli_fbase == candInfo.dli_fbase)
            getTf = (void *(*)(void *))cand;
    }
    if (!getTf)
        return nullptr;
    void *node = *(void **)((uintptr_t)player + oxo("0x6C8"));
    if (!node || (uintptr_t)node < 0x10000)
        return nullptr;
    void *t = getTf(node);
    return ((uintptr_t)t > 0x10000) ? t : nullptr;
}

void RunGhost()
{
    if (!Vars.Ghost)
    {
        if (g_ghRun)
        {
            g_ghRun = false;
            // Elite restore: groundY + 0.5 (10 repeated sets + root)
            void *m = game_sdk->Curent_Match();
            void *local = m ? game_sdk->GetLocalPlayer(m) : nullptr;
            void *tf = (local && game_sdk->get_MaxHP(local) > 0)
                           ? game_sdk->Component_GetTransform(local)
                           : nullptr;
            if (tf)
            {
                Vector3 p = game_sdk->get_position(tf);
                if (p != Vector3::zero())
                {
                    p.y = g_ghGroundY + 0.5f;
                    for (int i = 0; i < 10; i++)
                        Transform_SetPosition(tf, p);
                    void *rt = GhostRootTf(local);
                    if (rt)
                        for (int i = 0; i < 10; i++)
                            Transform_SetPosition(rt, p);
                }
            }
        }
        return;
    }
    InstallGhostHook();
    void *m = game_sdk->Curent_Match();
    if (!m)
    {
        g_ghRun = false;
        return;
    }
    void *local = game_sdk->GetLocalPlayer(m);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
    {
        g_ghRun = false;
        return;
    }
    void *tf = game_sdk->Component_GetTransform(local);
    if (!tf)
        return;
    Vector3 p = game_sdk->get_position(tf);
    if (p == Vector3::zero())
        return;
    if (!g_ghRun)
    {
        g_ghRun = true;
        g_ghGroundY = p.y;
        g_ghCamOff = 1.6f;
        void *cam0 = *(void **)((uintptr_t)local + oxo("0x3E8"));
        if (cam0)
        {
            Vector3 cp = game_sdk->get_position(cam0);
            if (cp != Vector3::zero())
            {
                g_ghCamOff = cp.y - g_ghGroundY;
                if (g_ghCamOff == 0.0f)
                    g_ghCamOff = 1.6f;
            }
        }
    }
    Vector3 ug = p;
    ug.y = g_ghGroundY - 2.5f;
    // freeze physX (GFLBFKKDAMK: dirs 0x28/0x34, speeds 0x40/0x44) — 3 cycles
    void *px = *(void **)((uintptr_t)local + oxo("0x1D48"));
    if (px && (uintptr_t)px > 0xFFFF)
    {
        for (int i = 0; i < 3; i++)
        {
            *(float *)((uintptr_t)px + oxo("0x40")) = 0.0f;
            *(float *)((uintptr_t)px + oxo("0x44")) = 0.0f;
            *(Vector3 *)((uintptr_t)px + oxo("0x34")) = Vector3(0, 0, 0);
            *(Vector3 *)((uintptr_t)px + oxo("0x28")) = Vector3(0, 0, 0);
        }
    }
    for (int i = 0; i < 5; i++)
        Transform_SetPosition(tf, ug);
    void *rt = GhostRootTf(local);
    if (rt)
        for (int i = 0; i < 5; i++)
            Transform_SetPosition(rt, ug);
    // camera stays at the surface (Elite ghCamOffsetY)
    void *cam = *(void **)((uintptr_t)local + oxo("0x3E8"));
    if (cam)
    {
        Vector3 cp = game_sdk->get_position(cam);
        if (cp != Vector3::zero())
        {
            cp.y = g_ghGroundY + g_ghCamOff;
            Transform_SetPosition(cam, cp);
        }
    }
}

// ---- MarkedLoc (Elite: MarkedLoc) -------------------------------------------
// Dump-verified chain off UIInGameScene (GetInGameUIScene):
//   m_BigMapCtrl @ +0x458 (UIBigMapController)
//   -> m_MapContentCtrl @ +0xA8 (UIMapContentController)
//   -> m_LocalMapMarkController @ +0xF8 (UIHudPlayerMarkController)
//   -> m_pos @ +0xB0 (Vector3 = my own map mark)
static Vector3 MarkedLoc()
{
    void *scene = GetInGameUIScene(); // GameFacade.CurrentInGameUIScene 0x5918DB4
    if (!scene)
        return Vector3::zero();
    void *bigMap = *(void **)((uintptr_t)scene + oxo("0x458"));
    if (!bigMap)
        return Vector3::zero();
    void *mapContent = *(void **)((uintptr_t)bigMap + oxo("0xA8"));
    if (!mapContent)
        return Vector3::zero();
    void *markCtrl = *(void **)((uintptr_t)mapContent + oxo("0xF8"));
    if (!markCtrl)
        return Vector3::zero();
    return *(Vector3 *)((uintptr_t)markCtrl + oxo("0xB0"));
}

// ---- forceGlideTP (Elite): spoof IsFoldWingGliding for the LOCAL player ----
// around a teleport so the movement system accepts the new position instead
// of ground-clamping/snapping it back to 0 m (Elite MasterBool.forceGlideTP +
// hook_IsFoldWingGliding). dump.cs: COW.GamePlay.Player.IsFoldWingGliding()
// RVA 0x570D98C. Hook body = plain ptr compares ONLY (no game calls) so it is
// safe even before game_sdk init.
static bool g_forceGlideTP = false;
static void *g_glideLocal = nullptr;
static bool (*g_origFoldWingGliding)(void *) = nullptr;
static bool Hook_FoldWingGliding(void *player)
{
    if (g_forceGlideTP && player && player == g_glideLocal)
        return true; // Elite: forceGlideTP && player == local
    if (g_origFoldWingGliding)
        return g_origFoldWingGliding(player);
    return false;
}
static void InstallGlideTPHook()
{
    static bool tried = false;
    if (tried)
        return;
    tried = true;
    HOOK(0x570D98C, Hook_FoldWingGliding, g_origFoldWingGliding);
}

// shared +7 m pin (Tele Mark + Fly Ultra cap-TP): hold the arrival spot for
// 500 ms so clamp/knockback cannot eat the 7 m, then release -> the game
// drops us ON the mark (user: "ay blas n teleporta liha khaso ytipini
// fou9ha b 7 m").
static bool g_tpPin = false;
static Vector3 g_tpPinPos;
static std::chrono::steady_clock::time_point g_tpPinStart;

// ---- TeleportToMark (Elite ExecuteTeleMark sequence) ------------------------
// hop +50 then arrive exactly +7 m above the target (mark), HOLD 500 ms
// (glide spoof + per-frame pin) then release -> drop onto the mark.
static bool TeleportToMark(Vector3 target)
{
    void *match = game_sdk->Curent_Match();
    if (!match)
        return false;
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
        return false;
    void *tf = game_sdk->Component_GetTransform(local);
    if (!tf)
        return false;
    Vector3 pos = game_sdk->get_position(tf);
    if (pos == Vector3::zero())
        return false;
    InstallGlideTPHook();
    g_glideLocal = local;
    g_forceGlideTP = true; // movement system accepts the hop/TP (Elite)
    Vector3 up = pos;
    up.y += 50.0f;
    Transform_SetPosition(tf, up); // Elite: hop first (anti ground-clamp)
    Vector3 dst = target;
    dst.y += 7.0f;
    Transform_SetPosition(tf, dst);
    g_tpPin = true; // hold +7 m for 500 ms, then drop onto the mark
    g_tpPinPos = dst;
    g_tpPinStart = std::chrono::steady_clock::now();
    return true;
}

// ---- RunMarkTP: Tele Mark (manual one-shot) + Auto Mark (Fight gate) --------
// Tele Mark: check -> one TP to my map mark; no mark -> auto-uncheck (Elite
// auto-clears too).  Auto Mark: ECSMatchPhase == 2 once per round -> TP to the
// mark + pin 1.5 s (Elite AutoEnableTPOnPhase2 + ExecuteGliderMarkTP 1500 ms).
// g_markDbg bits: 1 = tele done / 2 = auto fired / 4 = pin active.
static bool g_tmDone = false;
static bool g_amFired = false;
static bool g_amPin = false;
static Vector3 g_amTarget; // plain static (zero-init) - ma ndirch dynamic init f load time
static std::chrono::steady_clock::time_point g_amStart;
static int g_markDbg = 0;
void RunMarkTP()
{
    g_markDbg = 0;

    // shared +7 m pin: hold the TP arrival (Tele Mark / Fly Ultra cap-TP) for
    // 500 ms, then release glide+pin -> normal physics drop us ON the mark
    if (g_tpPin)
    {
        long long tpMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - g_tpPinStart)
                             .count();
        if (tpMs >= 500)
        {
            g_tpPin = false;
            g_forceGlideTP = false; // glide spoof off -> fall the last 7 m
        }
        else
        {
            void *tpMatch = game_sdk->Curent_Match();
            void *tpLocal = tpMatch ? game_sdk->GetLocalPlayer(tpMatch) : nullptr;
            void *tpTf = tpLocal ? game_sdk->Component_GetTransform(tpLocal) : nullptr;
            if (tpTf)
                Transform_SetPosition(tpTf, g_tpPinPos); // glued at +7 m
        }
    }

    // ---- Tele Mark (Elite ExecuteTeleMark, one-shot per toggle) ----
    if (!Vars.TeleMark)
    {
        g_tmDone = false;
    }
    else if (!g_tmDone)
    {
        Vector3 mark = MarkedLoc();
        if (mark == Vector3::zero())
            Vars.TeleMark = false; // no mark on the map -> uncheck (fail-closed)
        else if (TeleportToMark(mark))
            g_tmDone = true;
    }
    if (g_tmDone)
        g_markDbg |= 1;

    // ---- Auto Mark (Elite AutoEnableTPOnPhase2) ----
    if (!Vars.AutoMark)
    {
        g_amFired = false;
        g_amPin = false;
        return;
    }
    void *scene = GetInGameUIScene();
    uint32_t phase = scene ? *(uint32_t *)((uintptr_t)scene + oxo("0xD18")) : 0;
    if (phase != 2)
    {
        g_amFired = false; // new round -> re-arm (Elite resets at phase != 2)
        g_amPin = false;
        return;
    }
    if (!g_amFired)
    {
        Vector3 mark = MarkedLoc();
        if (mark != Vector3::zero() && TeleportToMark(mark))
        {
            g_amFired = true;
            g_amTarget = mark;
            g_amPin = true;
            g_amStart = std::chrono::steady_clock::now();
        }
        return; // waiting for the mark - retried every frame
    }
    g_markDbg |= 2;
    if (g_amPin)
    {
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now() - g_amStart)
                           .count();
        if (ms >= 1500)
            g_amPin = false; // Elite: 1500 ms then release (land on the mark)
        else
        {
            void *match = game_sdk->Curent_Match();
            void *local = match ? game_sdk->GetLocalPlayer(match) : nullptr;
            void *tf = local ? game_sdk->Component_GetTransform(local) : nullptr;
            if (tf)
            {
                Vector3 pin = g_amTarget;
                pin.y += 1.5f;
                Transform_SetPosition(tf, pin); // glued to the mark (anti-knockback)
            }
            g_markDbg |= 4;
        }
    }
}

// ---- SmartCoverKill (Elite SmartCoverKillv2 - the AimKill "cover") ----------
// Enemy behind cover? Sample positions around him (z line, x line, origin,
// 8x8 grid, downward steps), raycast from MY head (layer 12) to his head at
// each candidate; first clear spot -> TELEPORT him into my line of sight so
// the server raycast accepts the damage packet. Already visible -> true
// without moving him (Elite verbatim).
static bool SmartCoverKill(void *enemy)
{
    if (!enemy)
        return false;
    if (tanghinh::isVisible(enemy))
        return true;
    void *enemyTF = game_sdk->Component_GetTransform(enemy);
    if (!enemyTF)
        return false;
    Vector3 enemyRoot = game_sdk->get_position(enemyTF);
    if (enemyRoot == Vector3::zero())
        return false;
    Vector3 enemyHead = GetHeadPosition(enemy);
    if (enemyHead == Vector3::zero())
        return false;
    Vector3 headOffset(enemyHead.x - enemyRoot.x,
                       enemyHead.y - enemyRoot.y,
                       enemyHead.z - enemyRoot.z);
    void *match = game_sdk->Curent_Match();
    if (!match)
        return false;
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local)
        return false;
    Vector3 lpHead = GetHeadPosition(local);
    if (lpHead == Vector3::zero())
        return false;

    struct CoverCand
    {
        bool valid = false;
        Vector3 root = Vector3::zero();
        float dx = 0.0f, dy = 0.0f, dz = 0.0f;
    };
    auto isBetter = [](const CoverCand &a, const CoverCand &b) -> bool {
        if (!b.valid)
            return true;
        bool aFwd = (a.dz > 0.0f), bFwd = (b.dz > 0.0f);
        if (aFwd != bFwd)
            return aFwd && !bFwd;
        if (fabsf(a.dz) != fabsf(b.dz))
            return fabsf(a.dz) < fabsf(b.dz);
        if (fabsf(a.dx) != fabsf(b.dx))
            return fabsf(a.dx) < fabsf(b.dx);
        if (fabsf(a.dy) != fabsf(b.dy))
            return fabsf(a.dy) < fabsf(b.dy);
        return false;
    };
    const float pts[] = {-7.1f, -6.27f, -2.5f, -1.5f, 1.5f, 2.5f, 6.27f, 7.1f};
    auto checkPt = [&](float dx, float dy, float dz, CoverCand &best) -> bool {
        Vector3 tRoot = enemyRoot;
        tRoot.x += dx;
        tRoot.y += dy;
        tRoot.z += dz;
        Vector3 expHead(tRoot.x + headOffset.x, tRoot.y + headOffset.y, tRoot.z + headOffset.z);
        void *hitObj = nullptr;
        if (tanghinh::Physics_Raycast(lpHead, expHead, 12, &hitObj))
            return false;
        CoverCand c;
        c.valid = true;
        c.root = tRoot;
        c.dx = dx;
        c.dy = dy;
        c.dz = dz;
        if (isBetter(c, best))
            best = c;
        return true;
    };

    CoverCand best = {};
    for (int i = 0; i < 8; ++i)
        checkPt(0.0f, 0.0f, pts[i], best);
    if (best.valid)
    {
        Transform_SetPosition(enemyTF, best.root);
        return true;
    }
    for (int i = 0; i < 8; ++i)
        checkPt(pts[i], 0.0f, 0.0f, best);
    if (best.valid)
    {
        Transform_SetPosition(enemyTF, best.root);
        return true;
    }
    checkPt(0.0f, 0.0f, 0.0f, best);
    if (best.valid)
    {
        Transform_SetPosition(enemyTF, best.root);
        return true;
    }
    for (int z = 0; z < 8; ++z)
        for (int x = 0; x < 8; ++x)
            checkPt(pts[x], 0.0f, pts[z], best);
    if (best.valid)
    {
        Transform_SetPosition(enemyTF, best.root);
        return true;
    }
    for (int y = 0; y < 41; ++y)
    {
        float dy = -1.40f - 0.01f * (float)y;
        Vector3 tRoot = enemyRoot;
        tRoot.y += dy;
        Vector3 expHead(tRoot.x + headOffset.x, tRoot.y + headOffset.y, tRoot.z + headOffset.z);
        void *hitObj = nullptr;
        if (tanghinh::Physics_Raycast(lpHead, expHead, 12, &hitObj))
            continue;
        Transform_SetPosition(enemyTF, tRoot);
        return true;
    }
    return false;
}

// ==== StartAimkill — Elite StartAimKillSend / AimkillSendStrike path ==============
// st: 1 ran / 2 no-match / 3 not-live / 4 no-enemy / 5 enemy-fail / 6 no-gun /
// 7 not-a-gun (or weapon baseDamage 0) / 8 out-of-range / 9 SENT /
// 10 no-pool-or-head / 11 AikInit fail (or strike refused w/o a code) /
// 12 no-checkParams / 13 no-damage-msg / 14 strike gate (dead/LOS/head) /
// 15 no-TimeService.  n: 1 = target acquired.
// s: packets sent (AikStrike ++).  c: cover (always 0, cover removed).
void StartAimkill() {
    if (!Vars.AimKillSend) return;
    aikDbgSt = 1;
    aikDbgN = 0;
    aikDbgC = 0;

    void *match = game_sdk->Curent_Match();
    if (!match) { aikDbgSt = 2; return; }

    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || !game_sdk->Component_GetTransform(local)) { aikDbgSt = 2; return; }
    if (game_sdk->get_IsDieing(local) || game_sdk->GetHp(local) <= 0) { aikDbgSt = 3; return; }

    void *enemy = GetClosestEnemyAik(); // instant — bla FOV/angle gates (00s)
    if (!enemy || enemy == local) { aikDbgSt = 4; return; }
    if (!game_sdk->Component_GetTransform(enemy)) { aikDbgSt = 5; return; }
    if (game_sdk->get_MaxHP && !game_sdk->get_MaxHP(enemy)) { aikDbgSt = 5; return; }
    if (game_sdk->get_isLocalTeam && game_sdk->get_isLocalTeam(enemy)) { aikDbgSt = 5; return; }
    if (game_sdk->get_IsDieing && game_sdk->get_IsDieing(enemy)) { aikDbgSt = 5; return; }
    if (game_sdk->GetHp(enemy) <= 0) { aikDbgSt = 5; return; }
    // cover/LOS gates REMOVED (user: "hyd mnha l'cover ... 0 second") — the
    // candidate checks above are all that's left (zexisyy had no LOS gate)
    aikDbgN = 1;

    // fail-closed: every Send-path RVA must resolve (AikInit = dladdr anchor)
    if (!AikInit()) { aikDbgSt = 11; return; }

    void *weapon = aik.GetWeaponOnHand(local);
    if (!weapon) { aikDbgSt = 6; return; }
    if (!AikIsValidGun(weapon)) { aikDbgSt = 7; return; } // Elite IsValidGun gate
    if (!AikInRange(local, enemy, weapon)) { aikDbgSt = 8; return; } // Elite range gate

    // hit-object scratch = the local player's objectPool (Android _HitObjectInfoWp)
    void *objectPool = *(void **)((uint64_t)local + OFF_Zexis_Player_ObjectPool);
    if (!objectPool) { aikDbgSt = 10; return; }

    Vector3 localHead;
    if (!AikHeadPos(local, &localHead)) { aikDbgSt = 10; return; }

    // Elite AimkillSendStrike ("00" Send): hit-scratch fill -> hit hash ->
    // CheckParametros -> shuffle-validate -> StartFiring -> msg 106 with
    // RealDamageValue=HP / DamageValue=baseDamage|(1<<24) / back-area on ->
    // StopFire. AikStrike sends the packet itself and does aikDbgSent++.
    int hpBefore = game_sdk->GetHp(enemy);
    aikDbgSt = 11; // provisional: AikStrike overwrites with the exact gate
    if (!AikStrike(local, weapon, objectPool, localHead, enemy))
        return; // aikDbgSt = exact refusal code (12/13/14/15/7) set in AikStrike
    aikDbgSt = 9; // packet sent
    // Elite AimkillRefreshAfterKill: SwapWeapon 1/2 + reset after a confirmed kill
    if (game_sdk->GetHp(enemy) < hpBefore ||
        (game_sdk->get_IsDieing && game_sdk->get_IsDieing(enemy)))
        AikRefreshAfterKill(local);
}

// ==== Send108 — zexisyy part 2 verbatim (AimKill -> Vars.AimKillSend) =======
// NOTICE: "Message 108 is separate from Aimkill Send. Same code on iOS and
// Android... Channel is 2. Cache flag is 0. Weapon id 1 sends unique id 0."
static void *NewReloadPacket() {
    static void *klass = nullptr;
    static void *(*objectNew)(void *) = nullptr; // dlsym once — 0-second path
    if (!klass) klass = FindGameClass("message", "EOBFCLNNIJH");
    if (!objectNew) objectNew = (void *(*)(void *))UnitySym("il2cpp_object_new");
    if (!klass || !objectNew) return nullptr;
    void *obj = objectNew(klass);
    if (!obj) return nullptr;
    ((void (*)(void *))getRealOffset(OFF_Zexis_Msg108_Ctor))(obj);
    return obj;
}

static bool Send108(void *local, void *enemy) {
    if (!Vars.AimKillSend || !local || !enemy) return false;
    // cover/LOS gates REMOVED (user: "hyd mnha l'cover") — the 108 swap sync
    // goes with every strike, through cover

    auto getWeaponOnHand = (void *(*)(void *))getRealOffset(OFF_Zexis_GetWeaponOnHand);
    auto getItemDataID = (int (*)(void *))getRealOffset(OFF_Zexis_GetItemDataID);
    auto getPlayerID = (COW_GamePlay_IHAAMHPPLMG_o (*)(void *))getRealOffset(OFF_Zexis_GetPlayerID);
    auto playerIDToUint = (uint32_t (*)(COW_GamePlay_IHAAMHPPLMG_o))getRealOffset(OFF_Zexis_PlayerIDToUint);
    auto gameFacadeSend = (bool (*)(uint32_t, void *, uint8_t, bool))getRealOffset(OFF_Zexis_GameFacade_Send);

    void *weapon = getWeaponOnHand(local);
    if (!weapon) return false;

    int weaponId = getItemDataID(weapon);
    uint32_t uniqueId = *(uint32_t *)((uintptr_t)weapon + OFF_Zexis_Weapon_UniqueNetID);
    uint32_t weaponField = (weaponId == 1) ? 0u : uniqueId;
    uint32_t localId = playerIDToUint(getPlayerID(local));

    void *packet = NewReloadPacket();
    if (!packet) return false;

    *(uint32_t *)((char *)packet + OFF_Zexis_Msg108_Id) = localId;
    *(uint32_t *)((char *)packet + OFF_Zexis_Msg108_Unique) = weaponField;
    return gameFacadeSend(OFF_Zexis_ReloadMsgId, packet, 2, 0);
}
