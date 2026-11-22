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
    bool AimKill360 = {};  // AIMKILL 360: rotate toward target so the hit registers (Elite Aimkill360)
    bool fly = {};        // RUNN (ninja run)
    bool Flyingtest = {}; // Fly V2
    float flySpeed = 0.75f;
    float flyhei = 300.0f; // FlyV2 sky altitude (video ref = f l'jouj; 0 = ground zone center)
    float khoangcach = 10.0f;
    // ---- ported from Android EliteXzp (the "Elite" project) ----
    bool FlyOp = {};       // FLY OP (user snippet "FLY HACK OP"): +0.10 m/frame rise (iOS port)
    bool FlyJump = {};     // FLY JUMP (Elite HighJump): jump button held + lift ("ytl3ni")
    bool StopTime = {};   // Speed Timer freeze: m_FixedDeltaTime -> 0.00001 ("stop time")
    bool TeleMark = {};   // Tele Mark: one-shot TP to my own map mark
    bool AutoMark = {};   // Auto TP to the mark at Fight start (phase==2) + 1.5 s pin
    // (Fly Ultra sliders removed — user: "t7iyed Fix Fly w Fly Ultra")
    // ---- ported from Android EliteXzp round 2 (user: "DOWN KILL + V2, FLY FIX, GHOST") ----
    bool DownKillV2 = {};  // DOWN KILL V2: smooth -0.4/f descent to -2.5 m, restore off
    bool Ghost = {};       // GHOST (Elite Ghost Hack V4): underground + WaitForForceSync
    bool UnderGround = {}; // UNDER GROUND: manual underground with surface camera
    bool AutoUnder = {};   // AUTO UNDER: auto-activates underground when round starts (phase == 2)
    float UgDepth = 2.5f;  // Underground depth in meters
    bool AutoSky = {};     // AUTO SKY TP: teleport to rwida/mark -> auto return to current round safe-zone center in sky
    bool ResetGuest = {};  // RESET GUEST: unlock the game's own delete-guest option
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
    static bool s_inited = false;
    if (s_inited && this->Curent_Match)
        return;
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
    if (this->Curent_Match)
        s_inited = true;
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
            void *cam = game_sdk->get_camera();
            if (!cam) return false;
            void *camTF = game_sdk->Component_GetTransform(cam);
            if (!camTF) return false;
            void *hc = Player_GetHeadCollider(enemy);
            if (!hc) return false;
            void *hcTF = game_sdk->Component_GetTransform(hc);
            if (!hcTF) return false;
            void *hitObj = NULL;
            auto Camera = Transform_GetPosition(camTF);
            auto Target = Transform_GetPosition(hcTF);
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
            if (!tanghinh::isVisible(Player))
                continue;
            float distance = Vector3::Distance(getPosition(Player), getPosition(LocalPlayer));
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
// GameFacade.CurrentInGameUIScene() = 0x5918DB4 — guarded by live match & local player so it can NEVER crash at Garena logo/lobby
static void *GetInGameUIScene()
{
    if (!game_sdk || !game_sdk->Curent_Match || !game_sdk->GetLocalPlayer ||
        !game_sdk->Component_GetTransform || !game_sdk->get_MaxHP)
        return nullptr;
    void *match = game_sdk->Curent_Match();
    if (!match || (uintptr_t)match <= 0x100000000ULL || ((uintptr_t)match >> 48) != 0)
        return nullptr;
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || (uintptr_t)local <= 0x100000000ULL || ((uintptr_t)local >> 48) != 0)
        return nullptr;
    if (!game_sdk->Component_GetTransform(local))
        return nullptr;
    if (game_sdk->get_MaxHP(local) <= 0)
        return nullptr;
    void *(*fn)() = (void *(*)())getRealOffset(oxo("0x5918DB4"));
    void *scene = fn ? fn() : nullptr;
    if (!scene || (uintptr_t)scene <= 0x100000000ULL || ((uintptr_t)scene >> 48) != 0)
        return nullptr;
    return scene;
}

void draw_watermark()
{
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

// ============================================================================
//  SKY TP AUTO & PHYSICS-SYNCED TELEPORT (Ported from Android SkyLaunch/TPFly to iOS 1.132.X)
//  Dump-verified iOS RVAs & offsets:
//    - Player.m_CharacterController @ +0x4D8, get_CharacterController() = 0x5631D28
//    - UnityEngine.Collider.set_enabled(bool) = 0x8DF4858
//    - UnityEngine.Behaviour.set_enabled(bool) = 0x8D58C68
//    - UnityEngine.Physics.set_autoSyncTransforms(bool) = 0x8DEB608
//    - COW.UIMapContentController.GetSafeZoneInnerCenter() = 0x23912F0, m_InnerCenterPos @ +0x6C8
//    - COW.UIInGameScene.m_CSMatchInfoInfoCtrl @ +0x798 (UIHudCSMatchInfoController: mSeconds @ +0xB0, min @ +0xC0, sec @ +0xC4)
// ============================================================================
static int flyZSt = 0;         // 0 off / 1 safe-zone center / 2 gather center / 3 round anchor / 4 local fallback / 5 no scene / 6 phase != Fight
static float flyZR = 0.0f;     // OuterRadius (debug)
static int flyZPhase = 0;      // ECSMatchPhase raw (0 Waiting / 1 Prepare / 2 Fight / 3 Post)
static bool flyTPDone = false; // one-shot sky blast done -> free horizontal sky hover
static bool g_skyActivatedByAuto = false;
static bool g_skyTpNextDropZoneGather = false;
static Vector3 g_roundAnchorPos;       // saved position in the map BEFORE teleporting to rwida
static bool g_hasRoundAnchor = false;
static Vector3 g_preRwidaReturnPos;    // locked ground center of the pre-rwida map
static bool g_hasPreRwidaReturnPos = false;
static Vector3 g_lastRwidaMarkPos;     // rwida position to reject when returning to pre-rwida map
static bool g_hasLastRwidaMarkPos = false;
static bool g_skyReturnLockActive = false;
static std::chrono::steady_clock::time_point g_skyReturnLockUntil;

static void *GetLocalCharacterController(void *localPlayer)
{
    if (!localPlayer || (uintptr_t)localPlayer <= 0x100000000ULL)
        return nullptr;
    void *cc = *(void **)((uintptr_t)localPlayer + oxo("0x4D8"));
    if (cc && (uintptr_t)cc > 0x100000000ULL && ((uintptr_t)cc >> 48) == 0)
        return cc;
    static void *(*fnGetCC)(void *) = nullptr;
    static bool tried = false;
    if (!tried)
    {
        tried = true;
        void *cand = (void *)getRealOffset(oxo("0x5631D28"));
        Dl_info goodInfo{}, candInfo{};
        if (cand && dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) &&
            dladdr(cand, &candInfo) && goodInfo.dli_fbase == candInfo.dli_fbase)
        {
            fnGetCC = (void *(*)(void *))cand;
        }
    }
    return fnGetCC ? fnGetCC(localPlayer) : nullptr;
}

static void SetCCEnabled(void *cc, bool enabled)
{
    if (!cc || (uintptr_t)cc <= 0x100000000ULL)
        return;
    static void (*fnSetEnabled)(void *, bool) = nullptr;
    static void (*fnAutoSync)(bool) = nullptr;
    static bool tried = false;
    if (!tried)
    {
        tried = true;
        void *cand = (void *)getRealOffset(oxo("0x8DF4858")); // Collider.set_enabled
        void *syncCand = (void *)getRealOffset(oxo("0x8DEB608")); // Physics.set_autoSyncTransforms
        Dl_info goodInfo{}, candInfo{};
        if (dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) && goodInfo.dli_fbase)
        {
            if (cand && dladdr(cand, &candInfo) && candInfo.dli_fbase == goodInfo.dli_fbase)
                fnSetEnabled = (void (*)(void *, bool))cand;
            if (syncCand && dladdr(syncCand, &candInfo) && candInfo.dli_fbase == goodInfo.dli_fbase)
                fnAutoSync = (void (*)(bool))syncCand;
        }
    }
    if (fnAutoSync)
        fnAutoSync(true);
    if (fnSetEnabled)
        fnSetEnabled(cc, enabled);
}

static void ZeroPhysXFallVelocity(void *localPlayer)
{
    if (!localPlayer || (uintptr_t)localPlayer <= 0x100000000ULL)
        return;
    void *px = *(void **)((uintptr_t)localPlayer + oxo("0x1D48"));
    if (px && (uintptr_t)px > 0x100000000ULL && ((uintptr_t)px >> 48) == 0)
    {
        *(float *)((uintptr_t)px + oxo("0x40")) = 0.0f;
        *(float *)((uintptr_t)px + oxo("0x44")) = 0.0f;
        *(Vector3 *)((uintptr_t)px + oxo("0x34")) = Vector3(0.0f, 0.0f, 0.0f);
        *(Vector3 *)((uintptr_t)px + oxo("0x28")) = Vector3(0.0f, 0.0f, 0.0f);
    }
}

// Real player teleport: moves Transform + root Transform AND forces PhysX CharacterController capsule reposition
static void TeleportPlayerReal(void *localPlayer, void *tf, Vector3 newPos)
{
    if (!tf)
        return;
    void *cc = GetLocalCharacterController(localPlayer);
    if (cc)
        SetCCEnabled(cc, false);
    Transform_SetPosition(tf, newPos);
    if (localPlayer)
    {
        void *rt = *(void **)((uintptr_t)localPlayer + oxo("0x3D0"));
        if (rt && (uintptr_t)rt > 0x100000000ULL && rt != tf)
            Transform_SetPosition(rt, newPos);
    }
    if (cc)
        SetCCEnabled(cc, true);
    ZeroPhysXFallVelocity(localPlayer);
}

static inline float Dist2D_XZ(Vector3 a, Vector3 b)
{
    float dx = a.x - b.x;
    float dz = a.z - b.z;
    return sqrtf(dx * dx + dz * dz);
}

static inline bool IsNearRwidaMark(Vector3 p)
{
    if (!g_hasLastRwidaMarkPos || g_lastRwidaMarkPos == Vector3::zero())
        return false;
    return Dist2D_XZ(p, g_lastRwidaMarkPos) < 55.0f;
}

// 3D world SafeZone center ONLY (match->SafeZone +0x210 -> ZoneDefinition +0xB8 -> OuterCenter +0x14 / InnerCenter +0x24)
// NOTE: Never use UIMapContentController.GetSafeZoneInnerCenter() here because that returns 2D NGUI minimap pixel coords!
static bool BD_TryGetSafeZoneCenter(void *match, Vector3 &out)
{
    if (match && (uintptr_t)match > 0x100000000ULL)
    {
        void *safeZone = *(void **)((uintptr_t)match + oxo("0x210"));
        void *zoneDef = (safeZone && (uintptr_t)safeZone > 0x100000000ULL)
                            ? *(void **)((uintptr_t)safeZone + oxo("0xB8"))
                            : nullptr;
        if (zoneDef && (uintptr_t)zoneDef > 0x100000000ULL)
        {
            Vector3 zoneC = *(Vector3 *)((uintptr_t)zoneDef + oxo("0x14"));
            float zoneR = *(float *)((uintptr_t)zoneDef + oxo("0x20"));
            flyZR = zoneR;
            if (zoneR >= 8.0f && (fabsf(zoneC.x) > 1.0f || fabsf(zoneC.z) > 1.0f) && !IsNearRwidaMark(zoneC))
            {
                out = zoneC;
                return true;
            }
            Vector3 innerC = *(Vector3 *)((uintptr_t)zoneDef + oxo("0x24"));
            float innerR = *(float *)((uintptr_t)zoneDef + oxo("0x30"));
            if (innerR >= 5.0f && (fabsf(innerC.x) > 1.0f || fabsf(innerC.z) > 1.0f) && !IsNearRwidaMark(innerC))
            {
                out = innerC;
                return true;
            }
        }
    }
    return false;
}

// Secondary: average position of alive players in the current round (EXCLUDING localPlayer and anyone near the far rwida!)
static bool BD_TryGetZoneGatherCenter(void *match, void *localPlayer, Vector3 &out)
{
    if (!match || (uintptr_t)match <= 0x100000000ULL)
        return false;
    Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((uintptr_t)match + oxo("0x148"));
    if (!players)
        return false;
    int total = players->getSize();
    if (total <= 0)
        return false;
    if (total > 64)
        total = 64;

    Vector3 refAnchor = Vector3::zero();
    bool hasRef = false;
    if (g_hasPreRwidaReturnPos && g_preRwidaReturnPos != Vector3::zero() && !IsNearRwidaMark(g_preRwidaReturnPos))
    {
        refAnchor = g_preRwidaReturnPos;
        hasRef = true;
    }
    else if (g_hasRoundAnchor && g_roundAnchorPos != Vector3::zero() && !IsNearRwidaMark(g_roundAnchorPos))
    {
        refAnchor = g_roundAnchorPos;
        hasRef = true;
    }

    double sumX = 0.0, sumY = 0.0, sumZ = 0.0;
    int n = 0;
    for (int u = 0; u < total; u++)
    {
        void *player = players->getValues()[u];
        if (!player || player == localPlayer)
            continue;
        if (!game_sdk->Component_GetTransform(player))
            continue;
        if ((game_sdk->get_IsDieing && game_sdk->get_IsDieing(player)) || game_sdk->GetHp(player) <= 0)
            continue;
        Vector3 p = getPosition(player);
        if (p == Vector3::zero() || IsNearRwidaMark(p))
            continue;
        if (hasRef && Dist2D_XZ(p, refAnchor) > 220.0f)
            continue;
        sumX += p.x;
        sumY += p.y;
        sumZ += p.z;
        n++;
    }
    if (n == 0)
        return false;
    out = Vector3((float)(sumX / n), (float)(sumY / n), (float)(sumZ / n));
    return true;
}

// Resolves the exact (X, Y, Z) ground center of the PRE-RWIDA MAP (l map lli knti fiha 9bl ma t-teleporta l rwida!)
static int ResolveCurrentRoundGroundCenter(void *match, void *localPlayer, Vector3 curPos, Vector3 &outGround)
{
    Vector3 baseAnchor = Vector3::zero();
    bool hasBaseAnchor = false;
    if (g_hasPreRwidaReturnPos && g_preRwidaReturnPos != Vector3::zero() && !IsNearRwidaMark(g_preRwidaReturnPos))
    {
        baseAnchor = g_preRwidaReturnPos;
        hasBaseAnchor = true;
    }
    else if (g_hasRoundAnchor && g_roundAnchorPos != Vector3::zero() && !IsNearRwidaMark(g_roundAnchorPos))
    {
        baseAnchor = g_roundAnchorPos;
        hasBaseAnchor = true;
    }

    Vector3 safeC = Vector3::zero();
    Vector3 gatherC = Vector3::zero();
    bool hasSafe = BD_TryGetSafeZoneCenter(match, safeC);
    bool hasGather = BD_TryGetZoneGatherCenter(match, localPlayer, gatherC);

    // If we have a saved pre-rwida map anchor, ONLY accept SafeZone or GatherCenter if it belongs to that SAME map (within 200m)!
    if (hasBaseAnchor)
    {
        if (hasSafe && Dist2D_XZ(safeC, baseAnchor) <= 200.0f)
        {
            if (fabsf(safeC.y) < 0.5f)
                safeC.y = (hasGather && fabsf(gatherC.y) >= 0.5f) ? gatherC.y : baseAnchor.y;
            outGround = safeC;
            return 1; // st=1: SafeZone center inside pre-rwida map
        }
        if (hasGather && Dist2D_XZ(gatherC, baseAnchor) <= 200.0f)
        {
            outGround = gatherC;
            return 2; // st=2: Alive players center inside pre-rwida map
        }
        outGround = baseAnchor;
        return 3; // st=3: Exact pre-rwida map position saved before rwida TP
    }

    if (hasSafe)
    {
        if (fabsf(safeC.y) < 0.5f)
            safeC.y = (hasGather && fabsf(gatherC.y) >= 0.5f) ? gatherC.y : curPos.y;
        outGround = safeC;
        return 1;
    }
    if (hasGather)
    {
        outGround = gatherC;
        return 2;
    }
    outGround = curPos;
    return 4;
}

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
        flyTPDone = false;
        g_skyActivatedByAuto = false;
        g_skyReturnLockActive = false;
        return;
    }

    void *match = game_sdk->Curent_Match();
    if (!match)
        return;

    void *localPlayer = game_sdk->GetLocalPlayer(match);
    if (!localPlayer)
        return;
    if (game_sdk->get_MaxHP(localPlayer) <= 0 ||
        (game_sdk->get_IsDieing && game_sdk->get_IsDieing(localPlayer)) ||
        game_sdk->GetHp(localPlayer) <= 0)
    {
        if (g_skyActivatedByAuto)
        {
            Vars.Flyingtest = false;
            g_skyActivatedByAuto = false;
        }
        flyTPDone = false;
        g_skyReturnLockActive = false;
        return;
    }

    void *tf = game_sdk->Component_GetTransform(localPlayer);
    if (!tf)
        return;

    Vector3 pos = game_sdk->get_position(tf);

    void *scene = GetInGameUIScene();
    if (!scene)
    {
        flyZSt = 5;
        return;
    }
    uint32_t phase = *(uint32_t *)((uintptr_t)scene + oxo("0xD18"));
    flyZPhase = (int)phase;
    if (phase != 2) // Waiting/Prepare/Post: reset auto-sky hover so next round buy phase is on the ground
    {
        flyZSt = 6;
        flyTPDone = false;
        g_skyReturnLockActive = false;
        if (g_skyActivatedByAuto)
        {
            Vars.Flyingtest = false;
            g_skyActivatedByAuto = false;
        }
        return;
    }

    float alt = (Vars.flyhei >= 20.0f) ? Vars.flyhei : 300.0f;

    // One-shot high-altitude teleport directly back to the PRE-RWIDA MAP in the sky
    if (!flyTPDone || g_skyTpNextDropZoneGather)
    {
        g_skyTpNextDropZoneGather = false;
        Vector3 zoneGround = pos;
        flyZSt = ResolveCurrentRoundGroundCenter(match, localPlayer, pos, zoneGround);

        Vector3 target(zoneGround.x, zoneGround.y + alt, zoneGround.z);
        TeleportPlayerReal(localPlayer, tf, target);

        FlyalturaEXposeX = target.x;
        FlyalturaEXposeY = zoneGround.y;
        FlyalturaEXposeZ = target.z;
        FlyalturaEXpose = localPlayer;
        flyTPDone = true;
        return;
    }

    // Update telemetry radius
    {
        void *safeZone = *(void **)((uintptr_t)match + oxo("0x210"));
        void *zoneDef = safeZone ? *(void **)((uintptr_t)safeZone + oxo("0xB8")) : nullptr;
        if (zoneDef)
            flyZR = *(float *)((uintptr_t)zoneDef + oxo("0x20"));
    }

    if ((FlyalturaEXposeX == 0.0f && FlyalturaEXposeY == 0.0f && FlyalturaEXposeZ == 0.0f) ||
        FlyalturaEXpose == nullptr)
    {
        Vector3 zoneGround = pos;
        ResolveCurrentRoundGroundCenter(match, localPlayer, pos, zoneGround);
        FlyalturaEXposeX = zoneGround.x;
        FlyalturaEXposeY = zoneGround.y;
        FlyalturaEXposeZ = zoneGround.z;
        FlyalturaEXpose = localPlayer;
    }

    // While the post-rwida return lock is active (850ms) OR if the tire-launch parabola in the rwida's map
    // pulls the player back toward the rwida / away from the pre-rwida map (> 165m), force full (X, Y, Z) return!
    if (g_skyReturnLockActive)
    {
        if (std::chrono::steady_clock::now() >= g_skyReturnLockUntil)
            g_skyReturnLockActive = false;
    }

    Vector3 anchorXZ(FlyalturaEXposeX, FlyalturaEXposeY, FlyalturaEXposeZ);
    bool pulledBackToRwida = IsNearRwidaMark(pos) || (Dist2D_XZ(pos, anchorXZ) > 165.0f);
    if (g_skyReturnLockActive || pulledBackToRwida)
    {
        Vector3 lockedSkyPos(FlyalturaEXposeX, FlyalturaEXposeY + alt, FlyalturaEXposeZ);
        TeleportPlayerReal(localPlayer, tf, lockedSkyPos);
        return;
    }

    // Hover loop inside pre-rwida map: keep altitude locked in the sky while allowing free horizontal movement/look
    ZeroPhysXFallVelocity(localPlayer);
    Transform_SetPosition(tf, Vector3(pos.x, FlyalturaEXposeY + alt, pos.z));
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

// null-checked head position — prefers HeadCollider transform (RIOSSM 0x000a1f4c..0x000a1f8c)
// because Unity Animator can cull bone transforms when an enemy is off-screen / behind the camera (360°),
// whereas HeadCollider's physics transform is always live in 3D space.
static bool AikHeadPos(void *p, Vector3 *out)
{
    if (!p)
        return false;
    if (aik.HeadCollider)
    {
        void *hc = aik.HeadCollider(p);
        if (hc)
        {
            void *hcTf = game_sdk->Component_GetTransform(hc);
            if (hcTf)
            {
                Vector3 pos = game_sdk->get_position(hcTf);
                if (pos != Vector3::zero())
                {
                    *out = pos;
                    return true;
                }
            }
        }
    }
    void *tf = game_sdk->GetHeadPositions(p);
    if (!tf)
        return false;
    *out = game_sdk->get_position(tf);
    return *out != Vector3::zero();
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
    //old zexisyy: WeaponDoSomething/Validate called UNCONDITIONALLY (the
    // Android EnableShuffleParams gate never fired on iOS -> strike refused)
    void (*wValidate)(void *, void *, uint32_t) =
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
//  Fly Op / Stop Time / Mark TP / AimKill cover
//  Every offset dump-verified vs THIS build's dump.cs (fail-closed).
// ============================================================
#include <chrono>
#include <math.h>

// ---- FLY OP (user snippet "FLY HACK OP" — ported to iOS) --------------------
// Original snippet: pos.y += 0.10f per frame via Transform_INTERNAL_SetPosition,
// guarded by its own flag and exclusive with the other fly ("if (realFly) return").
// iOS port: game_sdk->Component_GetTransform / get_position / Transform_SetPosition,
// and the step is dt-based (6.0 m/s = the snippet's 0.10 f @60fps) so it climbs at
// the SAME speed on 60 Hz and 120 Hz screens (user: "t-updati liya had l'fly w
// trado iOS"). Exclusive with Fly V2 (Flyingtest = the snippet's realFly).
// Lobby/dead guard via get_MaxHP <= 0. Endless rise (no cap) — same as snippet.
// mark helpers are defined further down (kept for Tele Mark)
static Vector3 MarkedLoc();
static bool TeleportToMark(Vector3 target);
void RunFlyOp(float dt)
{
    if (!Vars.FlyOp)
        return;
    if (Vars.Flyingtest)
        return; // exclusive: FlyV2 owns Y (snippet: if (realFly) return)
    void *match = game_sdk->Curent_Match();
    if (!match)
        return;
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
        return; // lobby / dead
    void *tf = game_sdk->Component_GetTransform(local);
    if (!tf)
        return;
    Vector3 pos = game_sdk->get_position(tf);
    if (pos == Vector3::zero())
        return;
    // snippet: +0.10f per frame (60 fps) -> dt-based 6 m/s (clamped dt = safety)
    float step = (dt > 0.0f && dt < 0.25f) ? dt : 0.016f;
    pos.y += 6.0f * step;
    Transform_SetPosition(tf, pos);
}

// ---- FLY JUMP (Elite HighJumpLoop port — user: "zid m3a Fly Op Fly Jup") -----
// Elite HighJump = JumpButton_OnBtnDown(jumpCtrl) EVERY frame (the game's OWN
// jump button held -> the game jumps for real, physics-accepted) + pos.Y += 0.5
// per frame lift. Dump-verified (iOS 1.132.X):
//   UIInGameScene.m_JumpBtnCtrl            0x4D8  (UIHudJumpController)
//   UIHudJumpController.OnBtnDown (override) RVA 0x3384BA4
//   UIHudJumpController.OnBtnUp   (override) RVA 0x3384D60
// OnBtnDown/Up resolved once + dladdr-checked vs get_position's image
// (fail-closed: garbage -> no call). On OFF transition we release the button
// once (OnBtnUp) so the jump state can't stick. Lift = Elite 0.5 f @60fps ->
// dt-based 30 m/s (clamped dt = safety). Runs WITH Fly Op (same direction,
// no fight) — user: "m3a Fly Op".
static void *g_fjBtnDown = nullptr;
static void *g_fjBtnUp = nullptr;
static bool g_fjTried = false;
static bool g_fjHeld = false;
static void *FjResolve(uint64_t rva)
{
    void *cand = (void *)getRealOffset(rva);
    Dl_info goodInfo{};
    Dl_info candInfo{};
    if (cand && dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) &&
        dladdr(cand, &candInfo) && goodInfo.dli_fbase && candInfo.dli_fbase &&
        goodInfo.dli_fbase == candInfo.dli_fbase)
        return cand;
    return nullptr;
}
void RunFlyJump(float dt)
{
    if (!Vars.FlyJump)
    {
        if (g_fjHeld)
        {
            g_fjHeld = false;
            // release the jump button once (OnBtnUp) — no stuck jump
            void *scene = GetInGameUIScene();
            void *ctrl = scene ? *(void **)((uintptr_t)scene + oxo("0x4D8")) : nullptr;
            if (g_fjBtnUp && ctrl && (uintptr_t)ctrl > 0x10000)
                ((void (*)(void *))g_fjBtnUp)(ctrl);
        }
        return;
    }
    void *match = game_sdk->Curent_Match();
    if (!match)
        return;
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || game_sdk->get_MaxHP(local) <= 0)
        return; // lobby / dead
    if (!g_fjTried)
    {
        g_fjTried = true;
        g_fjBtnDown = FjResolve(oxo("0x3384BA4")); // UIHudJumpController.OnBtnDown
        g_fjBtnUp = FjResolve(oxo("0x3384D60"));   // UIHudJumpController.OnBtnUp
    }
    void *scene = GetInGameUIScene();
    void *ctrl = scene ? *(void **)((uintptr_t)scene + oxo("0x4D8")) : nullptr;
    if (g_fjBtnDown && ctrl && (uintptr_t)ctrl > 0x10000)
    {
        ((void (*)(void *))g_fjBtnDown)(ctrl); // hold JUMP = game jumps for real
        g_fjHeld = true;
    }
    // lift assist (Elite HighJumpLoop: pos.Y += 0.5/frame @60fps -> 30 m/s)
    void *tf = game_sdk->Component_GetTransform(local);
    if (!tf)
        return;
    Vector3 pos = game_sdk->get_position(tf);
    if (pos == Vector3::zero())
        return;
    float step = (dt > 0.0f && dt < 0.25f) ? dt : 0.016f;
    pos.y += 30.0f * step;
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

// ---- DOWN KILL V2 (Elite DownKillV2Loop; V1 REMOVED: "thyd Down Kill normal") -
// V2: nozol b shewwia -0.4 m/f l -2.5 m men groundY l9itah men l'awl, pin;
// OFF -> 6 frames restore groundY + 0.5 (Elite dkv2ExitHold).
// GHOST COMBO (user: "twali tkhdem b Ghost w bla m-thbetni"): Ghost hard-sets
// my position every frame (underground pin) -> while Ghost is ON, V2 writes NO
// position at all (2 writers katqatelou = stuck / m-thbetek feel) but KEEPS
// WORKING: it still pulls the enemies down (the kill part). V2 re-arms fresh
// from the surface after Ghost goes OFF (run=false while ghost owns Y).
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

static bool g_dkv2Run = false;
static float g_dkv2GroundY = 0.0f;
static float g_dkv2CurY = 0.0f;
static int g_dkv2Exit = 0;
void RunDownKillV2()
{
    // GHOST COMBO (user: "twali tkhdem b Ghost w bla m-thbetni"): see header above.
    bool ghostOwns = Vars.Ghost;
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
            if (ghostOwns)
                return; // Ghost owns Y: NO restore write (no fight = no stuck)
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
    if (ghostOwns)
    {
        // Ghost hard-sets my position every frame (underground pin) — V2 must
        // NOT add its own write on top (2 writers = double-pin / "m-thbetek").
        // V2 still WORKS m3a Ghost: it keeps pulling enemies down (kill part).
        // Re-arm fresh from the surface after Ghost goes OFF.
        g_dkv2Run = false;
        DownKillPullEnemies(local, p);
        return;
    }
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

// ---- FIX FLY + Fly Ultra REMOVED (user: "t7iyed Fix Fly w Fly Ultra") --------
// Replaced by FLY OP (user snippet "FLY HACK OP", dt-based rise) above.

// ---- GHOST (Elite Ghost Hack V4, floating label "GHOST HACK") ----------------
// iOS dump-verified (1.132.X):
//   Player.MainCameraTransform        0x3E8
//   Player.IIPBIDIBJDK (root node)    0x6C8  (ITransformNode)
//   Player.GBJCJJNPPEC (physx)        0x1D48 (GFLBFKKDAMK)
//     -> MovingDirection 0x28 / FallingDirection 0x34 / VSpeed 0x40 / HSpeed 0x44
//       (field order nefs dyal Android: 2 Vector3 + 2 float)
//   GCommon.TransformNode.get_transform 0x6B4964C
//   Player.get_WaitForForceSync        0x5636F20 (Elite hook_get_WaitForForceSync
//     -> if GHOSTMODE_ENABLED() return true) — HOOK SKIPPED, fail-closed:
//     hooking needs MSHookFunction -> links libsubstrate.dylib, which does NOT
//     exist inside an ESign-injected IPA (iOS 26 / iPhone 11, no jailbreak):
//     dyld can't find it at app launch -> CRASH (user: "knht dylib mn ESign
//     f l'ipa kycrashi"). Substrate is now 100% removed from this project
//     (Makefile LIBRARIES + all MSHookFunction calls). Ghost keeps working
//     WITHOUT the hook: the underground/physx/camera pin below re-asserts the
//     position every frame client-side (own position = client authoritative).
// Android-only raw offsets (GhostHackFlag 0x548 / GhostPosition 0x12DC /
// net-shadow 0x18C8 + sub-offsets / MoveXYZ chain) don't match the iOS dump
// layout (0x548 iOS = uint GetCount) -> SKIPPED (fail-closed: dump-verified ghir).
static bool g_ghRun = false;
static float g_ghGroundY = 0.0f;
static float g_ghCamOff = 1.6f;
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
    // (no hook: substrate removed for ESign/IPA — see block comment above)
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

// ---- UNDER GROUND & AUTO UNDER (Round-Start Underground) --------------------
// 1) UnderGround: manual toggle — sinks character below surface (-UgDepth),
//    freezes PhysX gravity/fall, keeps MainCameraTransform at surface level.
// 2) AutoUnder: automatically engages underground when ECSMatchPhase == 2
//    (round starts / Fight phase), and restores + re-arms when phase != 2
//    (between rounds in CS / lobby / post-match) or if player dies.
static bool g_ugRun = false;
static float g_ugGroundY = 0.0f;
static float g_ugCamOff = 1.6f;
static bool g_auRoundLocked = false; // prevents re-arming mid-round if player died

static void RestoreUnderGround(void *local, void *tf)
{
    if (!g_ugRun)
        return;
    g_ugRun = false;
    if (tf)
    {
        Vector3 p = game_sdk->get_position(tf);
        if (p != Vector3::zero())
        {
            p.y = g_ugGroundY + 0.5f;
            for (int i = 0; i < 8; i++)
                Transform_SetPosition(tf, p);
            void *rt = GhostRootTf(local);
            if (rt)
                for (int i = 0; i < 8; i++)
                    Transform_SetPosition(rt, p);
        }
    }
}

void RunUnderGround()
{
    if (Vars.Ghost)
    {
        g_ugRun = false;
        return; // Ghost owns underground position if both are enabled
    }
    if (!Vars.UnderGround && !Vars.AutoUnder && !g_ugRun)
    {
        g_auRoundLocked = false;
        return; // Early return: NEVER touch game pointers at startup/lobby when OFF
    }

    void *m = game_sdk->Curent_Match ? game_sdk->Curent_Match() : nullptr;
    if (!m)
    {
        g_ugRun = false;
        g_auRoundLocked = false;
        return;
    }
    void *local = game_sdk->GetLocalPlayer ? game_sdk->GetLocalPlayer(m) : nullptr;
    if (!local || !game_sdk->get_MaxHP || game_sdk->get_MaxHP(local) <= 0)
    {
        g_ugRun = false;
        g_auRoundLocked = false;
        return;
    }
    void *tf = game_sdk->Component_GetTransform ? game_sdk->Component_GetTransform(local) : nullptr;
    if (!tf)
    {
        g_ugRun = false;
        return;
    }

    void *scene = GetInGameUIScene();
    uint32_t phase = (scene && (uintptr_t)scene > 0x100000000ULL) ? *(uint32_t *)((uintptr_t)scene + oxo("0xD18")) : 0;

    if (phase != 2)
    {
        g_auRoundLocked = false; // re-arm Auto Under for next round
    }

    bool isDead = (game_sdk->get_IsDieing && game_sdk->get_IsDieing(local)) || game_sdk->GetHp(local) <= 0;
    if (isDead)
    {
        if (Vars.AutoUnder && phase == 2)
            g_auRoundLocked = true;
        RestoreUnderGround(local, tf);
        return;
    }

    bool autoWant = (Vars.AutoUnder && scene && phase == 2 && !g_auRoundLocked);
    bool wantUG = Vars.UnderGround || autoWant;

    if (!wantUG)
    {
        RestoreUnderGround(local, tf);
        return;
    }

    Vector3 p = game_sdk->get_position(tf);
    if (p == Vector3::zero())
        return;

    if (!g_ugRun)
    {
        g_ugRun = true;
        g_ugGroundY = p.y;
        g_ugCamOff = 1.6f;
        void *cam0 = *(void **)((uintptr_t)local + oxo("0x3E8"));
        if (cam0)
        {
            Vector3 cp = game_sdk->get_position(cam0);
            if (cp != Vector3::zero())
            {
                float diff = cp.y - g_ugGroundY;
                if (diff > 0.2f && diff < 5.0f)
                    g_ugCamOff = diff;
            }
        }
    }

    float depth = Vars.UgDepth;
    if (depth < 0.5f) depth = 0.5f;
    if (depth > 8.0f) depth = 8.0f;

    Vector3 ug = p;
    ug.y = g_ugGroundY - depth;

    // Freeze PhysX vertical/fall velocity so anti-clip doesn't rubberband us up
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

    // Keep camera at surface level so view never sinks underground
    void *cam = *(void **)((uintptr_t)local + oxo("0x3E8"));
    if (cam)
    {
        Vector3 cp = game_sdk->get_position(cam);
        if (cp != Vector3::zero())
        {
            cp.y = g_ugGroundY + g_ugCamOff;
            Transform_SetPosition(cam, cp);
        }
    }
    void *mainCamObj = game_sdk->get_camera ? game_sdk->get_camera() : nullptr;
    void *mainCamTf = mainCamObj ? game_sdk->Component_GetTransform(mainCamObj) : nullptr;
    if (mainCamTf)
    {
        Vector3 mcp = game_sdk->get_position(mainCamTf);
        if (mcp != Vector3::zero() && mcp.y < g_ugGroundY + 0.5f)
        {
            mcp.y = g_ugGroundY + g_ugCamOff;
            Transform_SetPosition(mainCamTf, mcp);
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

// ---- RESET GUEST (Elite "Reset Guest" switch parity) -------------------------
// Elite hooks COW.GameConfig.get_ResetGuest() -> true (Dobby, Android). We can
// NO LONGER hook (no substrate inside an ESign-injected IPA: dyld fails at
// launch -> CRASH), so we write the getter's backing store DIRECTLY — dump
// verified on this iOS 1.132.X dump:
//   GameConfig (static class, ns "COW") m_LocalConfig @ statics +0x130
//   LocalConfig.resetGuest (bool) @ +0xD6
// (getter neighborhood proves the read: get_GMSDKEnableDebugLog sits right
//  next to get_ResetGuest and reads LocalConfig.gmsdkEnableDebugLog @ +0xA3.)
// While ON: everywhere the game checks the config sees resetGuest = true ->
// its OWN "delete/reset guest" option unlocks (ban popup
// UIAccountForbiddenPopWndView.IsGuestAccountDeleteEnabledInCurrentRegion ->
// ShowDeleteGuestAccountConfirmation -> the GAME's confirm dialog). We NEVER
// delete anything ourselves (fail-closed). m_LocalConfig is re-read from
// statics EVERY frame (if the config object is re-parsed we follow the new
// one, never a stale pointer). Missing il2cpp exports -> status -1 (INFO tab).
static int g_rgState = 0; // 0 = ready, 1 = OK, -1 = failed
static bool g_rgLastToggle = false;

static void ExecuteDirectGuestReset()
{
    // Validate that GarenaMSDK RVAs live inside the loaded UnityFramework image
    Dl_info goodInfo{};
    Dl_info candInfo{};
    void *getMsdkInstPtr = (void *)getRealOffset(oxo("0x6D076E8"));
    void *msdkResetPtr = (void *)getRealOffset(oxo("0x6D098F0"));
    void *rawResetPtr = (void *)getRealOffset(oxo("0x6D14780"));
    if (!dladdr((void *)getRealOffset(0x8D6C1EC), &goodInfo) || !goodInfo.dli_fbase) {
        g_rgState = -1;
        return;
    }

    if (getMsdkInstPtr && msdkResetPtr &&
        dladdr(getMsdkInstPtr, &candInfo) && candInfo.dli_fbase == goodInfo.dli_fbase) {
        auto getMsdkInst = (void *(*)())getMsdkInstPtr;
        auto msdkResetGuest = (void (*)(void *))msdkResetPtr;
        void *inst = getMsdkInst();
        if (inst && (uintptr_t)inst > 0x100000000ULL && ((uintptr_t)inst >> 48) == 0) {
            msdkResetGuest(inst);
            g_rgState = 1;
            return;
        }
    }
    if (rawResetPtr && dladdr(rawResetPtr, &candInfo) && candInfo.dli_fbase == goodInfo.dli_fbase) {
        auto rawResetGuest = (void (*)())rawResetPtr;
        rawResetGuest();
        g_rgState = 1;
    }
}

void RunResetGuest()
{
    if (!Vars.ResetGuest) {
        g_rgLastToggle = false;
        return; // Never touch game memory at startup/logo when OFF
    }
    if (!g_rgLastToggle) {
        g_rgLastToggle = true;
        ExecuteDirectGuestReset();
    }
}

// ---- forceGlideTP — REMOVED (crash fix, user: "mnin fixiti teleport la 7m
// tra wahd l'problem: l'crash — fixih m3ak had l'build") ----------------------
// v1 (f450242) hooked Player.IsFoldWingGliding (0x570D98C) and HELD the spoof
// TRUE for the whole 500 ms pin. Elite NEVER does that: it wraps forceGlideTP
// ONLY around Transform_set_position in the SAME tick (Elite ApplyTeleSpot:
// forceGlideTP = true -> set_position -> forceGlideTP = false, plus their
// "filet de securite: jamais bloque en glide" safety clears). Holding the
// spoof true for 500 ms makes the game run fold-wing-glide logic
// (movement/camera/state machine) with NO real glide started -> CRASH.
// FIX: no hook, no spoof at all — the +7 m is held by the plain per-frame pin
// below (same mechanism as Auto Mark's proven 1.5 s pin, never crashed), and
// the teleport itself worked WITHOUT any spoof before the 7 m change
// (430e8d2) — the pin re-asserts the position every frame anyway.

// shared +7 m pin (Tele Mark): hold the arrival spot for
// 350-500 ms (PLAIN Transform_SetPosition, NO spoof — crash fix) so clamp /
// knockback cannot eat the 7 m, then release -> the game drops us ON the mark/rwida
// (user: "ay blas n teleporta liha khaso ytipini fou9ha b 7 m").
static bool g_tpPin = false;
static Vector3 g_tpPinPos;
static std::chrono::steady_clock::time_point g_tpPinStart;

// ---- AUTO SKY TP CHAIN (Pre-Rwida Map Capture -> Rwida Bounce -> Return to Pre-Rwida Map in Sky) ----
static bool s_teleMarkChainPending = false;
static bool s_teleMarkChainSkyTpPending = false;
static bool s_teleMarkChainWithSkyTp = false;
static bool s_teleMarkChainHasMark = false;
static Vector3 s_chainMarkPos; // plain zero-init static (no dynamic init in __mod_init_func)
static float s_rwidaMinY = 999999.0f;
static std::chrono::steady_clock::time_point s_teleMarkChainStart;
static std::chrono::steady_clock::time_point s_rwidaFallStart;
static bool s_rwidaFallStarted = false;
static bool s_skyTpFiredForRound = false;
static bool s_sawPrepareCountdown = false;
static bool s_lastAutoSkyToggle = false;

// Captures & locks the exact map position (X, Y, Z) where the player was BEFORE teleporting to the rwida
static void CapturePreRwidaReturnMap(void *match, void *local, Vector3 curPos, Vector3 rwidaTarget)
{
    if (rwidaTarget != Vector3::zero())
    {
        g_lastRwidaMarkPos = rwidaTarget;
        g_hasLastRwidaMarkPos = true;
    }

    if (curPos != Vector3::zero() && (rwidaTarget == Vector3::zero() || Dist2D_XZ(curPos, rwidaTarget) > 45.0f))
    {
        g_roundAnchorPos = curPos;
        g_hasRoundAnchor = true;
    }

    Vector3 anchor = (g_hasRoundAnchor && g_roundAnchorPos != Vector3::zero() && !IsNearRwidaMark(g_roundAnchorPos))
                         ? g_roundAnchorPos
                         : curPos;
    if (anchor == Vector3::zero() || IsNearRwidaMark(anchor))
        return;

    Vector3 safeC = Vector3::zero();
    Vector3 gatherC = Vector3::zero();
    bool hasSafe = BD_TryGetSafeZoneCenter(match, safeC);
    bool hasGather = BD_TryGetZoneGatherCenter(match, local, gatherC);

    if (hasSafe && Dist2D_XZ(safeC, anchor) <= 200.0f)
    {
        if (fabsf(safeC.y) < 0.5f)
            safeC.y = (hasGather && fabsf(gatherC.y) >= 0.5f) ? gatherC.y : anchor.y;
        g_preRwidaReturnPos = safeC;
        g_hasPreRwidaReturnPos = true;
        return;
    }
    if (hasGather && Dist2D_XZ(gatherC, anchor) <= 200.0f)
    {
        g_preRwidaReturnPos = gatherC;
        g_hasPreRwidaReturnPos = true;
        return;
    }
    g_preRwidaReturnPos = anchor;
    g_hasPreRwidaReturnPos = true;
}

static void ArmTeleportMarkChain(bool withSkyTp, Vector3 markPos, bool hasMark)
{
    s_teleMarkChainPending = true;
    s_teleMarkChainSkyTpPending = false;
    s_teleMarkChainWithSkyTp = withSkyTp;
    s_teleMarkChainHasMark = hasMark;
    s_chainMarkPos = markPos;
    s_rwidaMinY = markPos.y + 7.0f;
    s_teleMarkChainStart = std::chrono::steady_clock::now();
    s_rwidaFallStarted = false;
}

static void PollTeleportMarkChain()
{
    if (!s_teleMarkChainPending && !s_teleMarkChainSkyTpPending)
        return;

    long long elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - s_teleMarkChainStart)
                              .count();

    if (s_teleMarkChainPending && !s_teleMarkChainSkyTpPending)
    {
        // Always disable Speed Timer after 250ms so PhysX & tire bounce run at normal speed
        if (elapsedMs >= 250 && Vars.StopTime)
        {
            Vars.StopTime = false;
        }

        if (!s_teleMarkChainWithSkyTp)
        {
            if (elapsedMs >= 300)
                s_teleMarkChainPending = false;
            return;
        }

        // If no mark (direct SafeZone Auto Sky), wait 250ms then fire Sky TP
        if (!s_teleMarkChainHasMark)
        {
            if (elapsedMs >= 250)
            {
                s_teleMarkChainPending = false;
                s_teleMarkChainSkyTpPending = true;
            }
            return;
        }

        // With a mark (rwida in another map):
        // 1) Wait for the pin above the rwida to release so the player starts falling onto the rwida
        if (g_tpPin)
            return;

        if (!s_rwidaFallStarted)
        {
            s_rwidaFallStarted = true;
            s_rwidaFallStart = std::chrono::steady_clock::now();
        }

        long long fallMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::steady_clock::now() - s_rwidaFallStart)
                               .count();

        // 2) Watch player Y as they fall onto the rwida and bounce upward
        void *match = game_sdk->Curent_Match();
        void *local = match ? game_sdk->GetLocalPlayer(match) : nullptr;
        void *tf = local ? game_sdk->Component_GetTransform(local) : nullptr;
        bool bouncedOnRwida = false;
        if (tf)
        {
            Vector3 curPos = game_sdk->get_position(tf);
            if (curPos != Vector3::zero())
            {
                if (curPos.y < s_rwidaMinY)
                    s_rwidaMinY = curPos.y;
                // Tire launch detected: player dropped onto the rwida and is now rising upward!
                if (s_rwidaMinY <= s_chainMarkPos.y + 4.5f && curPos.y >= s_rwidaMinY + 0.45f)
                {
                    bouncedOnRwida = true;
                }
            }
        }

        // Trigger return to the PRE-RWIDA MAP as soon as rwida bounce starts OR after 700ms of fall time
        if (bouncedOnRwida || fallMs >= 700)
        {
            s_teleMarkChainPending = false;
            s_teleMarkChainSkyTpPending = true;
        }
        return;
    }

    if (s_teleMarkChainSkyTpPending)
    {
        if (Vars.StopTime)
            Vars.StopTime = false;
        s_teleMarkChainSkyTpPending = false;
        g_tpPin = false;
        g_skyTpNextDropZoneGather = true;
        flyTPDone = false;
        g_skyActivatedByAuto = true;
        // Lock full (X, Y, Z) at the pre-rwida map in the sky for 850ms so the rwida jump parabola cannot pull us back!
        g_skyReturnLockActive = true;
        g_skyReturnLockUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(850);
        Vars.Flyingtest = true; // Activates RunFlyV2 -> returns to the map we were in BEFORE teleporting to the rwida!
    }
}

// ---- TeleportToMark (Elite ExecuteTeleMark sequence + Auto Sky rwida chain) ----
// Saves the exact map we were in BEFORE teleporting to the rwida, teleports above the rwida,
// drops onto the rwida, and when AutoSky is ON returns in the sky to the pre-rwida map!
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

    // BEFORE leaving our current map for the rwida in the other map, capture & lock our pre-rwida map position!
    CapturePreRwidaReturnMap(match, local, pos, target);

    // If we were hovering in the sky, pause sky hover while visiting the rwida
    if (Vars.AutoSky && Vars.Flyingtest)
    {
        Vars.Flyingtest = false;
        flyTPDone = false;
        g_skyReturnLockActive = false;
    }

    Vector3 up = pos;
    up.y += 50.0f;
    TeleportPlayerReal(local, tf, up); // hop first (anti ground-clamp)
    Vector3 dst = target;
    // When AutoSky is ON, drop from +5.0m above the rwida so we hit the tire fast and cleanly; +7.0m for normal TeleMark
    dst.y += Vars.AutoSky ? 5.0f : 7.0f;
    TeleportPlayerReal(local, tf, dst);
    g_tpPin = true;
    g_tpPinPos = dst;
    g_tpPinStart = std::chrono::steady_clock::now();

    if (Vars.AutoSky)
    {
        ArmTeleportMarkChain(true, target, true);
    }
    return true;
}

// ---- RunMarkTP: Tele Mark + Auto Mark + Auto Sky (Phase 2 Round Start) --------
static bool g_tmDone = false;
static bool g_amFired = false;
static bool g_amPin = false;
static Vector3 g_amTarget;
static std::chrono::steady_clock::time_point g_amStart;
static int g_markDbg = 0;

void RunMarkTP()
{
    g_markDbg = 0;

    if (!Vars.TeleMark && !Vars.AutoMark && !Vars.AutoSky &&
        !g_tpPin && !s_teleMarkChainPending && !s_teleMarkChainSkyTpPending && !g_skyActivatedByAuto)
        return;

    // Poll the Auto Sky chain every frame
    PollTeleportMarkChain();

    // Hold the TP arrival above the rwida for 220 ms (when AutoSky is ON) or 500 ms (normal Tele Mark), then release onto the rwida
    if (g_tpPin)
    {
        long long pinLimitMs = Vars.AutoSky ? 220 : 500;
        long long tpMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - g_tpPinStart)
                             .count();
        if (tpMs >= pinLimitMs)
        {
            g_tpPin = false; // pin off -> fall onto the rwida!
        }
        else
        {
            void *tpMatch = game_sdk->Curent_Match();
            void *tpLocal = tpMatch ? game_sdk->GetLocalPlayer(tpMatch) : nullptr;
            void *tpTf = tpLocal ? game_sdk->Component_GetTransform(tpLocal) : nullptr;
            if (tpTf)
                Transform_SetPosition(tpTf, g_tpPinPos);
        }
    }

    // ---- Tele Mark (manual one-shot per toggle; auto-resets switch so you can tap it again anytime) ----
    if (!Vars.TeleMark)
    {
        g_tmDone = false;
    }
    else if (!g_tmDone)
    {
        Vector3 mark = MarkedLoc();
        if (mark == Vector3::zero())
        {
            Vars.TeleMark = false; // no mark on the map -> uncheck (fail-closed)
        }
        else if (TeleportToMark(mark))
        {
            g_tmDone = true;
            if (Vars.AutoSky)
                Vars.TeleMark = false; // auto-uncheck after arming rwida -> sky return chain
        }
    }
    if (g_tmDone)
        g_markDbg |= 1;

    // ---- Round state & Prepare-phase pre-rwida map tracking ----
    void *scene = GetInGameUIScene();
    uint32_t phase = (scene && (uintptr_t)scene > 0x100000000ULL)
                         ? *(uint32_t *)((uintptr_t)scene + oxo("0xD18"))
                         : 0;

    if (phase != 2)
    {
        // In Prepare/Buy phase (phase == 1), continuously record our current round's map position
        // BEFORE the round starts (and NEVER overwrite with any position near the marked rwida!)
        if (phase == 1 && !g_tpPin && !s_teleMarkChainPending)
        {
            void *match = game_sdk->Curent_Match();
            void *local = match ? game_sdk->GetLocalPlayer(match) : nullptr;
            void *tf = local ? game_sdk->Component_GetTransform(local) : nullptr;
            if (tf)
            {
                Vector3 p = game_sdk->get_position(tf);
                Vector3 mark = MarkedLoc();
                if (p != Vector3::zero() && (mark == Vector3::zero() || Dist2D_XZ(p, mark) > 45.0f))
                {
                    CapturePreRwidaReturnMap(match, local, p, mark);
                }
            }
        }
        g_amFired = false;
        g_amPin = false;
        s_skyTpFiredForRound = false;
        s_teleMarkChainPending = false;
        s_teleMarkChainSkyTpPending = false;
        s_rwidaFallStarted = false;
        g_skyReturnLockActive = false;
        if (g_skyActivatedByAuto)
        {
            Vars.Flyingtest = false;
            g_skyActivatedByAuto = false;
        }
    }

    // ---- Handle Auto Sky manual toggle ON/OFF during a round ----
    if (!Vars.AutoSky)
    {
        if (s_lastAutoSkyToggle && g_skyActivatedByAuto)
        {
            Vars.Flyingtest = false;
            g_skyActivatedByAuto = false;
            g_skyReturnLockActive = false;
        }
        s_lastAutoSkyToggle = false;
        s_skyTpFiredForRound = false;
    }
    else if (!s_lastAutoSkyToggle)
    {
        s_lastAutoSkyToggle = true;
        // If toggled ON mid-round (phase == 2), allow it to fire immediately for this round
        s_skyTpFiredForRound = false;
    }

    // ---- AUTO SKY TP (Fires when round enters Fight phase == 2) ----
    if (Vars.AutoSky && (phase == 2) && !s_skyTpFiredForRound)
    {
        void *match = game_sdk->Curent_Match();
        void *local = match ? game_sdk->GetLocalPlayer(match) : nullptr;
        void *tf = local ? game_sdk->Component_GetTransform(local) : nullptr;
        if (local && tf && game_sdk->get_MaxHP(local) > 0 && game_sdk->GetHp(local) > 0)
        {
            Vector3 curPos = game_sdk->get_position(tf);
            Vector3 mark = MarkedLoc();
            if (mark != Vector3::zero())
            {
                // Capture the pre-rwida map position BEFORE teleporting to the rwida in the other map!
                CapturePreRwidaReturnMap(match, local, curPos, mark);

                if (Dist2D_XZ(curPos, mark) > 25.0f)
                {
                    if (TeleportToMark(mark))
                    {
                        s_skyTpFiredForRound = true;
                        s_sawPrepareCountdown = false;
                        g_amFired = true; // prevent AutoMark from interfering
                    }
                }
                else
                {
                    // Already at the rwida -> return immediately to the pre-rwida map in the sky!
                    s_skyTpFiredForRound = true;
                    s_sawPrepareCountdown = false;
                    g_skyTpNextDropZoneGather = true;
                    flyTPDone = false;
                    g_skyActivatedByAuto = true;
                    g_skyReturnLockActive = true;
                    g_skyReturnLockUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(850);
                    Vars.Flyingtest = true;
                }
            }
            else
            {
                // No map mark placed: capture current map and fire Auto Sky straight to current round in sky!
                CapturePreRwidaReturnMap(match, local, curPos, Vector3::zero());
                s_skyTpFiredForRound = true;
                s_sawPrepareCountdown = false;
                ArmTeleportMarkChain(true, Vector3::zero(), false);
            }
        }
    }

    // When AutoSky is ON, AutoSky owns the rwida teleport & sky return — never let AutoMark glue us at the rwida!
    if (Vars.AutoSky)
    {
        g_amPin = false;
        return;
    }

    // ---- Auto Mark (when AutoSky is OFF, classic 1500ms pin on mark) ----
    if (!Vars.AutoMark)
    {
        g_amFired = false;
        g_amPin = false;
        return;
    }
    if (phase != 2)
        return;
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
        return;
    }
    g_markDbg |= 2;
    if (g_amPin)
    {
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::steady_clock::now() - g_amStart)
                           .count();
        if (ms >= 1500)
            g_amPin = false;
        else
        {
            void *match = game_sdk->Curent_Match();
            void *local = match ? game_sdk->GetLocalPlayer(match) : nullptr;
            void *tf = local ? game_sdk->Component_GetTransform(local) : nullptr;
            if (tf)
            {
                Vector3 pin = g_amTarget;
                pin.y += 1.5f;
                Transform_SetPosition(tf, pin);
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
// Plain 4-float HFA struct: guaranteed to pass in s0, s1, s2, s3 under ARM64 AAPCS64
struct FFQuat {
    float x, y, z, w;
};

// ---- resolved engine functions (cached, resolved once) ----
struct AimkillApi {
    void *(*getWeaponOnHand)(void *);
    int   (*getItemDataID)(void *);
    void *(*getHeadCollider)(void *);
    void *(*getGameObject)(void *);
    float (*getRange)(void *);
    void *(*getAimingEntity)(void *, void *);
    void *(*buildCheckParams)(void *, void *, void *, bool, void *);
    void  (*weaponDoSomething)(void *, void *, uint32_t);
    void  (*startFiring)(void *, void *);
    void  (*stopFire)(void *, void *);
    void *(*currentSimTimer)(void *);
    void *(*createDamageReq)(void *);
    COW_GamePlay_IHAAMHPPLMG_o (*getPlayerID)(void *);
    uint32_t (*playerIDToUint)(COW_GamePlay_IHAAMHPPLMG_o);
    void *(*packVector3)(Vector3);
    void *(*packQuaternion)(FFQuat);
    float (*realtimeSinceStartup)();
    bool  (*gameFacadeSend)(uint32_t, void *, uint8_t, bool);
};

static AimkillApi api;
static bool apiReady = false;

static void InitApi() {
    if (apiReady) return;
    api.getWeaponOnHand      = (decltype(api.getWeaponOnHand))      getRealOffset(OFF_Zexis_GetWeaponOnHand);
    api.getItemDataID        = (decltype(api.getItemDataID))        getRealOffset(OFF_Zexis_GetItemDataID);
    api.getHeadCollider      = (decltype(api.getHeadCollider))      getRealOffset(OFF_Zexis_GetHeadCollider);
    api.getGameObject        = (decltype(api.getGameObject))        getRealOffset(OFF_Zexis_GetGameObject);
    api.getRange             = (decltype(api.getRange))             getRealOffset(OFF_Zexis_Weapon_Range);
    api.getAimingEntity      = (decltype(api.getAimingEntity))      getRealOffset(OFF_Zexis_GetAimingEntity);
    api.buildCheckParams     = (decltype(api.buildCheckParams))     getRealOffset(OFF_Zexis_BuildCheckParams);
    api.weaponDoSomething    = (decltype(api.weaponDoSomething))    getRealOffset(OFF_Zexis_WeaponDoSomething);
    api.startFiring          = (decltype(api.startFiring))          getRealOffset(OFF_Zexis_StartFiring);
    api.stopFire             = (decltype(api.stopFire))             getRealOffset(OFF_Zexis_StopFire);
    api.currentSimTimer      = (decltype(api.currentSimTimer))      getRealOffset(OFF_Zexis_CurrentGameSimulationTimer);
    api.createDamageReq      = (decltype(api.createDamageReq))      getRealOffset(OFF_Zexis_CreateDamageReq);
    api.getPlayerID          = (decltype(api.getPlayerID))          getRealOffset(OFF_Zexis_GetPlayerID);
    api.playerIDToUint       = (decltype(api.playerIDToUint))       getRealOffset(OFF_Zexis_PlayerIDToUint);
    api.packVector3          = (decltype(api.packVector3))          getRealOffset(OFF_Zexis_PackVector3);
    api.packQuaternion       = (decltype(api.packQuaternion))       getRealOffset(0x3715ED4);
    api.realtimeSinceStartup = (decltype(api.realtimeSinceStartup)) getRealOffset(OFF_Zexis_RealtimeSinceStartup);
    api.gameFacadeSend       = (decltype(api.gameFacadeSend))       getRealOffset(OFF_Zexis_GameFacade_Send);
    apiReady = true;
}

// RIOSSM fn_a5508: server-side rotation packet (opcode 0x7E0 = 2016)
// Rotates the server state to face each enemy head in 360° right before their 106 packet
// WITHOUT touching client set_aim (camera never moves or sticks).
static void SendRot7E0(Vector3 firePos, Vector3 enemyHead) {
    if (!api.packQuaternion || !api.gameFacadeSend) return;
    Vector3 diff = enemyHead - firePos;
    if ((diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) < 0.0001f) return;
    Quaternion rot = Quaternion::LookRotation(diff, Vector3(0, 1, 0));
    FFQuat q = {rot.x, rot.y, rot.z, rot.w};
    void *rotMsg = api.packQuaternion(q);
    if (rotMsg)
        api.gameFacadeSend(0x7E0, rotMsg, 0, false);
}

// ---- 360° config (from V2 loop) ----
struct AimKill360Cfg {
    bool  Enable;
    int   MaxTargets;   // all visible targets in range
    int   Burst;        // shots per target per frame
    float FovRatio;     // 0..1   -- 0 or 1 => full 360 (distance-only)
    float MaxRange;     // default 6000.f
    float Cooldown;     // seconds; 0 = every frame
};
static AimKill360Cfg cfg{true, 32, 3, 1.f, 6000.f, 0.f};
static float g_nextFireTime = 0.f;

// ---- 360° target filtering (NO frustum/get_isVisible check so enemies behind the player are hit!) ----
static bool IsValidTarget(void *local, void *e, Vector3 firePos, Vector3 localHead, Vector3 *outHead, Vector3 *outOrigin) {
    if (!e || e == local) return false;
    if (!game_sdk->Component_GetTransform(e)) return false;
    if (game_sdk->get_MaxHP && !game_sdk->get_MaxHP(e)) return false;
    if (game_sdk->get_isLocalTeam && game_sdk->get_isLocalTeam(e)) return false;
    if (game_sdk->get_IsDieing && game_sdk->get_IsDieing(e)) return false;
    if (game_sdk->GetHp(e) <= 0) return false;
    // NOTE: Do NOT call game_sdk->get_isVisible(e) (0x563B10C)! That checks camera frustum culling
    // and returns false whenever an enemy is behind the player (off-screen). RIOSSM never calls it.
    if (!AikHeadPos(e, outHead)) return false;
    void *hitObj = nullptr;
    if (!tanghinh::Physics_Raycast(firePos, *outHead, 12, &hitObj)) {
        *outOrigin = firePos;
        return true;
    }
    hitObj = nullptr;
    if (!tanghinh::Physics_Raycast(localHead, *outHead, 12, &hitObj)) {
        *outOrigin = localHead;
        return true;
    }
    if (!g_ugRun && !g_ghRun && tanghinh::isVisible(e)) {
        *outOrigin = firePos;
        return true;
    }
    return false;
}

struct TargetCtx {
    void *enemy;
    Vector3 head;
    Vector3 fireOrigin;
    Vector3 dir;
    float dist;
};

static std::vector<void *> GetPlayerList360(void *match) {
    std::vector<void *> out;
    if (!match) return out;
    Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)match + oxo("0x148"));
    if (!players) return out;
    int sz = players->getSize();
    if (sz <= 0) return out;
    out.reserve(sz);
    for (int i = 0; i < sz; ++i) {
        void *p = players->getValues()[i];
        if (p) out.push_back(p);
    }
    return out;
}

// ---- send message 0x7E0 + 106 + 108 reset for one target (RIOSSM exact sequence) ----
static void SendTakeDamage(void *local, void *weapon, const TargetCtx &t,
                           int weaponId, int baseDamage,
                           void *objectPool,
                           COW_GamePlay_IHAAMHPPLMG_o localPlayerId,
                           void *damageInfo, uint32_t uniqueId,
                           Vector3 firePos) {

    if (game_sdk->get_IsDieing(t.enemy) || game_sdk->GetHp(t.enemy) <= 0) return;

    void *headCollider = api.getHeadCollider(t.enemy);
    if (!headCollider) return;
    void *hitObject = api.getGameObject(headCollider);
    if (!hitObject) return;

    // Reset weapon fire state (+0x5F0) BEFORE every target so CheckParams & server accept all targets at once
    *(float *)((uintptr_t)weapon + OFF_Zexis_Weapon_Field5F0) = 0.0f;

    if (damageInfo) {
        *(int   *)((char *)damageInfo + OFF_Zexis_DamageInfo_HitPart)     = 1;
        *(void **)((char *)damageInfo + OFF_Zexis_DamageInfo_Weapon)      = weapon;
        *(int   *)((char *)damageInfo + OFF_Zexis_DamageInfo_DamageValue) = baseDamage;
        *(COW_GamePlay_IHAAMHPPLMG_o *)((char *)damageInfo + OFF_Zexis_DamageInfo_AttackerID) = localPlayerId;
    }

    // RIOSSM 0x000a2288: pool+0x18 = hitObject (headCollider's GameObject), pool+0x20 = headCollider
    Vector3 origin = t.fireOrigin;
    char *pool = (char *)objectPool;
    *(void **)(pool + OFF_Zexis_Pool_HitGameObject) = hitObject;
    *(void **)(pool + OFF_Zexis_Pool_HitCollider)   = headCollider;
    *(Vector3 *)(pool + OFF_Zexis_Pool_HitLocation) = t.head;
    *(Vector3 *)(pool + OFF_Zexis_Pool_RayDir)      = t.dir;
    *(Vector3 *)(pool + OFF_Zexis_Pool_RayDir2)     = t.dir;
    *(Vector3 *)(pool + OFF_Zexis_Pool_StartPos)    = origin;
    *(int   *)(pool + OFF_Zexis_Pool_Damage)        = baseDamage;
    *(float *)(pool + OFF_Zexis_Pool_Distance)      = t.dist;
    *(int   *)(pool + OFF_Zexis_Pool_Flags60)       = 1;
    *(int   *)(pool + OFF_Zexis_Pool_Flags64)       = 1;
    *(bool  *)(pool + OFF_Zexis_Pool_Flag70)        = true;
    *(bool  *)(pool + OFF_Zexis_Pool_Flag71)        = true;

    uint32_t simTick = 0;
    void *simTimer = api.currentSimTimer(nullptr);
    if (simTimer) {
        simTick = *(uint32_t *)((char *)simTimer + OFF_Zexis_SimTimer_TickCount);
        AikStoreHitHash(weapon, t.enemy, simTick);
    }

    uint32_t enemyNetId = api.playerIDToUint(api.getPlayerID(t.enemy));
    if (!enemyNetId) return;
    uint32_t localNetId = api.playerIDToUint(localPlayerId);
    if (enemyNetId == localNetId) return;
    uint32_t weaponField = (weaponId == 1) ? 0u : uniqueId;

    auto *pkt = (message_C2S_RUDP_TakeDamage_Req_o2 *)api.createDamageReq(local);
    if (!pkt) return;

    pkt->AttackableEntityID = enemyNetId;
    pkt->RealDamageValue    = 0;
    pkt->ShieldDamageValue  = 0;
    pkt->DamageValue        = (uint32_t)game_sdk->GetHp(t.enemy);
    pkt->DamagerID          = localNetId;
    pkt->WeaponDataID       = weaponId;
    pkt->WeaponUniqueID     = weaponField;
    pkt->HitBodyPart        = 1;
    pkt->TickCount          = simTick;
    pkt->FirePos            = (message_PVector3IntPos_o *)api.packVector3(origin);
    pkt->TargetPos          = (message_PVector3IntPos_o *)api.packVector3(t.head);
    pkt->ArmorRatio         = 1.f;
    pkt->ExtraFlag          = 0;
    pkt->ForceNoHeadShot    = false;
    pkt->IsPlayerBackArea   = true;
    pkt->BaseTime           = api.realtimeSinceStartup();
    if (!pkt->FirePos || !pkt->TargetPos) return;

    // RIOSSM 0x000a2390..0x000a2524 exact order:
    // StartFiring -> SendRot7E0(0x7E0) -> GetAimingEntity -> BuildCheckParams -> WeaponDoSomething -> Send(106) -> Send(108) -> StopFire
    api.startFiring(local, weapon);
    SendRot7E0(origin, t.head);
    void *aimingEntity = api.getAimingEntity(local, objectPool);
    void *checkParams  = api.buildCheckParams(weapon, t.enemy, headCollider, false, nullptr);
    if (!checkParams || *(int32_t *)((uintptr_t)checkParams + 0x18) <= 0) {
        api.stopFire(local, weapon);
        aikDbgSt = 12;
        return;
    }

    pkt->CheckParams = (monoList<float *> *)checkParams;
    api.weaponDoSomething(weapon, checkParams, enemyNetId);
    api.gameFacadeSend(OFF_Zexis_TakeDamageMsgId, pkt, 2, 0);
    aikDbgSent++;
    aikDbgSt = 9;

    // RIOSSM 0x000a2498..0x000a2500: send 108 reload/swap packet while enemy alive before StopFire
    Send108(local, t.enemy);
    api.stopFire(local, weapon);
    aik.ResetSwapTime(local);
    (void)aimingEntity;
}

// ======================= MAIN MERGED METHOD ===============================
void StartAimkill() {
    cfg.Enable = Vars.AimKillSend;
    if (!cfg.Enable) return;
    aikDbgSt = 1;
    aikDbgN = 0;
    aikDbgC = 0;

    if (!AikInit()) { aikDbgSt = 11; return; }
    InitApi();

    void *match = game_sdk->Curent_Match();
    if (!match) { aikDbgSt = 2; return; }
    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || !game_sdk->Component_GetTransform(local)) { aikDbgSt = 2; return; }
    if (game_sdk->get_IsDieing(local) || game_sdk->GetHp(local) <= 0) { aikDbgSt = 3; return; }

    // cooldown (V2 behaviour, now driven by RealtimeSinceStartup)
    float now = api.realtimeSinceStartup();
    if (cfg.Cooldown > 0.f && now < g_nextFireTime) return;

    // ---- weapon setup (once) ----
    void *weapon = api.getWeaponOnHand(local);
    if (!weapon) { aikDbgSt = 6; return; }
    *(float *)((uintptr_t)weapon + OFF_Zexis_Weapon_Field5F0) = 0.0f;
    int weaponId = api.getItemDataID(weapon);
    int baseDamage = *(int *)((uintptr_t)weapon + OFF_Zexis_Weapon_BaseDamageField);
    if (weaponId == -1 || baseDamage == 0) { aikDbgSt = 7; return; }
    if (baseDamage < 100) baseDamage = 999;
    float range = api.getRange(weapon);
    if (range <= 0.0f) { aikDbgSt = 8; return; }

    // ---- 360° target acquisition: distance only, no FOV cone (V2) ----
    Vector3 localHead;
    if (!AikHeadPos(local, &localHead)) { aikDbgSt = 10; return; }

    // RIOSSM 0x000a1d54: firePos from local+0x3e8 (MainCameraTransform), fallback to localHead
    Vector3 firePos = localHead;
    void *mainCamTf = *(void **)((uintptr_t)local + oxo("0x3e8"));
    if (mainCamTf) {
        Vector3 cp = game_sdk->get_position(mainCamTf);
        if (cp != Vector3::zero()) firePos = cp;
    }
    // If Underground or Ghost is active, keep firePos & localHead at surface level so raycast & server hitreg succeed
    if (g_ugRun) {
        if (firePos.y < g_ugGroundY + 1.0f) firePos.y = g_ugGroundY + g_ugCamOff;
        if (localHead.y < g_ugGroundY + 1.0f) localHead.y = g_ugGroundY + g_ugCamOff;
    } else if (g_ghRun) {
        if (firePos.y < g_ghGroundY + 1.0f) firePos.y = g_ghGroundY + g_ghCamOff;
        if (localHead.y < g_ghGroundY + 1.0f) localHead.y = g_ghGroundY + g_ghCamOff;
    }

    std::vector<TargetCtx> targets;
    for (void *e : GetPlayerList360(match)) {
        Vector3 eh, origin;
        if (!IsValidTarget(local, e, firePos, localHead, &eh, &origin)) continue;
        float d = Vector3::Distance(origin, eh);
        // RIOSSM 0x000a1fe0: compare against cfg.MaxRange (6000.f) so Sky TP (+300m..400m) kills with any weapon!
        if (d < 0.0005f || (!Vars.Flyingtest && !Vars.AutoSky && d > range) || d > cfg.MaxRange) continue;
        Vector3 dir{0,0,0};
        if (d > 0.0001f) { float inv = 1.f/d; Vector3 v = eh - origin;
                           dir = {v.x*inv, v.y*inv, v.z*inv}; }
        targets.push_back({e, eh, origin, dir, d});
    }
    if (targets.empty()) { aikDbgSt = 4; return; }
    aikDbgN = (int)targets.size();
    std::sort(targets.begin(), targets.end(),
              [](const TargetCtx &a, const TargetCtx &b){ return a.dist < b.dist; });
    int maxTargets = std::clamp(cfg.MaxTargets, 1, 32);
    if ((int)targets.size() > maxTargets) targets.resize(maxTargets);

    void *objectPool = *(void **)((uint64_t)local + OFF_Zexis_Player_ObjectPool);
    if (!objectPool) { aikDbgSt = 10; return; }
    COW_GamePlay_IHAAMHPPLMG_o localPlayerId =
        *(COW_GamePlay_IHAAMHPPLMG_o *)((uintptr_t)local + OFF_Zexis_Player_PlayerID);

    void *damageInfo = g_dmg99;
    if (!damageInfo) {
        void *attrs = *(void **)((uint64_t)local + OFF_Zexis_Player_Attributes);
        if (attrs) {
            void *mod = *(void **)((uint64_t)attrs + OFF_Zexis_Player_DamageModule);
            if (mod) damageInfo = *(void **)((uint64_t)mod + OFF_Zexis_Player_DamageInfo);
        }
    }
    uint32_t uniqueId = *(uint32_t *)((uintptr_t)weapon + OFF_Zexis_Weapon_UniqueNetID);
    int shots = std::clamp(cfg.Burst, 1, 10);

    // ---- Round-robin multi-target strike: hit ALL visible enemies in the SAME frame ----
    int sent = 0;
    for (int s = 0; s < shots; ++s) {
        for (const TargetCtx &t : targets) {
            SendTakeDamage(local, weapon, t, weaponId, baseDamage,
                           objectPool, localPlayerId, damageInfo, uniqueId, firePos);
            ++sent;
        }
    }

    if (sent && cfg.Cooldown > 0.f)
        g_nextFireTime = now + cfg.Cooldown;
}

// ==== Send108 — RIOSSM fn_a56b8 direct RVA allocation (works on stripped iOS UnityFramework) =======
static void *NewReloadPacket() {
    auto objNew = (void *(*)(void *))getRealOffset(oxo("0xA1103B8"));
    auto clsInit = (void (*)(void **))getRealOffset(oxo("0xA11121C"));
    auto typeInfoSlot = (void **)getRealOffset(oxo("0xBB44DA8"));
    auto ctor = (void (*)(void *))getRealOffset(OFF_Zexis_Msg108_Ctor);
    if (objNew && typeInfoSlot && ctor) {
        uintptr_t k = (uintptr_t)(*typeInfoSlot);
        if ((k & 1) != 0 || (k >> 32) == 0) {
            if (clsInit) clsInit(typeInfoSlot);
            k = (uintptr_t)(*typeInfoSlot);
        }
        if ((k & 1) == 0 && (k >> 32) != 0) {
            void *obj = objNew((void *)k);
            if (obj) {
                ctor(obj);
                return obj;
            }
        }
    }
    static void *klass = nullptr;
    static void *(*objectNew)(void *) = nullptr;
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
