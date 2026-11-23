// Standalone CPU update candidate, 12 November 2026.

typedef unsigned long DWORD;
typedef int BOOL;
typedef void* HANDLE;
typedef void* HMODULE;
typedef unsigned int UINT_PTR;

#pragma comment(linker, "/alternatename:_GetModuleHandleW@4=_GetModuleHandleW")
#pragma comment(linker, "/alternatename:_Sleep@4=_Sleep")
#pragma comment(linker, "/alternatename:_CreateThread@24=_CreateThread")
#pragma comment(linker, "/alternatename:_DisableThreadLibraryCalls@4=_DisableThreadLibraryCalls")
#pragma comment(linker, "/alternatename:_VirtualProtect@16=_VirtualProtect")
#pragma comment(linker, "/alternatename:_FlushInstructionCache@12=_FlushInstructionCache")

extern "C" int _fltused = 0;
extern "C" void* __cdecl memset(void* dest, int value, unsigned size)
{
    unsigned char* p = (unsigned char*)dest;
    for (unsigned i = 0; i < size; ++i)
        p[i] = (unsigned char)value;
    return dest;
}

#define TRUE 1
#define DLL_PROCESS_ATTACH 1
#define GENERIC_WRITE 0x40000000
#define FILE_SHARE_READ 0x00000001
#define CREATE_ALWAYS 2
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define PAGE_EXECUTE_READWRITE 0x40
#define INVALID_HANDLE_VALUE ((HANDLE)(-1))

#define EXTRA_RVA 0x0E38890
#define EXTRA_LABEL "wall_vertices_1238890"

extern "C" HMODULE __stdcall GetModuleHandleW(const wchar_t* name);
extern "C" void __stdcall Sleep(DWORD milliseconds);
extern "C" HANDLE __stdcall CreateThread(
    void* attributes, DWORD stackSize, DWORD(__stdcall* start)(void*), void* parameter, DWORD flags, DWORD* threadId);
extern "C" BOOL __stdcall DisableThreadLibraryCalls(HMODULE module);
extern "C" BOOL __stdcall VirtualProtect(void* address, DWORD size, DWORD newProtect, DWORD* oldProtect);
extern "C" BOOL __stdcall FlushInstructionCache(HANDLE process, const void* address, unsigned int size);

static float g_originalStairDelta = 0.75f;
static float g_postConvertRaise = 0.75f;
static UINT_PTR g_postConvertResumeA = 0;
static UINT_PTR g_postConvertResumeB = 0;
static UINT_PTR g_callRefreshA = 0;
static UINT_PTR g_callRefreshB = 0;

static unsigned Len(const char* text)
{
    unsigned n = 0;
    while (text[n])
        ++n;
    return n;
}

static void PatchFloat(UINT_PTR address, float value, const char* label)
{
    DWORD oldProtect = 0;
    if (!VirtualProtect((void*)address, 4, PAGE_EXECUTE_READWRITE, &oldProtect))
        return;

    unsigned before = *(unsigned*)address;
    *(float*)address = value;

    DWORD ignored = 0;
    VirtualProtect((void*)address, 4, oldProtect, &ignored);
    FlushInstructionCache((HANDLE)-1, (void*)address, 4);

}

static void PatchDword(UINT_PTR address, unsigned value, const char* label)
{
    DWORD oldProtect = 0;
    if (!VirtualProtect((void*)address, 4, PAGE_EXECUTE_READWRITE, &oldProtect))
        return;

    unsigned before = *(unsigned*)address;
    *(unsigned*)address = value;

    DWORD ignored = 0;
    VirtualProtect((void*)address, 4, oldProtect, &ignored);
    FlushInstructionCache((HANDLE)-1, (void*)address, 4);

}

static void PatchJump(UINT_PTR address, void* target, DWORD stolenSize, const char* label)
{
    DWORD oldProtect = 0;
    if (!VirtualProtect((void*)address, stolenSize, PAGE_EXECUTE_READWRITE, &oldProtect))
        return;

    unsigned char* p = (unsigned char*)address;
    p[0] = 0xE9;
    *(int*)(p + 1) = (int)((UINT_PTR)target - (address + 5));
    for (DWORD i = 5; i < stolenSize; ++i)
        p[i] = 0x90;

    DWORD ignored = 0;
    VirtualProtect((void*)address, stolenSize, oldProtect, &ignored);
    FlushInstructionCache((HANDLE)-1, (void*)address, stolenSize);

}

static UINT_PTR g_categoryResume=0, g_worldCall=0, g_floorClassResume=0;
static UINT_PTR g_floorTraceResume=0; static float g_previewHeight=0.75f; static unsigned g_selectedStage=4; static UINT_PTR g_productResume=0;
static UINT_PTR g_toolResume=0;
static UINT_PTR g_products[64];
static unsigned g_productCount=0;
static unsigned g_lastDeclared=0xFFFFFFFF;
static unsigned g_lastRuntime=0xFFFFFFFF;
extern "C" void __cdecl RecordProduct(unsigned product,unsigned idPointer,unsigned category)
{
    if(category!=4 || !product || !idPointer) return;
    for(unsigned i=0;i<g_productCount;++i) if(g_products[i]==product) return;
    if(g_productCount>=64) return;
    g_products[g_productCount++]=product;

    for(unsigned i=0;i<40;++i) {   }
    
}
extern "C" void __cdecl RecordStageTool(unsigned tool)
{
    unsigned runtime=*(unsigned*)(tool+0x74);
    unsigned declared=*(unsigned*)(tool+0x78);
    if(runtime==g_lastRuntime && declared==g_lastDeclared) return;
    g_lastRuntime=runtime; g_lastDeclared=declared;
    
    g_selectedStage=declared;
    g_previewHeight=declared==80 ? 0.375f : declared==81 ? 1.5f : declared==82 ? 2.25f : 0.75f;

    for(unsigned i=0;i<32;++i) {   }

}
extern "C" __declspec(naked) void CatalogProductHook()
{
    __asm {
        pushfd
        pushad
        cld
        mov edx,eax
        lea eax,[esp+36]
        push dword ptr [eax+8]
        push dword ptr [eax+4]
        push edx
        call RecordProduct
        add esp,12
        popad
        popfd
        mov edx,[eax]
        mov ecx,eax
        call dword ptr [edx+0x30]
        jmp dword ptr [g_productResume]
    }
}
extern "C" __declspec(naked) void StageToolHook()
{
    __asm {
        pushfd
        pushad
        cld
        push ebx
        call RecordStageTool
        add esp,4
        popad
        popfd
        cmp byte ptr [ebx+0xCA],0
        jmp dword ptr [g_toolResume]
    }
}
struct StagePoint { float x,y; int layer; };
struct StageGrid { int x,y,layer; unsigned char mask; unsigned char padding[3]; };
static UINT_PTR g_getWorld=0,g_convertPoint=0,g_baseSurface=0;
static UINT_PTR g_wallResume=0;
static UINT_PTR g_floorResume0=0,g_floorResume1=0,g_floorResume2=0,g_floorResume3=0,g_floorResume4=0;

static unsigned g_cachedMesh=0,g_cachedTriangle=0;
static float g_cachedHeight=0.75f;
static unsigned FloatBits(float v) { return *(unsigned*)&v; }
// Diagnostic values stay in ASI memory; never used to make gameplay decisions.
// Header: signature, core count, landing count, shadow calls, last native offset,
// last shadow point xyz, context count. Core snapshot at16; context at48;
// 32 landing snapshots of24 words at80. Slots are bounded even if counters wrap.
// Former memory-only diagnostic copies are deliberately disabled in this build.

static float HeightForId(unsigned id) { return id==80 ? 0.375f : id==81 ? 1.5f : id==82 ? 2.25f : 0.75f; }
static int WorldBound(void* world,unsigned offset)
{
    typedef int (__thiscall *BoundFn)(void*);
    return ((BoundFn)(*(UINT_PTR*)(*(UINT_PTR*)world+offset)))(world);
}
static unsigned IdAt(StagePoint* p)
{
    StageGrid grid={0};
    typedef void* (__cdecl *WorldFn)();
    typedef StageGrid* (__cdecl *ConvertFn)(StageGrid*,StagePoint*);
    typedef unsigned (__thiscall *QueryFn)(void*,StageGrid*);
    ((ConvertFn)g_convertPoint)(&grid,p);
    void* world=((WorldFn)g_getWorld)();
    if(!world) return ~0u;
    if(grid.x<WorldBound(world,0x144) || grid.x>WorldBound(world,0x14C) ||
       grid.y<WorldBound(world,0x148) || grid.y>WorldBound(world,0x150) ||
       grid.layer<WorldBound(world,0x160) || grid.layer>WorldBound(world,0x164)) return ~0u;
    UINT_PTR query=*(UINT_PTR*)(*(UINT_PTR*)world+0xE8);
    return ((QueryFn)query)(world,&grid);
}
extern "C" unsigned __cdecl FloorHeightBits(unsigned mesh,unsigned byteOffset)
{
    unsigned begin=*(unsigned*)(mesh+0x1C),end=*(unsigned*)(mesh+0x20);
    unsigned triangle=(byteOffset/40)/3*3;
    if(mesh==g_cachedMesh && triangle==g_cachedTriangle) return FloatBits(g_cachedHeight);
    g_cachedMesh=mesh;g_cachedTriangle=triangle;g_cachedHeight=0.75f;
    if(end<begin || (triangle+3)*40>end-begin) return FloatBits(g_cachedHeight);
    const float* a=(const float*)(begin+triangle*40);
    const float* b=(const float*)(begin+(triangle+1)*40);
    const float* c=(const float*)(begin+(triangle+2)*40);
    StagePoint point={(a[0]+b[0]+c[0])/3.0f,(a[1]+b[1]+c[1])/3.0f,0};
    float sourceZ=(a[2]+b[2]+c[2])/3.0f;
    typedef float (__cdecl *SurfaceFn)(StagePoint*);
    // Identify the existing floor by both persisted deck ID and raw surface Z.
    // This avoids deriving a storey number from terrain elevation.
    typedef void* (__cdecl *WorldFn)();
    void* world=((WorldFn)g_getWorld)();
    if(!world) return FloatBits(g_cachedHeight);
    int low=WorldBound(world,0x160),high=WorldBound(world,0x164);
    if(low < -128 || high>255 || high<low) return FloatBits(g_cachedHeight);
    for(int layer=low;layer<=high;++layer) {
        point.layer=layer;
        unsigned id=IdAt(&point);
        if(id<80 || id>82) continue;
        float raw=((SurfaceFn)g_baseSurface)(&point);
        float delta=raw-sourceZ;
        if(delta < -0.02f || delta > 0.02f) continue;
        g_cachedHeight=HeightForId(id);
        
        break;
    }
    return FloatBits(g_cachedHeight);
}
extern "C" unsigned __cdecl WallHeightBits(StagePoint* source)
{
    if(!source) { return 0x3F400000;}
    // Wall endpoints can lie on the boundary. Sample their immediate quadrants.
    // Ambiguous junctions retain native height in this geometry test.
    unsigned chosen=0;bool ambiguous=false;
    static const float samples[8][2]={
        {-.025f,-.0125f},{-.0125f,-.025f},{.0125f,-.025f},{.025f,-.0125f},
        {.025f,.0125f},{.0125f,.025f},{-.0125f,.025f},{-.025f,.0125f}};
    for(unsigned i=0;i<8;++i) {
        StagePoint p={source->x+samples[i][0],source->y+samples[i][1],source->layer};
        unsigned id=IdAt(&p);
        if(id!=4 && (id<80 || id>82)) continue;
        if(chosen && chosen!=id) ambiguous=true;
        chosen=id;
    }
    float h=ambiguous || !chosen ? 0.75f : HeightForId(chosen);
    
    return FloatBits(h);
}
extern "C" __declspec(naked) void FloorHeightHook0()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[ecx+0]
        push eax
        push ebx
        call FloorHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fld dword ptr [esi+8]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_floorResume0]
    }
}
extern "C" __declspec(naked) void FloorHeightHook1()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[ecx+40]
        push eax
        push ebx
        call FloorHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fld dword ptr [esi+48]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_floorResume1]
    }
}
extern "C" __declspec(naked) void FloorHeightHook2()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[ecx+80]
        push eax
        push ebx
        call FloorHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fld dword ptr [esi+88]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_floorResume2]
    }
}
extern "C" __declspec(naked) void FloorHeightHook3()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[ecx+120]
        push eax
        push ebx
        call FloorHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fld dword ptr [esi+128]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_floorResume3]
    }
}
extern "C" __declspec(naked) void FloorHeightHook4()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[ecx+0]
        push eax
        push ebx
        call FloorHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fld dword ptr [esi+8]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_floorResume4]
    }
}
extern "C" __declspec(naked) void WallHeightHook()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        // C23340 replaces its stack argument with the floor number.
        // The original caller point survives at [original ESP+0x30]:
        // A2BD3A reads [ESP+0x2C], then pushes one argument before the call.
        mov eax,[esp+196]
        push eax
        call WallHeightBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_wallResume]
    }
}
static UINT_PTR g_surfaceResume0=0,g_surfaceResume1=0,g_surfaceResume2=0;

extern "C" unsigned __cdecl SurfaceHeightBits(unsigned id)
{
    unsigned bits=id==80 ? 0x3EC00000 : id==81 ? 0x3FC00000 : id==82 ? 0x40100000 : 0x3F400000;
    
    return bits;
}
extern "C" __declspec(naked) void SurfaceHeightHook0()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        mov eax,[esp+160]
        push eax
        call SurfaceHeightBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_surfaceResume0]
    }
}
extern "C" __declspec(naked) void SurfaceHeightHook1()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[esp+168]
        push eax
        call WallHeightBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_surfaceResume1]
    }
}
extern "C" __declspec(naked) void SurfaceHeightHook2()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        mov eax,[esp+160]
        push eax
        call SurfaceHeightBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_surfaceResume2]
    }
}
static UINT_PTR g_stairContextResume=0,g_stairStartResume=0,g_stairPreviewResume=0;
static UINT_PTR g_terrainClassResume=0,g_terrainClassMatch=0,g_stairLandingResume=0;
static unsigned g_stairHeightBits=0x3F400000,g_stairStartId=4;

static unsigned GridStageId(const StageGrid* grid)
{
    StagePoint p={grid->x+0.5f,grid->y+0.5f,grid->layer};
    return IdAt(&p);
}
extern "C" unsigned __cdecl StairPreviewHeightBits(const StageGrid* grid)
{
    return SurfaceHeightBits(GridStageId(grid));
}
extern "C" void __cdecl SetStairHeight(unsigned frame)
{
    const StageGrid* start=*(StageGrid**)(frame+0x0C);
    g_stairStartId=GridStageId(start);
    g_stairHeightBits=SurfaceHeightBits(g_stairStartId);
    
}
extern "C" void __cdecl RecordStairLanding(unsigned frame)
{}
extern "C" __declspec(naked) void StairContextHook()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        push ebp
        call SetStairHeight
        add esp,4
        frstor [esp]
        add esp,108
        popad
        popfd
        push dword ptr [ebp+0x18]
        lea eax,[ebp-0x17C]
        jmp dword ptr [g_stairContextResume]
    }
}
extern "C" __declspec(naked) void StairStartHook()
{
    __asm {
        push eax
        mov eax,[g_stairHeightBits]
        mov [ebp+0x14],eax
        pop eax
        jmp dword ptr [g_stairStartResume]
    }
}
extern "C" __declspec(naked) void StairPreviewHook()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push ebx
        call StairPreviewHeightBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_stairPreviewResume]
    }
}
extern "C" __declspec(naked) void StairTerrainClassHook()
{
    __asm {
        cmp eax,4
        je stageMatch
        cmp eax,80
        jb noStageMatch
        cmp eax,82
        jbe stageMatch
    noStageMatch:
        jmp dword ptr [g_terrainClassResume]
    stageMatch:
        jmp dword ptr [g_terrainClassMatch]
    }
}
extern "C" __declspec(naked) void StairLandingHook()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        push ebp
        call RecordStairLanding
        add esp,4
        frstor [esp]
        add esp,108
        popad
        popfd
        fld dword ptr [ebp-0x60]
        lea eax,[ebp-0xF0]
        jmp dword ptr [g_stairLandingResume]
    }
}
static UINT_PTR g_stairFinderResume=0;

extern "C" unsigned __cdecl StairFinderHeightBits(const StageGrid* grid)
{
    unsigned id=GridStageId(grid),bits=SurfaceHeightBits(id);
    
    return bits;
}
extern "C" __declspec(naked) void StairFinderHook()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        // Unlike the core's pointer argument, this grid is inline at EBP+0C.
        lea eax,[ebp+0x0C]
        push eax
        call StairFinderHeightBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_stairFinderResume]
    }
}
static unsigned g_upperStage=0,g_upperFloor=0,g_lowerFloor=0,g_landingMatched=0,g_upperValidation=0;
static float g_lowerSurface=0,g_landingTerrainDeduct=0.75f;
static UINT_PTR g_landingSurface=0,g_landingContextResume=0,g_landingStepResume=0,g_landingStepNonzero=0;
static UINT_PTR g_landingCreateResume=0,g_originalPlacement=0,g_finderRejectResume=0;
static UINT_PTR g_finderExitSlot=0;
static UINT_PTR g_cornerResume0=0,g_cornerResume1=0,g_cornerResume2=0,g_cornerResume3=0;

static unsigned g_landingTargetId=~0u;

static UINT_PTR g_coreRejectResume=0;
struct StageVertex { int x,y,layer; };
static unsigned g_autoFinderDepth=0,g_autoFinderFrame=0;

// Restrict the correction to the two already-placeable same-storey drops.
// A vertex shared with the source must not borrow the highest adjacent deck
// when the finder is asking about the lower candidate tile's surface.
static unsigned CandidateOwnerBits(unsigned frame,const StageVertex* vertex)
{
    if(!g_autoFinderDepth || !frame)return 0;
    const StageGrid* source=(const StageGrid*)(frame+0xC);
    const StageVertex* cell=(const StageVertex*)(frame-0x2C);
    if(source->layer!=cell->layer || vertex->layer!=cell->layer)return 0;
    if(vertex->x<cell->x || vertex->x>cell->x+1 || vertex->y<cell->y || vertex->y>cell->y+1)return 0;
    unsigned sourceId=GridStageId(source);
    if(sourceId!=81 && sourceId!=82)return 0;
    StagePoint center={cell->x+0.5f,cell->y+0.5f,cell->layer};
    unsigned owner=IdAt(&center);
    
    if(!((sourceId==82 && (owner==81 || owner==4)) || (sourceId==81 && owner==4)))return 0;
    
    return FloatBits(HeightForId(owner));
}

extern "C" unsigned __cdecl CornerHeightBits(const StageVertex* vertex)
{
    // 606BA3 uses C26E60 to construct INTEGER grid vertices. These are not
    // StagePoint floats. Match native C204F0's adjacent-stage test at that vertex.
    bool found=false;float height=0;
    for(unsigned i=0;i<4;++i) {
        StagePoint sample={vertex->x+((i&1)?0.025f:-0.025f),vertex->y+((i&2)?0.025f:-0.025f),vertex->layer};
        unsigned id=IdAt(&sample);
        if(id!=4 && (id<80 || id>82))continue;
        float h=HeightForId(id);
        if(!found || h>height)height=h;
        found=true;
    }
    unsigned bits=found ? FloatBits(height) : 0x3F400000;
    
    return bits;
}
extern "C" unsigned __cdecl CandidateCornerHeightBits(const StageVertex* vertex,unsigned frame)
{
    if(g_autoFinderDepth)g_autoFinderFrame=frame;
    unsigned owned=CandidateOwnerBits(frame,vertex);
    return owned ? owned : CornerHeightBits(vertex);
}
static unsigned g_flatStageFlight=0;
static float g_flatStageRaw=0;
extern "C" unsigned char __cdecl ProposeStageEndpoint(unsigned,unsigned,unsigned,unsigned,StagePoint*,unsigned);
extern "C" void __cdecl SetLandingContext(unsigned frame)
{

    g_flatStageFlight=0;
    g_upperStage=0;g_landingMatched=0;g_landingTerrainDeduct=0.75f;
    const StageGrid* start=*(StageGrid**)(frame+0xC);
    StagePoint* end=*(StagePoint**)(frame+0x18);
    g_upperFloor=start->layer;g_lowerFloor=end->layer;
    unsigned targetId=IdAt(end);g_landingTargetId=targetId;
    if(targetId!=4 && (targetId<80 || targetId>82))return;
    typedef float (__cdecl *SampleFn)(const void*);
    float raw=((SampleFn)g_baseSurface)(end),surface=((SampleFn)g_landingSurface)(end);
    float source=*(float*)(frame-0xB4),delta=surface-raw;
    // Same-storey stages need the same landing match as upstairs flights.
    if((int)g_upperFloor>=(int)g_lowerFloor && delta>0.001f && source>surface+0.001f){
        g_upperStage=1;g_lowerSurface=surface;g_landingTerrainDeduct=delta;
    }
    if(g_upperStage && g_upperFloor==g_lowerFloor){
        StagePoint proposed;
        if(ProposeStageEndpoint(start->x,start->y,start->layer,start->mask,&proposed,0) &&
           proposed.layer==end->layer && end->x>=proposed.x-0.5f && end->x<proposed.x+0.5f &&
           end->y>=proposed.y-0.5f && end->y<proposed.y+0.5f){
            StagePoint sourceCenter={start->x+0.5f,start->y+0.5f,start->layer};
            g_flatStageRaw=((SampleFn)g_baseSurface)(&sourceCenter);g_flatStageFlight=1;
        }
    }
    
}
extern "C" void __cdecl MatchLanding(unsigned frame)
{

    if(!g_upperStage || g_landingMatched)return;
    unsigned index=*(unsigned*)(frame-0x34);float current=*(float*)(frame+0x14);
    if(index<1 || index>256 || current<g_lowerSurface-0.001f || current>g_lowerSurface+0.001f)return;
    typedef float (__cdecl *SampleFn)(const void*);
    float actual=((SampleFn)g_landingSurface)((void*)(frame-0x30));

    bool atSurface=actual>=g_lowerSurface-0.001f && actual<=g_lowerSurface+0.001f;
    if(!atSurface) {
        // Last stair anchor may sit just before the destination stage edge.
        // Check ONE native step increment forward, without moving any packet.
        // Upper-floor flights need this too: the negative-axis boundary belongs
        // to the outside tile, while the positive-axis boundary belongs to the
        // landing. Require the same target ID and matching surface height.
        const StagePoint* currentPoint=(const StagePoint*)(frame-0x30);
        float dx=*(float*)(frame-0x58),dy=*(float*)(frame-0x54);
        if(dx>=-0.251f && dx<=0.251f && dy>=-0.251f && dy<=0.251f &&
           (dx>0.001f || dx< -0.001f || dy>0.001f || dy< -0.001f)) {
            StagePoint next={currentPoint->x+dx,currentPoint->y+dy,currentPoint->layer};
            unsigned nextId=IdAt(&next);
            float nextSurface=((SampleFn)g_landingSurface)(&next);

            atSurface=nextId==g_landingTargetId && nextSurface>=g_lowerSurface-0.001f && nextSurface<=g_lowerSurface+0.001f;
            
        }
    }
    if(!atSurface)return;
    unsigned oldSpan=*(unsigned*)(frame-0x50);
    if(oldSpan<=index || oldSpan>1024)return;
    *(unsigned*)(frame-0x50)=index+1;g_landingMatched=1;
    
}
extern "C" void __cdecl RecordFinderReject(unsigned frame)
{}
extern "C" __declspec(naked) void CornerHook0()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        lea eax,[ebp-0xE8]
        push ebp
        push eax
        call CandidateCornerHeightBits
        add esp,8
        mov [ebp-0xD4],eax
        frstor [esp]
        add esp,108
        popad
        popfd
        jmp dword ptr [g_cornerResume0]
    }
}
extern "C" __declspec(naked) void CornerHook1()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        lea eax,[ebp-0x10C]
        push ebp
        push eax
        call CandidateCornerHeightBits
        add esp,8
        mov [ebp-0xDC],eax
        frstor [esp]
        add esp,108
        popad
        popfd
        jmp dword ptr [g_cornerResume1]
    }
}
extern "C" __declspec(naked) void CornerHook2()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        lea eax,[ebp-0xA8]
        push ebp
        push eax
        call CandidateCornerHeightBits
        add esp,8
        mov [ebp-0xD8],eax
        frstor [esp]
        add esp,108
        popad
        popfd
        jmp dword ptr [g_cornerResume2]
    }
}
extern "C" __declspec(naked) void CornerHook3()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        lea eax,[ebp-0xCC]
        push ebp
        push eax
        call CandidateCornerHeightBits
        add esp,8
        mov [ebp-0xD0],eax
        frstor [esp]
        add esp,108
        popad
        popfd
        jmp dword ptr [g_cornerResume3]
    }
}
extern "C" __declspec(naked) void LandingContextHook()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        push ebp
        call SetLandingContext
        add esp,4
        frstor [esp]
        add esp,108
        popad
        popfd
        lea esi,[ebp-0x17C]
        jmp dword ptr [g_landingContextResume]
    }
}
extern "C" __declspec(naked) void LandingStepHook()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        push ebp
        call MatchLanding
        add esp,4
        frstor [esp]
        add esp,108
        popad
        popfd
        cmp dword ptr [ebp-0x34],ebx
        jne nonzeroStep
        jmp dword ptr [g_landingStepResume]
    nonzeroStep:
        jmp dword ptr [g_landingStepNonzero]
    }
}
typedef unsigned char (__cdecl *PlacementFn)(unsigned,unsigned,unsigned,unsigned,unsigned);
typedef unsigned char (__thiscall *CreateFn)(void*,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned);
extern "C" unsigned char __cdecl LandingPlacement(unsigned object,unsigned position,unsigned direction,unsigned height,unsigned flags)
{
    unsigned copy[3];unsigned checkPosition=position;
    if(g_upperValidation && g_upperStage && ((unsigned*)position)[2]==g_lowerFloor){
        copy[0]=((unsigned*)position)[0];copy[1]=((unsigned*)position)[1];copy[2]=g_upperFloor;checkPosition=(unsigned)copy;
    }
    unsigned char result=((PlacementFn)g_originalPlacement)(object,checkPosition,direction,height,flags);
    
    return result;
}
extern "C" unsigned char __fastcall LandingCreate(unsigned self,unsigned table,unsigned product,unsigned position,unsigned direction,unsigned height,unsigned a4,unsigned a5,unsigned a6,unsigned a7)
{
    unsigned previous=g_upperValidation;
    g_upperValidation=(g_upperStage && g_upperFloor>g_lowerFloor && ((unsigned*)position)[2]==g_lowerFloor && *(float*)&height>=g_lowerSurface-0.001f);
    unsigned char result=((CreateFn)(*(unsigned*)(table+0x64)))((void*)self,product,position,direction,height,a4,a5,a6,a7);
    
    g_upperValidation=previous;return result;
}
extern "C" __declspec(naked) void LandingCreateHook()
{
    __asm {call LandingCreate}
    __asm {test al,al}
    __asm {jmp dword ptr [g_landingCreateResume]}
}
// Native failure enters607A98 (XOR BL,BL), success enters607A9A.
// Hook ONLY at the shared607A9A boundary, preserving both incoming paths.
extern "C" __declspec(naked) void FinderRejectHook()
{
    __asm {
        pushfd
        test bl,bl
        jne finderExitRestore
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        push ebp
        call RecordFinderReject
        add esp,4
        frstor [esp]
        add esp,108
        popad
    finderExitRestore:
        popfd
        or dword ptr [ebp-4],-1
        // Replay the original JMP [native slot], preserving EAX and ESP.
        push eax
        mov eax,[g_finderExitSlot]
        mov eax,[eax]
        xchg eax,[esp]
        ret
    }
}
extern "C" void __cdecl RecordCoreReject(unsigned frame)
{}
extern "C" __declspec(naked) void CoreRejectHook()
{
    __asm {
        pushfd
        pushad
        sub esp,108
        fnsave [esp]
        fwait
        push ebp
        call RecordCoreReject
        add esp,4
        frstor [esp]
        add esp,108
        popad
        popfd
        mov ecx,[ebp+0x24]
        lea eax,[ebp-0x4C]
        jmp dword ptr [g_coreRejectResume]
    }
}
static UINT_PTR g_nativeFinder=0,g_validatorSurfaceResume=0;

typedef unsigned char (__cdecl *FinderFn)(unsigned,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned);

// Propose the same endpoint that an explicit drag can choose. This only feeds
// 608ED1's existing call to 608095; it does not return that builder's result.
// The caller still runs native footprint, terrain, object and placement checks.
extern "C" unsigned char __cdecl ProposeStageEndpointOne(unsigned x,unsigned y,unsigned layer,unsigned direction,StagePoint* output,unsigned shortOnly,unsigned twelveToFour)
{
    if(!output)return 0;
    unsigned mask=direction&255;
    int dx=mask==1?-1:mask==4?1:0,dy=mask==2?1:mask==8?-1:0;
    if(!dx && !dy)return 0;
    // Bounds are checked by IdAt before any world surface query.
    StagePoint source={(int)x+0.5f,(int)y+0.5f,(int)layer};
    unsigned id=IdAt(&source);
    if(shortOnly ? id!=80 : (id!=81 && id!=82))return 0;
    // A two-step stage can land on its same-storey raw floor plane too.
    // Native builder checks still determine whether an actual floor supports it.
    if(twelveToFour && id!=82)return 0;
    unsigned distance=id==80?1:twelveToFour?3:2;
    StagePoint target={source.x+dx*(int)distance,source.y+dy*(int)distance,(int)layer};
    unsigned targetId=IdAt(&target),required=id==80?~0u:twelveToFour?4:id==81?4:81;
    if(targetId!=required)return 0;
    typedef float (__cdecl *SampleFn)(const void*);
    // Reject off-lot candidates before calling the surface sampler. A missing
    // deck ID is also the out-of-bounds sentinel, so check the target grid too.
    typedef void* (__cdecl *WorldFn)();
    void* world=((WorldFn)g_getWorld)();
    if(!world)return 0;
    int tx=(int)x+dx*(int)distance,ty=(int)y+dy*(int)distance;
    if(tx<WorldBound(world,0x144) || tx>WorldBound(world,0x14C) || ty<WorldBound(world,0x148) || ty>WorldBound(world,0x150))return 0;
    for(unsigned i=0;i<4;++i){
        StagePoint inside={target.x+((i&1)?0.25f:-0.25f),target.y+((i&2)?0.25f:-0.25f),target.layer};
        if(IdAt(&inside)!=required)return 0;
    }
    float sourceRaw=((SampleFn)g_baseSurface)(&source),targetRaw=((SampleFn)g_baseSurface)(&target);
    float rawDelta=sourceRaw-targetRaw;
    // Limit this fallback to level ground/floor planes. Slopes keep the native
    // endpoint search, rather than inventing or reshaping a landing.
    if(!(rawDelta>=-0.001f && rawDelta<=0.001f))return 0;
    for(unsigned gapStep=1;gapStep<distance;++gapStep){
        StagePoint gap={source.x+dx*(int)gapStep,source.y+dy*(int)gapStep,source.layer};
        if(IdAt(&gap)!=~0u)return 0;
        float gapDelta=((SampleFn)g_baseSurface)(&gap)-sourceRaw;
        if(!(gapDelta>=-0.001f && gapDelta<=0.001f))return 0;
    }
    *output=target;
    
    return 1;
}

extern "C" unsigned char __cdecl ProposeStageEndpoint(unsigned x,unsigned y,unsigned layer,unsigned direction,StagePoint* output,unsigned shortOnly) {
    if(ProposeStageEndpointOne(x,y,layer,direction,output,shortOnly,0))return 1;
    return !shortOnly && ProposeStageEndpointOne(x,y,layer,direction,output,0,1);
}

static unsigned g_previewEndpointValid=0,g_previewEndpointMask=0;
static StageGrid g_previewEndpointSource={0};
static StagePoint g_previewEndpoint={0};
static UINT_PTR g_nativeCommitCore=0;
extern "C" unsigned char __cdecl ProposeUpperStageEndpoint(unsigned x,unsigned y,unsigned layer,unsigned direction,StagePoint* output)
{
    if(!output || layer<1)return 0;
    unsigned mask=direction&255;
    int dx=mask==1?-1:mask==4?1:0,dy=mask==2?1:mask==8?-1:0;
    if(!dx && !dy)return 0;
    typedef void* (__cdecl *WorldFn)();
    typedef float (__cdecl *SampleFn)(const void*);
    void* world=((WorldFn)g_getWorld)();if(!world)return 0;
    int minX=WorldBound(world,0x144),maxX=WorldBound(world,0x14C),minY=WorldBound(world,0x148),maxY=WorldBound(world,0x150);
    if((int)x<minX || (int)x>maxX || (int)y<minY || (int)y>maxY || (int)layer>WorldBound(world,0x164) || (int)layer-1<WorldBound(world,0x160))return 0;
    StagePoint source={(int)x+0.5f,(int)y+0.5f,(int)layer};
    unsigned sourceId=IdAt(&source);
    // Same-storey stage sources retain the confirmed v29 endpoint rules.
    if(sourceId==4 || (sourceId>=80 && sourceId<=82))return 0;
    float sourceZ=((SampleFn)g_baseSurface)(&source);
    static float stepHeight=0.1875f;
    for(int distance=1;distance<=5;++distance){
        int tx=(int)x+dx*distance,ty=(int)y+dy*distance;
        if(tx<minX || tx>maxX || ty<minY || ty>maxY)break;
        StagePoint target={tx+0.5f,ty+0.5f,(int)layer-1};
        unsigned id=IdAt(&target);if(id!=4 && (id<80 || id>82))continue;
        float targetRaw=((SampleFn)g_baseSurface)(&target),drop=sourceZ-targetRaw-HeightForId(id);
        if(!(drop>=0.374f && drop<=3.001f))continue;
        int steps=0;
        __asm {fld drop} __asm {fdiv dword ptr [stepHeight]} __asm {fistp steps}
        float mismatch=drop-steps*0.1875f;
        if(steps<2 || steps>16 || !(mismatch>=-0.001f && mismatch<=0.001f))continue;
        int span=distance*4-2;
        if(span<steps || span>=steps+4)continue;
        bool supported=true;
        for(unsigned k=0;k<4;++k){StagePoint p={target.x+((k&1)?0.25f:-0.25f),target.y+((k&2)?0.25f:-0.25f),target.layer};if(IdAt(&p)!=id)supported=false;}
        for(int n=1;supported && n<distance;++n){
            StagePoint p={source.x+dx*n,source.y+dy*n,target.layer};unsigned along=IdAt(&p);
            if(along!=~0u && along!=id){supported=false;break;}
            float delta=((SampleFn)g_baseSurface)(&p)-targetRaw;if(!(delta>=-0.001f && delta<=0.001f))supported=false;
        }
        if(!supported)continue;
        *output=target;
        
        return 1;
    }
    return 0;
}
static void RememberPreviewEndpoint(unsigned x,unsigned y,unsigned layer,unsigned direction,const StagePoint* output,unsigned result)
{
    g_previewEndpointValid=0;
    if(!result || !output)return;
    unsigned mask=direction&255;
    if(mask!=1 && mask!=2 && mask!=4 && mask!=8)return;
    StageGrid source={(int)x,(int)y,(int)layer,(unsigned char)direction,{0,0,0}};
    unsigned id=GridStageId(&source);
    // Two-step source is deliberately excluded from the new click adapter.
    if(id==80)return;
    unsigned targetId=IdAt((StagePoint*)output);
    bool pair=output->layer==(int)layer && ((id==81 && targetId==4) || (id==82 && (targetId==81 || targetId==4)));
    bool upper=output->layer<(int)layer && (targetId==4 || (targetId>=80 && targetId<=82));
    if(!pair && !upper)return;
    g_previewEndpointSource=source;g_previewEndpointMask=direction&255;g_previewEndpoint=*output;g_previewEndpointValid=1;
}
typedef unsigned char (__cdecl *CommitCoreFn)(unsigned,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned);
extern "C" unsigned char __cdecl ClickLandingCore(unsigned product,unsigned sourcePointer,unsigned rounding,unsigned mode,unsigned endpointPointer,unsigned terrainBuilder,unsigned objectBuilder,unsigned error)
{
    const StageGrid* source=(const StageGrid*)sourcePointer;
    StagePoint* requested=(StagePoint*)endpointPointer;
    StagePoint chosen={0};unsigned usePreview=0;
    if(g_previewEndpointValid && source && requested && source->x==g_previewEndpointSource.x && source->y==g_previewEndpointSource.y && source->layer==g_previewEndpointSource.layer && (source->mask&g_previewEndpointMask) && GridStageId(source)!=80){
        float dx=requested->x-(source->x+0.5f),dy=requested->y-(source->y+0.5f);
        int axisX=g_previewEndpointMask==1?-1:g_previewEndpointMask==4?1:0,axisY=g_previewEndpointMask==2?1:g_previewEndpointMask==8?-1:0;
        float forward=dx*axisX+dy*axisY,lateral=dx*axisY-dy*axisX;
        // A simple click remains near the source edge. A drag that has already
        // selected a distant landing retains its explicitly chosen endpoint.
        if(forward>=-0.251f && forward<=1.251f && lateral>=-0.511f && lateral<=0.511f){
            chosen=g_previewEndpoint;usePreview=1;
        }
    }
    unsigned endpoint=usePreview?(unsigned)&chosen:endpointPointer;
    unsigned char result=((CommitCoreFn)g_nativeCommitCore)(product,sourcePointer,rounding,mode,endpoint,terrainBuilder,objectBuilder,error);
    // This caller's endpoint is its typed stack-local StagePoint. Publish the
    // accepted point so the original post-commit code uses the same endpoint.
    // Never alter the source grid or any actual piece position/floor packet.
    if(usePreview && result)*requested=chosen;
    
    return result;
}

static UINT_PTR g_clearFinderErrors=0;

static void ResetSupersededFinderErrors(unsigned error,unsigned wasEmpty,unsigned x,unsigned y,unsigned layer)
{
    if(!error || !g_clearFinderErrors)return;
    unsigned rejected=*((unsigned*)error+1);
    // The tool's native placement gate at4D45FC/4D46E7 calls8DCF80,
    // which tests this container's count at+4. A failed finder has already
    // appended an error viaC1E950. That rejected candidate is superseded by
    // the fallback endpoint, but leaving its error blocks placement later.
    // Preserve pre-existing errors and never clear a later core rejection.
    if(wasEmpty){
        typedef void (__thiscall *ClearFn)(void*);
        ((ClearFn)g_clearFinderErrors)((void*)error);
    }
    unsigned after=*((unsigned*)error+1);
    
}

extern "C" unsigned char __cdecl ScopedAutoFinder(unsigned product,unsigned x,unsigned y,unsigned layer,unsigned direction,unsigned output,unsigned flags,unsigned error)
{
    // A two-step stage needs the adjacent ground center (native span=2),
    // rather than the stock finder's full-tile .75 descent before its first test.
    g_previewEndpointValid=0;
    if(ProposeStageEndpoint(x,y,layer,direction,(StagePoint*)output,1))return 1;
    // Prefer the validated 12->4 landing over a native search for ground.
    // The native core still validates and builds every actual stair piece.
    if(ProposeStageEndpointOne(x,y,layer,direction,(StagePoint*)output,0,1)){
        RememberPreviewEndpoint(x,y,layer,direction,(StagePoint*)output,1);return 1;
    }
    unsigned errorsWereEmpty=error && g_clearFinderErrors && *((unsigned*)error+1)==0;
    unsigned previous=g_autoFinderDepth,previousFrame=g_autoFinderFrame;g_autoFinderDepth=previous+1;g_autoFinderFrame=0;
    unsigned char result=((FinderFn)g_nativeFinder)(product,x,y,layer,direction,output,flags,error);
    g_autoFinderDepth=previous;g_autoFinderFrame=previousFrame;
    unsigned fallback=0;
    if(!result && ProposeStageEndpoint(x,y,layer,direction,(StagePoint*)output,0))result=fallback=1;
    if(!result && ProposeUpperStageEndpoint(x,y,layer,direction,(StagePoint*)output))result=fallback=1;
    if(fallback)ResetSupersededFinderErrors(error,errorsWereEmpty,x,y,layer);
    RememberPreviewEndpoint(x,y,layer,direction,(StagePoint*)output,result);
    return result;
}
extern "C" unsigned __cdecl AutoVertexOffsetBits(const StageVertex* vertex)
{
    if(!g_autoFinderDepth)return 0;
    unsigned owned=CandidateOwnerBits(g_autoFinderFrame,vertex);
    if(owned)return owned;
    bool found=false;float height=0;
    for(unsigned i=0;i<4;++i){
        StagePoint p={vertex->x+((i&1)?0.025f:-0.025f),vertex->y+((i&2)?0.025f:-0.025f),vertex->layer};
        unsigned id=IdAt(&p);
        if(id!=4 && (id<80 || id>82))continue;
        float h=HeightForId(id);if(!found || h>height)height=h;found=true;
    }
    unsigned bits=found ? FloatBits(height) : 0;
    
    return bits;
}
extern "C" __declspec(naked) void ValidatorSurfaceHook()
{
    __asm {
        // Execute the original raw vertex-height method with its original argument.
        call dword ptr [edx+0xF8]
        pushfd
        cmp dword ptr [g_autoFinderDepth],0
        je nativeSurfaceOnly
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push esi
        call AutoVertexOffsetBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
    nativeSurfaceOnly:
        popfd
        jmp dword ptr [g_validatorSurfaceResume]
    }
}
extern "C" void __cdecl RecordFloorMesh(unsigned mesh)
{g_cachedMesh=0;}
// Preserve native lookup for every declaration except the three custom stages.
// World descriptors contain the declaration ID, while the legacy category lookup
// can resolve these custom numbers to unrelated products.
extern "C" __declspec(naked) void CategoryHook()
{
    __asm {
        mov eax,[esp+4]
        mov eax,[eax]
        cmp eax,80
        jb nativeLookup
        cmp eax,82
        ja nativeLookup
        mov eax,[esp+8]
        cmp eax,4
        sete al
        movzx eax,al
        ret
    nativeLookup:
        call dword ptr [g_worldCall]
        jmp dword ptr [g_categoryResume]
    }
}
// Native floor-cap classification independently hardcodes declaration 4.
// Set its existing Boolean for 4 and 80..82, without changing the stored ID.
extern "C" __declspec(naked) void FloorClassHook()
{
    __asm {
        cmp eax,4
        je recognized
        cmp eax,80
        jb originalCompare
        cmp eax,82
        jbe recognized
    originalCompare:
        cmp eax,4
        jmp finish
    recognized:
        cmp eax,eax
    finish:
        mov ecx,ebx
        sete byte ptr [esp+0x38]
        jmp dword ptr [g_floorClassResume]
    }
}
extern "C" __declspec(naked) void FloorTraceHook()
{
    __asm {
        pushfd
        pushad
        cld
        push ebx
        call RecordFloorMesh
        add esp,4
        popad
        popfd
        mov cl,byte ptr [ebx+0x4D]
        test cl,cl
        mov [esp+0x18],eax
        jmp dword ptr [g_floorTraceResume]
    }
}

// A8CFA0 builds the secondary surface from stage-named mesh index buffers.
// Its stage vertices still used +.75 at A8D24C after the visible mesh moved.
// Resolve the owning indexed TRIANGLE, so shared boundary vertices do not pick
// a neighbouring stage and the vertex/index orders need not be identical.
static UINT_PTR g_secondaryStageResume=0;
extern "C" unsigned __cdecl IndexedStageHeightBits(unsigned mesh,unsigned indexPointer)
{
    const unsigned native=0x3F400000;
    if(!mesh)return native;
    unsigned vertices=*(unsigned*)(mesh+0x1C),verticesEnd=*(unsigned*)(mesh+0x20);
    unsigned indices=*(unsigned*)(mesh+0x28),indicesEnd=*(unsigned*)(mesh+0x2C);
    if(!vertices || verticesEnd<vertices || verticesEnd-vertices>0x1000000 ||
       !indices || indicesEnd<indices || indicesEnd-indices>0x1000000 ||
       indexPointer<indices || indexPointer>=indicesEnd || ((indexPointer-indices)&1))return native;
    unsigned vertexCount=(verticesEnd-vertices)/40,indexCount=(indicesEnd-indices)/2;
    unsigned triangle=((indexPointer-indices)/2)/3*3;
    if(triangle+3>indexCount)return native;
    const unsigned short* order=(const unsigned short*)indices+triangle;
    if(order[0]>=vertexCount || order[1]>=vertexCount || order[2]>=vertexCount)return native;
    const float* a=(const float*)(vertices+order[0]*40);
    const float* b=(const float*)(vertices+order[1]*40);
    const float* c=(const float*)(vertices+order[2]*40);
    StagePoint point={(a[0]+b[0]+c[0])/3.0f,(a[1]+b[1]+c[1])/3.0f,0};
    float sourceZ=(a[2]+b[2]+c[2])/3.0f;
    if(!(point.x>=-1000000 && point.x<=1000000 && point.y>=-1000000 && point.y<=1000000 &&
         sourceZ>=-1000000 && sourceZ<=1000000))return native;
    typedef void* (__cdecl *WorldFn)();
    typedef float (__cdecl *SurfaceFn)(StagePoint*);
    void* world=((WorldFn)g_getWorld)();
    if(!world)return native;
    int low=WorldBound(world,0x160),high=WorldBound(world,0x164);
    if(low < -128 || high>255 || high<low)return native;
    for(int layer=low;layer<=high;++layer) {
        point.layer=layer;
        unsigned id=IdAt(&point);
        if(id<80 || id>82)continue;
        float delta=((SurfaceFn)g_baseSurface)(&point)-sourceZ;
        if(delta>=-0.02f && delta<=0.02f)return FloatBits(HeightForId(id));
    }
    return native;
}
extern "C" __declspec(naked) void SecondaryStageHeightHook()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push ebx
        push edi
        call IndexedStageHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_secondaryStageResume]
    }
}
static void InstallSecondaryStageSurface(UINT_PTR base)
{
    // Optional and independent: a conflicting render site must never disable
    // the existing, working height/stair hooks. No global constants are changed.
    const unsigned char offset[]={0xD8,0x05,0x1C,0xB3,0x23,0x01};
    const unsigned char indexLookup[]={0x66,0x8B,0x03,0x8B,0x4F,0x1C,0x0F,0xB7,0xC0,0x8D,0x14,0x80,0x8D,0x04,0xD1};
    const unsigned char tail[]={0xD9,0x5C,0x24,0x64,0x8B,0x01,0xFF,0x90,0xA4,0,0,0};
    for(unsigned i=0;i<sizeof(offset);++i)if(((unsigned char*)(base+0x68D24C))[i]!=offset[i])return;
    for(unsigned i=0;i<sizeof(indexLookup);++i)if(((unsigned char*)(base+0x68D1D4))[i]!=indexLookup[i])return;
    for(unsigned i=0;i<sizeof(tail);++i)if(((unsigned char*)(base+0x68D252))[i]!=tail[i])return;
    g_secondaryStageResume=base+0x68D252;
    PatchJump(base+0x68D24C,(void*)SecondaryStageHeightHook,6,"secondary_stage_surface_height");
}

// B69DC0 populates the stage mask handed to the OverlayShadowFloorMaterial
// by AE64F5..AE6514. Its test recognized only declaration 4.
static UINT_PTR g_shadowStageYes=0,g_shadowStageNo=0;
extern "C" __declspec(naked) void ShadowStageMaskHook()
{
    __asm {
        cmp eax,4
        je stageAccepted
        cmp eax,80
        jb nativeStageCompare
        cmp eax,82
        jbe stageAccepted
    nativeStageCompare:
        cmp eax,4
        jmp dword ptr [g_shadowStageNo]
    stageAccepted:
        cmp eax,eax
        jmp dword ptr [g_shadowStageYes]
    }
}
static void InstallShadowStageMask(UINT_PTR base)
{
    const unsigned char query[]={0x8B,0x17,0x55,0x8B,0xCF,0xFF,0x92,0xE8,0,0,0};
    const unsigned char branch[]={0x83,0xF8,0x04,0x75,0x13};
    const unsigned char writeMask[]={0x0F,0xBE,0x55,0x0C,0x8B,0x4C,0x24,0x14,0x8B,0x83,0xAC,0,0,0,0xD3,0xE2,0x09,0x14,0xB0};
    for(unsigned i=0;i<sizeof(query);++i)if(((unsigned char*)(base+0x769F93))[i]!=query[i])return;
    for(unsigned i=0;i<sizeof(branch);++i)if(((unsigned char*)(base+0x769F9E))[i]!=branch[i])return;
    for(unsigned i=0;i<sizeof(writeMask);++i)if(((unsigned char*)(base+0x769FA3))[i]!=writeMask[i])return;
    g_shadowStageYes=base+0x769FA3;g_shadowStageNo=base+0x769FB6;
    PatchJump(base+0x769F9E,(void*)ShadowStageMaskHook,5,"custom_stage_shadow_mask");
}

// Shadow overlay builders B68390/B693xx generate a ground and a stage pass,
// choosing 0 or .75. Replace only the duplicated .75 on each emitted vertex;
// keep the row/pass x87 value and the ground pass exactly as native.
struct ShadowSurfacePoint {float x,y,z;};

extern "C" unsigned __cdecl ShadowPlaneOffsetBits(const ShadowSurfacePoint* source,unsigned nativeBits)
{
    if(nativeBits!=0x3F400000 || !source)return nativeBits;
    if(!(source->x>=-1000000 && source->x<=1000000 && source->y>=-1000000 && source->y<=1000000 &&
         source->z>=-1000000 && source->z<=1000000))return nativeBits;

    typedef void* (__cdecl *WorldFn)();typedef float (__cdecl *SurfaceFn)(StagePoint*);
    void* world=((WorldFn)g_getWorld)();if(!world)return nativeBits;
    int low=WorldBound(world,0x160),high=WorldBound(world,0x164);
    if(low < -128 || high>255 || high<low)return nativeBits;
    unsigned chosen=0;
    for(int layer=low;layer<=high;++layer){
        bool surfaceMatches=false,checkedSurface=false;
        for(unsigned quadrant=0;quadrant<4;++quadrant){
            StagePoint p={source->x+((quadrant&1)?0.025f:-0.025f),source->y+((quadrant&2)?0.025f:-0.025f),layer};
            unsigned id=IdAt(&p);if(id!=4 && (id<80 || id>82))continue;
            
            if(!checkedSurface){
                StagePoint vertex={source->x,source->y,layer};
                float delta=((SurfaceFn)g_baseSurface)(&vertex)-source->z;
                
                surfaceMatches=delta>=-0.02f && delta<=0.02f;checkedSurface=true;
            }
            if(!surfaceMatches)continue;
            // A shared receiver vertex cannot represent two different planes.
            // Preserve native at ambiguous junctions instead of warping a
            // neighbouring Maxis stage or assigning an unrelated storey.
            if(chosen && chosen!=id){return nativeBits;}
            chosen=id;
        }
    }
    return chosen>=80 && chosen<=82?FloatBits(HeightForId(chosen)):nativeBits;
}

// Bounded, sampled, memory-only receiver trace. No change to returned height.

extern "C" unsigned __cdecl ShadowPlaneTrace(const ShadowSurfacePoint* source,unsigned nativeBits,unsigned) {
    return ShadowPlaneOffsetBits(source,nativeBits);
}

static UINT_PTR g_shadowPlaneResume0=0;
extern "C" __declspec(naked) void ShadowPlaneHook0()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        cmp dword ptr [esp+108],3F400000h
        jne nativeShadowHeight
        fnsave [esp]
        fwait
        mov eax,[esp+180] // native ESP+20h: CPU position array
        add eax,[esp+276] // native ESP+80h: current source byte offset
        push dword ptr [esp+108]
        push eax
        call ShadowPlaneOffsetBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
    nativeShadowHeight:
        fadd dword ptr [esp+128] // saved ebx: source Z bits, no GPU readback
        add esp,112
        popad
        popfd
        mov ebx,[esp+24h]
        jmp dword ptr [g_shadowPlaneResume0]
    }
}

static UINT_PTR g_shadowPlaneResume1=0;
extern "C" __declspec(naked) void ShadowPlaneHook1()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        cmp dword ptr [esp+108],3F400000h
        jne nativeShadowHeight
        fnsave [esp]
        fwait
        mov eax,[esp+180] // native ESP+20h: CPU position array
        add eax,[esp+276] // native ESP+80h: current source byte offset
        push dword ptr [esp+108]
        push eax
        call ShadowPlaneOffsetBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
    nativeShadowHeight:
        fadd dword ptr [esp+112] // saved edi: source Z bits, no GPU readback
        add esp,112
        popad
        popfd
        mov ebx,[esp+80h]
        jmp dword ptr [g_shadowPlaneResume1]
    }
}

static UINT_PTR g_shadowPlaneResume2=0;
extern "C" __declspec(naked) void ShadowPlaneHook2()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        cmp dword ptr [esp+108],3F400000h
        jne nativeShadowHeight
        fnsave [esp]
        fwait
        mov eax,[esp+200] // native ESP+34h: CPU position array
        add eax,[esp+136] // saved ECX: current source byte offset
        push dword ptr [esp+108]
        push eax
        call ShadowPlaneOffsetBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
    nativeShadowHeight:
        fadd dword ptr [esp+132] // saved edx: source Z bits, no GPU readback
        add esp,112
        popad
        popfd
        mov edx,[esp+28h]
        jmp dword ptr [g_shadowPlaneResume2]
    }
}

static UINT_PTR g_shadowPlaneResume3=0;
extern "C" __declspec(naked) void ShadowPlaneHook3()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        cmp dword ptr [esp+108],3F400000h
        jne nativeShadowHeight
        fnsave [esp]
        fwait
        mov eax,[esp+200] // native ESP+34h: CPU position array
        add eax,[esp+136] // saved ECX: current source byte offset
        push dword ptr [esp+108]
        push eax
        call ShadowPlaneOffsetBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
    nativeShadowHeight:
        fadd dword ptr [esp+132] // saved edx: source Z bits, no GPU readback
        add esp,112
        popad
        popfd
        mov edi,[esp+28h]
        jmp dword ptr [g_shadowPlaneResume3]
    }
}

static void InstallShadowStagePlanes(UINT_PTR base)
{
    struct PlaneSite {unsigned rva,length;unsigned char original[10];void* hook;UINT_PTR* resume;};
    PlaneSite sites[]={
        {0x7685AA,7,{0xD8,0x41,0x08,0x8B,0x5C,0x24,0x24},(void*)ShadowPlaneHook0,&g_shadowPlaneResume0},
        {0x7685FB,10,{0xD8,0x41,0x08,0x8B,0x9C,0x24,0x80,0,0,0},(void*)ShadowPlaneHook1,&g_shadowPlaneResume1},
        {0x7695DD,7,{0xD8,0x40,0x08,0x8B,0x54,0x24,0x28},(void*)ShadowPlaneHook2,&g_shadowPlaneResume2},
        {0x769606,7,{0xD8,0x40,0x08,0x8B,0x7C,0x24,0x28},(void*)ShadowPlaneHook3,&g_shadowPlaneResume3}
    };
    const unsigned char pass0[]={0xC7,0x44,0x24,0x5C,0,0,0,0,0xC7,0x44,0x24,0x60,0,0,0x40,0x3F};
    const unsigned char pass1[]={0xC7,0x44,0x24,0x64,0,0,0,0,0xC7,0x44,0x24,0x68,0,0,0x40,0x3F};
    for(unsigned i=0;i<sizeof(pass0);++i)if(((unsigned char*)(base+0x76851F))[i]!=pass0[i])return;
    for(unsigned i=0;i<sizeof(pass1);++i)if(((unsigned char*)(base+0x76956C))[i]!=pass1[i])return;
    for(unsigned j=0;j<4;++j)for(unsigned i=0;i<sites[j].length;++i)
        if(((unsigned char*)(base+sites[j].rva))[i]!=sites[j].original[i])return;
    for(unsigned j=0;j<4;++j){
        *sites[j].resume=base+sites[j].rva+sites[j].length;
        PatchJump(base+sites[j].rva,sites[j].hook,sites[j].length,"custom_stage_shadow_plane");
    }

}

// 603DC3 restores foundation flooring after changes to the floor above it.
// At603EB3 native stages4 and5 are excluded; custom stages must be too.
// Leave the world IDs and every other foundation's restoration behavior intact.
static UINT_PTR g_aboveFloorNative=0,g_aboveFloorExcluded=0;
extern "C" __declspec(naked) void AboveStageFloorHook() {
    __asm {
        cmp eax,4
        je excludedStage
        cmp eax,80
        jb ordinaryFoundation
        cmp eax,82
        jbe excludedStage
    ordinaryFoundation:
        jmp dword ptr [g_aboveFloorNative]
    excludedStage:
        jmp dword ptr [g_aboveFloorExcluded]
    }
}
static void InstallAboveStageFloor(UINT_PTR base) {
    const unsigned char query[]={0x8B,0x07,0x8D,0x4D,0xA8,0x51,0x8B,0xCF,0xFF,0x90,0xE8,0,0,0,0x3B,0xC3,0x89,0x45,0xF0,0x74,0x0A};
    const unsigned char site[]={0x83,0xF8,0x04,0x74,0x05};
    const unsigned char tail[]={0x83,0xF8,0x05,0x75,0x09,0x39,0x5D,0xE8,0x0F,0x84,0x80,0,0,0};
    for(unsigned i=0;i<sizeof(query);++i)if(((unsigned char*)(base+0x203E9E))[i]!=query[i])return;
    for(unsigned i=0;i<sizeof(site);++i)if(((unsigned char*)(base+0x203EB3))[i]!=site[i])return;
    for(unsigned i=0;i<sizeof(tail);++i)if(((unsigned char*)(base+0x203EB8))[i]!=tail[i])return;
    g_aboveFloorNative=base+0x203EB8;g_aboveFloorExcluded=base+0x203EBD;
    PatchJump(base+0x203EB3,(void*)AboveStageFloorHook,5,"custom_stage_upper_floor_exception");
}

// Captured native rejection79/phase8 occurs at6085DB before MatchLanding.
// Retain the native stage-blocker query except for the exact validated lower
// stage tile at its actual landing height. No terrain/object checks are skipped.
static UINT_PTR g_nativeStageObstacleQuery=0;
extern "C" unsigned char __cdecl StageLandingObstacle(const StageGrid* grid,unsigned frame) {
    typedef unsigned char (__cdecl *QueryFn)(const StageGrid*);
    unsigned char original=((QueryFn)g_nativeStageObstacleQuery)(grid);
    if(!original || !grid || !frame || !g_upperStage || g_upperFloor!=g_lowerFloor)return original;
    const StageGrid* source=*(StageGrid**)(frame+0xC);
    const StagePoint* endpoint=*(StagePoint**)(frame+0x18);
    if(!source || !endpoint || source->layer!=grid->layer || grid->layer!=(int)g_lowerFloor)return original;
    unsigned sourceId=GridStageId(source);
    if(!((sourceId==82 && (g_landingTargetId==81 || g_landingTargetId==4)) || (sourceId==81 && g_landingTargetId==4)))return original;
    if(!(endpoint->x>=grid->x && endpoint->x<grid->x+1 && endpoint->y>=grid->y && endpoint->y<grid->y+1) || endpoint->layer!=grid->layer)return original;
    float current=*(float*)(frame+0x14);
    if(!(current>=g_lowerSurface-0.001f && current<=g_lowerSurface+0.001f))return original;
    StagePoint center={grid->x+0.5f,grid->y+0.5f,grid->layer};
    if(IdAt(&center)!=g_landingTargetId)return original;
    typedef float (__cdecl *SurfaceFn)(const void*);
    float actual=((SurfaceFn)g_landingSurface)(&center);
    if(!(actual>=g_lowerSurface-0.001f && actual<=g_lowerSurface+0.001f))return original;
    return 0;
}
extern "C" __declspec(naked) void StageLandingObstacleHook() {
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push ebp
        push dword ptr [esp+156]
        call StageLandingObstacle
        add esp,8
        mov [esp+140],eax
        frstor [esp]
        add esp,112
        popad
        popfd
        ret
    }
}
static void InstallStageLandingObstacle(UINT_PTR base) {
    const unsigned char lead[]={0x8D,0x45,0xD0,0x50,0x8D,0x45,0xBC,0x50,0xE8};
    const unsigned char tail[]={0x83,0xC4,0x0C,0x3A,0xC3,0x0F,0x85,0xD5,0x05,0,0};
    for(unsigned i=0;i<sizeof(lead);++i)if(((unsigned char*)(base+0x2085C3))[i]!=lead[i])return;
    for(unsigned i=0;i<sizeof(tail);++i)if(((unsigned char*)(base+0x2085D6))[i]!=tail[i])return;
    UINT_PTR call=base+0x2085D1;
    if(*(unsigned char*)call!=0xE8 || call+5+*(int*)(call+1)!=base+0x820D10)return;
    if(base+0x2085D0+*(int*)(base+0x2085CC)!=base+0x827030)return;
    g_nativeStageObstacleQuery=base+0x820D10;
    PatchDword(call+1,(unsigned)((UINT_PTR)StageLandingObstacleHook-(call+5)),"validated_stage_landing_obstacle");
}

static UINT_PTR g_shadowPlaneResume4=0;
extern "C" __declspec(naked) void ShadowPlaneHook4()
{
    __asm {
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        fnsave [esp]
        fwait
        push 4
        push dword ptr [esp+112]
        push eax
        call ShadowPlaneTrace
        add esp,12
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
        mov eax,[esp+140]
        fadd dword ptr [eax+8]
        add esp,112
        popad
        popfd
        mov edx,[esp+24h]
        jmp dword ptr [g_shadowPlaneResume4]
    }
}

static void InstallThirdShadowStagePlane(UINT_PTR base) {
    const unsigned char pass[]={0xC7,0x44,0x24,0x50,0,0,0,0,0xC7,0x44,0x24,0x54,0,0,0x40,0x3F};
    const unsigned char site[]={0xD8,0x40,0x08,0x8B,0x54,0x24,0x24};
    for(unsigned i=0;i<sizeof(pass);++i)if(((unsigned char*)(base+0x767E6A))[i]!=pass[i])return;
    for(unsigned i=0;i<sizeof(site);++i)if(((unsigned char*)(base+0x767EEE))[i]!=site[i])return;
    g_shadowPlaneResume4=base+0x767EF5;
    PatchJump(base+0x767EEE,(void*)ShadowPlaneHook4,7,"custom_stage_third_shadow_plane");
}

// Native corner comparisons otherwise sculpt terrain up to each suspended
// stair block's base. Supply that block's validation plane only for a checked,
// level, empty-gap stage flight, and only when the actual corner is still at
// that level. Actual terrain, floor IDs and created stair packets stay intact.
extern "C" unsigned __cdecl StageValidationPlaneBits(const StageVertex* vertex,unsigned frame,unsigned nativeBits){
    if(!g_flatStageFlight || !g_upperStage || g_upperFloor!=g_lowerFloor || !vertex || !frame)return nativeBits;
    float raw=*(float*)&nativeBits;
    if(!(raw>=g_flatStageRaw-0.001f && raw<=g_flatStageRaw+0.001f))return nativeBits;
    const StageGrid* source=*(StageGrid**)(frame+0xC);const StagePoint* end=*(StagePoint**)(frame+0x18);
    if(!source || !end || vertex->layer!=source->layer || vertex->layer!=end->layer)return nativeBits;
    int dx=(source->mask&255)==1?-1:(source->mask&255)==4?1:0;
    int dy=(source->mask&255)==2?1:(source->mask&255)==8?-1:0;
    if(!dx && !dy)return nativeBits;
    float forward=(vertex->x-source->x)*dx+(vertex->y-source->y)*dy;
    float endForward=(end->x-source->x-0.5f)*dx+(end->y-source->y-0.5f)*dy;
    bool onWidth=dx?(vertex->y>=source->y && vertex->y<=source->y+1):(vertex->x>=source->x && vertex->x<=source->x+1);
    if(!onWidth || forward< -1.0f || forward>endForward+1.0f)return nativeBits;
    float expected=*(float*)(frame-0x10),sourceZ=*(float*)(frame-0xB4);
    if(!(expected>=g_lowerSurface-0.001f && expected<=sourceZ+0.001f))return nativeBits;
    return FloatBits(expected);
}
static UINT_PTR g_stagePlaneResume0=0;
extern "C" __declspec(naked) void StageValidationPlaneHook0() {
    __asm {
        push dword ptr [esp]
        call dword ptr [eax+0F8h]
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        fnsave [esp]
        fwait
        push dword ptr [esp+108]
        push ebp
        push dword ptr [esp+156]
        call StageValidationPlaneBits
        add esp,12
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
        add esp,112
        popad
        popfd
        add esp,4
        jmp dword ptr [g_stagePlaneResume0]
    }
}
static UINT_PTR g_stagePlaneResume1=0;
extern "C" __declspec(naked) void StageValidationPlaneHook1() {
    __asm {
        push dword ptr [esp]
        call dword ptr [eax+0F8h]
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        fnsave [esp]
        fwait
        push dword ptr [esp+108]
        push ebp
        push dword ptr [esp+156]
        call StageValidationPlaneBits
        add esp,12
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
        add esp,112
        popad
        popfd
        add esp,4
        jmp dword ptr [g_stagePlaneResume1]
    }
}
static UINT_PTR g_stagePlaneResume2=0;
extern "C" __declspec(naked) void StageValidationPlaneHook2() {
    __asm {
        push dword ptr [esp]
        call dword ptr [eax+0F8h]
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        fnsave [esp]
        fwait
        push dword ptr [esp+108]
        push ebp
        push dword ptr [esp+156]
        call StageValidationPlaneBits
        add esp,12
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
        add esp,112
        popad
        popfd
        add esp,4
        jmp dword ptr [g_stagePlaneResume2]
    }
}
static UINT_PTR g_stagePlaneResume3=0;
extern "C" __declspec(naked) void StageValidationPlaneHook3() {
    __asm {
        push dword ptr [esp]
        call dword ptr [eax+0F8h]
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        fnsave [esp]
        fwait
        push dword ptr [esp+108]
        push ebp
        push dword ptr [esp+156]
        call StageValidationPlaneBits
        add esp,12
        mov [esp+108],eax
        frstor [esp]
        fstp st(0)
        fld dword ptr [esp+108]
        add esp,112
        popad
        popfd
        add esp,4
        jmp dword ptr [g_stagePlaneResume3]
    }
}
static void InstallSuspendedStageValidation(UINT_PTR base){
    const unsigned rvas[]={0x2087F5,0x208858,0x2088D0,0x208930};
    void* hooks[]={(void*)StageValidationPlaneHook0,(void*)StageValidationPlaneHook1,(void*)StageValidationPlaneHook2,(void*)StageValidationPlaneHook3};
    UINT_PTR* resumes[]={&g_stagePlaneResume0,&g_stagePlaneResume1,&g_stagePlaneResume2,&g_stagePlaneResume3};
    const unsigned char original[]={0xFF,0x90,0xF8,0,0,0,0xD9,0x45,0xF0,0xDA,0xE9,0xDF,0xE0,0xF6,0xC4,0x44};
    for(unsigned j=0;j<4;++j)for(unsigned i=0;i<sizeof(original);++i)if(((unsigned char*)(base+rvas[j]))[i]!=original[i])return;
    for(unsigned j=0;j<4;++j){*resumes[j]=base+rvas[j]+6;PatchJump(base+rvas[j],hooks[j],6,"suspended_stage_validation_plane");}
}

// The shadow-map height rasterizer has its own stage offset, independent
// of the receiver meshes. Only correct unambiguous twelve-step ownership here;
// preserve the already working lower stages and native non-stage branches.
static UINT_PTR g_shadowRasterResume=0;
extern "C" unsigned __cdecl ShadowRasterHeightBits(int x,int y,int layer,unsigned stageFlag){

    if(!(stageFlag&255))return 0x3F400000;
    unsigned chosen=0;
    for(unsigned q=0;q<4;++q){
        StagePoint p={x+((q&1)?0.025f:-0.025f),y+((q&2)?0.025f:-0.025f),layer};
        unsigned id=IdAt(&p);
        if(id!=4 && (id<80 || id>82))continue;
        if(chosen && chosen!=id){return 0x3F400000;}
        chosen=id;
    }
    
    if(chosen!=82)return 0x3F400000;
    return 0x40100000;
}
extern "C" __declspec(naked) void ShadowRasterHeightHook(){
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push eax
        push ebx
        push ebp
        push edi
        call ShadowRasterHeightBits
        add esp,16
        mov [esp+220],eax
        frstor [esp]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_shadowRasterResume]
    }
}
static void InstallShadowRasterHeight(UINT_PTR base){
    const unsigned char preceding[]={0x83,0xC4,0x04,0x84,0xC0};
    const unsigned char block[]={0xC7,0x44,0x24,0x48,0,0,0x40,0x3F,0x75,0x08,0xC7,0x44,0x24,0x48,0,0,0,0};
    UINT_PTR call=base+0x76D842;
    if(*(unsigned char*)call!=0xE8 || call+5+*(int*)(call+1)!=base+0x822B20)return;
    for(unsigned i=0;i<sizeof(preceding);++i)if(((unsigned char*)(base+0x76D847))[i]!=preceding[i])return;
    for(unsigned i=0;i<sizeof(block);++i)if(((unsigned char*)(base+0x76D84C))[i]!=block[i])return;
    g_shadowRasterResume=base+0x76D854;
    PatchJump(base+0x76D84C,(void*)ShadowRasterHeightHook,8,"twelve_step_shadow_map_height");
}

// Match CPU sequential geometry and bounds to the visible cap's owning
// triangle. This is a fresh lookup; it never changes the GPU height cache.
extern "C" unsigned __cdecl CpuCapHeightBits(unsigned mesh,unsigned vertex,unsigned nativeBits,unsigned kind){

    if(nativeBits!=0x3F400000 || !mesh)return nativeBits;
    unsigned begin=*(unsigned*)(mesh+0x1C),end=*(unsigned*)(mesh+0x20);
    if(!begin || end<begin || end-begin>0x1000000 || vertex<begin || vertex>=end || (vertex-begin)%40)return nativeBits;
    unsigned triangle=((vertex-begin)/40)/3*3;
    if((triangle+3)*40>end-begin)return nativeBits;
    const float* a=(const float*)(begin+triangle*40);
    const float* b=(const float*)(begin+(triangle+1)*40);
    const float* c=(const float*)(begin+(triangle+2)*40);
    StagePoint point={(a[0]+b[0]+c[0])/3.0f,(a[1]+b[1]+c[1])/3.0f,0};
    float rawZ=(a[2]+b[2]+c[2])/3.0f;
    if(!(point.x>=-1000000 && point.x<=1000000 && point.y>=-1000000 && point.y<=1000000 && rawZ>=-1000000 && rawZ<=1000000))return nativeBits;
    typedef void* (__cdecl *WorldFn)();typedef float (__cdecl *SurfaceFn)(StagePoint*);
    void* world=((WorldFn)g_getWorld)();if(!world)return nativeBits;
    int low=WorldBound(world,0x160),high=WorldBound(world,0x164);
    if(low < -128 || high>255 || high<low)return nativeBits;
    for(int layer=low;layer<=high;++layer){
        point.layer=layer;unsigned id=IdAt(&point);
        if(id<80 || id>82)continue;
        float delta=((SurfaceFn)g_baseSurface)(&point)-rawZ;

        if(!(delta>=-0.02f && delta<=0.02f))continue;
        return FloatBits(HeightForId(id));
    }
    return nativeBits;
}
static UINT_PTR g_cpuCapResume=0,g_capBoundsResume=0;
extern "C" __declspec(naked) void CpuCapHeightHook(){
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        mov eax,[esp+176]
        mov edx,[esp+172]
        push 0
        push eax
        push esi
        push edx
        call CpuCapHeightBits
        add esp,16
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        fstp dword ptr [esp+48h]
        jmp dword ptr [g_cpuCapResume]
    }
}
extern "C" __declspec(naked) void CapBoundsHeightHook(){
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        mov eax,[esp+224]
        push 1
        push eax
        push edi
        push esi
        call CpuCapHeightBits
        add esp,16
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        fstp dword ptr [esp+88h]
        jmp dword ptr [g_capBoundsResume]
    }
}
static void InstallCpuCapConsistency(UINT_PTR base){
    const unsigned char seqInit[]={0xC7,0x44,0x24,0x14,0,0,0x40,0x3F,0x75,0x08,0xC7,0x44,0x24,0x14,0,0,0,0};
    const unsigned char seqSite[]={0xD8,0x44,0x24,0x1C,0xD9,0x5C,0x24,0x48};
    const unsigned char boundsInit[]={0xC7,0x44,0x24,0x48,0,0,0x40,0x3F,0x75,0x08,0xC7,0x44,0x24,0x48,0,0,0,0};
    const unsigned char boundsSite[]={0xD8,0x44,0x24,0x4C,0xD9,0x9C,0x24,0x88,0,0,0};
    bool seq=true,bounds=true;
    for(unsigned i=0;i<sizeof(seqInit);++i)if(((unsigned char*)(base+0x68CDD2))[i]!=seqInit[i])seq=false;
    for(unsigned i=0;i<sizeof(seqSite);++i)if(((unsigned char*)(base+0x68CE7B))[i]!=seqSite[i])seq=false;
    for(unsigned i=0;i<sizeof(boundsInit);++i)if(((unsigned char*)(base+0x68E3D0))[i]!=boundsInit[i])bounds=false;
    for(unsigned i=0;i<sizeof(boundsSite);++i)if(((unsigned char*)(base+0x68E478))[i]!=boundsSite[i])bounds=false;
    if(seq){g_cpuCapResume=base+0x68CE83;PatchJump(base+0x68CE7B,(void*)CpuCapHeightHook,8,"cpu_cap_triangle_height");}
    if(bounds){g_capBoundsResume=base+0x68E483;PatchJump(base+0x68E478,(void*)CapBoundsHeightHook,11,"cap_bounds_triangle_height");}
}

// C24890 chooses the nearest ordinary storey. Above the halfway height, a
// twelve-step cap is incorrectly assigned to the storey above its owner.
extern "C" unsigned __cdecl ResolveTwelveStepLayer(const ShadowSurfacePoint* xyz,unsigned nativeLayer){
    
    if(!xyz || !g_getWorld || !g_convertPoint || !g_baseSurface)return nativeLayer;
    if(!(xyz->x>=-1000000 && xyz->x<=1000000 && xyz->y>=-1000000 && xyz->y<=1000000 && xyz->z>=-1000000 && xyz->z<=1000000))return nativeLayer;
    int lower=(int)nativeLayer-1;
    typedef void* (__cdecl *WorldFn)();typedef float (__cdecl *SurfaceFn)(StagePoint*);
    void* world=((WorldFn)g_getWorld)();if(!world)return nativeLayer;
    if(lower<WorldBound(world,0x160) || lower>=WorldBound(world,0x164))return nativeLayer;
    StagePoint p={xyz->x,xyz->y,lower};
    if(IdAt(&p)!=82)return nativeLayer;
    float top=((SurfaceFn)g_baseSurface)(&p)+HeightForId(82),delta=xyz->z-top;
    if(!(delta>=-.02f && delta<=.02f))return nativeLayer;

    return (unsigned)lower;
}
static UINT_PTR g_layerOwnerResume=0;
extern "C" __declspec(naked) void TwelveStepLayerOwnerHook(){
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[esi+1]
        push eax
        push edi
        call ResolveTwelveStepLayer
        add esp,8
        mov [esp+140],eax
        frstor [esp]
        add esp,112
        popad
        popfd
        pop edi
        pop esi
        jmp dword ptr [g_layerOwnerResume]
    }
}
static void InstallTwelveStepLayerOwner(UINT_PTR base){
    const unsigned char entry[]={0x83,0xEC,0x10,0x53,0x56,0x57,0xE8};
    const unsigned char midpoint[]={0xD9,0x44,0x24,0x0C,0xD8,0xC1,0xD8,0x0D,0xC8,0x69,0x1D,0x01,0xD9,0x47,0x08,0xDE,0xD9};
    const unsigned char ret[]={0x5F,0x8D,0x46,0x01,0x5E,0x5B,0x83,0xC4,0x10,0xC3};
    for(unsigned i=0;i<sizeof(entry);++i)if(((unsigned char*)(base+0x824890))[i]!=entry[i])return;
    for(unsigned i=0;i<sizeof(midpoint);++i)if(((unsigned char*)(base+0x8248FF))[i]!=midpoint[i])return;
    for(unsigned i=0;i<sizeof(ret);++i)if(((unsigned char*)(base+0x824922))[i]!=ret[i])return;
    UINT_PTR call=base+0x824896;
    if(*(unsigned char*)call!=0xE8 || call+5+*(int*)(call+1)!=base+0xAE0DC)return;
    call=base+0x8248DB;
    if(*(unsigned char*)call!=0xE8 || call+5+*(int*)(call+1)!=base+0x823340)return;
    g_layerOwnerResume=base+0x824927;PatchJump(base+0x824922,(void*)TwelveStepLayerOwnerHook,5,"twelve_step_world_point_owner");
}

// Native wall reconciliation (609A92) filters stage declarations with four
// literal ID4 checks. Preserve the actual ID for equality checks: different
// stage heights must retain their separating wall.
static UINT_PTR g_joinResume[4]={0},g_joinTaken[4]={0};
extern "C" __declspec(naked) void StageJoinHook0() {
    __asm {
        cmp eax,4
        je recognized
        cmp eax,80
        jb original
        cmp eax,82
        jbe recognized
    original:
        cmp eax,4
        jmp dword ptr [g_joinResume+0]
    recognized:
        cmp eax,eax
        jmp dword ptr [g_joinTaken+0]
    }
}
extern "C" __declspec(naked) void StageJoinHook1() {
    __asm {
        cmp eax,4
        je recognized
        cmp eax,80
        jb original
        cmp eax,82
        jbe recognized
    original:
        cmp eax,4
        jmp dword ptr [g_joinResume+4]
    recognized:
        cmp eax,eax
        jmp dword ptr [g_joinTaken+4]
    }
}
extern "C" __declspec(naked) void StageJoinHook2() {
    __asm {
        cmp edi,4
        je recognized
        cmp edi,80
        jb original
        cmp edi,82
        jbe recognized
    original:
        cmp edi,4
        jmp dword ptr [g_joinResume+8]
    recognized:
        cmp edi,edi
        jmp dword ptr [g_joinTaken+8]
    }
}
extern "C" __declspec(naked) void StageJoinHook3() {
    __asm {
        cmp edi,4
        je recognized
        cmp edi,80
        jb original
        cmp edi,82
        jbe recognized
    original:
        cmp edi,4
        jmp dword ptr [g_joinResume+12]
    recognized:
        cmp edi,edi
        jmp dword ptr [g_joinTaken+12]
    }
}
static void InstallStageJoins(UINT_PTR base) {
    const unsigned rvas[4]={0x209C1C,0x209C23,0x209C47,0x209C89};
    const unsigned taken[4]={0x209C28,0x209C2B,0x209C8E,0x209C91};
    const unsigned char expected[4][5]={{0x83,0xF8,4,0x74,7},{0x83,0xF8,4,0x74,3},{0x83,0xFF,4,0x74,0x42},{0x83,0xFF,4,0x74,3}};
    void* hooks[4]={(void*)StageJoinHook0,(void*)StageJoinHook1,(void*)StageJoinHook2,(void*)StageJoinHook3};
    // Validate all four before modifying any; a conflict leaves this optional
    // correction inactive and preserves the other height/stair hooks.
    for(unsigned i=0;i<4;++i)for(unsigned j=0;j<5;++j)
        if(((unsigned char*)(base+rvas[i]))[j]!=expected[i][j])return;
    for(unsigned i=0;i<4;++i){
        g_joinResume[i]=base+rvas[i]+5;g_joinTaken[i]=base+taken[i];
        PatchJump(base+rvas[i],hooks[i],5,"stage_shared_wall_classification");
    }
}
// A6BB30 builds the fence/half-wall segment transform from two StagePoints.
// Its native stage branch adds .75 separately to each raw endpoint height.
static unsigned __cdecl FenceModelHeightBits(StagePoint* p,unsigned kind){return WallHeightBits(p);}
static UINT_PTR g_fenceResume[2]={0};
extern "C" __declspec(naked) void FenceStageHeightHook0() {
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[esi+0]
        push 0
        push eax
        call FenceModelHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_fenceResume+0]
    }
}
extern "C" __declspec(naked) void FenceStageHeightHook1() {
    __asm {
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[esi+12]
        push 1
        push eax
        call FenceModelHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_fenceResume+4]
    }
}
static UINT_PTR g_fenceModelResume=0;
extern "C" __declspec(naked) void FenceModelHeightHook(){
    __asm{
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push 2
        push esi
        call FenceModelHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_fenceModelResume]
    }
}
static void InstallFenceModelHeight(UINT_PTR base){
    const unsigned char expected[]={0xD9,0x44,0x24,0x3C,0xD8,0x05,0x34,0x85,0x23,0x01,0xD9,0x5C,0x24,0x3C};
    for(unsigned i=0;i<14;++i)if(((unsigned char*)(base+0x62A108))[i]!=expected[i])return;
    g_fenceModelResume=base+0x62A112;
    PatchJump(base+0x62A10C,(void*)FenceModelHeightHook,6,"fence_model_stage_height");
}
static void InstallFenceStageHeights(UINT_PTR base) {
    const unsigned rvas[2]={0x66BB90,0x66BB9E};
    const unsigned char bytes[6]={0xD8,0x05,0x2C,0xA8,0x23,0x01};
    // This dependency establishes ESI as two StagePoint inputs, and keeps
    // the original native "both endpoints on a stage" branch in control.
    const unsigned char context[13]={0x56,0xE8,0x9B,0x6F,0x1B,0,0x83,0xC4,0x14,0x84,0xC0,0x74,0x1C};
    for(unsigned j=0;j<13;++j)if(((unsigned char*)(base+0x66BB7F))[j]!=context[j])return;
    for(unsigned i=0;i<2;++i)for(unsigned j=0;j<6;++j)
        if(((unsigned char*)(base+rvas[i]))[j]!=bytes[j])return;
    void* hooks[2]={(void*)FenceStageHeightHook0,(void*)FenceStageHeightHook1};
    for(unsigned i=0;i<2;++i){g_fenceResume[i]=base+rvas[i]+6;PatchJump(base+rvas[i],hooks[i],6,"fence_stage_endpoint_height");}
}
// These paths store a stage offset in a local, rather than FADD a constant.
// Keep their surrounding native stage/non-stage branches and saved point data.
static UINT_PTR g_uiStageResume[3]={0};
static unsigned __cdecl UiStageHeightBits(StagePoint* p,unsigned kind){return WallHeightBits(p);}
extern "C" __declspec(naked) void UiStageHeightHook0(){
    __asm{
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[esp+224]
        push 0
        push eax
        call UiStageHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        mov eax,[esp+108]
        mov [esp+168],eax
        add esp,112
        popad
        popfd
        jmp dword ptr [g_uiStageResume+0]
    }
}
extern "C" __declspec(naked) void UiStageHeightHook1(){
    __asm{
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        lea eax,[esp+312]
        push 1
        push eax
        call UiStageHeightBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        mov eax,[esp+108]
        mov [esp+168],eax
        add esp,112
        popad
        popfd
        jmp dword ptr [g_uiStageResume+4]
    }
}
static void InstallPanelMarkerHeights(UINT_PTR base){
    const unsigned rvas[2]={0x629CC5,0x65D18E};
    const unsigned char store[8]={0xC7,0x44,0x24,0x14,0,0,0x40,0x3F};
    for(unsigned i=0;i<2;++i)for(unsigned j=0;j<8;++j)
        if(((unsigned char*)(base+rvas[i]))[j]!=store[j])return;
    const unsigned char panelBranch[4]={0x3A,0xC3,0x74,8};
    const unsigned char markerBranch[10]={0x75,0x08,0xC7,0x44,0x24,0x14,0,0,0,0};
    for(unsigned j=0;j<4;++j)if(((unsigned char*)(base+0x629CC1))[j]!=panelBranch[j])return;
    for(unsigned j=0;j<10;++j)if(((unsigned char*)(base+0x65D196))[j]!=markerBranch[j])return;
    void* hooks[2]={(void*)UiStageHeightHook0,(void*)UiStageHeightHook1};
    for(unsigned i=0;i<2;++i){g_uiStageResume[i]=base+rvas[i]+8;PatchJump(base+rvas[i],hooks[i],8,"panel_marker_stage_offset");}
}
// Object placement footprint (4F292E), separate from A5D18E's marker.
// EBX is the original StagePoint; the native local offset is EBP-20.
extern "C" __declspec(naked) void ObjectStageGridHook(){
    __asm{
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push 2
        push ebx
        call UiStageHeightBits
        add esp,8
        mov [ebp-0x20],eax
        frstor [esp]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_uiStageResume+8]
    }
}
static void InstallObjectStageGrid(UINT_PTR base){
    const unsigned char expected[7]={0xC7,0x45,0xE0,0,0,0x40,0x3F};
    for(unsigned i=0;i<7;++i)if(((unsigned char*)(base+0xF292E))[i]!=expected[i])return;
    g_uiStageResume[2]=base+0xF2935;
    PatchJump(base+0xF292E,(void*)ObjectStageGridHook,7,"object_stage_placement_outline");
}
// Shared build-tool arrow: native stage branch adds .75 after resolving
// the supporting floor. EDI is the original StagePoint on every storey.
static UINT_PTR g_wallCursorResume=0;
static unsigned __cdecl WallCursorHeightBits(StagePoint* point){return WallHeightBits(point);}
extern "C" __declspec(naked) void WallCursorHeightHook(){
    __asm{
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push edi
        call WallCursorHeightBits
        add esp,4
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        jmp dword ptr [g_wallCursorResume]
    }
}
static void InstallWallCursorHeight(UINT_PTR base){
    const unsigned char expected[6]={0xD8,0x05,0xB4,0xC1,0x1F,0x01};
    const unsigned char branch[9]={0x83,0xC4,0x0C,0x84,0xC0,0x74,0x0C,0xD9,0x45};
    for(unsigned i=0;i<6;++i)if(((unsigned char*)(base+0x20695E))[i]!=expected[i])return;
    for(unsigned i=0;i<9;++i)if(((unsigned char*)(base+0x206954))[i]!=branch[i])return;
    if(((unsigned char*)(base+0x20695D))[0]!=0xF8)return;
    g_wallCursorResume=base+0x206964;
    PatchJump(base+0x20695E,(void*)WallCursorHeightHook,6,"build_arrow_stage_height");
}
static UINT_PTR g_halfwallBodyResume=0;
static unsigned __cdecl HalfwallStageDeltaBits(unsigned owner,const StagePoint* edge){
    
    if(!owner || !edge || edge[0].layer!=edge[1].layer)return 0;
    // Render vertices sit .05 outside the wall centreline. Resolve the
    // logical edge midpoint, not those offset face positions.
    StagePoint p={(edge[0].x+edge[1].x)*0.5f,(edge[0].y+edge[1].y)*0.5f,edge[0].layer};
    unsigned bits=WallHeightBits(&p);
    if(bits!=0x3EC00000 && bits!=0x3FC00000 && bits!=0x40100000)return 0;
    float native=*(float*)(owner+0x188);
    // Owner clipping plane is floor-relative, even on elevated storeys.
    float difference=native-0.75f;
    // Only the stage-height clipping group; ordinary wall/cutaway groups
    // use different offsets and must retain native rendering.
    if(!(difference>=-0.02f && difference<=0.02f))return 0;
    float delta=*(float*)&bits-0.75f;unsigned result=*(unsigned*)&delta;

    return result;
}
static unsigned __cdecl HalfwallInitialDeltaBits(unsigned owner,const StagePoint* edge,float chosen,unsigned flags){
    unsigned bits=HalfwallStageDeltaBits(owner,edge);
    if(!bits || (flags&0x10))return bits;
    StagePoint p={(edge[0].x+edge[1].x)*.5f,(edge[0].y+edge[1].y)*.5f,edge[0].layer};
    float target=((float(__cdecl*)(StagePoint*))g_baseSurface)(&p)+.75f+*(float*)&bits;
    float d=chosen-target;
    return d>=-.0001f && d<=.0001f?0:bits;
}
extern "C" __declspec(naked) void HalfwallBodyHeightHook(){
    __asm{
        pushfd
        pushad
        sub esp,112
        fst dword ptr [esp+108]
        fnsave [esp]
        fwait
        mov eax,[esp+172]
        add eax,16
        mov ecx,[esp+108]
        push dword ptr [ebx-4]
        push ecx
        push eax
        push ebp
        call HalfwallInitialDeltaBits
        add esp,16
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        mov ecx,[esp+0x4c]
        fld dword ptr [esp+0x58]
        jmp dword ptr [g_halfwallBodyResume]
    }
}
static UINT_PTR g_halfwallUpdateResume=0;
extern "C" __declspec(naked) void HalfwallBodyUpdateHook(){
    __asm{
        fadd dword ptr [esi+8]
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        mov eax,[esp+176]
        add eax,16
        push eax
        push ebp
        call HalfwallStageDeltaBits
        add esp,8
        mov [esp+108],eax

        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        lea edx,[esp+0x64]
        jmp dword ptr [g_halfwallUpdateResume]
    }
}
static void InstallHalfwallStageUpdate(UINT_PTR base){
    // Native refresh uploads source vertices again after initial construction.
    const unsigned char expected[7]={0xD8,0x46,8,0x8D,0x54,0x24,0x64};
    for(unsigned i=0;i<7;++i)if(((unsigned char*)(base+0x633D27))[i]!=expected[i])return;
    g_halfwallUpdateResume=base+0x633D2E;
    PatchJump(base+0x633D27,(void*)HalfwallBodyUpdateHook,7,"halfwall_stage_refresh");
}
static void InstallHalfwallStageBody(UINT_PTR base){
    const unsigned char expected[8]={0x8B,0x4C,0x24,0x4C,0xD9,0x44,0x24,0x58};
    const unsigned char heightBranch[14]={0xD9,0x85,0x88,1,0,0,0xD8,3,0xEB,4,0xD9,0x44,0x24,0x60};
    for(unsigned i=0;i<8;++i)if(((unsigned char*)(base+0x633B1A))[i]!=expected[i])return;
    for(unsigned i=0;i<14;++i)if(((unsigned char*)(base+0x633B0C))[i]!=heightBranch[i])return;
    g_halfwallBodyResume=base+0x633B22;
    PatchJump(base+0x633B1A,(void*)HalfwallBodyHeightHook,8,"halfwall_stage_surface");
}
// Visible wall surfaces have a separate packed vertex-buffer upload from
// the auxiliary geometry at A33980. Adjust only the destination copy.
static UINT_PTR g_wallVisibleCopy=0,g_wallNextNode=0;
static void __fastcall CopyVisibleStageWall(unsigned owner,void*,unsigned destination,unsigned group,unsigned index){
    ((void(__thiscall*)(void*,unsigned,unsigned,unsigned))g_wallVisibleCopy)((void*)owner,destination,group,index);
    
    if(!owner || !destination || !group)return;
    unsigned slots=*(unsigned*)(owner+0x7C);
    unsigned cursor=*(unsigned*)(slots+index*0x7C+0x6C)*48;
    unsigned head=*(unsigned*)(group+8),node=*(unsigned*)(head+8);
    for(unsigned visits=0;node!=head && node && visits<65536;++visits){
        unsigned mesh=*(unsigned*)(node+0x28);
        unsigned begin=*(unsigned*)(mesh+4),end=*(unsigned*)(mesh+8),bytes=end-begin;
        if(end<begin || bytes%48 || bytes>0x3000000)return;
        unsigned bits=HalfwallStageDeltaBits(owner,(const StagePoint*)(node+0x10));
        if(bits){
            float delta=*(float*)&bits;
            for(unsigned at=0;at<bytes;at+=48)*(float*)(destination+cursor+at+8)+=delta;

        }
        cursor+=bytes;
        node=((unsigned(__cdecl*)(unsigned))g_wallNextNode)(node);
    }
}
static void InstallVisibleStageWall(UINT_PTR base){
    const unsigned char expected[5]={0xE8,7,0xC0,0xFF,0xFF};
    for(unsigned i=0;i<5;++i)if(((unsigned char*)(base+0x6307D4))[i]!=expected[i])return;
    g_wallVisibleCopy=base+0x62C7E0;g_wallNextNode=base+0xA496F0;
    DWORD old=0;unsigned char* call=(unsigned char*)(base+0x6307D4);
    if(!VirtualProtect(call,5,PAGE_EXECUTE_READWRITE,&old))return;
    *(int*)(call+1)=(int)((UINT_PTR)CopyVisibleStageWall-((UINT_PTR)call+5));
    DWORD ignored=0;VirtualProtect(call,5,old,&ignored);FlushInstructionCache((HANDLE)-1,call,5);
}
static UINT_PTR g_wallSourceBuild=0,g_wallSourceNext=0;
static bool CanonicalHalfwallMesh(unsigned owner,const StagePoint* edge,unsigned mesh){
    unsigned bits=HalfwallStageDeltaBits(owner,edge);
    if(!bits)return false;
    unsigned begin=*(unsigned*)(mesh+4),end=*(unsigned*)(mesh+8),meta=*(unsigned*)(mesh+0x10);
    if(!begin || !meta || end<begin || (end-begin)%48 || end-begin>0x3000000)return false;
    StagePoint p={(edge[0].x+edge[1].x)*.5f,(edge[0].y+edge[1].y)*.5f,edge[0].layer};
    float raw=((float(__cdecl*)(StagePoint*))g_baseSurface)(&p),delta=*(float*)&bits;
    unsigned count=(end-begin)/48;
    // Preflight every vertex. Repeated refreshes accept the already-correct
    // target as well as native source heights, without accumulating offsets.
    for(unsigned i=0;i<count;++i){
        float bias=(*(unsigned*)(meta+i*28+12)&0x10)?*(float*)(meta+i*28+16):0;
        // Upper metadata bias already includes the supporting floor elevation.
        float native=.75f+((*(unsigned*)(meta+i*28+12)&0x10)?bias:raw),target=native+delta,z=*(float*)(begin+i*48+8);
        float a=z-native,b=z-target;
        if(!((a>=-.0001f && a<=.0001f)||(b>=-.0001f && b<=.0001f)))return false;
    }
    for(unsigned i=0;i<count;++i){
        float bias=(*(unsigned*)(meta+i*28+12)&0x10)?*(float*)(meta+i*28+16):0;
        *(float*)(begin+i*48+8)=.75f+((*(unsigned*)(meta+i*28+12)&0x10)?bias:raw)+delta;
    }
    
    return true;
}
static void __fastcall BuildCanonicalHalfwall(unsigned owner,void*){
    
    for(unsigned surface=0;surface<2;++surface){
    unsigned head=*(unsigned*)(owner+0x134+surface*12),group=*(unsigned*)(head+8);
    for(unsigned groups=0;group && group!=head && groups<65536;++groups){
        unsigned inner=*(unsigned*)(group+0x24),node=*(unsigned*)(inner+8);
        for(unsigned edges=0;node && node!=inner && edges<65536;++edges){
            CanonicalHalfwallMesh(owner,(const StagePoint*)(node+0x10),*(unsigned*)(node+0x28));
            node=((unsigned(__cdecl*)(unsigned))g_wallSourceNext)(node);
        }
        group=((unsigned(__cdecl*)(unsigned))g_wallSourceNext)(group);
    }
    }
    ((void(__thiscall*)(void*))g_wallSourceBuild)((void*)owner);
}
static void InstallCanonicalHalfwall(UINT_PTR base){
    const unsigned char expected[5]={0xE8,0x3D,0xD9,0xFF,0xFF};
    for(unsigned i=0;i<5;++i)if(((unsigned char*)(base+0x63603E))[i]!=expected[i])return;
    g_wallSourceBuild=base+0x633980;g_wallSourceNext=base+0xA496F0;
    DWORD old=0;unsigned char* call=(unsigned char*)(base+0x63603E);
    if(!VirtualProtect(call,5,PAGE_EXECUTE_READWRITE,&old))return;
    *(int*)(call+1)=(int)((UINT_PTR)BuildCanonicalHalfwall-((UINT_PTR)call+5));
    DWORD ignored=0;VirtualProtect(call,5,old,&ignored);FlushInstructionCache((HANDLE)-1,call,5);
}
static UINT_PTR g_halfwallVertexResume=0;
static unsigned __cdecl HalfwallVertexDeltaBits(unsigned owner,const StagePoint* point){
    
    if(!owner || !point)return 0;
    unsigned bits=WallHeightBits((StagePoint*)point);
    if(bits!=0x3EC00000 && bits!=0x3FC00000 && bits!=0x40100000)return 0;
    float d=*(float*)(owner+0x188)-.75f;
    if(!(d>=-.02f && d<=.02f))return 0;
    float delta=*(float*)&bits-.75f;unsigned result=*(unsigned*)&delta;
    
    return result;
}
extern "C" __declspec(naked) void HalfwallVertexRefreshHook(){
    __asm{
        pushfd
        pushad
        sub esp,112
        fnsave [esp]
        fwait
        push ebx
        push edi
        call HalfwallVertexDeltaBits
        add esp,8
        mov [esp+108],eax
        frstor [esp]
        fadd dword ptr [esp+108]
        add esp,112
        popad
        popfd
        fstp dword ptr [esi+8]
        mov [esi+0x24],edx
        jmp dword ptr [g_halfwallVertexResume]
    }
}
static void InstallHalfwallVertexRefresh(UINT_PTR base){
    const unsigned char expected[6]={0xD9,0x5E,8,0x89,0x56,0x24};
    for(unsigned i=0;i<6;++i)if(((unsigned char*)(base+0x62FFB1))[i]!=expected[i])return;
    g_halfwallVertexResume=base+0x62FFB7;
    PatchJump(base+0x62FFB1,(void*)HalfwallVertexRefreshHook,6,"halfwall_upper_refresh");
}
static DWORD __stdcall WorkerThread(void* selfModule)
{
    Sleep(2000); 
    UINT_PTR base=(UINT_PTR)GetModuleHandleW(0);
    
    const unsigned char productBytes[7]={0x8B,0x10,0x8B,0xC8,0xFF,0x52,0x30};
    const unsigned char toolBytes[7]={0x80,0xBB,0xCA,0,0,0,0};
    for(unsigned i=0;i<7;++i) {
        if(((unsigned char*)(base+0x81EC11))[i]!=productBytes[i] ||
           ((unsigned char*)(base+0xDB4F1))[i]!=toolBytes[i]) {
             return 0;
        }
    }
    const unsigned char floorBytes[9]={0x8A,0x4B,0x4D,0x84,0xC9,0x89,0x44,0x24,0x18};
    for(unsigned i=0;i<9;++i) if(((unsigned char*)(base+0x689D1B))[i]!=floorBytes[i]) {
         return 0;
    }
    if(*(unsigned*)(base+0xDB4FC)!=base+0xDE3848) {
         return 0;
    }
    struct HeightSite { unsigned rva; unsigned char bytes[12]; unsigned length; void* hook; UINT_PTR* resume; };
    HeightSite sites[]={
        {0x689E17,{0xD9,0x46,0x08,0xD8,0x05,0x1C,0xB3,0x23,0x01},9,(void*)FloorHeightHook0,&g_floorResume0},
        {0x689E80,{0xD9,0x46,0x30,0xD8,0x05,0x1C,0xB3,0x23,0x01},9,(void*)FloorHeightHook1,&g_floorResume1},
        {0x689EEA,{0xD9,0x46,0x58,0xD8,0x05,0x1C,0xB3,0x23,0x01},9,(void*)FloorHeightHook2,&g_floorResume2},
        {0x689F4F,{0xD9,0x86,0x80,0,0,0,0xD8,0x05,0x1C,0xB3,0x23,0x01},12,(void*)FloorHeightHook3,&g_floorResume3},
        {0x68A02D,{0xD9,0x46,0x08,0xD8,0x05,0x1C,0xB3,0x23,0x01},9,(void*)FloorHeightHook4,&g_floorResume4},
        {0x62BD44,{0xD8,0x05,0x90,0x88,0x23,0x01},6,(void*)WallHeightHook,&g_wallResume}
    };
    for(unsigned j=0;j<6;++j) for(unsigned k=0;k<sites[j].length;++k)
        if(((unsigned char*)(base+sites[j].rva))[k]!=sites[j].bytes[k]) {return 0;}
    unsigned surfaceRvas[3]={0x82383C,0x8243C2,0x82537E};
    void* surfaceHooks[3]={(void*)SurfaceHeightHook0,(void*)SurfaceHeightHook1,(void*)SurfaceHeightHook2};
    UINT_PTR* surfaceResumes[3]={&g_surfaceResume0,&g_surfaceResume1,&g_surfaceResume2};
    const unsigned char surfaceBytes[6]={0xD8,0x05,0xCC,0x6B,0x25,0x01};
    for(unsigned j=0;j<3;++j) for(unsigned k=0;k<6;++k)
        if(((unsigned char*)(base+surfaceRvas[j]))[k]!=surfaceBytes[k]) {return 0;}
    struct StairSite {unsigned rva;unsigned char bytes[9];unsigned len;void* hook;UINT_PTR* resume;};
    StairSite stairSites[]={
        {0x20827A,{0xFF,0x75,0x18,0x8D,0x85,0x84,0xFE,0xFF,0xFF},9,(void*)StairContextHook,&g_stairContextResume},
        {0x208535,{0xC7,0x45,0x14,0,0,0x40,0x3F},7,(void*)StairStartHook,&g_stairStartResume},
        {0xD4B92,{0xD8,0x05,0xFC,0x32,0x1E,0x01},6,(void*)StairPreviewHook,&g_stairPreviewResume},
        {0x8205E4,{0x83,0xF8,0x04,0x74,0x40},5,(void*)StairTerrainClassHook,&g_terrainClassResume},
        {0x208B3C,{0xD9,0x45,0xA0,0x8D,0x85,0x10,0xFF,0xFF,0xFF},9,(void*)StairLandingHook,&g_stairLandingResume}
    };
    for(unsigned j=0;j<5;++j)for(unsigned k=0;k<stairSites[j].len;++k)
        if(((unsigned char*)(base+stairSites[j].rva))[k]!=stairSites[j].bytes[k]) {return 0;}
    const unsigned stairOperands[2]={0x2082B2,0x2085B8};
    for(unsigned j=0;j<2;++j)if(*(unsigned*)(base+stairOperands[j])!=base+0xDFC48C){return 0;}
    const unsigned char finderBytes[6]={0xD8,0x05,0x8C,0xC4,0x1F,0x01};
    for(unsigned k=0;k<6;++k)if(((unsigned char*)(base+0x207407))[k]!=finderBytes[k]) {return 0;}
    struct LandingSite {unsigned rva;unsigned char bytes[10];unsigned len;void* hook;UINT_PTR* resume;};
    LandingSite landingSites[]={
        {0x2076C9,{0xC7,0x85,0x2C,0xFF,0xFF,0xFF,0,0,0x40,0x3F},10,(void*)CornerHook0,&g_cornerResume0},
        {0x207700,{0xC7,0x85,0x24,0xFF,0xFF,0xFF,0,0,0x40,0x3F},10,(void*)CornerHook1,&g_cornerResume1},
        {0x207754,{0xC7,0x85,0x28,0xFF,0xFF,0xFF,0,0,0x40,0x3F},10,(void*)CornerHook2,&g_cornerResume2},
        {0x207788,{0xC7,0x85,0x30,0xFF,0xFF,0xFF,0,0,0x40,0x3F},10,(void*)CornerHook3,&g_cornerResume3},
        {0x2082BC,{0x8D,0xB5,0x84,0xFE,0xFF,0xFF},6,(void*)LandingContextHook,&g_landingContextResume},
        {0x208992,{0x39,0x5D,0xCC,0x75,0x76},5,(void*)LandingStepHook,&g_landingStepResume},
        {0x208A44,{0xFF,0x52,0x64,0x84,0xC0},5,(void*)LandingCreateHook,&g_landingCreateResume},
        {0x207A9A,{0x83,0x4D,0xFC,0xFF,0xFF,0x25,0xA4,0x1E,0xEB,0x01},10,(void*)FinderRejectHook,&g_finderRejectResume}
    };
    for(unsigned j=0;j<8;++j)for(unsigned k=0;k<landingSites[j].len;++k)
        if(((unsigned char*)(base+landingSites[j].rva))[k]!=landingSites[j].bytes[k]) {return 0;}
    unsigned sampleCalls[2]={0x20862D,0x20863C};
    for(unsigned j=0;j<2;++j){UINT_PTR a=base+sampleCalls[j];if(*(unsigned char*)a!=0xE8 || a+5+*(int*)(a+1)!=base+0x823340){return 0;}}
    UINT_PTR placementCall=base+0xB1E43;
    if(*(unsigned char*)placementCall!=0xE8 || placementCall+5+*(int*)(placementCall+1)!=base+0x1FCCC2 || *(unsigned*)(base+0x208B35)!=base+0xDFC48C){return 0;}
    const unsigned char rejectBytes[6]={0x8B,0x4D,0x24,0x8D,0x45,0xB4};
    for(unsigned k=0;k<6;++k)if(((unsigned char*)(base+0x208193))[k]!=rejectBytes[k]){return 0;}
    const unsigned char validatorBytes[6]={0xFF,0x92,0xF8,0,0,0};
    for(unsigned k=0;k<6;++k)if(((unsigned char*)(base+0x20330B))[k]!=validatorBytes[k]){return 0;}
    UINT_PTR finderCall=base+0x208F0E;
    if(*(unsigned char*)finderCall!=0xE8 || finderCall+5+*(int*)(finderCall+1)!=base+0x2072A2){return 0;}
    g_getWorld=base+0xAE0DC;g_convertPoint=base+0x827030;g_baseSurface=base+0x823340;
    g_productResume=base+0x81EC18;
    g_toolResume=base+0xDB4F8;
    const unsigned char categoryBytes[5]={0xE8,0x18,0xAE,0xB7,0xFF};
    const unsigned char classBytes[10]={0x83,0xF8,0x04,0x8B,0xCB,0x0F,0x94,0x44,0x24,0x38};
    for(unsigned i=0;i<5;++i) if(((unsigned char*)(base+0x81EBF0))[i]!=categoryBytes[i]) {return 0;}
    for(unsigned i=0;i<10;++i) if(((unsigned char*)(base+0x6E6C94))[i]!=classBytes[i]) {return 0;}
    g_worldCall=base+0x399A0D; g_categoryResume=base+0x81EBF5; g_floorClassResume=base+0x6E6C9E;
    PatchJump(base+0x81EBF0,(void*)CategoryHook,5,"custom_stage_category");
    PatchJump(base+0x6E6C94,(void*)FloorClassHook,10,"custom_stage_floor_class");
    PatchJump(base+0xDB4F1,(void*)StageToolHook,7,"hook_stage_selection");
    g_floorTraceResume=base+0x689D24;
    PatchJump(base+0x689D1B,(void*)FloorTraceHook,9,"hook_floor_mesh_trace");
    PatchDword(base+0xDB4FC,(unsigned)(UINT_PTR)&g_previewHeight,"per_entry_preview_height");
    for(unsigned j=0;j<6;++j) {
        *sites[j].resume=base+sites[j].rva+sites[j].length;
        PatchJump(base+sites[j].rva,sites[j].hook,sites[j].length,"geometry_height");
    }
    for(unsigned j=0;j<3;++j) {
        *surfaceResumes[j]=base+surfaceRvas[j]+6;
        PatchJump(base+surfaceRvas[j],surfaceHooks[j],6,"stage_support_height");
    }
    g_terrainClassMatch=base+0x820629;
    for(unsigned j=0;j<5;++j){
        *stairSites[j].resume=base+stairSites[j].rva+stairSites[j].len;
        PatchJump(base+stairSites[j].rva,stairSites[j].hook,stairSites[j].len,"stage_stair_height");
    }
    for(unsigned j=0;j<2;++j)PatchDword(base+stairOperands[j],(unsigned)&g_stairHeightBits,"per_stage_stair_height");
    g_stairFinderResume=base+0x20740D;
    PatchJump(base+0x207407,(void*)StairFinderHook,6,"stage_stair_finder_height");
    g_finderExitSlot=*(unsigned*)(base+0x207AA0);
    g_landingSurface=base+0x8237C0;g_originalPlacement=base+0x1FCCC2;g_landingStepNonzero=base+0x208A0D;
    for(unsigned j=0;j<8;++j){
        *landingSites[j].resume=base+landingSites[j].rva+landingSites[j].len;
        PatchJump(base+landingSites[j].rva,landingSites[j].hook,landingSites[j].len,"stage_landing");
    }
    PatchDword(base+0x208B35,(unsigned)&g_landingTerrainDeduct,"raised_landing_compensation");
    for(unsigned j=0;j<2;++j){UINT_PTR a=base+sampleCalls[j];PatchDword(a+1,(unsigned)(g_landingSurface-(a+5)),"stage_aware_step_sample");}
    PatchDword(placementCall+1,(unsigned)((UINT_PTR)LandingPlacement-(placementCall+5)),"scoped_stair_placement");
    g_coreRejectResume=base+0x208199;
    PatchJump(base+0x208193,(void*)CoreRejectHook,6,"core_reject_trace");
    g_clearFinderErrors=base+0x81E4B0;
    g_nativeFinder=base+0x2072A2;g_validatorSurfaceResume=base+0x203311;
    PatchJump(base+0x20330B,(void*)ValidatorSurfaceHook,6,"auto_stage_surface_validation");
    PatchDword(finderCall+1,(unsigned)((UINT_PTR)ScopedAutoFinder-(finderCall+5)),"scope_auto_finder");
    // Optional mouse-tool adapter: an incompatible click site cannot prevent
    // the already-validated and installed v29 geometry/height hooks loading.
    UINT_PTR clickCall=base+0xD4CBB;
    if(*(unsigned char*)clickCall==0xE8 && clickCall+5+*(int*)(clickCall+1)==base+0x208095){
        g_nativeCommitCore=base+0x208095;
        PatchDword(clickCall+1,(unsigned)((UINT_PTR)ClickLandingCore-(clickCall+5)),"click_uses_preview_landing");
    }else{}
    InstallSuspendedStageValidation(base);
    InstallStageLandingObstacle(base);
    InstallAboveStageFloor(base);
    InstallSecondaryStageSurface(base);
    InstallCpuCapConsistency(base);
    InstallTwelveStepLayerOwner(base);
    InstallStageJoins(base);
    InstallFenceStageHeights(base);
    InstallFenceModelHeight(base);
    InstallPanelMarkerHeights(base);
    InstallObjectStageGrid(base);
    InstallWallCursorHeight(base);
    InstallHalfwallStageBody(base);
    // Refresh now reads canonical source Z; no additional delta.
    // The optional static copy path also reads canonical source Z.
    InstallCanonicalHalfwall(base);
    InstallHalfwallVertexRefresh(base);
    InstallShadowStageMask(base);
    InstallShadowStagePlanes(base);
    InstallThirdShadowStagePlane(base);
    
    return 0;
}
extern "C" BOOL __stdcall DllMain(HMODULE module,DWORD reason,void*)
{
    if(reason==DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        CreateThread(0,0,WorkerThread,module,0,0);
    }
    return TRUE;
}

