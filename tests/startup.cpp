#define STAGE_REFERENCE_EXE L"tests/game-reference.exe"
#define GetModuleHandleW FixtureGetModuleHandleW
#include "../TS2-CustomStages.cpp"
#pragma optimize("", off) // fixture frame layout only; included production code remains /O2
#pragma comment(linker, "/alternatename:_CreateFileW@28=_CreateFileW")
extern "C" HANDLE __stdcall CreateFileW(const wchar_t*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
#pragma comment(linker, "/alternatename:_CloseHandle@4=_CloseHandle")
extern "C" BOOL __stdcall CloseHandle(HANDLE);
static unsigned char* fixtureImage;
extern "C" HMODULE __stdcall FixtureGetModuleHandleW(const wchar_t*) {return fixtureImage;}
#pragma comment(linker, "/alternatename:_VirtualAlloc@16=_VirtualAlloc")
#pragma comment(linker, "/alternatename:_ReadFile@20=_ReadFile")
extern "C" void* __stdcall VirtualAlloc(void*,unsigned,unsigned,unsigned);
extern "C" BOOL __stdcall ReadFile(HANDLE,void*,DWORD,DWORD*,void*);
extern "C" int __cdecl TestStart()
{
    const unsigned capacity=0x3000000;
    unsigned char* disk=(unsigned char*)VirtualAlloc(0,capacity,0x3000,0x04);
    fixtureImage=(unsigned char*)VirtualAlloc(0,capacity,0x3000,0x40);
    if(!disk || !fixtureImage)return 1;
    HANDLE file=CreateFileW(STAGE_REFERENCE_EXE,0x80000000,3,0,3,0x80,0);
    DWORD read=0;if(file==INVALID_HANDLE_VALUE || !ReadFile(file,disk,capacity,&read,0) || read!=46208224)return 2;
    unsigned pe=*(unsigned*)(disk+0x3C);
    unsigned sections=*(unsigned short*)(disk+pe+6),optional=pe+24;
    unsigned table=optional+*(unsigned short*)(disk+pe+20);
    for(unsigned j=0;j<sections;++j){
        unsigned char* header=disk+table+j*40;
        unsigned va=*(unsigned*)(header+12),size=*(unsigned*)(header+16),raw=*(unsigned*)(header+20);
        if(va>capacity || size>capacity-va || raw>read || size>read-raw)return 3;
        for(unsigned i=0;i<size;++i)fixtureImage[va+i]=disk[raw+i];
    }
    unsigned base=(unsigned)fixtureImage;
    // These original v26 operand checks are relative to the loaded image base.
    *(unsigned*)(fixtureImage+0xDB4FC)=base+0xDE3848;
    *(unsigned*)(fixtureImage+0x2082B2)=base+0xDFC48C;
    *(unsigned*)(fixtureImage+0x2085B8)=base+0xDFC48C;
    *(unsigned*)(fixtureImage+0x208B35)=base+0xDFC48C;
    // Deliberately invalidate a v27-only call signature. This must not disable
    // the working v26 geometry/surface hooks in v28.
    fixtureImage[0x2032CA]=0xE9;
    WorkerThread(0);
    unsigned expectedJumps[]={0x81EBF0,0x6E6C94,0xDB4F1,0x689D1B,
        0x689E17,0x689E80,0x689EEA,0x689F4F,0x68A02D,0x62BD44,
        0x82383C,0x8243C2,0x82537E,0x20827A,0x208535,0xD4B92,
        0x8205E4,0x208B3C,0x207407,0x2076C9,0x207700,0x207754,
        0x207788,0x2082BC,0x208992,0x208A44,0x207A9A,0x208193,0x20330B};
    for(unsigned j=0;j<sizeof(expectedJumps)/sizeof(expectedJumps[0]);++j)
        if(fixtureImage[expectedJumps[j]]!=0xE9)return 10+j;
    if(*(unsigned*)(fixtureImage+0xDB4FC)!=(unsigned)&g_previewHeight)return 50;
    if(fixtureImage[0x2032CA]!=0xE9)return 51;

    if(g_clearFinderErrors!=base+0x81E4B0)return 56;
    if(fixtureImage[0x68D24C]!=0xE9 || g_secondaryStageResume!=base+0x68D252)return 57;
    if(fixtureImage[0x769F9E]!=0xE9 || g_shadowStageYes!=base+0x769FA3 || g_shadowStageNo!=base+0x769FB6)return 60;
    const unsigned planes[]={0x7685AA,0x7685FB,0x7695DD,0x769606};
    for(unsigned j=0;j<4;++j)if(fixtureImage[planes[j]]!=0xE9)return 62;
    if(fixtureImage[0x203EB3]!=0xE9 || g_aboveFloorNative!=base+0x203EB8 || g_aboveFloorExcluded!=base+0x203EBD)return 64;
    if(fixtureImage[0x767EEE]!=0xE9 || g_shadowPlaneResume4!=base+0x767EF5)return 66;
    if(g_nativeStageObstacleQuery!=base+0x820D10 || base+0x2085D6+*(int*)(fixtureImage+0x2085D2)!=(UINT_PTR)StageLandingObstacleHook)return 67;
    const unsigned suspendedSites[]={0x2087F5,0x208858,0x2088D0,0x208930};
    for(unsigned j=0;j<4;++j)if(fixtureImage[suspendedSites[j]]!=0xE9)return 69;
    if(fixtureImage[0x76D84C]!=0xC7 || g_shadowRasterResume)return 71;
    if(fixtureImage[0x68CE7B]!=0xE9 || fixtureImage[0x68E478]!=0xE9 || g_cpuCapResume!=base+0x68CE83 || g_capBoundsResume!=base+0x68E483)return 73;
    if(fixtureImage[0x824922]!=0xE9 || g_layerOwnerResume!=base+0x824927)return 76;
    const unsigned joinRvas[]={0x209C1C,0x209C23,0x209C47,0x209C89};
    for(unsigned i=0;i<4;++i)if(fixtureImage[joinRvas[i]]!=0xE9 || g_joinResume[i]!=base+joinRvas[i]+5)return 81+i;
    if(fixtureImage[0x66BB90]!=0xE9 || fixtureImage[0x66BB9E]!=0xE9 || g_fenceResume[0]!=base+0x66BB96 || g_fenceResume[1]!=base+0x66BBA4)return 90;
    if(fixtureImage[0x62A10C]!=0xE9 || g_fenceModelResume!=base+0x62A112)return 109;
    if(fixtureImage[0x629CC5]!=0xE9 || fixtureImage[0x65D18E]!=0xE9 || g_uiStageResume[0]!=base+0x629CCD || g_uiStageResume[1]!=base+0x65D196)return 125;
    if(fixtureImage[0xF292E]!=0xE9 || g_uiStageResume[2]!=base+0xF2935)return 130;
    if(fixtureImage[0x633B1A]!=0xE9 || g_halfwallBodyResume!=base+0x633B22)return 139;
    if(fixtureImage[0x633D27]!=0xD8)return 162;
    if(base+0x6307D9+*(int*)(fixtureImage+0x6307D5)!=base+0x62C7E0)return 170;
    if(g_wallSourceBuild!=base+0x633980 || g_wallSourceNext!=base+0xA496F0 || base+0x636043+*(int*)(fixtureImage+0x63603F)!=(UINT_PTR)BuildCanonicalHalfwall)return 176;
    if(fixtureImage[0x62FFB1]!=0xE9 || g_halfwallVertexResume!=base+0x62FFB7)return 184;
    unsigned clickTarget=base+0xD4CC0+*(int*)(fixtureImage+0xD4CBC);
    if(clickTarget!=(unsigned)ClickLandingCore || g_nativeCommitCore!=base+0x208095)return 52;

    // Replay startup with a conflicting mouse adapter call. The optional
    // adapter must be skipped while every original height hook still installs.
    for(unsigned j=0;j<sections;++j){
        unsigned char* header=disk+table+j*40;
        unsigned va=*(unsigned*)(header+12),size=*(unsigned*)(header+16),raw=*(unsigned*)(header+20);
        for(unsigned i=0;i<size;++i)fixtureImage[va+i]=disk[raw+i];
    }
    *(unsigned*)(fixtureImage+0xDB4FC)=base+0xDE3848;
    *(unsigned*)(fixtureImage+0x2082B2)=base+0xDFC48C;
    *(unsigned*)(fixtureImage+0x2085B8)=base+0xDFC48C;
    *(unsigned*)(fixtureImage+0x208B35)=base+0xDFC48C;
    fixtureImage[0x2032CA]=0xE9;fixtureImage[0xD4CBB]=0xE9;g_nativeCommitCore=0;
    WorkerThread(0);
    if(g_nativeCommitCore || fixtureImage[0xD4CBB]!=0xE9)return 53;
    for(unsigned j=0;j<sizeof(expectedJumps)/sizeof(expectedJumps[0]);++j)
        if(fixtureImage[expectedJumps[j]]!=0xE9)return 54;
    if(*(unsigned*)(fixtureImage+0xDB4FC)!=(unsigned)&g_previewHeight)return 55;

    // A conflicting new surface site must not prevent any established height
    // or stair hook from installing. Replay the original image before testing.
    for(unsigned j=0;j<sections;++j){
        unsigned char* header=disk+table+j*40;
        unsigned va=*(unsigned*)(header+12),size=*(unsigned*)(header+16),raw=*(unsigned*)(header+20);
        for(unsigned i=0;i<size;++i)fixtureImage[va+i]=disk[raw+i];
    }
    *(unsigned*)(fixtureImage+0xDB4FC)=base+0xDE3848;
    *(unsigned*)(fixtureImage+0x2082B2)=base+0xDFC48C;
    *(unsigned*)(fixtureImage+0x2085B8)=base+0xDFC48C;
    *(unsigned*)(fixtureImage+0x208B35)=base+0xDFC48C;
    fixtureImage[0x824922]=0xCC;g_layerOwnerResume=0;
    fixtureImage[0x68CE7B]=0xCC;fixtureImage[0x68E478]=0xCC;g_cpuCapResume=g_capBoundsResume=0;
    fixtureImage[0x76D84C]=0xCC;g_shadowRasterResume=0;
    fixtureImage[0x208858]=0xCC;g_stagePlaneResume0=g_stagePlaneResume1=g_stagePlaneResume2=g_stagePlaneResume3=0;
    fixtureImage[0x767EEE]=0xCC;g_shadowPlaneResume4=0;fixtureImage[0x2085D1]=0xCC;g_nativeStageObstacleQuery=0;
    fixtureImage[0x203EB3]=0xCC;g_aboveFloorNative=g_aboveFloorExcluded=0;
    fixtureImage[0x7685AA]=0xCC;g_shadowPlaneResume0=g_shadowPlaneResume1=g_shadowPlaneResume2=g_shadowPlaneResume3=0;fixtureImage[0x68D24C]=0xCC;g_secondaryStageResume=0;fixtureImage[0x769F9E]=0xCC;g_shadowStageYes=g_shadowStageNo=0;
    WorkerThread(0);
    if(g_secondaryStageResume || fixtureImage[0x68D24C]!=0xCC)return 58;
    if(g_shadowStageYes || g_shadowStageNo || fixtureImage[0x769F9E]!=0xCC)return 61;
    if(g_shadowPlaneResume0 || g_shadowPlaneResume1 || g_shadowPlaneResume2 || g_shadowPlaneResume3 || fixtureImage[0x7685AA]!=0xCC || fixtureImage[0x7685FB]!=0xD8)return 63;
    for(unsigned j=0;j<sizeof(expectedJumps)/sizeof(expectedJumps[0]);++j)
        if(fixtureImage[expectedJumps[j]]!=0xE9)return 59;
    if(fixtureImage[0x203EB3]!=0xCC || g_aboveFloorNative || g_aboveFloorExcluded)return 65;
    if(g_shadowPlaneResume4 || g_nativeStageObstacleQuery || fixtureImage[0x767EEE]!=0xCC || fixtureImage[0x2085D1]!=0xCC)return 68;
    if(g_stagePlaneResume0 || g_stagePlaneResume1 || g_stagePlaneResume2 || g_stagePlaneResume3 || fixtureImage[0x2087F5]!=0xFF || fixtureImage[0x208858]!=0xCC)return 70;
    if(g_shadowRasterResume || fixtureImage[0x76D84C]!=0xCC)return 72;
    if(g_cpuCapResume || g_capBoundsResume || fixtureImage[0x68CE7B]!=0xCC || fixtureImage[0x68E478]!=0xCC)return 74;
    if(g_layerOwnerResume || fixtureImage[0x824922]!=0xCC)return 77;
    // A mismatch at any one of the four sites must leave all sites unchanged.
    const unsigned char joinBytes[4][5]={{0x83,0xF8,4,0x74,7},{0x83,0xF8,4,0x74,3},{0x83,0xFF,4,0x74,0x42},{0x83,0xFF,4,0x74,3}};
    for(unsigned conflict=0;conflict<4;++conflict){
        for(unsigned i=0;i<4;++i)for(unsigned j=0;j<5;++j)fixtureImage[joinRvas[i]+j]=joinBytes[i][j];
        fixtureImage[joinRvas[conflict]]=0xCC;
        InstallStageJoins(base);
        for(unsigned i=0;i<4;++i)for(unsigned j=0;j<5;++j)
            if(fixtureImage[joinRvas[i]+j]!=(i==conflict && j==0?0xCC:joinBytes[i][j]))return 85+conflict;
    }
    const unsigned fenceSites[]={0x66BB90,0x66BB9E};
    const unsigned char fenceBytes[]={0xD8,0x05,0x2C,0xA8,0x23,0x01};
    for(unsigned conflict=0;conflict<3;++conflict){
        fixtureImage[0x66BB7F]=0x56;
        for(unsigned i=0;i<2;++i)for(unsigned j=0;j<6;++j)fixtureImage[fenceSites[i]+j]=fenceBytes[j];
        unsigned bad=conflict<2?fenceSites[conflict]:0x66BB7F;fixtureImage[bad]=0xCC;
        InstallFenceStageHeights(base);
        for(unsigned i=0;i<2;++i)for(unsigned j=0;j<6;++j)
            if(fixtureImage[fenceSites[i]+j]!=(bad==fenceSites[i] && j==0?0xCC:fenceBytes[j]))return 91+conflict;
    }
    const unsigned char modelBytes[]={0xD9,0x44,0x24,0x3C,0xD8,0x05,0x34,0x85,0x23,0x01,0xD9,0x5C,0x24,0x3C};
    for(unsigned conflict=0;conflict<14;++conflict){
        for(unsigned i=0;i<14;++i)fixtureImage[0x62A108+i]=modelBytes[i];
        fixtureImage[0x62A108+conflict]^=1;
        InstallFenceModelHeight(base);
        for(unsigned i=0;i<14;++i)if(fixtureImage[0x62A108+i]!=(i==conflict?(modelBytes[i]^1):modelBytes[i]))return 110+conflict;
    }
    const unsigned uiStarts[]={0x629CC1,0x65D18C},uiLengths[]={12,20};
    const unsigned char uiOriginal[2][20]={{0x3A,0xC3,0x74,8,0xC7,0x44,0x24,0x14,0,0,0x40,0x3F},{0x84,0xC0,0xC7,0x44,0x24,0x14,0,0,0x40,0x3F,0x75,8,0xC7,0x44,0x24,0x14,0,0,0,0}};
    for(unsigned conflict=0;conflict<4;++conflict){
        for(unsigned i=0;i<2;++i)for(unsigned j=0;j<uiLengths[i];++j)fixtureImage[uiStarts[i]+j]=uiOriginal[i][j];
        unsigned bad=conflict==0?0x629CC5:conflict==1?0x65D18E:conflict==2?0x629CC1:0x65D196;
        fixtureImage[bad]^=1;InstallPanelMarkerHeights(base);
        for(unsigned i=0;i<2;++i)for(unsigned j=0;j<uiLengths[i];++j)if(fixtureImage[uiStarts[i]+j]!=(uiStarts[i]+j==bad?(uiOriginal[i][j]^1):uiOriginal[i][j]))return 126+conflict;
    }
    const unsigned char gridBytes[]={0xC7,0x45,0xE0,0,0,0x40,0x3F};
    for(unsigned conflict=0;conflict<7;++conflict){
        for(unsigned i=0;i<7;++i)fixtureImage[0xF292E+i]=gridBytes[i];
        fixtureImage[0xF292E+conflict]^=1;InstallObjectStageGrid(base);
        for(unsigned i=0;i<7;++i)if(fixtureImage[0xF292E+i]!=(i==conflict?(gridBytes[i]^1):gridBytes[i]))return 131+conflict;
    }
    const unsigned char bodyOriginal[]={0xD9,0x85,0x88,1,0,0,0xD8,3,0xEB,4,0xD9,0x44,0x24,0x60,0x8B,0x4C,0x24,0x4C,0xD9,0x44,0x24,0x58};
    for(unsigned conflict=0;conflict<22;++conflict){
        for(unsigned i=0;i<22;++i)fixtureImage[0x633B0C+i]=bodyOriginal[i];
        fixtureImage[0x633B0C+conflict]^=1;InstallHalfwallStageBody(base);
        for(unsigned i=0;i<22;++i)if(fixtureImage[0x633B0C+i]!=(i==conflict?(bodyOriginal[i]^1):bodyOriginal[i]))return 140+conflict;
    }
    const unsigned char updateOriginal[]={0xD8,0x46,8,0x8D,0x54,0x24,0x64};
    for(unsigned conflict=0;conflict<7;++conflict){
        for(unsigned i=0;i<7;++i)fixtureImage[0x633D27+i]=updateOriginal[i];
        fixtureImage[0x633D27+conflict]^=1;InstallHalfwallStageUpdate(base);
        for(unsigned i=0;i<7;++i)if(fixtureImage[0x633D27+i]!=(i==conflict?(updateOriginal[i]^1):updateOriginal[i]))return 163+conflict;
    }
    const unsigned char visibleOriginal[]={0xE8,7,0xC0,0xFF,0xFF};
    for(unsigned conflict=0;conflict<5;++conflict){
        for(unsigned i=0;i<5;++i)fixtureImage[0x6307D4+i]=visibleOriginal[i];
        fixtureImage[0x6307D4+conflict]^=1;InstallVisibleStageWall(base);
        for(unsigned i=0;i<5;++i)if(fixtureImage[0x6307D4+i]!=(i==conflict?(visibleOriginal[i]^1):visibleOriginal[i]))return 171+conflict;
    }
    const unsigned char canonicalOriginal[]={0xE8,0x3D,0xD9,0xFF,0xFF};
    for(unsigned conflict=0;conflict<5;++conflict){
        for(unsigned i=0;i<5;++i)fixtureImage[0x63603E+i]=canonicalOriginal[i];
        fixtureImage[0x63603E+conflict]^=1;InstallCanonicalHalfwall(base);
        for(unsigned i=0;i<5;++i)if(fixtureImage[0x63603E+i]!=(i==conflict?(canonicalOriginal[i]^1):canonicalOriginal[i]))return 177+conflict;
    }
    const unsigned char upperOriginal[]={0xD9,0x5E,8,0x89,0x56,0x24};
    for(unsigned conflict=0;conflict<6;++conflict){
        for(unsigned i=0;i<6;++i)fixtureImage[0x62FFB1+i]=upperOriginal[i];
        fixtureImage[0x62FFB1+conflict]^=1;InstallHalfwallVertexRefresh(base);
        for(unsigned i=0;i<6;++i)if(fixtureImage[0x62FFB1+i]!=(i==conflict?(upperOriginal[i]^1):upperOriginal[i]))return 185+conflict;
    }
    return 0;
}

