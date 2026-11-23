#include "../TS2-CustomStages.cpp"
#pragma optimize("", off) // legacy fixture edits EBP; production source above remains /O2
static UINT_PTR testVtable[100];
struct FakeWorld {UINT_PTR* table;};
static FakeWorld testWorld={testVtable};
static bool landingEdgeTest=false;


struct ErrorFixture {unsigned head,count,lastCode;};
static bool errorFixtureEnabled=false;
static unsigned resetErrorCalls=0,errorsSeenByCore=0;
static void __fastcall ResetErrorsFixture(ErrorFixture* errors,void*){++resetErrorCalls;errors->count=0;errors->lastCode=0;}

static bool upperFixture=false,upperObstacle=false,upperSourceIsStage=false;
static unsigned upperTarget=82;static float upperSourceZ=3.0f;
static bool endpointFixture=false,endpointSlope=false;
static int endpointDistance=2;
static unsigned endpointSource=81,endpointTarget=4,endpointMask=8,nativeFinderResult=1,nativeFinderCalls=0;
static int endpointSourceX=20,endpointSourceY=20;
static int EndpointDX(){return endpointMask==1?-1:endpointMask==4?1:0;}
static int EndpointDY(){return endpointMask==2?1:endpointMask==8?-1:0;}

static bool ownerFixture=false;static unsigned ownerSource=82,ownerTarget=81,ownerDirection=0;
static unsigned testId=81;static int testLayer=0;static float testBase=0;static bool boundary=false;
static void* __cdecl GetFakeWorld(){return &testWorld;}
static int __fastcall Low(void*,void*){return 0;}
static int __fastcall High(void*,void*){return 64;}
static int __fastcall FloorHigh(void*,void*){return 8;}
static int Trunc(float f){int value;__asm {cvttss2si eax,f} __asm {mov value,eax} return value;}
static StageGrid* __cdecl ConvertFake(StageGrid* g,StagePoint* p){g->x=Trunc(p->x);g->y=Trunc(p->y);g->layer=p->layer;g->mask=1;return g;}
static unsigned __fastcall QueryFake(void*,void*,StageGrid* g){if(upperFixture){if(g->layer==1)return upperSourceIsStage?81:~0u;if(g->layer!=0)return ~0u;int d=(g->x-20)*EndpointDX()+(g->y-20)*EndpointDY();return d==1 && upperObstacle?(upperTarget==82?4:82):d>=1?upperTarget:~0u;}if(endpointFixture){if(g->layer!=testLayer)return ~0u;int d=(g->x-endpointSourceX)*EndpointDX()+(g->y-endpointSourceY)*EndpointDY();return d<=0?endpointSource:endpointSource==80?endpointTarget:d<endpointDistance?~0u:endpointTarget;}if(ownerFixture){if(g->layer!=testLayer)return ~0u;bool upper=ownerDirection==0 ? g->x<=10 : ownerDirection==1 ? g->x>=10 : ownerDirection==2 ? g->y<=10 : g->y>=10;return upper?ownerSource:ownerTarget;}return g->layer!=testLayer || (landingEdgeTest && g->x<11) ? ~0u : boundary && g->x<10 ? 4 : testId;}
static float __cdecl BaseFake(StagePoint* p){if(upperFixture)return p->layer==1?upperSourceZ:0.0f;return testBase+(endpointFixture && endpointSlope && (p->x-endpointSourceX-0.5f)*EndpointDX()+(p->y-endpointSourceY-0.5f)*EndpointDY()>0.1f?0.25f:0.0f);}
static unsigned char testMesh[96];static float testVertices[24][10];
extern "C" __declspec(naked) void ReturnOnly(){__asm {ret}}
extern "C" __declspec(naked) void WallReturn(){__asm {add esp,52} __asm {ret}}
extern "C" __declspec(naked) void SurfaceReturn(){__asm {add esp,16} __asm {ret}}
extern "C" __declspec(naked) void EdgeReturn(){__asm {add esp,32} __asm {ret}}
static unsigned terrainBranch=0;
extern "C" __declspec(naked) void TerrainYes(){__asm {mov terrainBranch,1} __asm {ret}}
extern "C" __declspec(naked) void TerrainNo(){__asm {mov terrainBranch,0} __asm {ret}}
extern "C" __declspec(naked) void ContextReturn(){__asm {add esp,4} __asm {ret}}
static float __cdecl SurfaceFake(const void* p){return landingEdgeTest && ((const StagePoint*)p)->x<11.0f ? 0.0f : testBase+HeightForId(testId);}
static unsigned checkedFloor=99,requiredFloor=1,storedFloor=99;
static unsigned char __cdecl PlacementFake(unsigned,unsigned pos,unsigned,unsigned,unsigned){checkedFloor=((unsigned*)pos)[2];return checkedFloor==requiredFloor;}
static unsigned char __fastcall CreateFake(void*,void*,unsigned product,unsigned pos,unsigned dir,unsigned height,unsigned a4,unsigned a5,unsigned a6,unsigned a7){
    unsigned char ok=LandingPlacement(1,pos,dir,height,0);storedFloor=((unsigned*)pos)[2];return ok;
}
static UINT_PTR finderFixtureTarget=(UINT_PTR)ReturnOnly;
extern "C" __declspec(naked) void FinderExitFixture()
{
    __asm {
        test edi,edi
        jz failedExit
        jmp sharedExit
    failedExit:
        xor bl,bl
    sharedExit:
        or dword ptr [ebp-4],-1
        jmp dword ptr [finderFixtureTarget]
    }
}
static unsigned seenFinderDepth=0,seenFinderArg=0;
static unsigned char __cdecl FinderFake(unsigned a,unsigned b,unsigned c,unsigned d,unsigned e,unsigned f,unsigned g,unsigned h){++nativeFinderCalls;if(errorFixtureEnabled && h && !nativeFinderResult){ErrorFixture* errors=(ErrorFixture*)h;++errors->count;errors->lastCode=0x78;}seenFinderDepth=g_autoFinderDepth;seenFinderArg=a+b+c+d+e+f+g+h;return (unsigned char)nativeFinderResult;}
static float __fastcall RawVertexFake(void*,void*,const StageVertex*){return testBase;}
extern "C" __declspec(naked) void ValidatorCallFixture(){__asm {push esi} __asm {call dword ptr [edx+0xF8]} __asm {ret}}

// Execute the game's actual automatic-caller machine code, with only external
// calls redirected to fixture callbacks. This proves a proposal does not force
// placement success: the original code must return the core builder's result.
static float halfFixture=0.5f,fourFixture=4.0f;
static unsigned char nativeAutoEntry[]={0x55,0x8B,0xEC,0x83,0xEC,0x0C,0xD9,0x05,0xE0,0x75,0x1D,0x01,0x83,0x65,0xFC,0x00,0x56,0xD9,0x55,0xF4,0x57,0xD9,0x5D,0xF8,0x8B,0x7D,0x14,0x85,0xFF,0x75,0x03,0x8D,0x7D,0xF4,0xFF,0x75,0x20,0x8B,0x75,0x18,0x56,0x57,0x83,0xEC,0x10,0x8B,0xCC,0x89,0x65,0x14,0xFF,0x75,0x0C,0xE8,0x09,0x90,0xEA,0xFF,0xFF,0x75,0x08,0xE8,0x8F,0xE3,0xFF,0xFF,0x83,0xC4,0x20,0x84,0xC0,0x74,0x24,0x8B,0x06,0x8B,0xCE,0xFF,0x50,0x20,0xFF,0x75,0x20,0xFF,0x75,0x1C,0x56,0x57,0xFF,0x75,0x10,0x6A,0x00,0xFF,0x75,0x0C,0xFF,0x75,0x08,0xE8,0x5C,0xF1,0xFF,0xFF,0x83,0xC4,0x20,0xEB,0x02,0x32,0xC0,0x5F,0x5E,0xC9,0xC3};
static float nativeDefaultPoint=0;
static unsigned nativeCoreResult=0,nativeCoreCalls=0,nativeClearCalls=0,nativeArgs[8],nativePoint[3];
static UINT_PTR nativeBuilderVtable[9];
struct NativeBuilderFixture {UINT_PTR* vtable;};
static NativeBuilderFixture nativeBuilder={nativeBuilderVtable};
static void* __fastcall NativeCopyFixture(StageGrid* out,void*,const StageGrid* in){*out=*in;return out;}
static void __fastcall NativeClearFixture(void*,void*){++nativeClearCalls;}
static unsigned char __cdecl NativeCoreFixture(unsigned a,unsigned b,unsigned c,unsigned d,unsigned e,unsigned f,unsigned g,unsigned h){
    ++nativeCoreCalls;if(errorFixtureEnabled && h){ErrorFixture* errors=(ErrorFixture*)h;errorsSeenByCore=errors->count;if(!nativeCoreResult){++errors->count;errors->lastCode=0x70;}}nativeArgs[0]=a;nativeArgs[1]=b;nativeArgs[2]=c;nativeArgs[3]=d;nativeArgs[4]=e;nativeArgs[5]=f;nativeArgs[6]=g;nativeArgs[7]=h;
    for(unsigned k=0;k<3;++k)nativePoint[k]=((unsigned*)e)[k];
    return (unsigned char)nativeCoreResult;
}


static unsigned char nativeErrorGate[]={0x8B,0x51,0x04,0x33,0xC0,0x85,0xD2,0x0F,0x95,0xC0,0xC3};
// Reproduce v30's endpoint success without clearing the failed search's
// error. This control uses the same callback and proposal helpers as v31.
static unsigned char __cdecl LegacyV30Finder(unsigned product,unsigned x,unsigned y,unsigned layer,unsigned direction,unsigned output,unsigned flags,unsigned error){
    if(ProposeStageEndpoint(x,y,layer,direction,(StagePoint*)output,1))return 1;
    unsigned char result=((FinderFn)g_nativeFinder)(product,x,y,layer,direction,output,flags,error);
    if(!result && ProposeStageEndpoint(x,y,layer,direction,(StagePoint*)output,0))result=1;
    if(!result && ProposeUpperStageEndpoint(x,y,layer,direction,(StagePoint*)output))result=1;
    return result;
}


static unsigned char secondaryNativeCode[]={0x66,0x8b,0x03,0x8b,0x4f,0x1c,0x0f,0xb7,0xc0,0x8d,0x14,0x80,0x8d,0x04,0xd1,0x8b,0x4c,0x24,0x48,0x8b,0x54,0x24,0x4c,0xd9,0x00,0x89,0x4c,0x24,0x3c,0x8b,0x4c,0x24,0x50,0xd9,0x44,0x24,0x3c,0x89,0x54,0x24,0x40,0xd8,0xc1,0x89,0x4c,0x24,0x44,0xd9,0x5c,0x24,0x3c,0x8b,0x54,0x24,0x3c,0xdd,0xd8,0xd9,0x40,0x04,0xd9,0x44,0x24,0x40,0xd8,0xc1,0xd9,0x5c,0x24,0x40,0xdd,0xd8,0xd9,0x40,0x08,0x8b,0x44,0x24,0x40,0xd9,0x44,0x24,0x44,0x89,0x54,0x24,0x54,0xd8,0xc1,0x8d,0x54,0x24,0x54,0x52,0x89,0x44,0x24,0x5c,0xd9,0x5c,0x24,0x48,0x8b,0x4c,0x24,0x48,0x89,0x4c,0x24,0x60,0x8b,0x4d,0x00,0xdd,0xd8,0xd9,0x44,0x24,0x60,0x56,0xd8,0x05,0x1c,0xb3,0x23,0x01,0xd9,0x5c,0x24,0x64,0x8b,0x01,0xff,0x90,0xa4,0x00,0x00,0x00,0x8b,0xc8,0xe8,0x1b,0x52,0xbd,0xff,0xc3};
static float nativeSecondaryOffset=0.75f,emittedSecondary[3];
static UINT_PTR secondaryWorldVtable[42];
struct SecondaryWorld {UINT_PTR* vtable;};
static SecondaryWorld secondaryWorld={secondaryWorldVtable};
static SecondaryWorld* secondaryWorldPointer=&secondaryWorld;
static void* __fastcall GetSecondaryGeometry(void*,void*){return &secondaryWorld;}
static void __fastcall EmitSecondaryVertex(void*,void*,unsigned index,const float* point){for(unsigned i=0;i<3;++i)emittedSecondary[i]=point[i];}
extern "C" __declspec(naked) void __cdecl RunSecondaryNative(unsigned mesh,unsigned indexPointer)
{
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        sub esp,128
        mov edi,esp
        mov ecx,32
        xor eax,eax
        rep stosd
        mov edi,[esp+148]
        mov ebx,[esp+152]
        xor esi,esi
        lea ebp,secondaryWorldPointer
        lea eax,secondaryNativeCode
        call eax
        add esp,128
        pop edi
        pop esi
        pop ebx
        pop ebp
        ret
    }
}


static unsigned char nativeShadowMask[]={0x8b,0x17,0x55,0x8b,0xcf,0xff,0x92,0xe8,0x00,0x00,0x00,0x83,0xf8,0x04,0x75,0x13,0x0f,0xbe,0x55,0x0c,0x8b,0x4c,0x24,0x14,0x8b,0x83,0xac,0x00,0x00,0x00,0xd3,0xe2,0x09,0x14,0xb0,0xc3};
static unsigned shadowMaskWords[2];
static unsigned char shadowMaskObject[196];
static StageGrid shadowMaskGrid={10,10,0,15,{0,0,0}};
extern "C" __declspec(naked) void __cdecl ReplayShadowMask()
{
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        sub esp,32
        mov dword ptr [esp+16],5
        mov dword ptr [esp+20],5
        lea edi,testWorld
        lea ebx,shadowMaskObject
        lea ebp,shadowMaskGrid
        xor esi,esi
        lea eax,nativeShadowMask
        call eax
        add esp,32
        pop edi
        pop esi
        pop ebx
        pop ebp
        ret
    }
}


static unsigned char planeNative[5][16]={
    {0xD8,0x41,0x08,0x8B,0x5C,0x24,0x24,0xC3},
    {0xD8,0x41,0x08,0x8B,0x9C,0x24,0x80,0,0,0,0xC3},
    {0xD8,0x40,0x08,0x8B,0x54,0x24,0x28,0xC3},
    {0xD8,0x40,0x08,0x8B,0x7C,0x24,0x28,0xC3},
    {0xD8,0x40,0x08,0x8B,0x54,0x24,0x24,0xC3}
};
static ShadowSurfacePoint planePoint={10.5f,10.5f,1.0f};
static UINT_PTR planeTarget;
static unsigned planeSiteKind;
static UINT_PTR planeGpuOverride=0;
static float planeNativeOffset=0.75f,planeSentinel=19.25f,planeResult,planeRemainingOffset,planeRemainingSentinel;
static unsigned planeEBX,planeEDX,planeEDI,planeFlags;
extern "C" __declspec(naked) void __cdecl RunPlaneVertex()
{
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        sub esp,160
        mov dword ptr [esp+20h],11111111h
        mov dword ptr [esp+24h],22222222h
        mov dword ptr [esp+7ch],0
        lea eax,planePoint
        mov [esp+1ch],eax
        mov [esp+30h],eax
        mov eax,planeGpuOverride
        test eax,eax
        jnz destinationReady
        lea eax,planePoint
    destinationReady:
        mov ecx,eax
        cmp planeSiteKind,2
        jb sourceIndexReady
        cmp planeSiteKind,4
        je sourceIndexReady
        xor ecx,ecx
    sourceIndexReady:
        mov ebx,dword ptr [planePoint+8]
        mov edx,ebx
        mov edi,ebx
        fld planeSentinel
        fld planeNativeOffset
        fld st(0)
        cmp eax,eax
        call dword ptr [planeTarget]
        mov planeEBX,ebx
        mov planeEDX,edx
        mov planeEDI,edi
        pushfd
        pop dword ptr [planeFlags]
        fstp planeResult
        fstp planeRemainingOffset
        fstp planeRemainingSentinel
        add esp,160
        pop edi
        pop esi
        pop ebx
        pop ebp
        ret
    }
}


static unsigned char upperFloorNative[]={0x3B,0xC3,0x89,0x45,0xF0,0x74,0x0A,0x83,0xF8,0x04,0x74,0x05,0x83,0xF8,0x05,0x75,0x09,0x39,0x5D,0xE8,0x0F,0x84,0,0,0,0,0xB8,0xC0,0,0,0,0xC3,0xB8,0xD0,0,0,0,0xC3};
static unsigned upperFloorFrame[32],upperFloorId,lowerFloorId;
extern "C" __declspec(naked) unsigned ReplayUpperFloor() {
    __asm {
        push ebp
        push ebx
        lea ebp,upperFloorFrame
        add ebp,64
        mov ebx,-1
        mov eax,upperFloorId
        mov [ebp-18h],eax
        mov eax,lowerFloorId
        lea edx,upperFloorNative
        call edx
        pop ebx
        pop ebp
        ret
    }
}


static unsigned char landingGateNative[]={0x8D,0x45,0xD0,0x50,0x8D,0x45,0xBC,0x50,0xE8,0,0,0,0,0x50,0xE8,0,0,0,0,0x83,0xC4,0x0C,0x3A,0xC3,0x0F,0x85,0,0,0,0,0xB8,0xC0,0,0,0,0xC3,0xB8,0xD0,0,0,0,0xC3};
static unsigned landingGateStorage[128],landingNativeCalls,landingNativeResult=1;
static StageGrid landingGateSource={14,12,0,4,{0,0,0}};
static StagePoint landingGateEndpoint={16.5f,12.5f,0};
static unsigned char __cdecl LandingNativeQuery(const StageGrid*){++landingNativeCalls;return (unsigned char)landingNativeResult;}
extern "C" __declspec(naked) unsigned ReplayLandingGate(){
    __asm {
        push ebp
        push ebx
        lea ebp,landingGateStorage
        add ebp,256
        xor ebx,ebx
        lea eax,landingGateNative
        call eax
        pop ebx
        pop ebp
        ret
    }
}


static unsigned char suspendedNative[]={0x8B,0x06,0x8D,0x8D,0x38,0xFF,0xFF,0xFF,0x51,0x8B,0xCE,0xFF,0x90,0xF8,0,0,0,0xD9,0x45,0xF0,0xDA,0xE9,0xDF,0xE0,0xF6,0xC4,0x44,0x7B,0x06,0xB8,0xD0,0,0,0,0xC3,0xB8,0xC0,0,0,0,0xC3};
extern "C" __declspec(naked) unsigned ReplaySuspendedCorner(){
    __asm {
        push ebp
        push esi
        lea ebp,landingGateStorage
        add ebp,256
        lea esi,testWorld
        lea eax,suspendedNative
        call eax
        pop esi
        pop ebp
        ret
    }
}


static unsigned char shadowRasterNative[]={0xC7,0x44,0x24,0x48,0,0,0x40,0x3F,0x75,0x08,0xC7,0x44,0x24,0x48,0,0,0,0,0xC3};
static unsigned shadowRasterFlag=1,shadowRasterResult,shadowRasterFlags,shadowRasterEAX,shadowRasterEBX,shadowRasterEDI,shadowRasterEBP;
extern "C" __declspec(naked) void ReplayShadowRaster(){
    __asm {
        push ebp
        push ebx
        push edi
        sub esp,96
        mov edi,10
        mov ebp,10
        mov ebx,0
        mov eax,shadowRasterFlag
        test al,al
        lea edx,shadowRasterNative
        call edx
        mov shadowRasterEAX,eax
        mov shadowRasterEBX,ebx
        mov shadowRasterEDI,edi
        mov shadowRasterEBP,ebp
        pushfd
        pop dword ptr [shadowRasterFlags]
        mov eax,[esp+44h]
        mov shadowRasterResult,eax
        add esp,96
        pop edi
        pop ebx
        pop ebp
        ret
    }
}

static int TestShadowRaster(){
    float sentinel=19.25f,remaining=0;
    // Exact native stage-offset store and following JNE/zero-store preserve
    // ground and lower-stage behavior; only unambiguous ID82 gets2.25.
    endpointFixture=false;ownerFixture=false;upperFixture=false;boundary=false;landingEdgeTest=false;
    testLayer=0;testBase=0;testId=82;
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;
    DWORD rasterProtection=0;
    if(!VirtualProtect(shadowRasterNative,sizeof(shadowRasterNative),PAGE_EXECUTE_READWRITE,&rasterProtection))return 390;
    for(unsigned pass=0;pass<2;++pass){shadowRasterFlag=pass;ReplayShadowRaster();if(shadowRasterResult!=(pass?0x3F400000u:0))return 391;}
    g_shadowRasterResume=(UINT_PTR)shadowRasterNative+8;
    shadowRasterNative[0]=0xE9;
    *(unsigned*)(shadowRasterNative+1)=(unsigned)((UINT_PTR)ShadowRasterHeightHook-((UINT_PTR)shadowRasterNative+5));
    shadowRasterNative[5]=shadowRasterNative[6]=shadowRasterNative[7]=0x90;
    FlushInstructionCache((HANDLE)-1,shadowRasterNative,sizeof(shadowRasterNative));
    const unsigned rasterIds[]={4,80,81,82,70,71,72,79,83,~0u};
    for(unsigned k=0;k<10;++k)for(unsigned pass=0;pass<2;++pass){
        testId=rasterIds[k];shadowRasterFlag=pass;
        __asm {fld sentinel}
        ReplayShadowRaster();
        __asm {fstp remaining}
        unsigned expected=pass?(testId==82?0x40100000u:0x3F400000u):0;
        if(shadowRasterResult!=expected || remaining!=sentinel || shadowRasterEAX!=pass || shadowRasterEBX || shadowRasterEDI!=10 || shadowRasterEBP!=10 || !!(shadowRasterFlags&0x40)==!!pass)return 392;
    }
    testId=82;boundary=true;shadowRasterFlag=1;ReplayShadowRaster();
    if(shadowRasterResult!=0x3F400000)return 393;
    boundary=false;testLayer=2;
    if(ShadowRasterHeightBits(10,10,2,1)!=0x40100000 || ShadowRasterHeightBits(10,10,0,1)!=0x3F400000)return 394;
    return 0;
}

static unsigned char seqCapNative[]={0x8b,0x44,0x24,0x2c,0xd9,0x6,0x8b,0x4c,0x24,0x34,0x89,0x44,0x24,0x20,0xd9,0x44,0x24,0x20,0xd8,0xc1,0x89,0x5c,0x24,0x24,0x89,0x4c,0x24,0x28,0xd9,0x5c,0x24,0x20,0x8b,0x54,0x24,0x20,0x89,0x54,0x24,0x38,0xdd,0xd8,0x8d,0x54,0x24,0x38,0xd9,0x46,0x4,0x52,0xd9,0x44,0x24,0x28,0x57,0xd8,0xc1,0xd9,0x5c,0x24,0x2c,0x8b,0x44,0x24,0x2c,0x89,0x44,0x24,0x44,0xdd,0xd8,0xd9,0x46,0x8,0xd9,0x44,0x24,0x30,0xd8,0xc1,0xd9,0x5c,0x24,0x30,0x8b,0x4c,0x24,0x30,0x89,0x4c,0x24,0x48,0x8b,0x4d,0x0,0xdd,0xd8,0xd9,0x44,0x24,0x48,0xd8,0x44,0x24,0x1c,0xd9,0x5c,0x24,0x48,0x8b,0x1,0xff,0x90,0xa4,0x0,0x0,0x0,0x8b,0xc8,0xe8,0xee,0x55,0xbd,0xff,0xc3};
static unsigned char boundsCapNative[]={0x8b,0x54,0x24,0x70,0xd9,0x7,0x8b,0x44,0x24,0x74,0x8b,0x4c,0x24,0x78,0x89,0x54,0x24,0x3c,0xd9,0x44,0x24,0x3c,0xd8,0xc1,0x89,0x44,0x24,0x40,0x89,0x4c,0x24,0x44,0xd9,0x5c,0x24,0x3c,0x8b,0x54,0x24,0x3c,0x89,0x54,0x24,0x7c,0xdd,0xd8,0x8d,0x54,0x24,0x7c,0xd9,0x47,0x4,0x52,0xd9,0x44,0x24,0x44,0xd8,0xc1,0xd9,0x5c,0x24,0x44,0x8b,0x44,0x24,0x44,0x89,0x84,0x24,0x84,0x0,0x0,0x0,0xdd,0xd8,0xd9,0x47,0x8,0xd9,0x44,0x24,0x48,0xd8,0xc1,0xd9,0x5c,0x24,0x48,0x8b,0x4c,0x24,0x48,0x89,0x8c,0x24,0x88,0x0,0x0,0x0,0xdd,0xd8,0x8d,0x4c,0x24,0x50,0xd9,0x84,0x24,0x88,0x0,0x0,0x0,0xd8,0x44,0x24,0x4c,0xd9,0x9c,0x24,0x88,0x0,0x0,0x0,0xe8,0x38,0x2c,0xbd,0xff,0xc3};

static float capOffset=.75f,boundsPoint[3];
static void __fastcall CaptureBounds(void* box,void*,const float* p){
    float* b=(float*)box;
    for(unsigned k=0;k<3;++k){boundsPoint[k]=p[k];if(p[k]<b[k])b[k]=p[k];if(p[k]>b[k+3])b[k+3]=p[k];}
}
extern "C" __declspec(naked) void __cdecl RunSeqCap(unsigned mesh,unsigned vertex){
 __asm {
 push ebp
 push ebx
 push esi
 push edi
 sub esp,160
 mov edi,esp
 mov ecx,40
 xor eax,eax
 rep stosd
 mov eax,[esp+180]
 mov [esp+12],eax
 mov eax,capOffset
 mov [esp+16],eax
 mov esi,[esp+184]
 xor ebx,ebx
 xor edi,edi
 lea ebp,secondaryWorldPointer
 lea eax,seqCapNative
 call eax
 add esp,160
 pop edi
 pop esi
 pop ebx
 pop ebp
 ret
 }
}
extern "C" __declspec(naked) void __cdecl RunBoundsCap(unsigned mesh,unsigned vertex){
 __asm {
 push ebp
 push ebx
 push esi
 push edi
 sub esp,160
 mov edi,esp
 mov ecx,40
 xor eax,eax
 rep stosd
 mov eax,capOffset
 mov [esp+68],eax
 mov dword ptr [esp+72],0x7F7FFFFF
 mov dword ptr [esp+76],0x7F7FFFFF
 mov dword ptr [esp+80],0x7F7FFFFF
 mov dword ptr [esp+84],0xFF7FFFFF
 mov dword ptr [esp+88],0xFF7FFFFF
 mov dword ptr [esp+92],0xFF7FFFFF
 mov esi,[esp+180]
 mov edi,[esp+184]
 lea eax,boundsCapNative
 call eax
 add esp,160
 pop edi
 pop esi
 pop ebx
 pop ebp
 ret
 }
}
static int TestCpuConsistency(){
    unsigned char mesh[96]={0};float vertices[6][10]={0};
    *(unsigned*)(mesh+0x1C)=(unsigned)vertices;*(unsigned*)(mesh+0x20)=(unsigned)(vertices+6);
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)BaseFake;
    upperFixture=endpointFixture=ownerFixture=landingEdgeTest=boundary=false;
    testVtable[0xE8/4]=(UINT_PTR)QueryFake;
    testVtable[0x144/4]=testVtable[0x148/4]=testVtable[0x160/4]=(UINT_PTR)Low;
    testVtable[0x14C/4]=testVtable[0x150/4]=(UINT_PTR)High;testVtable[0x164/4]=(UINT_PTR)FloorHigh;
    secondaryWorldVtable[0xA4/4]=(UINT_PTR)GetSecondaryGeometry;
    DWORD old=0;
    if(!VirtualProtect(seqCapNative,sizeof(seqCapNative),PAGE_EXECUTE_READWRITE,&old) || !VirtualProtect(boundsCapNative,sizeof(boundsCapNative),PAGE_EXECUTE_READWRITE,&old))return 400;
    *(unsigned*)(seqCapNative+0x77+1)=(unsigned)((UINT_PTR)EmitSecondaryVertex-((UINT_PTR)seqCapNative+0x77+5));
    *(unsigned*)(boundsCapNative+0x7D+1)=(unsigned)((UINT_PTR)CaptureBounds-((UINT_PTR)boundsCapNative+0x7D+5));
    testBase=0;testLayer=0;testId=82;
    for(unsigned k=0;k<6;++k){vertices[k][0]=12.25f;vertices[k][1]=12.25f;}
    RunSeqCap((unsigned)mesh,(unsigned)vertices);RunBoundsCap((unsigned)mesh,(unsigned)vertices);
    if(emittedSecondary[2]!=.75f || boundsPoint[2]!=.75f)return 401;
    g_cpuCapResume=(UINT_PTR)seqCapNative+0x6D;g_capBoundsResume=(UINT_PTR)boundsCapNative+0x7D;
    seqCapNative[0x65]=0xE9;*(unsigned*)(seqCapNative+0x66)=(unsigned)((UINT_PTR)CpuCapHeightHook-((UINT_PTR)seqCapNative+0x6A));
    for(unsigned k=0x6A;k<0x6D;++k)seqCapNative[k]=0x90;
    boundsCapNative[0x72]=0xE9;*(unsigned*)(boundsCapNative+0x73)=(unsigned)((UINT_PTR)CapBoundsHeightHook-((UINT_PTR)boundsCapNative+0x77));
    for(unsigned k=0x77;k<0x7D;++k)boundsCapNative[k]=0x90;
    FlushInstructionCache((HANDLE)-1,seqCapNative,sizeof(seqCapNative));FlushInstructionCache((HANDLE)-1,boundsCapNative,sizeof(boundsCapNative));
    const unsigned ids[]={4,80,81,82,70};float sentinel=17.25f,remaining=0;
    for(unsigned layer=0;layer<2;++layer)for(unsigned h=0;h<5;++h)for(unsigned pass=0;pass<2;++pass)for(unsigned v=0;v<6;++v){
        testLayer=layer;testBase=layer?12.0f:0;testId=ids[h];capOffset=pass?.75f:0;
        for(unsigned k=0;k<6;++k)vertices[k][2]=testBase;
        float expected=testBase+(pass?HeightForId(testId):0);
        __asm {fld sentinel}
        RunSeqCap((unsigned)mesh,(unsigned)vertices[v]);
        __asm {fstp remaining}
        if(emittedSecondary[0]!=12.25f || emittedSecondary[1]!=12.25f || emittedSecondary[2]!=expected || remaining!=sentinel)return 402+h;
        __asm {fld sentinel}
        RunBoundsCap((unsigned)mesh,(unsigned)vertices[v]);
        __asm {fstp remaining}
        if(boundsPoint[0]!=12.25f || boundsPoint[1]!=12.25f || boundsPoint[2]!=expected || remaining!=sentinel)return 410+h;
        for(unsigned k=0;k<6;++k)if(vertices[k][2]!=testBase)return 415;
    }
    testLayer=0;testBase=0;testId=82;
    if(CpuCapHeightBits((unsigned)mesh,(unsigned)vertices+1,0x3F400000,0)!=0x3F400000)return 416;
    *(unsigned*)(mesh+0x20)=(unsigned)(vertices+2);
    if(CpuCapHeightBits((unsigned)mesh,(unsigned)vertices,0x3F400000,1)!=0x3F400000)return 417;
    // The sampled trace must leave both native and custom outputs unchanged.
    for(unsigned path=0;path<5;++path)for(unsigned pass=0;pass<2;++pass){
        ShadowSurfacePoint p={12.25f,12.25f,0};unsigned bits=pass?0x3F400000u:0;
        unsigned expected=ShadowPlaneOffsetBits(&p,bits);
        for(unsigned k=0;k<64;++k)if(ShadowPlaneTrace(&p,bits,path)!=expected)return 420+path;
    }

    return 0;
}
static unsigned char nearestLayerNative[]={0x83,0xec,0x10,0x53,0x56,0x57,0xe8,0x41,0x98,0x88,0xff,0x8b,0xf0,0x8b,0x6,0x8b,0xce,0xff,0x90,0x60,0x1,0x0,0x0,0x8b,0x16,0x8b,0xce,0x8b,0xd8,0xc7,0x44,0x24,0xc,0xff,0xff,0x7f,0x7f,0xff,0x92,0x64,0x1,0x0,0x0,0x8b,0xf0,0x3b,0xf3,0x7c,0x35,0x8b,0x7c,0x24,0x20,0x8b,0x7,0x8b,0x4f,0x4,0x8d,0x54,0x24,0x10,0x52,0x89,0x44,0x24,0x14,0x89,0x4c,0x24,0x18,0x89,0x74,0x24,0x1c,0xe8,0x60,0xea,0xff,0xff,0xd8,0x57,0x8,0x83,0xc4,0x4,0xdf,0xe0,0xf6,0xc4,0x41,0x7b,0x12,0x4e,0xd9,0x5c,0x24,0xc,0x3b,0xf3,0x7d,0xcf,0x5f,0x5e,0x8b,0xc3,0x5b,0x83,0xc4,0x10,0xc3,0xd9,0x44,0x24,0xc,0xd8,0xc1,0xd8,0xd,0xc8,0x69,0x1d,0x1,0xd9,0x47,0x8,0xde,0xd9,0xdf,0xe0,0xdd,0xd8,0xf6,0xc4,0x41,0x7a,0x9,0x5f,0x8b,0xc6,0x5e,0x5b,0x83,0xc4,0x10,0xc3,0x5f,0x8d,0x46,0x1,0x5e,0x5b,0x83,0xc4,0x10,0xc3};

static unsigned layerCount=1;static float layerTerrain=0;
static int __fastcall LayerCountFake(void*,void*){return (int)layerCount;}
static float __cdecl LayerRawFake(StagePoint* p){return layerTerrain+3.0f*p->layer;}
static unsigned RunNearestLayer(ShadowSurfacePoint* p){typedef unsigned (__cdecl *Fn)(ShadowSurfacePoint*);return ((Fn)(UINT_PTR)nearestLayerNative)(p);}
static int TestLayerOwner(){
    endpointFixture=ownerFixture=upperFixture=boundary=landingEdgeTest=false;
    testLayer=0;testId=82;layerCount=1;layerTerrain=0;
    testVtable[0x164/4]=(UINT_PTR)LayerCountFake;
    g_baseSurface=(UINT_PTR)LayerRawFake;
    DWORD old=0;if(!VirtualProtect(nearestLayerNative,sizeof(nearestLayerNative),PAGE_EXECUTE_READWRITE,&old))return 450;
    *(unsigned*)(nearestLayerNative+7)=(unsigned)((UINT_PTR)GetFakeWorld-((UINT_PTR)nearestLayerNative+11));
    *(unsigned*)(nearestLayerNative+0x4C)=(unsigned)((UINT_PTR)LayerRawFake-((UINT_PTR)nearestLayerNative+0x50));
    // Relocate the native half-height constant to a fixture-owned float.
    static float half=.5f;*(unsigned*)(nearestLayerNative+0x77)=(unsigned)&half;
    ShadowSurfacePoint p={12.5f,12.5f,2.25f};
    if(RunNearestLayer(&p)!=1)return 451;
    p.z=1.5f;if(RunNearestLayer(&p)!=0)return 452;
    p.z=.375f;if(RunNearestLayer(&p)!=0)return 453;
    g_layerOwnerResume=(UINT_PTR)nearestLayerNative+0x97;
    nearestLayerNative[0x92]=0xE9;*(unsigned*)(nearestLayerNative+0x93)=(unsigned)((UINT_PTR)TwelveStepLayerOwnerHook-((UINT_PTR)nearestLayerNative+0x97));
    FlushInstructionCache((HANDLE)-1,nearestLayerNative,sizeof(nearestLayerNative));
    unsigned ids[]={4,80,81,82,70,71,72,79,83,~0u};
    float sentinel=27.5f,remaining=0;
    for(unsigned floor=0;floor<3;++floor)for(unsigned elevated=0;elevated<2;++elevated)for(unsigned k=0;k<10;++k){
        testLayer=floor;layerCount=floor+1;layerTerrain=elevated?11.0f:0;testId=ids[k];
        p.z=layerTerrain+3.0f*floor+2.25f;ShadowSurfacePoint original=p;
        __asm {fld sentinel}
        unsigned selected=RunNearestLayer(&p);
        __asm {fstp remaining}
        if(selected!=(ids[k]==82?floor:floor+1) || remaining!=sentinel)return 454+k;
        if(p.x!=original.x || p.y!=original.y || p.z!=original.z)return 464;
    }
    testLayer=0;layerCount=1;testId=82;layerTerrain=0;
    float z[]={0,.375f,.75f,1.5f,2.0f,2.229f,2.24f,2.25f,2.26f,2.271f,3.0f,4.0f};
    for(unsigned k=0;k<12;++k){
        p.z=z[k];unsigned expected=k<=3 || (k>=6 && k<=8)?0:1;
        if(RunNearestLayer(&p)!=expected)return 465+k;
    }
    p=ShadowSurfacePoint{-2,12.5f,2.25f};if(RunNearestLayer(&p)!=1)return 477;
    p=ShadowSurfacePoint{12.5f,12.5f,2.25f};
    if(ResolveTwelveStepLayer(&p,0)!=0 || ResolveTwelveStepLayer(0,1)!=1)return 478;

    return 0;
}

// Regression: the lower stage occupies tile [10,11) on both axes.
// Its north/west entry edge can sample the outside tile at coordinate 11.
static unsigned directionTarget=4,directionQueryTarget=4;
static float directionTerrain=0,directionSurfaceError=0;
static unsigned __fastcall DirectionQuery(void*,void*,StageGrid* g){return g->layer==0 && g->x==10 && g->y==10?directionQueryTarget:~0u;}
static float __cdecl DirectionSurface(const void* input){const StagePoint* p=(const StagePoint*)input;return directionTerrain+(p->layer==0 && p->x>=10 && p->x<11 && p->y>=10 && p->y<11?HeightForId(directionTarget)+directionSurfaceError:0);}
static int TestLandingDirections(){
    testVtable[0x160/4]=(UINT_PTR)Low;testVtable[0x164/4]=(UINT_PTR)FloorHigh;
    testVtable[0x144/4]=testVtable[0x148/4]=(UINT_PTR)Low;testVtable[0x14C/4]=testVtable[0x150/4]=(UINT_PTR)High;
    static unsigned storage[256];unsigned frame=(unsigned)(storage+160);
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;
    testVtable[0xE8/4]=(UINT_PTR)DirectionQuery;g_landingSurface=(UINT_PTR)DirectionSurface;
    const unsigned ids[4]={4,80,81,82},masks[4]={1,2,4,8};
    for(unsigned elevated=0;elevated<2;++elevated)for(unsigned upstairs=0;upstairs<2;++upstairs)
    for(unsigned id=0;id<4;++id)for(unsigned d=0;d<4;++d){
        directionTarget=directionQueryTarget=ids[id];directionTerrain=elevated?11.0f:0;directionSurfaceError=0;
        float dx=masks[d]==1?-0.25f:masks[d]==4?0.25f:0,dy=masks[d]==2?0.25f:masks[d]==8?-0.25f:0;
        StagePoint* point=(StagePoint*)(frame-0x30);*point=StagePoint{dx<0?11.0f:dx>0?10.0f:10.5f,dy<0?11.0f:dy>0?10.0f:10.5f,0};StagePoint original=*point;
        *(float*)(frame-0x58)=dx;*(float*)(frame-0x54)=dy;
        g_upperStage=1;g_upperFloor=upstairs;g_lowerFloor=0;g_landingTargetId=ids[id];g_lowerSurface=directionTerrain+HeightForId(ids[id]);
        *(unsigned*)(frame-0x34)=11;*(float*)(frame+0x14)=g_lowerSurface;
        for(unsigned negative=0;negative<5;++negative){
            g_landingMatched=0;*(unsigned*)(frame-0x50)=14;
            directionQueryTarget=negative==1?79:ids[id];directionSurfaceError=negative==2?0.05f:0;
            *(float*)(frame+0x14)=g_lowerSurface+(negative==3?0.1875f:0);
            g_upperStage=negative==4?0:1;
            // Wrong next ID and surface checks apply to negative-axis edge probes;
            // a point already on the correct physical surface uses the native check.
            MatchLanding(frame);
            bool expected=negative==0 || (negative==1 && dx>=0 && dy>=0);
            if(g_landingMatched!=(unsigned)expected || *(unsigned*)(frame-0x50)!=(expected?12u:14u))return 510+negative;
            if(point->x!=original.x || point->y!=original.y || point->layer!=original.layer)return 516;
        }
        if(dx<0 || dy<0){
            directionQueryTarget=ids[id];directionSurfaceError=0;g_upperStage=1;g_landingMatched=0;*(unsigned*)(frame-0x50)=14;*(float*)(frame+0x14)=g_lowerSurface;
            *(float*)(frame-0x58)=dx*2;*(float*)(frame-0x54)=dy*2;
            MatchLanding(frame);if(g_landingMatched || *(unsigned*)(frame-0x50)!=14)return 517;
        }
    }
    return 0;
}


static int TestTwoStepFloors(){
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)BaseFake;
    testVtable[0xE8/4]=(UINT_PTR)QueryFake;testVtable[0x144/4]=testVtable[0x148/4]=testVtable[0x160/4]=(UINT_PTR)Low;
    testVtable[0x14C/4]=testVtable[0x150/4]=(UINT_PTR)High;testVtable[0x164/4]=(UINT_PTR)FloorHigh;
    endpointFixture=true;upperFixture=ownerFixture=landingEdgeTest=boundary=false;
    endpointSource=80;endpointTarget=~0u;endpointSourceX=endpointSourceY=20;endpointSlope=false;
    g_nativeFinder=(UINT_PTR)FinderFake;nativeFinderResult=0;
    const unsigned masks[4]={1,2,4,8};
    typedef unsigned char (__cdecl *AutoFn)(unsigned,StageGrid*,unsigned,StagePoint*,void*,unsigned,unsigned);
    for(unsigned layer=0;layer<4;++layer)for(unsigned terrain=0;terrain<2;++terrain)for(unsigned d=0;d<4;++d){
        testLayer=layer;testBase=terrain?11.0f:0.75f;endpointMask=masks[d];
        StagePoint proposed={-1,-1,-1};
        if(!ProposeStageEndpoint(20,20,layer,endpointMask,&proposed,1) || proposed.layer!=(int)layer || proposed.x!=20.5f+EndpointDX() || proposed.y!=20.5f+EndpointDY())return 530;
        for(unsigned accepted=0;accepted<2;++accepted){
            StageGrid source={20,20,(int)layer,(unsigned char)endpointMask,{0,0,0}};StageGrid original=source;
            nativeCoreResult=accepted;nativeCoreCalls=nativeFinderCalls=nativeClearCalls=0;proposed=StagePoint{-1,-1,-1};
            unsigned char result=((AutoFn)(UINT_PTR)nativeAutoEntry)(1,&source,1,&proposed,&nativeBuilder,0,0);
            if(result!=accepted || nativeCoreCalls!=1 || nativeFinderCalls || nativeClearCalls!=1)return 531;
            if(nativePoint[0]!=FloatBits(20.5f+EndpointDX()) || nativePoint[1]!=FloatBits(20.5f+EndpointDY()) || nativePoint[2]!=layer)return 532;
            if(source.x!=original.x || source.y!=original.y || source.layer!=original.layer || source.mask!=original.mask)return 533;
        }
        endpointSlope=true;proposed=StagePoint{-1,-1,-1};
        if(ProposeStageEndpoint(20,20,layer,endpointMask,&proposed,1) || proposed.layer!=-1)return 534;
        endpointSlope=false;endpointTarget=4;
        if(ProposeStageEndpoint(20,20,layer,endpointMask,&proposed,1))return 535;
        endpointTarget=~0u;
    }
    endpointFixture=false;return 0;
}


static StagePoint trianglePoint={0};static unsigned triangleID=81,triangleRotation=0;
static StageGrid* __cdecl TriangleConvert(StageGrid* out,StagePoint* p){trianglePoint=*p;return ConvertFake(out,p);}
static unsigned __fastcall TriangleQuery(void*,void*,StageGrid*){
    float x=trianglePoint.x-14.0f,y=trianglePoint.y-22.0f;
    for(unsigned r=0;r<triangleRotation;++r){float previous=x;x=-y;y=previous;}
    x+=14;y+=22;int ix=Trunc(x),iy=Trunc(y);float fx=x-ix,fy=y-iy;
    unsigned bit=fx>fy?(1.0f-fx>fy?8:4):(1.0f-fx>fy?1:2),mask=0;
    if(ix==12&&iy==21)mask=6;if(ix==12&&iy==22)mask=12;
    if(ix==13&&iy==20)mask=6;if(ix==13&&(iy==21||iy==22))mask=15;if(ix==13&&iy==23)mask=12;
    if(ix==14&&iy==20)mask=3;if(ix==14&&(iy==21||iy==22))mask=15;if(ix==14&&iy==23)mask=9;
    if(ix==15&&iy==21)mask=3;if(ix==15&&iy==22)mask=9;
    return mask&bit?triangleID:~0u;
}
static int TestDiagonalWallCorners(){
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)TriangleConvert;testVtable[0xE8/4]=(UINT_PTR)TriangleQuery;
    const float tips[4][2]={{0,-2},{0,2},{-2,0},{2,0}};
    for(unsigned id=80;id<=82;++id)for(unsigned rotation=0;rotation<4;++rotation)for(unsigned corner=0;corner<4;++corner){
        triangleID=id;triangleRotation=rotation;
        float x=tips[corner][0],y=tips[corner][1];
        for(unsigned r=0;r<rotation;++r){float old=x;x=y;y=-old;}
        StagePoint p={x+14,y+22,0},before=p;
        if(WallHeightBits(&p)!=FloatBits(HeightForId(id)))return 550+corner;
        if(p.x!=before.x||p.y!=before.y||p.layer!=before.layer)return 555;
    }
    triangleID=4;StagePoint p={14,24,0};if(WallHeightBits(&p)!=0x3F400000)return 556;
    triangleID=70;if(WallHeightBits(&p)!=0x3F400000)return 557;
    p=StagePoint{2,2,0};triangleID=82;if(WallHeightBits(&p)!=0x3F400000)return 558;
    return 0;
}
static unsigned char joinNative[]={0x83,0x7d,0x8,0x4,0x88,0x45,0xf0,0xf,0x94,0x45,0xf3,0x80,0x7d,0xf1,0x0,0x89,0x7d,0xe4,0x74,0x26,0x8b,0x6,0x8d,0x4d,0xb0,0x51,0x8b,0xce,0xff,0x90,0xe8,0x0,0x0,0x0,0x80,0x7d,0xf3,0x0,0x89,0x45,0xe4,0x75,0x7,0x83,0xf8,0x4,0x74,0x7,0xeb,0x8,0x83,0xf8,0x4,0x74,0x3,0x89,0x7d,0xe4,0x80,0x7d,0xf0,0x0,0x74,0x60,0x8b,0x6,0x8d,0x4d,0xa0,0x51,0x8b,0xce,0xff,0x90,0xe8,0x0,0x0,0x0,0x80,0x7d,0xf3,0x0,0x8b,0xf8,0x75,0x42,0x83,0xff,0x4,0x74,0x42,0x8b,0x4d,0xdc,0x8b,0x1,0x8d,0x55,0x88,0x52,0xff,0x90,0xc8,0x0,0x0,0x0,0x89,0x45,0xe8,0x57,0xc6,0x45,0xf3,0x0,0xe8,0x8,0xf4,0xff,0xff,0x80,0x7d,0x18,0x0,0xf,0x84,0x90,0x0,0x0,0x0,0x8b,0x4d,0xe4,0x51,0x8b,0xd0,0xe8,0xf3,0xf3,0xff,0xff,0x83,0xc4,0x8,0x3b,0xc2,0x75,0x1c,0x83,0xf9,0xff,0xeb,0x75,0x83,0xff,0x4,0x74,0x3,0x83,0xcf,0xff,0x80,0x7d,0xf3,0x0,0x74,0xb5,0xc7,0x45,0xe8,0x2d,0x1,0x0,0x0,0xeb,0xbe,0x83,0xf9,0xff,0x8b,0x3,0x75,0x15,0x57,0x8b,0xcb,0xff,0x90,0x9c,0x0,0x0,0x0,0x8b,0x10,0x8b,0xc8,0xff,0x52,0x38,0x39,0x45,0xe8,0xeb,0x42,0x51,0x8b,0xcb,0xff,0x90,0x9c,0x0,0x0,0x0,0x83,0xff,0xff,0x8b,0x10,0x8b,0xc8,0x74,0xe6,0xff,0x52,0x30,0x83,0xf8,0x2,0x8b,0x3,0x57,0x8b,0xcb,0xf,0x94,0x45,0xf0,0xff,0x90,0x9c,0x0,0x0,0x0,0x8b,0x10,0x8b,0xc8,0xff,0x52,0x30,0x83,0xf8,0x2,0xf,0x94,0xc0,0x83,0x7d,0x8,0x2,0x75,0x21,0x80,0x7d,0xf0,0x0,0x75,0x17,0x84,0xc0,0x74,0x17,0xeb,0x11,0xff,0x75,0xe4,0x8b,0xc8,0xe8,0x64,0xf3,0xff,0xff,0x83,0xc4,0x8,0x3b,0xc1,0x75,0x4,0xc6,0x45,0xf3,0x1,0x8a,0x45,0xf3,0xc3};

// Execute the original 609BF1..609D17 reconciliation block, with the native
// ID normalization helper and fake read-only world/catalogue virtual methods.
static unsigned joinIDs[2],joinQueryCount=0,joinCatalogID=0;
static UINT_PTR joinWorldVT[100],joinWallVT[100],joinCatalogVT[100],joinProductVT[100];
static FakeWorld joinWorld={joinWorldVT},joinWall={joinWallVT},joinCatalog={joinCatalogVT},joinProduct={joinProductVT};
static unsigned char joinFrameStorage[256];static unsigned char* joinFrame=joinFrameStorage+160;
static UINT_PTR joinEntry=(UINT_PTR)joinNative;
static unsigned __fastcall JoinQuery(void*,void*,void*){return joinIDs[(joinQueryCount++)&1];}
static unsigned __fastcall JoinWallStyle(void*,void*,void*){return 301;}
static void* __fastcall JoinProduct(void*,void*,unsigned id){joinCatalogID=id;return &joinProduct;}
static unsigned __fastcall JoinCategory(void*,void*){return joinCatalogID==4 || (joinCatalogID>=80&&joinCatalogID<=82)?4:2;}
static unsigned __fastcall JoinStyle(void*,void*){return JoinCategory(0,0)==4?301:23;}
static unsigned __cdecl JoinNormalize(unsigned id){return id==5?3:id;}
static unsigned __declspec(naked) ReplayJoin(){__asm{
 push ebp
 push ebx
 push esi
 push edi
 mov ebp,joinFrame
 lea esi,joinWorld
 lea ebx,joinCatalog
 or edi,-1
 call dword ptr [joinEntry]
 movzx eax,al
 pop edi
 pop esi
 pop ebx
 pop ebp
 ret
}}
static unsigned RunJoin(unsigned a,unsigned b,unsigned category,unsigned apply,unsigned presentA=1,unsigned presentB=1){
 for(unsigned i=0;i<sizeof(joinFrameStorage);++i)joinFrameStorage[i]=0;
 *(unsigned*)(joinFrame+8)=category;*(unsigned*)(joinFrame+0x18)=apply;
 joinFrame[-0xf]=(unsigned char)presentA;joinFrame[-0x10]=(unsigned char)presentB;
 *(unsigned*)(joinFrame-0x24)=(unsigned)&joinWall;
 joinIDs[0]=a;joinIDs[1]=b;joinQueryCount=0;
 return ReplayJoin();
}
static int TestStageJoins(){
 DWORD protection=0;if(!VirtualProtect(joinNative,sizeof(joinNative),PAGE_EXECUTE_READWRITE,&protection))return 580;
 joinWorldVT[0xe8/4]=(UINT_PTR)JoinQuery;joinWallVT[0xc8/4]=(UINT_PTR)JoinWallStyle;
 joinCatalogVT[0x9c/4]=(UINT_PTR)JoinProduct;joinProductVT[0x30/4]=(UINT_PTR)JoinCategory;joinProductVT[0x38/4]=(UINT_PTR)JoinStyle;
 *(unsigned*)(joinNative+115)=(unsigned)((UINT_PTR)JoinNormalize-((UINT_PTR)joinNative+119));
 *(unsigned*)(joinNative+136)=(unsigned)((UINT_PTR)JoinNormalize-((UINT_PTR)joinNative+140));
 *(unsigned*)(joinNative+279)=(unsigned)((UINT_PTR)JoinNormalize-((UINT_PTR)joinNative+283));

#ifndef BASELINE_JOINS
 const unsigned sites[]={0x2b,0x32,0x56,0x98},taken[]={0x37,0x3a,0x9d,0xa0};
 void* hooks[]={(void*)StageJoinHook0,(void*)StageJoinHook1,(void*)StageJoinHook2,(void*)StageJoinHook3};
 for(unsigned i=0;i<4;++i){g_joinResume[i]=(UINT_PTR)joinNative+sites[i]+5;g_joinTaken[i]=(UINT_PTR)joinNative+taken[i];PatchJump((UINT_PTR)joinNative+sites[i],hooks[i],5,"fixture");}
#endif
 FlushInstructionCache((HANDLE)-1,joinNative,sizeof(joinNative));
 const unsigned stages[]={4,80,81,82};
 for(unsigned i=0;i<4;++i)for(unsigned j=0;j<4;++j)for(unsigned apply=0;apply<2;++apply){
  unsigned result=RunJoin(stages[i],stages[j],4,apply);
  if(result!=(i==j))return 581+i*4+j;
  if(joinIDs[0]!=stages[i] || joinIDs[1]!=stages[j] || joinQueryCount!=2)return 600;
 }
 // Outer perimeter and empty space must remain; no broad collision bypass.
 for(unsigned id=80;id<=82;++id){
  if(RunJoin(id,~0u,4,1,1,0) || RunJoin(~0u,id,4,1,0,1) || RunJoin(~0u,~0u,4,1,0,0))return 601;
  // Non-stage construction ignores stages exactly as it ignores native ID4.
  for(unsigned side=0;side<2;++side){
   unsigned expected=side?RunJoin(3,4,3,1):RunJoin(4,3,3,1);
   unsigned actual=side?RunJoin(3,id,3,1):RunJoin(id,3,3,1);
   if(actual!=expected)return 602;
  }
 }
 if(RunJoin(70,70,4,1) || RunJoin(83,83,4,1))return 603;
 return 0;
}
static unsigned char fenceNative[]={0x83,0xec,0x6c,0x56,0x8b,0x74,0x24,0x74,0x8b,0x6,0x8b,0x4e,0x4,0x8b,0x56,0x8,0x89,0x44,0x24,0x44,0x8b,0x46,0xc,0x89,0x4c,0x24,0x48,0x8b,0x4e,0x10,0x89,0x44,0x24,0x50,0x8d,0x44,0x24,0x44,0x89,0x4c,0x24,0x54,0x89,0x54,0x24,0x4c,0x8b,0x56,0x14,0x50,0x8d,0x4c,0x24,0x24,0x51,0x89,0x54,0x24,0x60,0xe8,0x80,0xbc,0x1b,0x0,0x8d,0x54,0x24,0x58,0x52,0x8d,0x44,0x24,0x38,0x50,0xe8,0x71,0xbc,0x1b,0x0,0x56,0xe8,0x9b,0x6f,0x1b,0x0,0x83,0xc4,0x14,0x84,0xc0,0x74,0x1c,0xd9,0x44,0x24,0x28,0xd8,0x5,0x2c,0xa8,0x23,0x1,0xd9,0x5c,0x24,0x28,0xd9,0x44,0x24,0x34,0xd8,0x5,0x2c,0xa8,0x23,0x1,0xd9,0x5c,0x24,0x34,0xe9,0,0,0,0};

static unsigned fenceID=81,fenceGate=1,fenceLayer=0;static bool fenceSplit=false;
static float fenceRawBase=0,fenceObserved[6];static UINT_PTR fenceEntry=(UINT_PTR)fenceNative;
static StagePoint fencePoints[2];static unsigned fenceOffsetNative=0x3f400000;
static unsigned __fastcall FenceQuery(void*,void*,StageGrid* g){
 if(g->layer!=(int)fenceLayer || g->x<10 || g->x>=14 || g->y<10 || g->y>=14)return ~0u;
 return fenceSplit && g->x>=12?82:fenceID;
}
static void __cdecl FenceRaw(float* out,StagePoint* p){out[0]=p->x;out[1]=p->y;out[2]=fenceRawBase;}
static unsigned __cdecl FenceGate(StagePoint*){return fenceGate;}
static void __cdecl FenceCapture(float* points){for(unsigned i=0;i<6;++i)fenceObserved[i]=points[i];}
static void __declspec(naked) FenceCaptureReturn(){__asm{
 lea eax,[esp+0x20]
 push eax
 call FenceCapture
 add esp,4
 pop esi
 add esp,0x6c
 ret
}}
static unsigned InvokeFence(){
 float sentinel=9.25f,remaining=0;
 __asm{fld sentinel}
 ((void(__cdecl*)(StagePoint*))fenceEntry)(fencePoints);
 __asm{fstp remaining}
 return remaining==sentinel;
}
static int TestFenceHeights(){
 DWORD protection=0;if(!VirtualProtect(fenceNative,sizeof(fenceNative),PAGE_EXECUTE_READWRITE,&protection))return 620;
 *(unsigned*)(fenceNative+60)=(unsigned)((UINT_PTR)FenceRaw-((UINT_PTR)fenceNative+64));
 *(unsigned*)(fenceNative+75)=(unsigned)((UINT_PTR)FenceRaw-((UINT_PTR)fenceNative+79));
 *(unsigned*)(fenceNative+81)=(unsigned)((UINT_PTR)FenceGate-((UINT_PTR)fenceNative+85));
 *(unsigned*)(fenceNative+121)=(unsigned)((UINT_PTR)FenceCaptureReturn-((UINT_PTR)fenceNative+125));

 *(unsigned*)(fenceNative+0x62)=(unsigned)&fenceOffsetNative;
 *(unsigned*)(fenceNative+0x70)=(unsigned)&fenceOffsetNative;
#ifndef BASELINE_FENCES
 g_fenceResume[0]=(UINT_PTR)fenceNative+0x66;g_fenceResume[1]=(UINT_PTR)fenceNative+0x74;
 PatchJump((UINT_PTR)fenceNative+0x60,(void*)FenceStageHeightHook0,6,"fixture");
 PatchJump((UINT_PTR)fenceNative+0x6e,(void*)FenceStageHeightHook1,6,"fixture");
#endif
 FlushInstructionCache((HANDLE)-1,fenceNative,sizeof(fenceNative));
 g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;testVtable[0xe8/4]=(UINT_PTR)FenceQuery;
 const unsigned ids[]={4,80,81,82};const float corners[][2]={{10,10},{14,10},{14,14},{10,14}};
 for(unsigned id=0;id<4;++id)for(unsigned layer=0;layer<3;++layer)for(unsigned side=0;side<4;++side){
  fenceID=ids[id];fenceLayer=layer;fenceRawBase=layer==0?0:layer==1?0.375f:6.0f;fenceGate=1;
  for(unsigned j=0;j<2;++j){unsigned c=(side+j)%4;fencePoints[j]=StagePoint{corners[c][0],corners[c][1],(int)layer};}
  StagePoint before[2]={fencePoints[0],fencePoints[1]};
  if(!InvokeFence())return 621;
  for(unsigned j=0;j<2;++j){
   if(fenceObserved[j*3]!=fencePoints[j].x || fenceObserved[j*3+1]!=fencePoints[j].y || fenceObserved[j*3+2]!=fenceRawBase+HeightForId(fenceID))return 622+id;
   if(fencePoints[j].x!=before[j].x || fencePoints[j].y!=before[j].y || fencePoints[j].layer!=before[j].layer)return 626;
  }
  // Native non-stage branch must bypass both stage-height hooks entirely.
  fenceGate=0;if(!InvokeFence() || fenceObserved[2]!=fenceRawBase || fenceObserved[5]!=fenceRawBase)return 627;
 }
 fenceGate=1;fenceLayer=0;fenceRawBase=0;fenceID=81;fenceSplit=true;
 fencePoints[0]=StagePoint{11,11,0};fencePoints[1]=StagePoint{13,11,0};
 if(!InvokeFence() || fenceObserved[2]!=1.5f || fenceObserved[5]!=2.25f)return 628;
 fenceSplit=false;fenceID=70;
 if(!InvokeFence() || fenceObserved[2]!=0.75f || fenceObserved[5]!=0.75f)return 629;
 fenceID=81;fencePoints[0]=StagePoint{2,2,0};fencePoints[1]=StagePoint{3,2,0};
 if(!InvokeFence() || fenceObserved[2]!=0.75f || fenceObserved[5]!=0.75f)return 630;
 return 0;
}

// Exact A2A108..A2A116 model-height block, separate from A6BB30's transform.
static unsigned char modelNative[]={0xD9,0x44,0x24,0x3C,0xD8,0x05,0x34,0x85,0x23,0x01,0xD9,0x5C,0x24,0x3C,0xE9,0,0,0,0};
static UINT_PTR modelEntry=(UINT_PTR)modelNative;
static void __declspec(naked) ModelCaptureReturn(){__asm{mov eax,[esp+0x3c]} __asm{ret}}
static unsigned __declspec(naked) InvokeModel(){__asm{
 push esi
 sub esp,64
 lea esi,fencePoints
 mov eax,fenceRawBase
 mov [esp+0x38],eax
 call dword ptr [modelEntry]
 add esp,64
 pop esi
 ret
}}
static int TestFenceModels(){
 DWORD protection=0;if(!VirtualProtect(modelNative,sizeof(modelNative),PAGE_EXECUTE_READWRITE,&protection))return 660;
 *(unsigned*)(modelNative+6)=(unsigned)&fenceOffsetNative;
 *(unsigned*)(modelNative+15)=(unsigned)((UINT_PTR)ModelCaptureReturn-((UINT_PTR)modelNative+19));
#ifndef BASELINE_FENCE_MODELS
 g_fenceModelResume=(UINT_PTR)modelNative+10;
 PatchJump((UINT_PTR)modelNative+4,(void*)FenceModelHeightHook,6,"fixture");
#endif
 FlushInstructionCache((HANDLE)-1,modelNative,sizeof(modelNative));
 const unsigned ids[]={4,80,81,82,70};const float corners[][2]={{10,10},{14,10},{14,14},{10,14}};
 for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<3;++layer)for(unsigned side=0;side<4;++side){
  fenceID=ids[id];fenceLayer=layer;fenceRawBase=layer==0?0:layer==1?0.375f:6.0f;
  fencePoints[0]=StagePoint{corners[side][0],corners[side][1],(int)layer};StagePoint before=fencePoints[0];
  float sentinel=9.25f,remaining=0;__asm{fld sentinel}
  unsigned result=InvokeModel();__asm{fstp remaining}
  if(result!=FloatBits(fenceRawBase+HeightForId(fenceID)))return 661+id;
  if(remaining!=sentinel || before.x!=fencePoints[0].x || before.y!=fencePoints[0].y || before.layer!=fencePoints[0].layer)return 666;
 }
#ifndef BASELINE_FENCE_MODELS

#endif
 return 0;
}
static unsigned char panelNative[]={0x3a,0xc3,0x74,0x8,0xc7,0x44,0x24,0x14,0x0,0x0,0x40,0x3f,0xe9,0,0,0,0};
static unsigned char markerNative[]={0x84,0xc0,0xc7,0x44,0x24,0x14,0x0,0x0,0x40,0x3f,0x75,0x8,0xc7,0x44,0x24,0x14,0x0,0x0,0x0,0x0,0xe9,0,0,0,0};

static UINT_PTR uiEntry=0;static unsigned uiGate=1;static unsigned uiPointOffset=0;
static unsigned char uiStack[512];static unsigned char* uiFrame=uiStack;
static void __declspec(naked) UiCaptureReturn(){__asm{mov eax,[esp+0x14]} __asm{ret}}
static unsigned __declspec(naked) InvokeUi(){__asm{
 push ebp
 push ebx
 mov ebp,esp
 sub esp,400
 mov edx,uiPointOffset
 lea ecx,[esp+edx-4]
 mov eax,fencePoints
 mov [ecx],eax
 mov eax,fencePoints+4
 mov [ecx+4],eax
 mov eax,fencePoints+8
 mov [ecx+8],eax
 mov dword ptr [esp+0x10],0
 xor ebx,ebx
 mov eax,uiGate
 call dword ptr [uiEntry]
 mov esp,ebp
 pop ebx
 pop ebp
 ret
}}
static int TestPanelMarkers(){
 DWORD protection=0;
 if(!VirtualProtect(panelNative,sizeof(panelNative),PAGE_EXECUTE_READWRITE,&protection) || !VirtualProtect(markerNative,sizeof(markerNative),PAGE_EXECUTE_READWRITE,&protection))return 700;
 *(unsigned*)(panelNative+13)=(unsigned)((UINT_PTR)UiCaptureReturn-((UINT_PTR)panelNative+17));
 *(unsigned*)(markerNative+21)=(unsigned)((UINT_PTR)UiCaptureReturn-((UINT_PTR)markerNative+25));
#ifndef BASELINE_PANELS
 g_uiStageResume[0]=(UINT_PTR)panelNative+12;g_uiStageResume[1]=(UINT_PTR)markerNative+10;
 PatchJump((UINT_PTR)panelNative+4,(void*)UiStageHeightHook0,8,"fixture");
 PatchJump((UINT_PTR)markerNative+2,(void*)UiStageHeightHook1,8,"fixture");
#endif
 FlushInstructionCache((HANDLE)-1,panelNative,sizeof(panelNative));FlushInstructionCache((HANDLE)-1,markerNative,sizeof(markerNative));
 const unsigned ids[]={4,80,81,82,70};const float corners[][2]={{10,10},{14,10},{14,14},{10,14}};
 for(unsigned path=0;path<2;++path){
  uiEntry=path?(UINT_PTR)markerNative:(UINT_PTR)panelNative;uiPointOffset=path?0xa4:0x4c;
  for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<3;++layer)for(unsigned side=0;side<4;++side)for(unsigned enabled=0;enabled<2;++enabled){
   fenceID=ids[id];fenceLayer=layer;uiGate=enabled;
   fencePoints[0]=StagePoint{corners[side][0],corners[side][1],(int)layer};StagePoint before=fencePoints[0];
   float sentinel=9.25f,remaining=0;__asm{fld sentinel}
   unsigned bits=InvokeUi();__asm{fstp remaining}
   if(bits!=(enabled?FloatBits(HeightForId(ids[id])):0))return 701+id+path*10;
   if(remaining!=sentinel || before.x!=fencePoints[0].x || before.y!=fencePoints[0].y || before.layer!=fencePoints[0].layer)return 720;
  }
 }
#ifndef BASELINE_PANELS

#endif
 return 0;
}

// The original footprint's branch and literal EBP-local assignment.
static unsigned char gridNative[]={0x74,7,0xC7,0x45,0xE0,0,0,0x40,0x3F,0xE9,0,0,0,0};
static UINT_PTR gridEntry=(UINT_PTR)gridNative;
static unsigned char gridFrameStorage[160];static unsigned char* gridFrame=gridFrameStorage+80;
static void __declspec(naked) GridCapture(){__asm{mov eax,[ebp-0x20]} __asm{ret}}
static unsigned __declspec(naked) InvokeGrid(){__asm{
 push ebp
 push ebx
 mov ebp,gridFrame
 lea ebx,fencePoints
 mov dword ptr [ebp-0x20],0
 mov eax,uiGate
 test eax,eax
 call dword ptr [gridEntry]
 pop ebx
 pop ebp
 ret
}}
static int TestObjectStageGrid(){
 DWORD protection=0;if(!VirtualProtect(gridNative,sizeof(gridNative),PAGE_EXECUTE_READWRITE,&protection))return 740;
 *(unsigned*)(gridNative+10)=(unsigned)((UINT_PTR)GridCapture-((UINT_PTR)gridNative+14));
#ifndef BASELINE_GRID
 g_uiStageResume[2]=(UINT_PTR)gridNative+9;
 PatchJump((UINT_PTR)gridNative+2,(void*)ObjectStageGridHook,7,"fixture");
#endif
 FlushInstructionCache((HANDLE)-1,gridNative,sizeof(gridNative));
 const unsigned ids[]={4,80,81,82,70};const float corners[][2]={{10,10},{14,10},{14,14},{10,14}};
 for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<3;++layer)for(unsigned side=0;side<4;++side)for(unsigned enabled=0;enabled<2;++enabled){
  fenceID=ids[id];fenceLayer=layer;uiGate=enabled;
  fencePoints[0]=StagePoint{corners[side][0],corners[side][1],(int)layer};StagePoint before=fencePoints[0];
  float sentinel=9.25f,remaining=0;__asm{fld sentinel}
  unsigned bits=InvokeGrid();__asm{fstp remaining}
  if(bits!=(enabled?FloatBits(HeightForId(ids[id])):0))return 741+id;
  if(remaining!=sentinel || before.x!=fencePoints[0].x || before.y!=fencePoints[0].y || before.layer!=fencePoints[0].layer)return 746;
 }
 return 0;
}
static unsigned char bodyNative[]={0xf6,0x43,0xfc,0x10,0x89,0x4c,0x24,0x58,0x89,0x54,0x24,0x5c,0x74,0xa,0xd9,0x85,0x88,0x1,0x0,0x0,0xd8,0x3,0xeb,0x4,0xd9,0x44,0x24,0x60,0x8b,0x4c,0x24,0x4c,0xd9,0x44,0x24,0x58,0xe9,0,0,0,0};

static UINT_PTR bodyEntry=(UINT_PTR)bodyNative;
static unsigned char bodyOwner[0x1a0],bodyNode[0x40],bodyMetadata[32];
static float bodyInputZ=0.75f,bodyInputX=9.95f,bodyInputY=10.5f,bodyOutputZ=0,bodyOutputX=0;
static float __cdecl BodyRaw(StagePoint*){return fenceRawBase;}
static void __declspec(naked) BodyCapture(){__asm{fstp bodyOutputX} __asm{fstp bodyOutputZ} __asm{ret}}
static void __declspec(naked) InvokeBody(){__asm{
 push ebp
 push ebx
 sub esp,128
 lea ebp,bodyOwner
 lea ebx,bodyMetadata+16
 lea eax,bodyNode
 mov [esp+0x14],eax
 mov dword ptr [esp+0x48],0
 mov eax,bodyInputZ
 mov [esp+0x5c],eax
 mov ecx,bodyInputX
 mov edx,bodyInputY
 call dword ptr [bodyEntry]
 add esp,128
 pop ebx
 pop ebp
 ret
}}
static unsigned char bodyUpdateNative[]={0xD8,0x46,8,0x8D,0x54,0x24,0x64,0xC3};
static UINT_PTR bodyUpdateEntry=(UINT_PTR)bodyUpdateNative;
static float bodyUpdateVertex[3],bodyUpdateZ=0;
static void __declspec(naked) InvokeBodyUpdate(){__asm{
 push ebp
 push esi
 sub esp,128
 lea ebp,bodyOwner
 lea esi,bodyUpdateVertex
 lea eax,bodyNode
 mov [esp+0x18],eax
 fldz
 call dword ptr [bodyUpdateEntry]
 fstp bodyUpdateZ
 add esp,128
 pop esi
 pop ebp
 ret
}}
static int TestHalfwallBodies(){
 DWORD protection=0;if(!VirtualProtect(bodyNative,sizeof(bodyNative),PAGE_EXECUTE_READWRITE,&protection))return 760;
 if(!VirtualProtect(bodyUpdateNative,sizeof(bodyUpdateNative),PAGE_EXECUTE_READWRITE,&protection))return 770;
#ifndef BASELINE_BODY_UPDATE
 g_halfwallUpdateResume=(UINT_PTR)bodyUpdateNative+7;
 PatchJump((UINT_PTR)bodyUpdateNative,(void*)HalfwallBodyUpdateHook,7,"fixture-update");
#endif

 *(unsigned*)(bodyNative+37)=(unsigned)((UINT_PTR)BodyCapture-((UINT_PTR)bodyNative+41));
#ifndef BASELINE_HALFWALL
 g_halfwallBodyResume=(UINT_PTR)bodyNative+36;
 PatchJump((UINT_PTR)bodyNative+28,(void*)HalfwallBodyHeightHook,8,"fixture");
#endif

 FlushInstructionCache((HANDLE)-1,bodyNative,sizeof(bodyNative));
 g_baseSurface=(UINT_PTR)BodyRaw;g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;testVtable[0xe8/4]=(UINT_PTR)FenceQuery;
 const unsigned ids[]={4,80,81,82,70};const float corners[][2]={{10,10},{14,10},{14,14},{10,14}};
 for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<7;++layer)for(unsigned side=0;side<4;++side)for(unsigned clipped=0;clipped<2;++clipped){
  fenceID=ids[id];fenceLayer=layer;fenceRawBase=layer==0?0:layer==1?.375f:layer==2?.75f:layer==3?1.125f:layer==4?1.5f:layer==5?3.0f:7.25f;
  StagePoint* edge=(StagePoint*)(bodyNode+0x10);
  edge[0]=StagePoint{corners[side][0],corners[side][1],(int)layer};edge[1]=StagePoint{corners[(side+1)%4][0],corners[(side+1)%4][1],(int)layer};
  StagePoint before[2]={edge[0],edge[1]};
  *(float*)(bodyOwner+0x188)=0.75f;
  *(unsigned*)(bodyMetadata+12)=clipped?0x10:0x9;*(float*)(bodyMetadata+16)=fenceRawBase+1.025f;
  bodyInputZ=fenceRawBase+0.75f;
  // Captured faces lie .05 outside the logical wall line.
  bodyInputX=(edge[0].x+edge[1].x)*0.5f-0.05f;bodyInputY=(edge[0].y+edge[1].y)*0.5f-0.05f;
  float sentinel=9.25f,remaining=0;__asm{fld sentinel}
  InvokeBody();__asm{fstp remaining}
  float expected=fenceRawBase+HeightForId(ids[id])+(clipped?1.025f:0);
  float difference=bodyOutputZ-expected;
  if(!(difference>=-0.00001f && difference<=0.00001f))return 761+id;
   // Replay the later GPU refresh from untouched native source Z. v1.21
   // leaves this at .75/1.775 even though initial construction was corrected.
   bodyUpdateVertex[2]=bodyInputZ+(clipped?1.025f:0);
   __asm{fld sentinel}
   InvokeBodyUpdate();__asm{fstp remaining}
   difference=bodyUpdateZ-expected;
   if(!(difference>=-0.00001f && difference<=0.00001f))return 771+id;
   if(remaining!=sentinel)return 777; float unchanged=bodyInputZ+(clipped?1.025f:0); if(bodyUpdateVertex[2]!=unchanged)return 778;

  if(remaining!=sentinel || bodyOutputX!=bodyInputX || edge[0].x!=before[0].x || edge[1].y!=before[1].y || edge[1].layer!=before[1].layer)return 766;
 }
 // Normal wall clipping groups and malformed/cross-storey edges keep native Z.
 fenceID=81;fenceLayer=0;fenceRawBase=0;bodyInputZ=0.75f;
 StagePoint* edge=(StagePoint*)(bodyNode+0x10);edge[0]=StagePoint{10,10,0};edge[1]=StagePoint{10,11,0};
 *(unsigned*)(bodyMetadata+12)=0x10;*(float*)(bodyMetadata+16)=1.025f;
 *(float*)(bodyOwner+0x188)=-0.01f;InvokeBody();if(bodyOutputZ!=1.015f)return 767;
 *(float*)(bodyOwner+0x188)=0.95f;InvokeBody();if(bodyOutputZ<1.97499f || bodyOutputZ>1.97501f)return 768;
 *(float*)(bodyOwner+0x188)=0.75f;edge[1].layer=1;InvokeBody();if(bodyOutputZ!=1.775f)return 769;
 return 0;
}
static unsigned char visibleNative[]={0x8b,0x44,0x24,0xc,0x8b,0x49,0x7c,0x6b,0xc0,0x7c,0x8b,0x54,0x24,0x8,0x53,0x55,0x8b,0x6c,0x8,0x6c,0x8b,0x42,0x8,0x8b,0x58,0x8,0x8d,0x6c,0x6d,0x0,0xc1,0xe5,0x4,0x3b,0xd8,0x89,0x44,0x24,0x14,0x74,0x5a,0x56,0x57,0xeb,0x3,0x8d,0x49,0x0,0x8b,0x73,0x28,0x8b,0x56,0x4,0x8b,0x4e,0x8,0x2b,0xca,0x8b,0x76,0x4,0xb8,0xab,0xaa,0xaa,0x2a,0xf7,0xe9,0xc1,0xfa,0x3,0x8b,0xc2,0xc1,0xe8,0x1f,0x3,0xc2,0x8b,0x54,0x24,0x14,0x8d,0x4,0x40,0xc1,0xe0,0x4,0x8d,0x3c,0x2a,0x8b,0xc8,0x8b,0xd1,0xc1,0xe9,0x2,0xf3,0xa5,0x8b,0xca,0x83,0xe1,0x3,0x53,0xf3,0xa4,0x3,0xe8,0xe8,0x9c,0xce,0x41,0x0,0x8b,0xd8,0x8b,0x44,0x24,0x20,0x83,0xc4,0x4,0x3b,0xd8,0x75,0xaf,0x5f,0x5e,0x5d,0x5b,0xc2};

static unsigned char visibleOwner[0x1a0],visibleSlots[0x100],visibleGroup[16],visibleHead[32],visibleNodes[2][64],visibleMesh[2][32];
static unsigned char visibleSource[2][48*10],visibleDestination[48*24];
static unsigned __cdecl VisibleNext(unsigned node){return node==(unsigned)visibleNodes[0]?(unsigned)visibleNodes[1]:(unsigned)visibleHead;}
static int TestVisibleWallCopy(){
 DWORD protection=0;
 if(!VirtualProtect(visibleNative,sizeof(visibleNative),PAGE_EXECUTE_READWRITE,&protection))return 780;
 *(unsigned*)(visibleNative+0x70)=(unsigned)((UINT_PTR)VisibleNext-((UINT_PTR)visibleNative+0x74));
 FlushInstructionCache((HANDLE)-1,visibleNative,sizeof(visibleNative));
 g_wallVisibleCopy=(UINT_PTR)visibleNative;g_wallNextNode=(UINT_PTR)VisibleNext;
 g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)BodyRaw;testVtable[0xe8/4]=(UINT_PTR)FenceQuery;
 *(unsigned*)(visibleOwner+0x7c)=(unsigned)visibleSlots;*(unsigned*)(visibleSlots+0x6c)=3;
 *(unsigned*)(visibleGroup+8)=(unsigned)visibleHead;*(unsigned*)(visibleHead+8)=(unsigned)visibleNodes[0];
 for(unsigned k=0;k<2;++k){
  *(unsigned*)(visibleNodes[k]+0x28)=(unsigned)visibleMesh[k];
  *(unsigned*)(visibleMesh[k]+4)=(unsigned)visibleSource[k];
  *(unsigned*)(visibleMesh[k]+8)=(unsigned)(visibleSource[k]+(k?10:6)*48);
 }
 const unsigned ids[]={4,80,81,82,70};
 for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<7;++layer)for(unsigned side=0;side<4;++side)for(unsigned fallback=0;fallback<3;++fallback){
  fenceID=ids[id];fenceLayer=layer;fenceRawBase=layer==0?0:layer==1?0.375f:6;
  *(float*)(visibleOwner+0x188)=fallback==1?-0.01f:.75f;
  for(unsigned k=0;k<2;++k){
   StagePoint* edge=(StagePoint*)(visibleNodes[k]+0x10);
   edge[0]=StagePoint{10,10,(int)layer};edge[1]=StagePoint{side%2?10.0f:14.0f,side%2?14.0f:10.0f,(int)layer+(fallback==2?1:0)};
   if(side>=2){StagePoint temp=edge[0];edge[0]=edge[1];edge[1]=temp;}
   for(unsigned v=0;v<(k?10u:6u);++v){
    unsigned char* p=visibleSource[k]+v*48;
    for(unsigned b=0;b<48;++b)p[b]=(unsigned char)(b+v+k);
    *(float*)(p+8)=fenceRawBase+.75f+(v%2?1.025f:0);
   }
  }
  for(unsigned b=0;b<sizeof(visibleDestination);++b)visibleDestination[b]=0xCD;
  for(unsigned repeat=0;repeat<2;++repeat){
   float sentinel=14.25f,remaining=0;__asm{fld sentinel}
#ifdef BASELINE_VISIBLE_WALL
   ((void(__thiscall*)(void*,unsigned,unsigned,unsigned))g_wallVisibleCopy)(visibleOwner,(unsigned)visibleDestination,(unsigned)visibleGroup,0);
#else
   CopyVisibleStageWall((unsigned)visibleOwner,0,(unsigned)visibleDestination,(unsigned)visibleGroup,0);
#endif
   __asm{fstp remaining}
   if(remaining!=sentinel)return 781;
   unsigned cursor=3*48;
   float delta=(!fallback && id>=1 && id<=3)?HeightForId(ids[id])-.75f:0;
   for(unsigned k=0;k<2;++k)for(unsigned v=0;v<(k?10u:6u);++v){
    unsigned char* src=visibleSource[k]+v*48;unsigned char* dst=visibleDestination+cursor;
    float expected=*(float*)(src+8)+delta,difference=*(float*)(dst+8)-expected;
    if(!(difference>=-.00001f && difference<=.00001f))return 782+id;
    float unchanged=fenceRawBase+.75f+(v%2?1.025f:0);
    if(*(float*)(src+8)!=unchanged)return 787;
    for(unsigned b=0;b<48;++b)if((b<8 || b>=12) && dst[b]!=src[b])return 788;
    cursor+=48;
   }
   for(unsigned b=0;b<3*48;++b)if(visibleDestination[b]!=0xCD)return 789;
   for(unsigned b=19*48;b<sizeof(visibleDestination);++b)if(visibleDestination[b]!=0xCD)return 790;
  }
 }
 return 0;
}
static unsigned char canonicalMetadata[2][280],canonicalGroupHead[16],canonicalGroup[48];
static unsigned canonicalBuildCalls=0;
static unsigned __cdecl CanonicalNext(unsigned node){
 if(node==(unsigned)canonicalGroup)return (unsigned)canonicalGroupHead;
 return VisibleNext(node);
}
static void __fastcall CanonicalBuildFake(void*,void*){++canonicalBuildCalls;}
static int TestCanonicalHalfwall(){
 g_wallSourceBuild=(UINT_PTR)CanonicalBuildFake;g_wallSourceNext=(UINT_PTR)CanonicalNext;
 *(unsigned*)(visibleOwner+0x134)=(unsigned)canonicalGroupHead;
 *(unsigned*)(visibleOwner+0x140)=(unsigned)canonicalGroupHead;
 *(unsigned*)(canonicalGroupHead+8)=(unsigned)canonicalGroup;
 *(unsigned*)(canonicalGroup+0x24)=(unsigned)visibleHead;
 const unsigned ids[]={4,80,81,82,70};
 for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<7;++layer)for(unsigned side=0;side<6;++side)for(unsigned fallback=0;fallback<4;++fallback){
  fenceID=ids[id];fenceLayer=layer;fenceRawBase=layer==0?0:layer==1?.375f:layer==2?.75f:layer==3?1.125f:layer==4?1.5f:layer==5?3.0f:7.25f;
  *(float*)(visibleOwner+0x188)=(fallback==1?-.01f:.75f);
  for(unsigned k=0;k<2;++k){
   StagePoint* edge=(StagePoint*)(visibleNodes[k]+0x10);
   edge[0]=StagePoint{10,10,(int)layer};edge[1]=StagePoint{side%3==1?10.0f:14.0f,side%3==0?10.0f:14.0f,(int)layer+(fallback==2?1:0)};
   if(side>=3){StagePoint t=edge[0];edge[0]=edge[1];edge[1]=t;}
   *(unsigned*)(visibleMesh[k]+0x10)=(unsigned)canonicalMetadata[k];
   for(unsigned v=0;v<(k?10u:6u);++v){
    *(unsigned*)(canonicalMetadata[k]+v*28+12)=v%2?0x11:9;
    *(float*)(canonicalMetadata[k]+v*28+16)=fenceRawBase+1.025f;
    *(float*)(visibleSource[k]+v*48+8)=fenceRawBase+.75f+(v%2?1.025f:0)+(fallback==3?.2f:0);
   }
  }
  bool corrected=!fallback && id>=1 && id<=3;
  for(unsigned repeat=0;repeat<3;++repeat){
   unsigned before=canonicalBuildCalls;
#ifdef BASELINE_CANONICAL_WALL
   CanonicalBuildFake(visibleOwner,0);
#else
   BuildCanonicalHalfwall((unsigned)visibleOwner,0);
#endif
   if(canonicalBuildCalls!=before+1)return 800;
   // Replay the native visible copier from the canonical source. This is
   // also the source read by the game's active dynamic material stream.
   ((void(__thiscall*)(void*,unsigned,unsigned,unsigned))g_wallVisibleCopy)(visibleOwner,(unsigned)visibleDestination,(unsigned)visibleGroup,0);
   unsigned cursor=3*48;
   for(unsigned k=0;k<2;++k)for(unsigned v=0;v<(k?10u:6u);++v){
    float target=fenceRawBase+(corrected?HeightForId(ids[id]):.75f)+(v%2?1.025f:0)+(fallback==3?.2f:0);
    float actual=*(float*)(visibleSource[k]+v*48+8),difference=actual-target;
    if(!(difference>=-.00001f && difference<=.00001f))return 801+id;
    if(*(float*)(visibleDestination+cursor+8)!=actual)return 806;
    // The initial auxiliary path must not shift a corrected bottom twice;
    // clipped top vertices still start from the native owner plane.
    StagePoint* edge=(StagePoint*)(visibleNodes[k]+0x10);
    float chosen=v%2?fenceRawBase+.75f+1.025f:actual;
    unsigned bits=HalfwallInitialDeltaBits((unsigned)visibleOwner,edge,chosen,v%2?0x11:9);
    if(corrected){float result=chosen+*(float*)&bits;float d=result-target;if(!(d>=-.00001f && d<=.00001f))return 807;}
    for(unsigned b=0;b<48;++b)if((b<8 || b>=12) && visibleDestination[cursor+b]!=visibleSource[k][v*48+b])return 808;
    cursor+=48;
   }
  }
 }
 return 0;
}
static unsigned char upperNative[]={0x83,0xec,0x24,0x53,0x8b,0x5c,0x24,0x30,0x8b,0x3,0x8b,0x53,0x8,0x56,0x57,0x8b,0xf9,0x8b,0x4b,0x4,0x89,0x44,0x24,0x24,0x8b,0x87,0x8c,0x1,0x0,0x0,0x89,0x4c,0x24,0x28,0x50,0x8d,0x4c,0x24,0x28,0x51,0x89,0x54,0x24,0x34,0xe8,0x3f,0xbb,0xff,0xff,0xd9,0x43,0x10,0x8b,0x44,0x24,0x44,0xd8,0xe1,0x83,0xc4,0x8,0x85,0xc0,0xd9,0x5c,0x24,0x38,0x75,0x6,0xd9,0x44,0x24,0x38,0xeb,0x6,0xd9,0x87,0x84,0x1,0x0,0x0,0x8a,0x44,0x24,0x40,0xd9,0x5,0x18,0x3a,0x1f,0x1,0x84,0xc0,0xd8,0xc9,0x74,0xc,0xd9,0x44,0x24,0x38,0xd8,0xd,0x18,0x3a,0x1f,0x1,0xeb,0x6,0xd9,0x5,0xac,0x74,0x1d,0x1,0x8b,0x74,0x24,0x34,0xd8,0xe1,0x8a,0x44,0x24,0x44,0x84,0xc0,0xd9,0x5c,0x24,0x40,0x8b,0x54,0x24,0x40,0xdd,0xd8,0xd8,0x87,0x88,0x1,0x0,0x0,0xd8,0xc1,0xd9,0x5e,0x8,0x89,0x56,0x24,0xdd,0xd8,0xf,0x85,0xa2,0x0,0x0,0x0,0x8b,0x8f,0x7c,0x1,0x0,0x0,0x85,0xc9,0xf,0x84,0x8d,0x0,0x0,0x0,0x83,0x7b,0x14,0xff,0xf,0x84,0x83,0x0,0x0,0x0,0xc7,0x44,0x24,0x18,0x0,0x0,0x0,0x0,0x8b,0x44,0x24,0x18,0x89,0x44,0x24,0xc,0xd9,0x44,0x24,0xc,0xd8,0x6,0xc7,0x44,0x24,0x1c,0x0,0x0,0x0,0x0,0x8b,0x54,0x24,0x1c,0x89,0x54,0x24,0x10,0xd9,0x5c,0x24,0xc,0x8b,0x54,0x24,0xc,0xd9,0x44,0x24,0x10,0xc7,0x44,0x24,0x20,0x0,0x0,0x0,0x0,0xd8,0x46,0x4,0x8b,0x44,0x24,0x20,0x89,0x44,0x24,0x14,0x89,0x54,0x24,0x18,0xd9,0x5c,0x24,0x10,0x8b,0x44,0x24,0x10,0xd9,0x44,0x24,0x14,0x89,0x44,0x24,0x1c,0xd8,0x46,0x8,0x8d,0x44,0x24,0x18,0x50,0xd9,0x5c,0x24,0x18,0x8b,0x54,0x24,0x18,0x89,0x54,0x24,0x24,0x8b,0x1,0x33,0xd2,0x66,0x8b,0x53,0x14,0x52,0xff,0x90,0xa4,0x0,0x0,0x0,0x8b,0xc8,0xe8,0x26,0x24,0xc3,0xff,0x8b,0x4c,0x24,0x40,0x89,0x4e,0x1c,0x5f,0x5e,0x5b,0x83,0xc4,0x24,0xc2};

static float upperHalf=.5f,upperZero=0;
static unsigned char upperOwner[0x1a0],upperVertex[48],upperMeta[28];
static int TestUpperRefresh(){
 DWORD old=0;if(!VirtualProtect(upperNative,sizeof(upperNative),PAGE_EXECUTE_READWRITE,&old))return 820;
 *(unsigned*)(upperNative+0x2d)=(unsigned)((UINT_PTR)BodyRaw-((UINT_PTR)upperNative+0x31));
 *(unsigned*)(upperNative+0x57)=(unsigned)&upperHalf;
 *(unsigned*)(upperNative+0x67)=(unsigned)&upperHalf;
 *(unsigned*)(upperNative+0x6f)=(unsigned)&upperZero;
#ifndef BASELINE_UPPER_REFRESH
 g_halfwallVertexResume=(UINT_PTR)upperNative+0x97;
 PatchJump((UINT_PTR)upperNative+0x91,(void*)HalfwallVertexRefreshHook,6,"upper-refresh-fixture");
#endif
 FlushInstructionCache((HANDLE)-1,upperNative,sizeof(upperNative));
 g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)BodyRaw;testVtable[0xe8/4]=(UINT_PTR)FenceQuery;
 typedef void(__thiscall*Fn)(void*,void*,void*,unsigned,unsigned,unsigned,unsigned,unsigned);
 const unsigned ids[]={4,80,81,82,70};
 for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<7;++layer)for(unsigned flags=0;flags<4;++flags)for(unsigned fallback=0;fallback<2;++fallback){
  fenceID=ids[id];fenceLayer=layer;fenceRawBase=layer==0?0:layer==1?.375f:layer==2?.75f:layer==3?1.125f:layer==4?1.5f:layer==5?3.0f:7.25f;
  *(float*)(upperOwner+0x188)=fallback?-.01f:.75f;*(float*)(upperOwner+0x184)=.2f;
  *(StagePoint*)upperMeta=StagePoint{12,12,(int)layer};*(float*)(upperMeta+16)=fenceRawBase+1.025f;
  *(float*)upperVertex=12;*(float*)(upperVertex+4)=12;
  for(unsigned repeat=0;repeat<3;++repeat){
   float sentinel=13.25f,remaining=0;__asm{fld sentinel}
   ((Fn)(UINT_PTR)upperNative)(upperOwner,upperVertex,upperMeta,(flags>>1)&1,flags&1,1,0,0);
   __asm{fstp remaining}
   float native=*(float*)(upperOwner+0x188)+((flags&2)?fenceRawBase+.2f:fenceRawBase+1.025f);
   float target=native+(!fallback && id>=1 && id<=3?HeightForId(ids[id])-.75f:0);
   float d=*(float*)(upperVertex+8)-target;
   if(!(d>=-.00001f && d<=.00001f))return 821+id;
   if(remaining!=sentinel || *(float*)upperVertex!=12 || *(float*)(upperVertex+4)!=12)return 826;
  }
 }
 return 0;
}

static unsigned char cursorNative[]={0x55,0x8b,0xec,0x83,0xec,0x20,0x83,0x65,0xfc,0x0,0x53,0x57,0x8b,0x7d,0x8,0x8d,0x45,0xf0,0x57,0x50,0x8b,0xd9,0xe8,0x7,0xf,0x62,0x0,0x83,0x7f,0x8,0x0,0x59,0x59,0x7d,0x53,0x56,0xe8,0xe5,0x77,0xea,0xff,0x8b,0x10,0x8b,0xc8,0xff,0x92,0x94,0x0,0x0,0x0,0x8b,0xf0,0x85,0xf6,0x74,0x21,0x8d,0x45,0xe0,0x57,0x50,0xe8,0x1f,0x7,0x62,0x0,0x50,0xe8,0x29,0xa5,0x61,0x0,0x83,0xc4,0xc,0x84,0xc0,0x74,0xa,0x8b,0x6,0x57,0x8b,0xce,0xff,0x50,0x44,0xeb,0x15,0xe8,0xaf,0x77,0xea,0xff,0x8b,0x10,0x8b,0xc8,0xff,0x92,0x7c,0x1,0x0,0x0,0xd8,0x25,0xa4,0xc1,0x1f,0x1,0xd8,0x45,0xf8,0x5e,0xd9,0x5d,0xf8,0x8d,0x45,0xe4,0x57,0x50,0xe8,0x2,0xd,0x62,0x0,0x50,0xe8,0x9c,0x9b,0x61,0x0,0x83,0xc4,0xc,0x84,0xc0,0x74,0xc,0xd9,0x45,0xf8,0xd8,0x5,0xb4,0xc1,0x1f,0x1,0xd9,0x5d,0xf8,0xff,0x73,0x68,0x8b,0x3,0x8d,0x4d,0xf0,0x51,0x8b,0xcb,0xff,0x50,0x68,0x5f,0x5b,0xc9,0xc2,0x4,0x0};
static unsigned char cursorTool[0x80];static UINT_PTR cursorVtable[40];
static float cursorOutput[3];static unsigned cursorWrites=0;
static void* __cdecl CursorConvert(float* out,StagePoint* point){out[0]=point->x;out[1]=point->y;out[2]=fenceRawBase;return out;}
static void* __cdecl CursorGrid(StageGrid* out,StagePoint* point){out->x=10;out->y=10;out->layer=point->layer;return out;}
static unsigned __cdecl CursorStage(StageGrid*){return fenceID==4 || (fenceID>=80 && fenceID<=82);}
static void __fastcall CursorCapture(void*,void*,const float* point,unsigned cursor){
    ++cursorWrites;for(unsigned i=0;i<3;++i)cursorOutput[i]=point[i];
}
static int TestWallCursor(){
 DWORD old=0;if(!VirtualProtect(cursorNative,sizeof(cursorNative),PAGE_EXECUTE_READWRITE,&old))return 840;
 *(unsigned*)(cursorNative+0x17)=(unsigned)((UINT_PTR)CursorConvert-((UINT_PTR)cursorNative+0x1b));
 *(unsigned*)(cursorNative+0x7c)=(unsigned)((UINT_PTR)CursorGrid-((UINT_PTR)cursorNative+0x80));
 *(unsigned*)(cursorNative+0x82)=(unsigned)((UINT_PTR)CursorStage-((UINT_PTR)cursorNative+0x86));
 static float nativeHeight=.75f;*(unsigned*)(cursorNative+0x92)=(unsigned)&nativeHeight;
#ifndef BASELINE_WALL_CURSOR
 g_wallCursorResume=(UINT_PTR)cursorNative+0x96;
 PatchJump((UINT_PTR)cursorNative+0x90,(void*)WallCursorHeightHook,6,"cursor-fixture");
#endif
 FlushInstructionCache((HANDLE)-1,cursorNative,sizeof(cursorNative));
 *(UINT_PTR*)cursorTool=(UINT_PTR)cursorVtable;cursorVtable[0x68/4]=(UINT_PTR)CursorCapture;
 g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;testVtable[0xe8/4]=(UINT_PTR)FenceQuery;
 typedef void(__thiscall*Fn)(void*,StagePoint*);
 const unsigned ids[]={4,80,81,82,70};
 for(unsigned id=0;id<5;++id)for(unsigned layer=0;layer<7;++layer)for(unsigned side=0;side<4;++side){
  fenceID=ids[id];fenceLayer=layer;fenceRawBase=layer==0?0:layer==1?.375f:layer==2?.75f:layer==3?1.125f:layer==4?1.5f:layer==5?3:7.25f;
  StagePoint point={side&1?14.0f:10.0f,side&2?14.0f:10.0f,(int)layer};StagePoint before=point;
  float sentinel=13.25f,remaining=0;unsigned writes=cursorWrites;__asm{fld sentinel}
  ((Fn)(UINT_PTR)cursorNative)(cursorTool,&point);__asm{fstp remaining}
  float target=fenceRawBase+(id<4?HeightForId(ids[id]):0),d=cursorOutput[2]-target;
  if(!(d>=-.00001f && d<=.00001f))return 841+id;
  if(remaining!=sentinel || cursorWrites!=writes+1 || cursorOutput[0]!=point.x || cursorOutput[1]!=point.y || point.x!=before.x || point.y!=before.y || point.layer!=before.layer)return 846;
 }
 return 0;
}
static int FinalSurfaceTests(){int r=TestShadowRaster();if(r)return r;r=TestCpuConsistency();if(r)return r;r=TestLayerOwner();if(r)return r;r=TestLandingDirections();if(r)return r;r=TestTwoStepFloors();if(r)return r;r=TestDiagonalWallCorners();if(r)return r;r=TestStageJoins();if(r)return r;r=TestFenceHeights();if(r)return r;r=TestFenceModels();if(r)return r;r=TestPanelMarkers();if(r)return r;r=TestObjectStageGrid();if(r)return r;r=TestWallCursor();return r?r:((r=TestHalfwallBodies())?r:((r=TestVisibleWallCopy())?r:((r=TestCanonicalHalfwall())?r:TestUpperRefresh())));}

#include "cpu-shadow-loops.h"
extern "C" int __cdecl TestStart()
{
    testVtable[0x144/4]=(UINT_PTR)Low;testVtable[0x148/4]=(UINT_PTR)Low;
    testVtable[0x14C/4]=(UINT_PTR)High;testVtable[0x150/4]=(UINT_PTR)High;
    testVtable[0x160/4]=(UINT_PTR)Low;testVtable[0x164/4]=(UINT_PTR)FloorHigh;
    testVtable[0xE8/4]=(UINT_PTR)QueryFake;
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)BaseFake;
    *(unsigned*)(testMesh+0x1C)=(unsigned)testVertices;
    *(unsigned*)(testMesh+0x20)=(unsigned)(testVertices+24);
    for(unsigned k=0;k<24;++k){testVertices[k][0]=10.0f+(k%3==2?0.5f:0);testVertices[k][1]=10.0f+(k%3==0?1.0f:k%3==2?0.5f:0);}
    for(unsigned id=80;id<=82;++id) {
        testId=id;g_cachedMesh=0;
        if(FloorHeightBits((unsigned)testMesh,0)!=FloatBits(HeightForId(id)))return 10+id;
        StagePoint p={10.5f,10.5f,0};if(WallHeightBits(&p)!=FloatBits(HeightForId(id)))return 20+id;
    }
    // The raw height is terrain elevation, independent of the floor index.
    testBase=12.0f;testLayer=0;testId=81;
    for(unsigned k=0;k<24;++k)testVertices[k][2]=testBase;
    g_cachedMesh=0;if(FloorHeightBits((unsigned)testMesh,0)!=0x3FC00000)return 30;
    testLayer=2;g_cachedMesh=0;if(FloorHeightBits((unsigned)testMesh,0)!=0x3FC00000)return 31;
    // A mismatch must retain native geometry rather than assign a guessed layer.
    testBase=13.0f;g_cachedMesh=0;if(FloorHeightBits((unsigned)testMesh,0)!=0x3F400000)return 32;
    testLayer=0;testBase=0;for(unsigned k=0;k<24;++k)testVertices[k][2]=0;
    boundary=true;StagePoint corner={10.0f,10.0f,0};if(WallHeightBits(&corner)!=0x3F400000)return 33;boundary=false;
    // Execute the actual compiled hooks with a nonempty x87 stack.
    UINT_PTR hooks[5]={(UINT_PTR)FloorHeightHook0,(UINT_PTR)FloorHeightHook1,(UINT_PTR)FloorHeightHook2,(UINT_PTR)FloorHeightHook3,(UINT_PTR)FloorHeightHook4};
    g_floorResume0=g_floorResume1=g_floorResume2=g_floorResume3=g_floorResume4=(UINT_PTR)ReturnOnly;
    float sentinel=3.25f,result=0,remaining=0;
    for(unsigned k=0;k<5;++k){
        UINT_PTR hook=hooks[k];g_cachedMesh=0;
        __asm {
            lea ebx,testMesh
            lea esi,testVertices
            xor ecx,ecx
            fld sentinel
            call dword ptr [hook]
            fstp result
            fstp remaining
        }
        if(result!=1.5f || remaining!=3.25f)return 40+k;
    }
    g_wallResume=(UINT_PTR)WallReturn;
    StagePoint wallPoint={10.5f,10.5f,0};float raw=1.0f;
    __asm {
        fld sentinel
        fld raw
        lea eax,afterWall
        push eax
        // Reproduce A2BD44 layout after C23340 has overwritten its argument.
        // [S]=floor number, [S+0x30]=original caller coordinate pointer.
        lea eax,wallPoint
        sub esp,48
        mov [esp+44],eax
        push 0
        jmp WallHeightHook
    afterWall:
        fstp result
        fstp remaining
    }
    if(result!=2.5f || remaining!=3.25f)return 50;
    typedef unsigned (__cdecl *CategoryFn)(unsigned*,unsigned);
    unsigned custom=81;
    if(!((CategoryFn)CategoryHook)(&custom,4) || ((CategoryFn)CategoryHook)(&custom,2))return 51;
    g_surfaceResume0=g_surfaceResume2=(UINT_PTR)SurfaceReturn;
    UINT_PTR supportHooks[2]={(UINT_PTR)SurfaceHeightHook0,(UINT_PTR)SurfaceHeightHook2};
    for(unsigned k=0;k<2;++k)for(unsigned variant=0;variant<4;++variant) {
        unsigned id=variant==0 ? 4 : 79+variant;
        float expected=1.0f+HeightForId(id);UINT_PTR hook=supportHooks[k];
        __asm {
            fld sentinel
            fld raw
            lea eax,afterSupport
            push eax
            sub esp,16
            mov eax,id
            mov [esp+12],eax
            jmp dword ptr [hook]
        afterSupport:
            fstp result
            fstp remaining
        }
        if(result!=expected || remaining!=3.25f)return 60+variant+k*4;
    }
    g_surfaceResume1=(UINT_PTR)EdgeReturn;
    __asm {
        fld sentinel
        fld raw
        lea eax,afterEdge
        push eax
        sub esp,32
        // Surface helper has overwritten its temporary argument with layer 0.
        mov dword ptr [esp],0
        mov dword ptr [esp+20],0x41280000
        mov dword ptr [esp+24],0x41280000
        mov dword ptr [esp+28],0
        jmp SurfaceHeightHook1
    afterEdge:
        fstp result
        fstp remaining
    }
    if(result!=2.5f || remaining!=3.25f)return 70;
    unsigned stairFrameStorage[256]={0};
    unsigned frame=(unsigned)(stairFrameStorage+128);
    StageGrid startGrid={10,10,0,15,{0,0,0}};StagePoint targetPoint={12.5f,10.5f,0};
    *(StageGrid**)(frame+0xC)=&startGrid;*(StagePoint**)(frame+0x18)=&targetPoint;
    g_stairContextResume=(UINT_PTR)ContextReturn;
    g_stairStartResume=g_stairPreviewResume=(UINT_PTR)ReturnOnly;
    g_terrainClassResume=(UINT_PTR)TerrainNo;g_terrainClassMatch=(UINT_PTR)TerrainYes;
    for(unsigned variant=0;variant<4;++variant){
        testId=variant==0 ? 4 : 79+variant;
        SetStairHeight(frame);
        if(g_stairHeightBits!=FloatBits(HeightForId(testId)))return 80+variant;
        float expected=raw+HeightForId(testId);
        __asm {
            fld sentinel
            fld raw
            lea ebx,startGrid
            call StairPreviewHook
            fstp result
            fstp remaining
        }
        if(result!=expected || remaining!=sentinel)return 84+variant;
        unsigned preserved=0;
        __asm {
            push ebp
            mov ebp,frame
            mov eax,0x12345678
            call StairStartHook
            pop ebp
            mov preserved,eax
        }
        if(*(unsigned*)(frame+0x14)!=g_stairHeightBits || preserved!=0x12345678)return 88+variant;
        unsigned id=testId;
        __asm {mov eax,id} __asm {call StairTerrainClassHook}
        if(terrainBranch!=1)return 92+variant;
        // Replay the context hook's native pushed argument on a real stack.
        g_stairHeightBits=0;
        __asm {
            fld sentinel
            push ebp
            mov ebp,frame
            call StairContextHook
            pop ebp
            fstp remaining
        }
        if(g_stairHeightBits!=FloatBits(HeightForId(testId)) || remaining!=sentinel)return 96+variant;
    }
    __asm {mov eax,70} __asm {call StairTerrainClassHook}
    if(terrainBranch!=0)return 100;
    *(StageGrid*)(frame+0x0C)=startGrid;
    g_stairFinderResume=(UINT_PTR)ReturnOnly;
    for(unsigned variant=0;variant<4;++variant){
        testId=variant==0 ? 4 : 79+variant;
        float expected=raw+HeightForId(testId);
        __asm {
            fld sentinel
            fld raw
            push ebp
            mov ebp,frame
            call StairFinderHook
            pop ebp
            fstp result
            fstp remaining
        }
        if(result!=expected || remaining!=sentinel)return 104+variant;
    }
    
    UINT_PTR cornerHooks[4]={(UINT_PTR)CornerHook0,(UINT_PTR)CornerHook1,(UINT_PTR)CornerHook2,(UINT_PTR)CornerHook3};
    unsigned pointOffsets[4]={0xE8,0x10C,0xA8,0xCC};unsigned heightOffsets[4]={0xD4,0xDC,0xD8,0xD0};
    g_cornerResume0=g_cornerResume1=g_cornerResume2=g_cornerResume3=(UINT_PTR)ReturnOnly;
    for(unsigned k=0;k<4;++k){
        StageVertex* point=(StageVertex*)(frame-pointOffsets[k]);*point=StageVertex{10,10,0};UINT_PTR hook=cornerHooks[k];
        for(unsigned variant=0;variant<4;++variant){
            testId=variant==0 ? 4 : 79+variant;
            __asm {
                fld sentinel
                mov edx,hook
                push ebp
                mov ebp,frame
                call edx
                pop ebp
                fstp remaining
            }
            if(*(unsigned*)(frame-heightOffsets[k])!=FloatBits(HeightForId(testId)) || remaining!=sentinel)return 110+k*4+variant;
        }
    }
    
    g_landingSurface=(UINT_PTR)SurfaceFake;
    *(StageGrid**)(frame+0xC)=&startGrid;*(StagePoint**)(frame+0x18)=&targetPoint;
    testBase=0;testId=82;startGrid.layer=1;*(float*)(frame-0xB4)=3.0f;
    SetLandingContext(frame);
    if(!g_upperStage || g_lowerSurface!=2.25f || g_landingTerrainDeduct!=2.25f)return 130;
    *(StagePoint*)(frame-0x30)=targetPoint;*(unsigned*)(frame-0x34)=3;*(float*)(frame+0x14)=2.25f;*(unsigned*)(frame-0x50)=16;
    MatchLanding(frame);
    if(!g_landingMatched || *(unsigned*)(frame-0x50)!=4)return 131;
    // Context also accepts a drop between two stages on the same storey.
    testId=81;startGrid.layer=0;*(float*)(frame-0xB4)=2.25f;SetLandingContext(frame);
    if(!g_upperStage || g_lowerSurface!=1.5f)return 132;
    *(unsigned*)(frame-0x34)=1;*(float*)(frame+0x14)=1.5f;*(unsigned*)(frame-0x50)=8;MatchLanding(frame);
    if(!g_landingMatched || *(unsigned*)(frame-0x50)!=2)return 133;
    // Original lower-floor packet must survive upper-floor validation.
    
    UINT_PTR fakeCreateTable[26]={0};fakeCreateTable[0x64/4]=(UINT_PTR)CreateFake;
    g_originalPlacement=(UINT_PTR)PlacementFake;g_upperStage=1;g_lowerSurface=1.5f;g_upperFloor=1;g_lowerFloor=0;g_upperValidation=0;
    StagePoint objectPos={10.5f,10.5f,0};unsigned height=FloatBits(2.0f);
    requiredFloor=1;
    if(!LandingCreate(1,(unsigned)fakeCreateTable,1,(unsigned)&objectPos,0,height,0,0,0,0) || checkedFloor!=1 || storedFloor!=0 || objectPos.layer!=0 || g_upperValidation!=0)return 134;
    g_upperFloor=0;requiredFloor=0;
    if(!LandingCreate(1,(unsigned)fakeCreateTable,1,(unsigned)&objectPos,0,height,0,0,0,0) || checkedFloor!=0 || storedFloor!=0)return 135;
    
    unsigned char* fixture=(unsigned char*)FinderExitFixture;unsigned offset=0;
    while(offset<48 && !(fixture[offset]==0x83 && fixture[offset+1]==0x4D && fixture[offset+2]==0xFC && fixture[offset+3]==0xFF && fixture[offset+4]==0xFF && fixture[offset+5]==0x25))++offset;
    if(offset>=48)return 140;
    g_finderExitSlot=(UINT_PTR)&finderFixtureTarget;
    PatchJump((UINT_PTR)(fixture+offset),(void*)FinderRejectHook,10,"test_shared_exit");
    *(StageGrid*)(frame+0xC)=startGrid;

    for(unsigned success=0;success<2;++success){
        unsigned savedEax=0,savedEbx=0;*(unsigned*)(frame-4)=0;
        __asm {
            fld sentinel
            mov edi,success
            push ebp
            mov ebp,frame
            mov eax,0x12345678
            mov ebx,0xAABBCC01
            call FinderExitFixture
            pop ebp
            mov savedEax,eax
            mov savedEbx,ebx
            fstp remaining
        }
        if(savedEax!=0x12345678 || savedEbx!=(0xAABBCC00|success) || *(unsigned*)(frame-4)!=~0u || remaining!=sentinel)return 141+success;
    }

    g_coreRejectResume=(UINT_PTR)ReturnOnly;
    unsigned resultEax=0,resultEcx=0;
    __asm {
        fld sentinel
        push ebp
        mov ebp,frame
        call CoreRejectHook
        pop ebp
        mov resultEax,eax
        mov resultEcx,ecx
        fstp remaining
    }
    if(resultEax!=frame-0x4C || resultEcx!=*(unsigned*)(frame+0x24) || remaining!=sentinel)return 150;
    // Stage-edge case: last anchor is over the gap, next quarter-step reaches8.
    landingEdgeTest=true;testId=81;testBase=0;testLayer=0;
    g_landingSurface=(UINT_PTR)SurfaceFake;g_upperStage=1;g_upperFloor=g_lowerFloor=0;g_landingTargetId=81;g_lowerSurface=1.5f;
    *(StagePoint*)(frame-0x30)=StagePoint{10.75f,10.5f,0};*(float*)(frame-0x58)=0.25f;*(float*)(frame-0x54)=0;
    *(unsigned*)(frame-0x34)=3;*(float*)(frame+0x14)=1.5f;*(unsigned*)(frame-0x50)=8;g_landingMatched=0;
    MatchLanding(frame);if(!g_landingMatched || *(unsigned*)(frame-0x50)!=4)return 160;
    // A longer empty gap must not be accepted.
    ((StagePoint*)(frame-0x30))->x=10.5f;*(unsigned*)(frame-0x50)=8;g_landingMatched=0;
    MatchLanding(frame);if(g_landingMatched || *(unsigned*)(frame-0x50)!=8)return 161;
    // Reaching another stage ID with the same height does not count.
    ((StagePoint*)(frame-0x30))->x=10.75f;g_landingTargetId=82;
    MatchLanding(frame);if(g_landingMatched)return 162;
    // Cross-storey flights now accept the same validated one-step edge.
    g_landingTargetId=81;g_upperFloor=1;
    MatchLanding(frame);if(!g_landingMatched || *(unsigned*)(frame-0x50)!=4)return 163;
    landingEdgeTest=false;testLayer=0;testId=81;boundary=true;
    StageVertex sharedVertex={10,10,0};
    // Highest actual adjacent stage is required, not four-step fallback.
    if(CornerHeightBits(&sharedVertex)!=FloatBits(1.5f))return 170;
    boundary=false;testId=80;
    if(CornerHeightBits(&sharedVertex)!=FloatBits(0.375f))return 171;
    // Integer coordinates must reach the lookup as positions around(10,10).
    landingEdgeTest=true;testId=81;sharedVertex.x=12;
    if(CornerHeightBits(&sharedVertex)!=FloatBits(1.5f))return 172;
    sharedVertex.x=9;
    if(CornerHeightBits(&sharedVertex)!=FloatBits(0.75f))return 173;
    landingEdgeTest=false;boundary=false;testBase=0;testLayer=0;
    g_nativeFinder=(UINT_PTR)FinderFake;g_autoFinderDepth=0;
    if(!ScopedAutoFinder(1,2,3,4,5,6,7,8) || seenFinderDepth!=1 || seenFinderArg!=36 || g_autoFinderDepth!=0)return 180;
    g_autoFinderDepth=3;
    if(!ScopedAutoFinder(1,2,3,4,5,6,7,8) || seenFinderDepth!=4 || g_autoFinderDepth!=3)return 181;
    g_autoFinderDepth=0;
    unsigned char* vfixture=(unsigned char*)ValidatorCallFixture;
    if(vfixture[0]!=0x56 || vfixture[1]!=0xFF || vfixture[2]!=0x92)return 182;
    g_validatorSurfaceResume=(UINT_PTR)(vfixture+7);
    PatchJump((UINT_PTR)(vfixture+1),(void*)ValidatorSurfaceHook,6,"test_validator_surface");
    testVtable[0xF8/4]=(UINT_PTR)RawVertexFake;
    StageVertex vertex={12,12,0};void* fakeWorld=&testWorld;UINT_PTR* fakeTable=testVtable;
    for(unsigned enabled=0;enabled<2;++enabled)for(unsigned variant=0;variant<4;++variant){
        testId=variant==0 ? 4 : 79+variant;g_autoFinderDepth=enabled;
        float expected=enabled ? HeightForId(testId) : 0.0f;
        __asm {
            fld sentinel
            lea esi,vertex
            mov ecx,fakeWorld
            mov edx,fakeTable
            call ValidatorCallFixture
            fstp result
            fstp remaining
        }
        if(result!=expected || remaining!=sentinel)return 183+enabled*4+variant;
    }
    // No stage at this vertex: retain the native raw elevation in finder scope.
    landingEdgeTest=true;vertex.x=9;g_autoFinderDepth=1;
    __asm {
        fld sentinel
        lea esi,vertex
        mov ecx,fakeWorld
        mov edx,fakeTable
        call ValidatorCallFixture
        fstp result
        fstp remaining
    }
    if(result!=0 || remaining!=sentinel)return 191;

    // Reproduce the actual ambiguity: a shared vertex sees upper and lower
    // stage tiles. The v26 max policy returns upper; candidate ownership must
    // return lower, consistently for corners AND the validator, in four directions.
    ownerFixture=true;testLayer=0;landingEdgeTest=false;boundary=false;
    for(unsigned pair=0;pair<2;++pair)for(unsigned direction=0;direction<4;++direction){
        ownerSource=pair?81:82;ownerTarget=pair?4:81;ownerDirection=direction;
        int dx=direction==0?1:direction==1?-1:0,dy=direction==2?1:direction==3?-1:0;
        StageVertex cell={10+dx,10+dy,0};
        *(StageGrid*)(frame+0xC)=StageGrid{10,10,0,1,{0,0,0}};
        *(StageVertex*)(frame-0x2C)=cell;
        StageVertex edge={dx>0?11:10,dy>0?11:10,0};
        g_autoFinderDepth=1;g_autoFinderFrame=0;
        if(CornerHeightBits(&edge)!=FloatBits(HeightForId(ownerSource)))return 201;
        if(CandidateCornerHeightBits(&edge,frame)!=FloatBits(HeightForId(ownerTarget)) || g_autoFinderFrame!=frame)return 202;
        if(AutoVertexOffsetBits(&edge)!=FloatBits(HeightForId(ownerTarget)))return 203;
        // Execute each changed naked wrapper in the owned context. It must
        // preserve registers/stack layout and a pre-existing x87 value.
        for(unsigned k=0;k<4;++k){
            *(StageVertex*)(frame-pointOffsets[k])=edge;UINT_PTR hook=cornerHooks[k];
            __asm {
                fld sentinel
                mov edx,hook
                push ebp
                mov ebp,frame
                call edx
                pop ebp
                fstp remaining
            }
            if(*(unsigned*)(frame-heightOffsets[k])!=FloatBits(HeightForId(ownerTarget)) || remaining!=sentinel)return 204;
        }
        // Do not override vertices outside the candidate footprint or floor.
        StageVertex unrelated={cell.x+2,cell.y,0};
        if(CandidateOwnerBits(frame,&unrelated))return 205;
        unrelated=edge;unrelated.layer=1;
        if(CandidateOwnerBits(frame,&unrelated))return 206;
        // Manual/out-of-scope behavior remains the v26 highest-adjacent rule.
        g_autoFinderDepth=0;
        if(CandidateCornerHeightBits(&edge,frame)!=FloatBits(HeightForId(ownerSource)) || AutoVertexOffsetBits(&edge))return 207;
        g_autoFinderDepth=1;
        // Ground, two-step, same-height and higher candidates must not use the
        // new rule. Neither should a cross-storey flight.
        unsigned saved=ownerTarget;
        unsigned otherTargets[4]={~0u,80,ownerSource,82};
        for(unsigned j=0;j<4;++j){ownerTarget=otherTargets[j];if(CandidateOwnerBits(frame,&edge))return 208;}
        ownerTarget=saved;((StageGrid*)(frame+0xC))->layer=1;
        if(CandidateOwnerBits(frame,&edge))return 209;
        ((StageGrid*)(frame+0xC))->layer=0;
        // The raw-height wrapper must add lower stage height, preserve x87
        // sentinel and keep baseline behavior outside finder scope.
        g_autoFinderFrame=frame;vertex=edge;testBase=0;
        __asm {
            fld sentinel
            lea esi,vertex
            mov ecx,fakeWorld
            mov edx,fakeTable
            call ValidatorCallFixture
            fstp result
            fstp remaining
        }
        if(result!=HeightForId(ownerTarget) || remaining!=sentinel)return 210;
    }
    ownerFixture=false;g_autoFinderDepth=0;g_autoFinderFrame=frame;
    g_nativeFinder=(UINT_PTR)FinderFake;
    if(!ScopedAutoFinder(1,2,3,4,5,6,7,8) || g_autoFinderFrame!=frame || g_autoFinderDepth)return 211;
    g_autoFinderFrame=0;

    ownerFixture=false;endpointFixture=true;testLayer=0;testBase=0;g_autoFinderDepth=0;g_autoFinderFrame=0;
    unsigned masks[4]={1,2,4,8};StagePoint proposed={-1,-1,-1};
    for(unsigned kind=0;kind<3;++kind)for(unsigned k=0;k<4;++k){
        endpointSource=kind==0?80:kind==1?81:82;endpointTarget=kind==0?~0u:kind==1?4:81;endpointMask=masks[k];
        unsigned distance=kind==0?1:2;
        if(!ProposeStageEndpoint(20,20,0,endpointMask,&proposed,kind==0))return 220;
        if(proposed.x!=20.5f+EndpointDX()*(int)distance || proposed.y!=20.5f+EndpointDY()*(int)distance || proposed.layer!=0)return 221;
        // Verify actual stock span calculation for the proposed source-target
        // distance: two-step uses span2, larger stage pairs enter span6 and the
        // existing landing matcher can stop at four steps before tile center.
        float fixtureDistance=(float)distance;
        unsigned computedSpan=0;
        __asm {fld fixtureDistance} __asm {fsub dword ptr [halfFixture]} __asm {fmul dword ptr [fourFixture]} __asm {fistp computedSpan}
        if(computedSpan!=(kind==0?2u:6u))return 222;
        endpointSlope=true;
        StagePoint unchanged={-1,-1,-1};if(ProposeStageEndpoint(20,20,0,endpointMask,&unchanged,kind==0) || unchanged.x!=-1)return 223;
        endpointSlope=false;
        unsigned wanted=endpointTarget;endpointTarget=80;
        if(ProposeStageEndpoint(20,20,0,endpointMask,&unchanged,kind==0))return 224;
        endpointTarget=wanted;
    }
    endpointSource=80;endpointTarget=~0u;endpointMask=4;endpointSourceX=64;
    if(ProposeStageEndpoint(64,20,0,4,&proposed,1))return 225;
    endpointSourceX=20;
    if(ProposeStageEndpoint(20,20,0,15,&proposed,1) || ProposeStageEndpoint(20,20,0,4,0,1))return 226;
    testLayer=1;if(!ProposeStageEndpoint(20,20,1,4,&proposed,1) || proposed.layer!=1)return 227;testLayer=0;
    // Wrong source/pair and native-success behavior must retain native output.
    endpointSource=81;endpointTarget=4;endpointMask=8;nativeFinderResult=1;
    proposed=StagePoint{-1,-1,-1};nativeFinderCalls=0;
    if(!ScopedAutoFinder(1,20,20,0,8,(unsigned)&proposed,0,0) || proposed.x!=-1 || nativeFinderCalls!=1)return 228;
    nativeFinderResult=0;nativeFinderCalls=0;
    if(!ScopedAutoFinder(1,20,20,0,8,(unsigned)&proposed,0,0) || proposed.y!=18.5f || nativeFinderCalls!=1)return 229;
    endpointSource=80;endpointTarget=~0u;nativeFinderCalls=0;
    if(!ScopedAutoFinder(1,20,20,0,8,(unsigned)&proposed,0,0) || proposed.y!=19.5f || nativeFinderCalls)return 230;
    // Invalid directions and out-of-range/null endpoints leave native failure.
    if(ScopedAutoFinder(1,20,20,0,15,(unsigned)&proposed,0,0) || ScopedAutoFinder(1,20,20,0,8,0,0,0))return 231;
    DWORD protection=0;
    if(!VirtualProtect(nativeAutoEntry,sizeof(nativeAutoEntry),PAGE_EXECUTE_READWRITE,&protection))return 232;
    *(unsigned*)(nativeAutoEntry+8)=(unsigned)&nativeDefaultPoint;
    unsigned callOffsets[3]={0x35,0x3D,0x63};
    UINT_PTR targets[3]={(UINT_PTR)NativeCopyFixture,(UINT_PTR)ScopedAutoFinder,(UINT_PTR)NativeCoreFixture};
    for(unsigned k=0;k<3;++k){unsigned off=callOffsets[k];if(nativeAutoEntry[off]!=0xE8)return 233;*(unsigned*)(nativeAutoEntry+off+1)=(unsigned)(targets[k]-((UINT_PTR)nativeAutoEntry+off+5));}
    FlushInstructionCache((HANDLE)-1,nativeAutoEntry,sizeof(nativeAutoEntry));
    nativeBuilderVtable[8]=(UINT_PTR)NativeClearFixture;
    typedef unsigned char (__cdecl *ActualAutoFn)(unsigned,StageGrid*,unsigned,StagePoint*,void*,unsigned,unsigned);
    for(unsigned kind=0;kind<3;++kind)for(unsigned accepted=0;accepted<2;++accepted){
        endpointSource=kind==0?80:kind==1?81:82;endpointTarget=kind==0?~0u:kind==1?4:81;endpointMask=8;
        StageGrid actualSource={20,20,0,8,{0,0,0}};
        nativeCoreResult=accepted;nativeCoreCalls=nativeClearCalls=nativeFinderCalls=0;
        proposed=StagePoint{-1,-1,-1};
        unsigned char actualResult=((ActualAutoFn)(UINT_PTR)nativeAutoEntry)(0x12345678,&actualSource,1,&proposed,&nativeBuilder,0x23456789,0);
        if(actualResult!=accepted || nativeCoreCalls!=1 || nativeClearCalls!=1)return 234;
        if(nativeArgs[0]!=0x12345678 || nativeArgs[1]!=(unsigned)&actualSource || nativeArgs[2]!=0 || nativeArgs[3]!=1 || nativeArgs[4]!=(unsigned)&proposed || nativeArgs[5]!=(unsigned)&nativeBuilder || nativeArgs[6]!=0x23456789 || nativeArgs[7]!=0)return 235;
        if(nativePoint[1]!=FloatBits(kind==0?19.5f:18.5f) || actualSource.x!=20 || actualSource.y!=20 || actualSource.layer!=0 || actualSource.mask!=8)return 236;
        if(nativeFinderCalls!=(kind==0?0u:1u))return 237;
    }
    endpointFixture=false;nativeFinderResult=1;

    // Cross-storey endpoint proposals: the native raw upper floor is3.0,
    // while the stages expose .375/.75/1.5/2.25 on floor0.
    upperFixture=true;endpointFixture=false;ownerFixture=false;upperSourceZ=3.0f;
    unsigned upperIds[4]={4,80,81,82};unsigned upperDistances[4]={4,4,3,2};
    for(unsigned j=0;j<4;++j)for(unsigned k=0;k<4;++k){
        upperTarget=upperIds[j];endpointMask=masks[k];proposed=StagePoint{-1,-1,-1};
        if(!ProposeUpperStageEndpoint(20,20,1,endpointMask,&proposed))return 240;
        if(proposed.layer!=0 || proposed.x!=20.5f+EndpointDX()*(int)upperDistances[j] || proposed.y!=20.5f+EndpointDY()*(int)upperDistances[j])return 241;
        RememberPreviewEndpoint(20,20,1,endpointMask,&proposed,1);
        if(!g_previewEndpointValid || g_previewEndpoint.layer!=0 || g_previewEndpointSource.layer!=1)return 242;
        upperObstacle=true;StagePoint untouched={-1,-1,-1};
        if(ProposeUpperStageEndpoint(20,20,1,endpointMask,&untouched) || untouched.x!=-1)return 243;
        upperObstacle=false;
    }
    upperTarget=81;upperSourceIsStage=true;
    if(ProposeUpperStageEndpoint(20,20,1,8,&proposed))return 244;upperSourceIsStage=false;
    if(ProposeUpperStageEndpoint(20,20,0,8,&proposed) || ProposeUpperStageEndpoint(20,20,1,15,&proposed) || ProposeUpperStageEndpoint(20,20,1,8,0))return 245;
    upperSourceZ=3.1f;if(ProposeUpperStageEndpoint(20,20,1,8,&proposed))return 246;upperSourceZ=3.0f;
    nativeFinderResult=0;g_nativeFinder=(UINT_PTR)FinderFake;
    if(!ScopedAutoFinder(1,20,20,1,8,(unsigned)&proposed,0,0) || !g_previewEndpointValid || proposed.layer!=0 || proposed.y!=17.5f)return 247;
    // A native successful upper endpoint is retained and also cached for click.
    nativeFinderResult=1;proposed=StagePoint{20.5f,17.5f,0};
    if(!ScopedAutoFinder(1,20,20,1,8,(unsigned)&proposed,0,0) || proposed.y!=17.5f || !g_previewEndpointValid)return 248;
    upperFixture=false;endpointFixture=true;endpointSourceX=endpointSourceY=20;
    g_nativeCommitCore=(UINT_PTR)NativeCoreFixture;
    // For all directions and both source types, a near-source click must pass
    // the cached endpoint and the untouched eight arguments to the native core.
    // Native failure leaves caller endpoint unchanged; success publishes the
    // accepted endpoint for the mouse tool's original post-commit code.
    for(unsigned sourceType=0;sourceType<2;++sourceType)for(unsigned k=0;k<4;++k)for(unsigned accepted=0;accepted<2;++accepted){
        endpointMask=masks[k];endpointSource=81;endpointTarget=4;
        StageGrid clickSource={20,20,sourceType?1:0,(unsigned char)endpointMask,{0,0,0}};
        StagePoint wanted={20.5f+EndpointDX()*3,20.5f+EndpointDY()*3,0};
        g_previewEndpointSource=clickSource;g_previewEndpointMask=endpointMask;g_previewEndpoint=wanted;g_previewEndpointValid=1;
        StagePoint clicked={20.5f+EndpointDX()*0.5f,20.5f+EndpointDY()*0.5f,clickSource.layer};
        StagePoint originalClick=clicked;nativeCoreResult=accepted;nativeCoreCalls=0;
        unsigned char got=ClickLandingCore(0x12345678,(unsigned)&clickSource,0xABCDE001,0x76540001,(unsigned)&clicked,0x34567890,0x45678901,0x56789012);
        if(got!=accepted || nativeCoreCalls!=1)return 249;
        if(nativeArgs[0]!=0x12345678 || nativeArgs[1]!=(unsigned)&clickSource || nativeArgs[2]!=0xABCDE001 || nativeArgs[3]!=0x76540001 || nativeArgs[5]!=0x34567890 || nativeArgs[6]!=0x45678901 || nativeArgs[7]!=0x56789012)return 250;
        if(nativePoint[0]!=FloatBits(wanted.x) || nativePoint[1]!=FloatBits(wanted.y) || nativePoint[2]!=0)return 251;
        if(accepted ? (clicked.x!=wanted.x || clicked.y!=wanted.y || clicked.layer!=wanted.layer) : (clicked.x!=originalClick.x || clicked.y!=originalClick.y || clicked.layer!=originalClick.layer))return 252;
        if(clickSource.x!=20 || clickSource.y!=20 || clickSource.layer!=(sourceType?1:0) || clickSource.mask!=endpointMask)return 253;
        // Explicit drags are forwarded as given, including endpoint pointer.
        StagePoint dragged={20.5f+EndpointDX()*4,20.5f+EndpointDY()*4,0};
        ClickLandingCore(1,(unsigned)&clickSource,1,1,(unsigned)&dragged,2,3,4);
        if(nativeArgs[4]!=(unsigned)&dragged || nativePoint[0]!=FloatBits(dragged.x) || nativePoint[1]!=FloatBits(dragged.y))return 254;
        // Stale preview from a different source cell cannot retarget a click.
        ++clickSource.x;clicked=originalClick;
        ClickLandingCore(1,(unsigned)&clickSource,1,1,(unsigned)&clicked,2,3,4);
        if(nativeArgs[4]!=(unsigned)&clicked)return 255;
    }
    // Confirmed two-step source never enters the new click retargeting path.
    endpointSource=80;endpointTarget=~0u;endpointMask=8;
    proposed=StagePoint{20.5f,19.5f,0};RememberPreviewEndpoint(20,20,0,8,&proposed,1);
    if(g_previewEndpointValid)return 256;
    // Even a stale cached preview from a previous stage cannot retarget the
    // now-confirmed two-step source at the same coordinates.
    g_previewEndpointSource=StageGrid{20,20,0,8,{0,0,0}};g_previewEndpointMask=8;g_previewEndpoint=StagePoint{20.5f,18.5f,0};g_previewEndpointValid=1;
    StageGrid shortClickSource={20,20,0,8,{0,0,0}};StagePoint shortClick={20.5f,20.0f,0};
    ClickLandingCore(1,(unsigned)&shortClickSource,0,1,(unsigned)&shortClick,2,3,4);
    if(nativeArgs[4]!=(unsigned)&shortClick || shortClick.y!=20.0f)return 258;
    nativeFinderResult=0;
    if(!ScopedAutoFinder(1,20,20,0,8,(unsigned)&proposed,0,0) || g_previewEndpointValid || proposed.y!=19.5f)return 257;
    endpointFixture=false;nativeFinderResult=1;

    // Exercise the actual8DCF80 machine code used by the mouse tool at
    //4D45FC and4D46E7. It gates placement on container.count, not on the
    // last preview/core return value alone.
    DWORD gateProtection=0;
    if(!VirtualProtect(nativeErrorGate,sizeof(nativeErrorGate),PAGE_EXECUTE_READWRITE,&gateProtection))return 260;
    FlushInstructionCache((HANDLE)-1,nativeErrorGate,sizeof(nativeErrorGate));
    typedef unsigned (__thiscall *HasErrorsFn)(void*);
    HasErrorsFn hasErrors=(HasErrorsFn)(UINT_PTR)nativeErrorGate;
    errorFixtureEnabled=true;g_clearFinderErrors=(UINT_PTR)ResetErrorsFixture;
    g_nativeFinder=(UINT_PTR)FinderFake;nativeFinderResult=0;nativeCoreResult=1;endpointMask=8;
    for(unsigned caseId=0;caseId<6;++caseId){
        upperFixture=caseId>=2;endpointFixture=caseId<2;
        endpointSource=caseId==0?81:82;endpointTarget=caseId==0?4:81;
        unsigned destinations[4]={4,80,81,82};upperTarget=caseId>=2?destinations[caseId-2]:82;upperSourceZ=3.0f;upperObstacle=false;upperSourceIsStage=false;
        StageGrid source={20,20,caseId>=2?1:0,8,{0,0,0}};
        ErrorFixture errors={0,0,0};proposed=StagePoint{-1,-1,-1};resetErrorCalls=0;errorsSeenByCore=99;
        // Control: actual608ED1 caller reports success, but the game's actual
        // mouse gate still rejects because the failed finder left an error.
        *(unsigned*)(nativeAutoEntry+0x3E)=(unsigned)((UINT_PTR)LegacyV30Finder-((UINT_PTR)nativeAutoEntry+0x42));
        FlushInstructionCache((HANDLE)-1,nativeAutoEntry,sizeof(nativeAutoEntry));
        unsigned char legacy=((ActualAutoFn)(UINT_PTR)nativeAutoEntry)(1,&source,1,&proposed,&nativeBuilder,0,(unsigned)&errors);
        if(!legacy || errors.count!=1 || !hasErrors(&errors) || resetErrorCalls || errorsSeenByCore!=1)return 261;
        *(unsigned*)(nativeAutoEntry+0x3E)=(unsigned)((UINT_PTR)ScopedAutoFinder-((UINT_PTR)nativeAutoEntry+0x42));
        FlushInstructionCache((HANDLE)-1,nativeAutoEntry,sizeof(nativeAutoEntry));
        for(unsigned accepted=0;accepted<2;++accepted){
            errors=ErrorFixture{0,0,0};resetErrorCalls=0;errorsSeenByCore=99;nativeCoreResult=accepted;nativeCoreCalls=0;
            unsigned char actual=((ActualAutoFn)(UINT_PTR)nativeAutoEntry)(1,&source,1,&proposed,&nativeBuilder,0,(unsigned)&errors);
            if(actual!=accepted || resetErrorCalls!=1 || nativeCoreCalls!=1 || errorsSeenByCore!=0)return 262;
            if(accepted ? (errors.count || hasErrors(&errors)) : (errors.count!=1 || errors.lastCode!=0x70 || !hasErrors(&errors)))return 263;
        }
        // Preserve errors that were already present before this search.
        errors=ErrorFixture{0,1,0xBAD};resetErrorCalls=0;nativeCoreResult=1;
        if(!((ActualAutoFn)(UINT_PTR)nativeAutoEntry)(1,&source,1,&proposed,&nativeBuilder,0,(unsigned)&errors) || resetErrorCalls || errors.count!=2 || !hasErrors(&errors))return 264;
    }
    upperFixture=false;endpointFixture=true;endpointSource=81;endpointTarget=80;
    StageGrid source={20,20,0,8,{0,0,0}};ErrorFixture errors={0,0,0};resetErrorCalls=0;nativeCoreCalls=0;
    // A native failure without an eligible fallback must keep its error.
    if(((ActualAutoFn)(UINT_PTR)nativeAutoEntry)(1,&source,1,&proposed,&nativeBuilder,0,(unsigned)&errors) || resetErrorCalls || errors.count!=1 || nativeCoreCalls || !hasErrors(&errors))return 265;
    // Native-success ground flights receive no extra reset or endpoint change.
    endpointSource=4;endpointTarget=~0u;errors=ErrorFixture{0,0,0};nativeFinderResult=1;nativeCoreResult=1;resetErrorCalls=0;
    proposed=StagePoint{20.5f,18.5f,0};
    if(!((ActualAutoFn)(UINT_PTR)nativeAutoEntry)(1,&source,1,&proposed,&nativeBuilder,0,(unsigned)&errors) || resetErrorCalls || errors.count || proposed.y!=18.5f)return 266;
    // Confirmed two-step short endpoint returns before failed native search.
    endpointSource=80;errors=ErrorFixture{0,0,0};nativeFinderResult=0;nativeFinderCalls=0;resetErrorCalls=0;
    if(!((ActualAutoFn)(UINT_PTR)nativeAutoEntry)(1,&source,1,&proposed,&nativeBuilder,0,(unsigned)&errors) || nativeFinderCalls || resetErrorCalls || errors.count || proposed.y!=19.5f)return 267;
    errorFixtureEnabled=false;g_clearFinderErrors=0;nativeFinderResult=1;endpointFixture=false;upperFixture=false;

    // Indexed secondary surface: shared vertices and a shuffled index order.
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)BaseFake;
    ownerFixture=false;endpointFixture=false;upperFixture=false;boundary=false;testBase=0;testLayer=0;
    unsigned short secondaryIndices[6]={7,4,10,4,2,10};
    *(unsigned*)(testMesh+0x28)=(unsigned)secondaryIndices;
    *(unsigned*)(testMesh+0x2C)=(unsigned)(secondaryIndices+6);
    testVertices[7][0]=10;testVertices[7][1]=10;
    testVertices[4][0]=11;testVertices[4][1]=10;
    testVertices[10][0]=11;testVertices[10][1]=11;
    testVertices[2][0]=12;testVertices[2][1]=11;
    for(unsigned k=0;k<24;++k)testVertices[k][2]=0;
    for(unsigned variant=0;variant<4;++variant){
        testId=variant?79+variant:4;
        for(unsigned k=0;k<6;++k)
            if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)(secondaryIndices+k))!=FloatBits(HeightForId(testId)))return 280+variant;
    }
    ownerFixture=true;ownerSource=82;ownerTarget=81;ownerDirection=0;
    for(unsigned k=0;k<6;++k)
        if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)(secondaryIndices+k))!=FloatBits(k<3?2.25f:1.5f))return 285;
    ownerTarget=4;
    if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)(secondaryIndices+3))!=0x3F400000)return 286;
    ownerFixture=false;testId=81;testLayer=2;testBase=12;
    for(unsigned k=0;k<24;++k)testVertices[k][2]=12;
    if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)secondaryIndices)!=0x3FC00000)return 287;
    testBase=13;
    if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)secondaryIndices)!=0x3F400000)return 288;
    testBase=0;testLayer=0;for(unsigned k=0;k<24;++k)testVertices[k][2]=0;
    secondaryIndices[0]=99;
    if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)secondaryIndices)!=0x3F400000)return 289;
    secondaryIndices[0]=7;
    if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)secondaryIndices+1)!=0x3F400000)return 290;
    *(unsigned*)(testMesh+0x2C)=(unsigned)(secondaryIndices+2);
    if(IndexedStageHeightBits((unsigned)testMesh,(unsigned)secondaryIndices)!=0x3F400000)return 291;
    *(unsigned*)(testMesh+0x2C)=(unsigned)(secondaryIndices+6);
    // Execute the actual naked hook with a live x87 value beneath the raw Z.
    g_secondaryStageResume=(UINT_PTR)ReturnOnly;
    unsigned char originalMesh[96];for(unsigned k=0;k<96;++k)originalMesh[k]=testMesh[k];
    unsigned short originalIndices[6];for(unsigned k=0;k<6;++k)originalIndices[k]=secondaryIndices[k];
    float originalVertices[24][10];for(unsigned k=0;k<24;++k)for(unsigned j=0;j<10;++j)originalVertices[k][j]=testVertices[k][j];
    for(unsigned variant=0;variant<4;++variant){
        testId=variant?79+variant:4;result=0;remaining=0;
        __asm {
            fld sentinel
            fld raw
            lea edi,testMesh
            lea ebx,secondaryIndices
            call SecondaryStageHeightHook
            fstp result
            fstp remaining
        }
        if(result!=raw+HeightForId(testId) || remaining!=sentinel)return 292+variant;
    }
    for(unsigned k=0;k<96;++k)if(testMesh[k]!=originalMesh[k])return 296;
    for(unsigned k=0;k<6;++k)if(secondaryIndices[k]!=originalIndices[k])return 297;
    for(unsigned k=0;k<24;++k)for(unsigned j=0;j<10;++j)if(testVertices[k][j]!=originalVertices[k][j])return 298;

    // Execute the game's actual indexed vertex -> surface-emission block.
    // Only its constant/callback addresses are relocated into this fixture.
    secondaryWorldVtable[0xA4/4]=(UINT_PTR)GetSecondaryGeometry;
    DWORD secondaryProtection=0;
    VirtualProtect(secondaryNativeCode,sizeof(secondaryNativeCode),PAGE_EXECUTE_READWRITE,&secondaryProtection);
    *(unsigned*)(secondaryNativeCode+0x7A)=(unsigned)&nativeSecondaryOffset;
    *(unsigned*)(secondaryNativeCode+0x8D)=(unsigned)((UINT_PTR)EmitSecondaryVertex-((UINT_PTR)secondaryNativeCode+0x91));
    ownerFixture=true;ownerSource=82;ownerTarget=81;ownerDirection=0;testBase=0;testLayer=0;
    for(unsigned k=0;k<6;++k){
        unsigned index=secondaryIndices[k];
        RunSecondaryNative((unsigned)testMesh,(unsigned)(secondaryIndices+k));
        if(emittedSecondary[0]!=testVertices[index][0] || emittedSecondary[1]!=testVertices[index][1] || emittedSecondary[2]!=0.75f)return 299;
    }
    g_secondaryStageResume=(UINT_PTR)secondaryNativeCode+0x7E;
    secondaryNativeCode[0x78]=0xE9;secondaryNativeCode[0x7D]=0x90;
    *(unsigned*)(secondaryNativeCode+0x79)=(unsigned)((UINT_PTR)SecondaryStageHeightHook-((UINT_PTR)secondaryNativeCode+0x7D));
    FlushInstructionCache((HANDLE)-1,secondaryNativeCode,sizeof(secondaryNativeCode));
    for(unsigned k=0;k<6;++k){
        unsigned index=secondaryIndices[k];
        __asm {fld sentinel}
        RunSecondaryNative((unsigned)testMesh,(unsigned)(secondaryIndices+k));
        __asm {fstp remaining}
        if(remaining!=sentinel || emittedSecondary[0]!=testVertices[index][0] || emittedSecondary[1]!=testVertices[index][1] || emittedSecondary[2]!=(k<3?2.25f:1.5f))return 300;
    }
    ownerFixture=false;

    // Control: exact native shadow-mask block rejects the custom IDs.
    *(unsigned*)(shadowMaskObject+0xAC)=(unsigned)shadowMaskWords;
    DWORD maskProtect=0;
    VirtualProtect(nativeShadowMask,sizeof(nativeShadowMask),PAGE_EXECUTE_READWRITE,&maskProtect);
    testLayer=0;ownerFixture=false;endpointFixture=false;upperFixture=false;boundary=false;
    const unsigned declarations[]={0,1,2,3,4,5,6,69,70,71,72,79,80,81,82,83,255,~0u};
    for(unsigned k=0;k<sizeof(declarations)/sizeof(declarations[0]);++k){
        testId=declarations[k];shadowMaskWords[0]=1;shadowMaskWords[1]=0xDEADBEEF;
        ReplayShadowMask();
        if(shadowMaskWords[0]!=(testId==4?0x1E1u:1u) || shadowMaskWords[1]!=0xDEADBEEF)return 310;
    }
    // The new hook changes only which IDs reach the unchanged native mask write.
    g_shadowStageYes=(UINT_PTR)nativeShadowMask+0x10;g_shadowStageNo=(UINT_PTR)nativeShadowMask+0x23;
    nativeShadowMask[0xB]=0xE9;
    *(unsigned*)(nativeShadowMask+0xC)=(unsigned)((UINT_PTR)ShadowStageMaskHook-((UINT_PTR)nativeShadowMask+0x10));
    FlushInstructionCache((HANDLE)-1,nativeShadowMask,sizeof(nativeShadowMask));
    for(unsigned k=0;k<sizeof(declarations)/sizeof(declarations[0]);++k){
        testId=declarations[k];shadowMaskWords[0]=1;shadowMaskWords[1]=0xDEADBEEF;
        ReplayShadowMask();
        bool stage=testId==4 || (testId>=80 && testId<=82);
        if(shadowMaskWords[0]!=(stage?0x1E1u:1u) || shadowMaskWords[1]!=0xDEADBEEF)return 311;
    }

    // Ground pass must not even call into the world. Other offsets stay native.
    g_getWorld=0;
    if(ShadowPlaneOffsetBits(&planePoint,0)!=0 || ShadowPlaneOffsetBits(&planePoint,0x3F000000)!=0x3F000000)return 320;
    g_getWorld=(UINT_PTR)GetFakeWorld;testBase=1;testLayer=0;ownerFixture=false;endpointFixture=false;upperFixture=false;boundary=false;
    const unsigned planeIds[]={4,80,81,82,70,79,83,~0u};
    for(unsigned k=0;k<sizeof(planeIds)/sizeof(planeIds[0]);++k){
        testId=planeIds[k];
        if(ShadowPlaneOffsetBits(&planePoint,0x3F400000)!=FloatBits(HeightForId(testId)))return 321;
    }
    testId=81;testLayer=2;testBase=12;planePoint.z=12;
    if(ShadowPlaneOffsetBits(&planePoint,0x3F400000)!=0x3FC00000)return 322;
    planePoint.z=13;
    if(ShadowPlaneOffsetBits(&planePoint,0x3F400000)!=0x3F400000)return 323;
    testLayer=0;testBase=1;planePoint=ShadowSurfacePoint{10.5f,10.5f,1.0f};
    boundary=true;planePoint.x=10;
    if(ShadowPlaneOffsetBits(&planePoint,0x3F400000)!=0x3F400000)return 324;
    boundary=false;planePoint.x=10.5f;
    if(ShadowPlaneOffsetBits(0,0x3F400000)!=0x3F400000)return 325;
    UINT_PTR planeHooks[]={(UINT_PTR)ShadowPlaneHook0,(UINT_PTR)ShadowPlaneHook1,(UINT_PTR)ShadowPlaneHook2,(UINT_PTR)ShadowPlaneHook3,(UINT_PTR)ShadowPlaneHook4};
    g_shadowPlaneResume0=g_shadowPlaneResume1=g_shadowPlaneResume2=g_shadowPlaneResume3=g_shadowPlaneResume4=(UINT_PTR)ReturnOnly;
    DWORD planeProtection=0;VirtualProtect(planeNative,sizeof(planeNative),PAGE_EXECUTE_READWRITE,&planeProtection);
    for(unsigned which=0;which<5;++which)for(unsigned pass=0;pass<2;++pass)for(unsigned k=0;k<sizeof(planeIds)/sizeof(planeIds[0]);++k){
        planeSiteKind=which;testId=planeIds[k];planeNativeOffset=pass?0.75f:0.0f;
        // The exact native instruction block demonstrates the fixed .75 plane.
        planeTarget=(UINT_PTR)planeNative[which];RunPlaneVertex();
        if(planeResult!=planePoint.z+planeNativeOffset || planeRemainingOffset!=planeNativeOffset || planeRemainingSentinel!=planeSentinel)return 326;
        planeTarget=planeHooks[which];RunPlaneVertex();
        float expected=planePoint.z+(pass?HeightForId(testId):0);
        if(planeResult!=expected || planeRemainingOffset!=planeNativeOffset || planeRemainingSentinel!=planeSentinel || !(planeFlags&0x40))return 327;
        if(planeEBX!=(which==0?0x11111111u:which==1?0u:FloatBits(planePoint.z)) ||
           planeEDX!=(which==2?0x22222222u:which==4?0x11111111u:FloatBits(planePoint.z)) || planeEDI!=(which==3?0x22222222u:FloatBits(planePoint.z)))return 328;
        if(planePoint.x!=10.5f || planePoint.y!=10.5f || planePoint.z!=1.0f)return 329;
    }
    // Exact native foundation-floor restoration branch: custom stages used to
    // take the restoration path; patched behavior must match native stage4.
    DWORD upperProtect=0;
    if(!VirtualProtect(upperFloorNative,sizeof(upperFloorNative),PAGE_EXECUTE_READWRITE,&upperProtect))return 340;
    *(unsigned*)(upperFloorNative+22)=6; // native JE target -> local skip sentinel
    const unsigned lowerIds[]={~0u,0,1,3,4,5,69,70,71,72,79,80,81,82,83,255};
    const unsigned floorUpperIds[]={~0u,0,4,81};
    for(unsigned u=0;u<4;++u)for(unsigned l=0;l<16;++l){
        upperFloorId=floorUpperIds[u];lowerFloorId=lowerIds[l];
        unsigned want=upperFloorId==~0u && (lowerFloorId==~0u || lowerFloorId==4 || lowerFloorId==5)?0xD0:0xC0;
        if(ReplayUpperFloor()!=want || upperFloorFrame[12]!=lowerFloorId)return 341;
    }
    g_aboveFloorNative=(UINT_PTR)upperFloorNative+12;g_aboveFloorExcluded=(UINT_PTR)upperFloorNative+17;
    upperFloorNative[7]=0xE9;
    *(unsigned*)(upperFloorNative+8)=(unsigned)((UINT_PTR)AboveStageFloorHook-((UINT_PTR)upperFloorNative+12));
    FlushInstructionCache((HANDLE)-1,upperFloorNative,sizeof(upperFloorNative));
    for(unsigned u=0;u<4;++u)for(unsigned l=0;l<16;++l){
        upperFloorId=floorUpperIds[u];lowerFloorId=lowerIds[l];
        bool excluded=lowerFloorId==~0u || lowerFloorId==4 || lowerFloorId==5 || (lowerFloorId>=80 && lowerFloorId<=82);
        unsigned want=upperFloorId==~0u && excluded?0xD0:0xC0;
        if(ReplayUpperFloor()!=want || upperFloorFrame[12]!=lowerFloorId)return 342;
    }
    // Actual native early-obstruction gate rejects the captured12->8 bottom.
    endpointFixture=true;endpointSource=82;endpointTarget=81;endpointDistance=2;
    endpointMask=4;endpointSourceX=14;endpointSourceY=12;endpointSlope=false;
    testLayer=0;testBase=0;testId=81;ownerFixture=false;upperFixture=false;boundary=false;
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)BaseFake;g_landingSurface=(UINT_PTR)SurfaceFake;
    unsigned gateFrame=(unsigned)landingGateStorage+256;
    *(StageGrid**)(gateFrame+0xC)=&landingGateSource;*(StagePoint**)(gateFrame+0x18)=&landingGateEndpoint;
    *(StagePoint*)(gateFrame-0x30)=StagePoint{16.0f,12.5f,0};
    *(float*)(gateFrame+0x14)=1.5f;
    g_upperStage=1;g_upperFloor=g_lowerFloor=0;g_lowerSurface=1.5f;g_landingTargetId=81;
    DWORD landingProtection=0;
    if(!VirtualProtect(landingGateNative,sizeof(landingGateNative),PAGE_EXECUTE_READWRITE,&landingProtection))return 350;
    *(unsigned*)(landingGateNative+9)=(unsigned)((UINT_PTR)ConvertFake-((UINT_PTR)landingGateNative+13));
    *(unsigned*)(landingGateNative+15)=(unsigned)((UINT_PTR)LandingNativeQuery-((UINT_PTR)landingGateNative+19));
    *(unsigned*)(landingGateNative+26)=6;
    landingNativeCalls=0;
    if(ReplayLandingGate()!=0xD0 || landingNativeCalls!=1)return 351;
    g_nativeStageObstacleQuery=(UINT_PTR)LandingNativeQuery;
    *(unsigned*)(landingGateNative+15)=(unsigned)((UINT_PTR)StageLandingObstacleHook-((UINT_PTR)landingGateNative+19));
    FlushInstructionCache((HANDLE)-1,landingGateNative,sizeof(landingGateNative));
    landingNativeCalls=0;
    __asm {fld sentinel}
    unsigned gateResult=ReplayLandingGate();
    __asm {fstp remaining}
    if(gateResult!=0xC0 || landingNativeCalls!=1 || remaining!=sentinel)return 352;
    *(unsigned*)(gateFrame-0x34)=3;*(unsigned*)(gateFrame-0x50)=6;g_landingMatched=0;
    *(float*)(gateFrame-0x58)=0.25f;*(float*)(gateFrame-0x54)=0;
    MatchLanding(gateFrame);
    if(!g_landingMatched || *(unsigned*)(gateFrame-0x50)!=4)return 366;
    g_landingMatched=0;
    // Higher/lower steps, another tile, wrong target, upper-floor flights,
    // disabled landing scope and surface mismatches retain the native block.
    for(unsigned variant=0;variant<7;++variant){
        *(float*)(gateFrame+0x14)=variant==0?1.6875f:variant==1?1.3125f:1.5f;
        ((StagePoint*)(gateFrame-0x30))->x=variant==2?15.75f:16.0f;
        g_landingTargetId=variant==3?82:81;g_upperFloor=variant==4?1:0;
        g_upperStage=variant==5?0:1;testBase=variant==6?0.25f:0;
        if(ReplayLandingGate()!=0xD0)return 353+variant;
    }
    *(float*)(gateFrame+0x14)=1.5f;g_landingTargetId=81;g_upperFloor=0;g_upperStage=1;testBase=0;
    landingNativeResult=0;
    if(ReplayLandingGate()!=0xC0)return 360;
    landingNativeResult=1;
    // 12->4 now proposes its native eight-step span over two empty tiles.
    endpointTarget=4;endpointDistance=3;testId=4;
    StagePoint twelveToFour={-1,-1,-1};
    if(!ProposeStageEndpoint(14,12,0,4,&twelveToFour,0) || twelveToFour.x!=17.5f || twelveToFour.y!=12.5f || twelveToFour.layer!=0)return 361;
    if(ProposeStageEndpoint(14,12,0,4,&twelveToFour,1))return 362;
    nativeFinderCalls=0;g_nativeFinder=(UINT_PTR)FinderFake;
    if(!ScopedAutoFinder(1,14,12,0,4,(unsigned)&twelveToFour,0,0) || nativeFinderCalls || !g_previewEndpointValid || g_previewEndpoint.x!=17.5f)return 363;
    endpointDistance=2;
    if(ProposeStageEndpoint(14,12,0,4,&twelveToFour,0))return 364;
    endpointDistance=3;endpointSlope=true;
    if(ProposeStageEndpoint(14,12,0,4,&twelveToFour,0))return 365;
    endpointSlope=false;endpointFixture=false;
    // Exact native corner comparison requests ground reshaping at raw0 when
    // the suspended block's expected base is1.5. The scoped plane avoids it.
    endpointFixture=true;endpointSource=82;endpointTarget=4;endpointDistance=3;
    endpointMask=4;endpointSourceX=14;endpointSourceY=12;testLayer=0;testBase=0;testId=4;
    landingGateEndpoint=StagePoint{17.5f,12.5f,0};
    *(float*)(gateFrame-0xB4)=2.25f;
    SetLandingContext(gateFrame);
    if(!g_flatStageFlight || g_flatStageRaw!=0)return 370;
    *(float*)(gateFrame-0x10)=1.5f;
    *(StageVertex*)(gateFrame-0xC8)=StageVertex{16,12,0};
    testVtable[0xF8/4]=(UINT_PTR)RawVertexFake;
    DWORD suspendedProtection=0;
    if(!VirtualProtect(suspendedNative,sizeof(suspendedNative),PAGE_EXECUTE_READWRITE,&suspendedProtection))return 371;
    if(ReplaySuspendedCorner()!=0xD0)return 372;
    UINT_PTR suspendedHooks[]={(UINT_PTR)StageValidationPlaneHook0,(UINT_PTR)StageValidationPlaneHook1,(UINT_PTR)StageValidationPlaneHook2,(UINT_PTR)StageValidationPlaneHook3};
    g_stagePlaneResume0=g_stagePlaneResume1=g_stagePlaneResume2=g_stagePlaneResume3=(UINT_PTR)suspendedNative+17;
    suspendedNative[11]=0xE9;suspendedNative[16]=0x90;
    for(unsigned h=0;h<4;++h){
        *(unsigned*)(suspendedNative+12)=(unsigned)(suspendedHooks[h]-((UINT_PTR)suspendedNative+16));
        FlushInstructionCache((HANDLE)-1,suspendedNative,sizeof(suspendedNative));
        __asm {fld sentinel}
        unsigned cornerResult=ReplaySuspendedCorner();
        __asm {fstp remaining}
        if(cornerResult!=0xC0 || remaining!=sentinel)return 373+h;
        *(float*)(gateFrame-0x10)=0.75f;
        if(ReplaySuspendedCorner()!=0xC0)return 377;
        *(float*)(gateFrame-0x10)=1.5f;
    }
    for(unsigned v=0;v<7;++v){
        g_flatStageFlight=v==0?0:1;testBase=v==1?0.25f:0;
        ((StageVertex*)(gateFrame-0xC8))->layer=v==2?1:0;
        ((StageVertex*)(gateFrame-0xC8))->y=v==3?15:12;
        *(float*)(gateFrame-0x10)=v==4?0.5f:v==5?3.0f:1.5f;
        g_upperFloor=v==6?1:0;
        if(ReplaySuspendedCorner()!=0xD0)return 378+v;
    }
    if(landingGateSource.x!=14 || landingGateSource.y!=12 || landingGateSource.layer!=0 || landingGateEndpoint.x!=17.5f || landingGateEndpoint.layer!=0)return 385;
    int cpuFixtureResult=FinalSurfaceTests();if(cpuFixtureResult)return cpuFixtureResult;
    cpuFixtureResult=TestStageCpuNativeLoops();return cpuFixtureResult?cpuFixtureResult:TestStageCpuNoReadback();
}











