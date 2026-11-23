// Replay real game loops so CPU source offsets/register choices are tested
// against native geometry emission, not only hand-built hook entry layouts.
#include "native-shadow-bytes.h"
static StagePoint stageCpuInput[16];
static float stageCpuUV[32],stageCpuOutput[16][5];
static unsigned stageCpuBounds[4],stageCpuWhich,stageCpuPass,stageCpuCode;
static float stageCpuOffset,stageCpuSentinel=17.25f,stageCpuRemaining;
extern "C" __declspec(naked) void RunStageCpuLoop(){
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        sub esp,256
        lea edi,[esp-4]
        lea eax,stageCpuBounds
        mov [edi+3ch],eax
        lea eax,stageCpuInput
        mov [edi+20h],eax
        mov [edi+34h],eax
        lea eax,stageCpuUV
        mov [edi+10h],eax
        mov dword ptr [edi+80h],0
        mov dword ptr [edi+28h],20
        mov dword ptr [edi+1ch],0
        mov dword ptr [edi+20h],0 // overwritten below for primary input
        mov dword ptr [edi+30h],0
        mov dword ptr [edi+3ch],0
        mov dword ptr [edi+50h],0
        mov eax,stageCpuPass
        mov [edi+2ch],eax
        mov dword ptr [edi+64h],0
        mov dword ptr [edi+68h],3f400000h
        fld stageCpuSentinel
        cmp stageCpuWhich,0
        je primaryLoop
        lea esi,stageCpuBounds
        lea eax,stageCpuOutput
        xor ebx,ebx
        call dword ptr [stageCpuCode]
        jmp finishedLoop
    primaryLoop:
        lea eax,stageCpuInput
        mov [edi+20h],eax
        mov dword ptr [edi+24h],20
        mov dword ptr [edi+28h],0
        lea eax,stageCpuBounds
        mov [edi+3ch],eax
        lea ebp,stageCpuBounds
        xor ebx,ebx
        xor eax,eax
        lea ecx,stageCpuOutput
        lea edx,[ecx+12]
        fld stageCpuOffset
        call dword ptr [stageCpuCode]
    finishedLoop:
        fstp stageCpuRemaining
        add esp,256
        pop edi
        pop esi
        pop ebx
        pop ebp
        ret
    }
}
static float __cdecl StageCpuFlatBase(StagePoint*){return testBase;}
static int TestStageCpuNativeLoops(){
    DWORD protection=0;
    if(!VirtualProtect(StageNativePrimary,sizeof(StageNativePrimary),PAGE_EXECUTE_READWRITE,&protection)||
       !VirtualProtect(StageNativeSecondary,sizeof(StageNativeSecondary),PAGE_EXECUTE_READWRITE,&protection))return 901;
    testLayer=0;testBase=1;boundary=false;landingEdgeTest=false;endpointFixture=false;ownerFixture=false;upperFixture=false;
    g_getWorld=(UINT_PTR)GetFakeWorld;g_convertPoint=(UINT_PTR)ConvertFake;g_baseSurface=(UINT_PTR)StageCpuFlatBase;
    testVtable[0x144/4]=testVtable[0x148/4]=(UINT_PTR)Low;
    testVtable[0x14c/4]=testVtable[0x150/4]=(UINT_PTR)High;
    testVtable[0x160/4]=(UINT_PTR)Low;testVtable[0x164/4]=(UINT_PTR)FloorHigh;testVtable[0xe8/4]=(UINT_PTR)QueryFake;
    for(unsigned i=0;i<16;++i){stageCpuInput[i].x=12.25f+i*.025f;stageCpuInput[i].y=12.25f;stageCpuInput[i].layer=FloatBits(testBase);stageCpuUV[i*2]=float(i);stageCpuUV[i*2+1]=-float(i);}
    // StagePoint and native XYZ both occupy 12 bytes; third word here is Z bits.
    stageCpuBounds[0]=stageCpuBounds[1]=stageCpuBounds[2]=0;stageCpuBounds[3]=7;
    const unsigned ids[]={4,80,81,82,70};
    for(unsigned patched=0;patched<2;++patched){
        if(patched){
            unsigned char* places[]={StageNativePrimary+(0x7685aa-0x768590),StageNativePrimary+(0x7685fb-0x768590),StageNativeSecondary+(0x7695dd-0x769590),StageNativeSecondary+(0x769606-0x769590)};
            void* hooks[]={(void*)ShadowPlaneHook0,(void*)ShadowPlaneHook1,(void*)ShadowPlaneHook2,(void*)ShadowPlaneHook3};
            UINT_PTR* resumes[]={&g_shadowPlaneResume0,&g_shadowPlaneResume1,&g_shadowPlaneResume2,&g_shadowPlaneResume3};
            unsigned lengths[]={7,10,7,7};
            for(unsigned i=0;i<4;++i){*resumes[i]=(UINT_PTR)places[i]+lengths[i];PatchJump((UINT_PTR)places[i],hooks[i],lengths[i],"fixture native loop");}
        }
        for(unsigned which=0;which<2;++which)for(unsigned pass=0;pass<2;++pass)for(unsigned h=0;h<5;++h){
            testId=ids[h];stageCpuWhich=which;stageCpuPass=pass;stageCpuOffset=pass?.75f:0.f;
            stageCpuCode=(unsigned)(which?StageNativeSecondary:StageNativePrimary);
            if(ShadowPlaneOffsetBits((ShadowSurfacePoint*)stageCpuInput,0x3f400000)!=FloatBits(HeightForId(testId)))return 930+h;
            for(unsigned i=0;i<16;++i)for(unsigned j=0;j<5;++j)stageCpuOutput[i][j]=-999.f;
            RunStageCpuLoop();if(stageCpuRemaining!=stageCpuSentinel)return 902;
            const float delta=pass?(patched&&testId>=80&&testId<=82?HeightForId(testId):.75f):0.f;
            for(unsigned i=0;i<16;++i){
                if(stageCpuOutput[i][0]!=stageCpuInput[i].x||stageCpuOutput[i][1]!=stageCpuInput[i].y||stageCpuOutput[i][2]!=testBase+delta)return 10000+patched*1000+which*400+pass*200+h*32+i;
                if(!which&&(stageCpuOutput[i][3]!=stageCpuUV[i*2]||stageCpuOutput[i][4]!=stageCpuUV[i*2+1]))return 905;
            }
        }
    }
    return 0;
}
#pragma comment(linker, "/alternatename:_VirtualAlloc@16=_VirtualAlloc")
#pragma comment(linker, "/alternatename:_VirtualFree@12=_VirtualFree")
extern "C" void* __stdcall VirtualAlloc(void*,unsigned,unsigned,unsigned);
extern "C" BOOL __stdcall VirtualFree(void*,unsigned,unsigned);
static int TestStageCpuNoReadback(){
    void* blocked=VirtualAlloc(0,4096,0x3000,1);if(!blocked)return 940;
    planeGpuOverride=(UINT_PTR)blocked;
    planePoint=ShadowSurfacePoint{10.5f,10.5f,testBase};planeNativeOffset=.75f;
    g_shadowPlaneResume0=g_shadowPlaneResume1=g_shadowPlaneResume2=g_shadowPlaneResume3=(UINT_PTR)ReturnOnly;
    UINT_PTR hooks[]={(UINT_PTR)ShadowPlaneHook0,(UINT_PTR)ShadowPlaneHook1,(UINT_PTR)ShadowPlaneHook2,(UINT_PTR)ShadowPlaneHook3};
    const unsigned ids[]={4,80,81,82,70};
    for(unsigned which=0;which<4;++which)for(unsigned h=0;h<5;++h){
        testId=ids[h];planeSiteKind=which;planeTarget=hooks[which];RunPlaneVertex();
        if(planeResult!=testBase+HeightForId(testId)||planeRemainingOffset!=.75f||planeRemainingSentinel!=planeSentinel)return 941+which;
    }


    planeGpuOverride=0;VirtualFree(blocked,0,0x8000);return 0;
}

