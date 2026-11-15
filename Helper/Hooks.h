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
    // user: "mnin ndkhol l'wast ta3 l'3oba, l'fou9 dir MT x CHEAT"
    // TOP-center, ghir f l'match (lobby → scene null → mkhbi)
    if (!GetInGameUIScene())
        return;
    std::string claw = oxorany("MT x CHEAT");
    ImVec2 text_size = verdana_smol->calc_size(16, claw);
    ImVec2 text_pos(
        (ImGui::GetIO().DisplaySize.x - text_size.x) * 0.5f, // TOP center (l'fou9)
        10); // top margin
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

// ---- Fly V2: stay at the height you enabled it on (+ Fly Height) ----
// FlyV2 teleport/pin state (debug UI line f FEATURES tab)
static int flyZSt = 0;        // 0 off / 1 zone center / 2 SafeZone null / 3 ZoneDef null / 4 radius ghda2a (sky fallback)
                             // 5 no in-game scene / 6 phase != Fight (spawn island/tayara = 9bel ma tnzel)
static float flyZR = 0.0f;    // OuterRadius (debug)
static int flyZPhase = 0;     // ECSMatchPhase raw (0 Waiting /1 Prepare /2 Fight /3 Post)
// fallback anchor — l'blasa l'oula li khebbet fih l'teleport (machi pos kola
// frame: kanta pos+flyhei KOLA FRAME = +300/frame = ROCKET infinite — "kytala3ni")
static float flyAX = 0.0f, flyAY = 0.0f, flyAZ = 0.0f;
static bool flyAnch = false;

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
        flyAnch = false;
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

    // >>> FlyV2 v4: TELEPORT instant + PIN continuous (video ref: ground →
    //     instant SKY, senna sotra f l'jouj w ybqa kayqtel men l'fou9 = BOOYAH)
    //     user: "yteleportini l centre dyal l'blasa l'amnina, ma kijerrenich"
    //     - zone mawjouda → (zoneCenter X/Z) + flyhei Y = wast l'blasa l'amnina
    //     - zone null    → sky 3la nefs l'X/Z (fallback, st=2/3/4)
    //     PIN kol frame (continuous set) = gravity ma yhbetich, w zone li
    //     kat9tar kat9elleb m3aha (center ytsarra f kol frame)
    //     chain mn dump iOS (fail-closed, machi mn Android):
    //       match(JMAGGLCNGIG) + 0x210 = SafeZone
    //       SafeZone + 0xB8          = ZoneDefinition (BNINENNKABI)
    //       ZoneDefinition + 0x14    = OuterCenter (Vector3), + 0x20 = OuterRadius
    {
        void *safeZone = *(void **)((uintptr_t)match + oxo("0x210"));
        void *zoneDef = safeZone ? *(void **)((uintptr_t)safeZone + oxo("0xB8")) : nullptr;
        Vector3 target;
        bool gotZone = false;
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
                flyZSt = 1; // zone center + sky altitude
                gotZone = true;
            }
            else
            {
                flyZSt = 4;
            }
        }
        else
        {
            flyZSt = safeZone ? 3 : 2;
        }
        if (!gotZone)
        {
            // fallback: sky 3la l'ANCHOR (l'blasa l'oula) — machi pos kola frame
            // (pos+flyhei kola frame = +300/frame → rocket infinite = "kytala3ni")
            if (!flyAnch)
            {
                flyAX = pos.x;
                flyAY = pos.y;
                flyAZ = pos.z;
                flyAnch = true;
            }
            target.x = flyAX;
            target.y = flyAY + Vars.flyhei;
            target.z = flyAZ;
        }
        Transform_SetPosition(tf, target); // TELEPORT instant + PIN (continuous)
    }
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

// isEnemyInRangeWeapon — range rule copied from Android: r <= 2 means the
// weapon reports km-ish units (r*250), otherwise metres, default 120 m.
static bool AikInRange(void *local, void *enemy, void *weapon)
{
    Vector3 h1, h2;
    if (!AikHeadPos(local, &h1) || !AikHeadPos(enemy, &h2))
        return false;
    float maxRange = 120.0f;
    float r = aik.GetRange(weapon);
    if (r > 0.0f)
        maxRange = (r <= 2.0f) ? (r * 250.0f) : r;
    return Vector3::Distance(h1, h2) <= maxRange;
}

// --- AimKill Send v2 (gaps from Android port) --------------------------------
// Telemetry: st = last RunAimKillSend gate (1 init / 2 no-match / 3 not-live /
// 4 no-gun / 5 no-hitInfo / 6 no-players / 7 no-firePos / 8 no-cands / 9 ran),
// n = candidate count, s = packets sent. Shown in the FEATURES tab.
static int aikDbgSt = 0, aikDbgN = 0, aikDbgSent = 0;

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
// and send the damage packet. Field order/semantics mirror the Android body.
static bool AikStrike(void *local, void *weapon, void *hitInfo, Vector3 firePos, void *target)
{
    if (!local || !weapon || !hitInfo || !target)
        return false;
    if (game_sdk->get_IsDieing(target) || game_sdk->GetHp(target) <= 0)
        return false;
    // raycast LOS = Android isVisible_Aimbot semantics (tanghinh::isVisible =
    // nefs Physics_Raycast layer12 li khdem f ESP — get_isVisible flag = ghlt
    // w mab9ach f candidates zada, Android candidate loop ma 3ndoch visibility)
    if (!tanghinh::isVisible(target))
        return false;

    void *headCollider = aik.HeadCollider(target);
    if (!headCollider)
        return false;
    void *headGameObject = aik.GameObjectOf(headCollider);
    if (!headGameObject)
        return false;

    // CGKJLKPMGDJ: JHAINOBIFMA @0x18 (GameObject), JINDNOFABKB @0x20 (collider)
    *(void **)((uintptr_t)hitInfo + 0x18) = headGameObject;
    *(void **)((uintptr_t)hitInfo + 0x20) = headCollider;

    void *aiming = aik.GetAiming(local, hitInfo); // Player.JGIEOGCGFEP
    if (!aiming)
        return false;

    int baseDamage = aik.GetDamage(weapon);
    if (baseDamage <= 0)
        baseDamage = 99;
    uint32_t weaponID = aik.GetWeaponID(weapon);

    void *timeService = aik.TimeService();
    if (!timeService)
        return false;
    void *msg = aik.MakeDamageMsg(local); // -> EBEDJLFHHHP
    if (!msg)
        return false;

    // Android order: ComputeAndStoreHashExact BEFORE CheckParametros (L2827→2828)
    uint32_t enemyId = aik.IdToU32(aik.GetPlayerID(target));
    AikStoreHitHash(weapon, target, *(uint32_t *)((uintptr_t)timeService + 0x20));

    void *checkParams = aik.CheckParams(weapon, aiming, headCollider, false, (void *)0);
    if (!checkParams)
        return false;
    // Android: shuffle-validate right after CheckParametros (L2836-2845)
    AikShuffleValidate(weapon, checkParams, enemyId); // seedEnc = IdToU32(enemyID)

    aik.StartFiring(local, weapon);

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
    aik.ResetSwapTime(local); // opcode-108 Syns_SwapWeapon_Impl skipped (see header)
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

void RunAimKillSend()
{
    if (!Vars.AimKillSend)
        return;
    static bool busy = false; // re-entrancy guard (Android g_inAimkillSend)
    if (busy)
        return;
    if (!AikInit())
    {
        aikDbgSt = 1;
        return;
    }

    busy = true;
    do
    {
        void *match = game_sdk->Curent_Match();
        if (!match)
        {
            aikDbgSt = 2;
            break;
        }
        void *local = game_sdk->GetLocalPlayer(match);
        if (!local || game_sdk->get_MaxHP(local) <= 0 || game_sdk->get_IsDieing(local))
        {
            aikDbgSt = 3; // lobby / dead: not a live player
            break;
        }
        void *weapon = aik.GetWeaponOnHand(local);
        if (!weapon || !AikIsValidGun(weapon))
        {
            aikDbgSt = 4;
            break;
        }
        void *hitInfo = *(void **)((uintptr_t)local + 0xE50); // Player.FDMIEDDNCEC -> CGKJLKPMGDJ
        if (!hitInfo)
        {
            aikDbgSt = 5;
            break;
        }

        Dictionary<uint8_t *, void **> *players =
            *(Dictionary<uint8_t *, void **> **)((long)match + 0x148);
        if (!players)
        {
            aikDbgSt = 6;
            break;
        }

        Vector3 firePos;
        if (!AikHeadPos(local, &firePos) || firePos == Vector3::zero())
        {
            aikDbgSt = 7;
            break;
        }

        // candidates: alive, non-teammate, visible, in weapon range — scored
        // by crosshair distance with the behind-camera z-check (Android order)
        static const int kMaxT = 64;
        void *cand[kMaxT];
        float score[kMaxT];
        int n = 0;
        ImVec2 middle(ImGui::GetIO().DisplaySize.x * 0.5f,
                      ImGui::GetIO().DisplaySize.y * 0.5f);
        for (int u = 0; u < players->getSize() && n < kMaxT; u++)
        {
            void *p = players->getValues()[u];
            if (!p || p == local)
                continue;
            if (!*(void **)p)
                continue; // stale klass guard (Android)
            if (game_sdk->get_IsDieing(p) || game_sdk->GetHp(p) <= 0)
                continue;
            if (aik.IsLocalTeammate(p))
                continue;
            // NO visibility filter here = Android candidate loop (visibility
            // kaytsenna ghir f AikStrike b raycast tanghinh::isVisible)
            if (!AikInRange(local, p, weapon))
                continue;
            Vector3 head;
            if (!AikHeadPos(p, &head))
                continue;
            bool inFront = false;
            ImVec2 sp = Camera$$WorldToScreen::Checker(head, inFront);
            float s;
            if (inFront)
            {
                float dx = sp.x - middle.x;
                float dy = sp.y - middle.y;
                s = sqrtf(dx * dx + dy * dy);
            }
            else
            {
                s = 100000.0f + Vector3::Distance(firePos, head);
            }
            cand[n] = p;
            score[n] = s;
            n++;
        }
        aikDbgN = n;
        if (n == 0)
        {
            aikDbgSt = 8;
            break;
        }
        aikDbgSt = 9; // all gates passed — strikes running

        // sort by score (bubble — n <= 64, avoids any <algorithm> dependency)
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
                if (score[j] < score[i])
                {
                    float ts = score[i]; score[i] = score[j]; score[j] = ts;
                    void *tp = cand[i]; cand[i] = cand[j]; cand[j] = tp;
                }

        // pass A: up to 2 strikes, kill-aware (Android behaviour)
        bool anyKilled = false;
        int struck = 0;
        for (int i = 0; i < n && struck < 2; i++)
        {
            void *t = cand[i];
            int hpBefore = game_sdk->GetHp(t);
            AikStrike(local, weapon, hitInfo, firePos, t);
            int hpAfter = game_sdk->GetHp(t);
            if (game_sdk->get_IsDieing(t) || hpAfter <= 0)
                anyKilled = true;
            else if (hpAfter < hpBefore - 1)
                anyKilled = true;
            struck++;
        }
        // pass B: every candidate gets its strike (Android double-hits the top 2)
        for (int i = 0; i < n; i++)
            AikStrike(local, weapon, hitInfo, firePos, cand[i]);

        if (anyKilled)
            AikRefreshAfterKill(local);
    } while (0);
    busy = false;
}