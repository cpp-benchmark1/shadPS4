// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "common/types.h"

namespace Core::Loader {
class SymbolsResolver;
}

// Define our own htonll and ntohll because its not available in some systems/platforms
#ifndef HTONLL
#define HTONLL(x) (((uint64_t)htonl((x) & 0xFFFFFFFFUL)) << 32) | htonl((uint32_t)((x) >> 32))
#endif
#ifndef NTOHLL
#define NTOHLL(x) (((uint64_t)ntohl((x) & 0xFFFFFFFFUL)) << 32) | ntohl((uint32_t)((x) >> 32))
#endif

namespace Libraries::Net {

using OrbisNetId = s32;

struct OrbisNetSockaddr {
    u8 sa_len;
    u8 sa_family;
    char sa_data[14];
};

int PS4_SYSV_ABI in6addr_any();
int PS4_SYSV_ABI in6addr_loopback();
int PS4_SYSV_ABI sce_net_dummy();
int PS4_SYSV_ABI sce_net_in6addr_any();
int PS4_SYSV_ABI sce_net_in6addr_linklocal_allnodes();
int PS4_SYSV_ABI sce_net_in6addr_linklocal_allrouters();
int PS4_SYSV_ABI sce_net_in6addr_loopback();
int PS4_SYSV_ABI sce_net_in6addr_nodelocal_allnodes();
OrbisNetId PS4_SYSV_ABI sceNetAccept(OrbisNetId s, OrbisNetSockaddr* addr, u32* paddrlen);
int PS4_SYSV_ABI sceNetAddrConfig6GetInfo();
int PS4_SYSV_ABI sceNetAddrConfig6Start();
int PS4_SYSV_ABI sceNetAddrConfig6Stop();
int PS4_SYSV_ABI sceNetAllocateAllRouteInfo();
int PS4_SYSV_ABI sceNetBandwidthControlGetDataTraffic();
int PS4_SYSV_ABI sceNetBandwidthControlGetDefaultParam();
int PS4_SYSV_ABI sceNetBandwidthControlGetIfParam();
int PS4_SYSV_ABI sceNetBandwidthControlGetPolicy();
int PS4_SYSV_ABI sceNetBandwidthControlSetDefaultParam();
int PS4_SYSV_ABI sceNetBandwidthControlSetIfParam();
int PS4_SYSV_ABI sceNetBandwidthControlSetPolicy();
int PS4_SYSV_ABI sceNetBind(OrbisNetId s, const OrbisNetSockaddr* addr, u32 addrlen);
int PS4_SYSV_ABI sceNetClearDnsCache();
int PS4_SYSV_ABI sceNetConfigAddArp();
int PS4_SYSV_ABI sceNetConfigAddArpWithInterface();
int PS4_SYSV_ABI sceNetConfigAddIfaddr();
int PS4_SYSV_ABI sceNetConfigAddMRoute();
int PS4_SYSV_ABI sceNetConfigAddRoute();
int PS4_SYSV_ABI sceNetConfigAddRoute6();
int PS4_SYSV_ABI sceNetConfigAddRouteWithInterface();
int PS4_SYSV_ABI sceNetConfigCleanUpAllInterfaces();
int PS4_SYSV_ABI sceNetConfigDelArp();
int PS4_SYSV_ABI sceNetConfigDelArpWithInterface();
int PS4_SYSV_ABI sceNetConfigDelDefaultRoute();
int PS4_SYSV_ABI sceNetConfigDelDefaultRoute6();
int PS4_SYSV_ABI sceNetConfigDelIfaddr();
int PS4_SYSV_ABI sceNetConfigDelIfaddr6();
int PS4_SYSV_ABI sceNetConfigDelMRoute();
int PS4_SYSV_ABI sceNetConfigDelRoute();
int PS4_SYSV_ABI sceNetConfigDelRoute6();
int PS4_SYSV_ABI sceNetConfigDownInterface();
int PS4_SYSV_ABI sceNetConfigEtherGetLinkMode();
int PS4_SYSV_ABI sceNetConfigEtherPostPlugInOutEvent();
int PS4_SYSV_ABI sceNetConfigEtherSetLinkMode();
int PS4_SYSV_ABI sceNetConfigFlushRoute();
int PS4_SYSV_ABI sceNetConfigGetDefaultRoute();
int PS4_SYSV_ABI sceNetConfigGetDefaultRoute6();
int PS4_SYSV_ABI sceNetConfigGetIfaddr();
int PS4_SYSV_ABI sceNetConfigGetIfaddr6();
int PS4_SYSV_ABI sceNetConfigRoutingShowRoutingConfig();
int PS4_SYSV_ABI sceNetConfigRoutingShowtCtlVar();
int PS4_SYSV_ABI sceNetConfigRoutingStart();
int PS4_SYSV_ABI sceNetConfigRoutingStop();
int PS4_SYSV_ABI sceNetConfigSetDefaultRoute();
int PS4_SYSV_ABI sceNetConfigSetDefaultRoute6();
int PS4_SYSV_ABI sceNetConfigSetDefaultScope();
int PS4_SYSV_ABI sceNetConfigSetIfaddr();
int PS4_SYSV_ABI sceNetConfigSetIfaddr6();
int PS4_SYSV_ABI sceNetConfigSetIfaddr6WithFlags();
int PS4_SYSV_ABI sceNetConfigSetIfFlags();
int PS4_SYSV_ABI sceNetConfigSetIfLinkLocalAddr6();
int PS4_SYSV_ABI sceNetConfigSetIfmtu();
int PS4_SYSV_ABI sceNetConfigUnsetIfFlags();
int PS4_SYSV_ABI sceNetConfigUpInterface();
int PS4_SYSV_ABI sceNetConfigUpInterfaceWithFlags();
int PS4_SYSV_ABI sceNetConfigWlanAdhocClearWakeOnWlan();
int PS4_SYSV_ABI sceNetConfigWlanAdhocCreate();
int PS4_SYSV_ABI sceNetConfigWlanAdhocGetWakeOnWlanInfo();
int PS4_SYSV_ABI sceNetConfigWlanAdhocJoin();
int PS4_SYSV_ABI sceNetConfigWlanAdhocLeave();
int PS4_SYSV_ABI sceNetConfigWlanAdhocPspEmuClearWakeOnWlan();
int PS4_SYSV_ABI sceNetConfigWlanAdhocPspEmuGetWakeOnWlanInfo();
int PS4_SYSV_ABI sceNetConfigWlanAdhocPspEmuSetWakeOnWlan();
int PS4_SYSV_ABI sceNetConfigWlanAdhocScanJoin();
int PS4_SYSV_ABI sceNetConfigWlanAdhocSetExtInfoElement();
int PS4_SYSV_ABI sceNetConfigWlanAdhocSetWakeOnWlan();
int PS4_SYSV_ABI sceNetConfigWlanApStart();
int PS4_SYSV_ABI sceNetConfigWlanApStop();
int PS4_SYSV_ABI sceNetConfigWlanBackgroundScanQuery();
int PS4_SYSV_ABI sceNetConfigWlanBackgroundScanStart();
int PS4_SYSV_ABI sceNetConfigWlanBackgroundScanStop();
int PS4_SYSV_ABI sceNetConfigWlanDiagGetDeviceInfo();
int PS4_SYSV_ABI sceNetConfigWlanDiagSetAntenna();
int PS4_SYSV_ABI sceNetConfigWlanDiagSetTxFixedRate();
int PS4_SYSV_ABI sceNetConfigWlanGetDeviceConfig();
int PS4_SYSV_ABI sceNetConfigWlanInfraGetRssiInfo();
int PS4_SYSV_ABI sceNetConfigWlanInfraLeave();
int PS4_SYSV_ABI sceNetConfigWlanInfraScanJoin();
int PS4_SYSV_ABI sceNetConfigWlanScan();
int PS4_SYSV_ABI sceNetConfigWlanSetDeviceConfig();
int PS4_SYSV_ABI sceNetConnect();
int PS4_SYSV_ABI sceNetControl();
int PS4_SYSV_ABI sceNetDhcpdStart();
int PS4_SYSV_ABI sceNetDhcpdStop();
int PS4_SYSV_ABI sceNetDhcpGetAutoipInfo();
int PS4_SYSV_ABI sceNetDhcpGetInfo();
int PS4_SYSV_ABI sceNetDhcpGetInfoEx();
int PS4_SYSV_ABI sceNetDhcpStart();
int PS4_SYSV_ABI sceNetDhcpStop();
int PS4_SYSV_ABI sceNetDumpAbort();
int PS4_SYSV_ABI sceNetDumpCreate();
int PS4_SYSV_ABI sceNetDumpDestroy();
int PS4_SYSV_ABI sceNetDumpRead();
int PS4_SYSV_ABI sceNetDuplicateIpStart();
int PS4_SYSV_ABI sceNetDuplicateIpStop();
int PS4_SYSV_ABI sceNetEpollAbort();
int PS4_SYSV_ABI sceNetEpollControl();
int PS4_SYSV_ABI sceNetEpollCreate();
int PS4_SYSV_ABI sceNetEpollDestroy();
int PS4_SYSV_ABI sceNetEpollWait();
int* PS4_SYSV_ABI sceNetErrnoLoc();
int PS4_SYSV_ABI sceNetEtherNtostr();
int PS4_SYSV_ABI sceNetEtherStrton();
int PS4_SYSV_ABI sceNetEventCallbackCreate();
int PS4_SYSV_ABI sceNetEventCallbackDestroy();
int PS4_SYSV_ABI sceNetEventCallbackGetError();
int PS4_SYSV_ABI sceNetEventCallbackWaitCB();
int PS4_SYSV_ABI sceNetFreeAllRouteInfo();
int PS4_SYSV_ABI sceNetGetArpInfo();
int PS4_SYSV_ABI sceNetGetDns6Info();
int PS4_SYSV_ABI sceNetGetDnsInfo();
int PS4_SYSV_ABI sceNetGetIfList();
int PS4_SYSV_ABI sceNetGetIfListOnce();
int PS4_SYSV_ABI sceNetGetIfName();
int PS4_SYSV_ABI sceNetGetIfnameNumList();
int PS4_SYSV_ABI sceNetGetMacAddress();
int PS4_SYSV_ABI sceNetGetMemoryPoolStats();
int PS4_SYSV_ABI sceNetGetNameToIndex();
int PS4_SYSV_ABI sceNetGetpeername();
int PS4_SYSV_ABI sceNetGetRandom();
int PS4_SYSV_ABI sceNetGetRouteInfo();
int PS4_SYSV_ABI sceNetGetSockInfo();
int PS4_SYSV_ABI sceNetGetSockInfo6();
int PS4_SYSV_ABI sceNetGetsockname(OrbisNetId s, OrbisNetSockaddr* addr, u32* paddrlen);
int PS4_SYSV_ABI sceNetGetsockopt(OrbisNetId s, int level, int optname, void* optval, u32* optlen);
int PS4_SYSV_ABI sceNetGetStatisticsInfo();
int PS4_SYSV_ABI sceNetGetStatisticsInfoInternal();
int PS4_SYSV_ABI sceNetGetSystemTime();
u32 PS4_SYSV_ABI sceNetHtonl(u32 host32);
u64 PS4_SYSV_ABI sceNetHtonll(u64 host64);
u16 PS4_SYSV_ABI sceNetHtons(u16 host16);
const char* PS4_SYSV_ABI sceNetInetNtop(int af, const void* src, char* dst, u32 size);
int PS4_SYSV_ABI sceNetInetNtopWithScopeId();
int PS4_SYSV_ABI sceNetInetPton(int af, const char* src, void* dst);
int PS4_SYSV_ABI sceNetInetPtonEx();
int PS4_SYSV_ABI sceNetInetPtonWithScopeId();
int PS4_SYSV_ABI sceNetInfoDumpStart();
int PS4_SYSV_ABI sceNetInfoDumpStop();
int PS4_SYSV_ABI sceNetInit();
int PS4_SYSV_ABI sceNetInitParam();
int PS4_SYSV_ABI sceNetIoctl();
int PS4_SYSV_ABI sceNetListen();
int PS4_SYSV_ABI sceNetMemoryAllocate();
int PS4_SYSV_ABI sceNetMemoryFree();
u32 PS4_SYSV_ABI sceNetNtohl(u32 net32);
int PS4_SYSV_ABI sceNetNtohll();
u16 PS4_SYSV_ABI sceNetNtohs(u16 net16);
int PS4_SYSV_ABI sceNetPoolCreate(const char* name, int size, int flags);
int PS4_SYSV_ABI sceNetPoolDestroy();
int PS4_SYSV_ABI sceNetPppoeStart();
int PS4_SYSV_ABI sceNetPppoeStop();
int PS4_SYSV_ABI sceNetRecv();
int PS4_SYSV_ABI sceNetRecvfrom(OrbisNetId s, void* buf, size_t len, int flags,
                                OrbisNetSockaddr* addr, u32* paddrlen);
int PS4_SYSV_ABI sceNetRecvmsg();
int PS4_SYSV_ABI sceNetResolverAbort();
int PS4_SYSV_ABI sceNetResolverConnect();
int PS4_SYSV_ABI sceNetResolverConnectAbort();
int PS4_SYSV_ABI sceNetResolverConnectCreate();
int PS4_SYSV_ABI sceNetResolverConnectDestroy();
int PS4_SYSV_ABI sceNetResolverCreate();
int PS4_SYSV_ABI sceNetResolverDestroy();
int PS4_SYSV_ABI sceNetResolverGetError();
int PS4_SYSV_ABI sceNetResolverStartAton();
int PS4_SYSV_ABI sceNetResolverStartAton6();
int PS4_SYSV_ABI sceNetResolverStartNtoa();
int PS4_SYSV_ABI sceNetResolverStartNtoa6();
int PS4_SYSV_ABI sceNetResolverStartNtoaMultipleRecords();
int PS4_SYSV_ABI sceNetResolverStartNtoaMultipleRecordsEx();
int PS4_SYSV_ABI sceNetSend();
int PS4_SYSV_ABI sceNetSendmsg();
int PS4_SYSV_ABI sceNetSendto();
int PS4_SYSV_ABI sceNetSetDns6Info();
int PS4_SYSV_ABI sceNetSetDns6InfoToKernel();
int PS4_SYSV_ABI sceNetSetDnsInfo();
int PS4_SYSV_ABI sceNetSetDnsInfoToKernel();
int PS4_SYSV_ABI sceNetSetsockopt();
int PS4_SYSV_ABI sceNetShowIfconfig();
int PS4_SYSV_ABI sceNetShowIfconfigForBuffer();
int PS4_SYSV_ABI sceNetShowIfconfigWithMemory();
int PS4_SYSV_ABI sceNetShowNetstat();
int PS4_SYSV_ABI sceNetShowNetstatEx();
int PS4_SYSV_ABI sceNetShowNetstatExForBuffer();
int PS4_SYSV_ABI sceNetShowNetstatForBuffer();
int PS4_SYSV_ABI sceNetShowNetstatWithMemory();
int PS4_SYSV_ABI sceNetShowPolicy();
int PS4_SYSV_ABI sceNetShowPolicyWithMemory();
int PS4_SYSV_ABI sceNetShowRoute();
int PS4_SYSV_ABI sceNetShowRoute6();
int PS4_SYSV_ABI sceNetShowRoute6ForBuffer();
int PS4_SYSV_ABI sceNetShowRoute6WithMemory();
int PS4_SYSV_ABI sceNetShowRouteForBuffer();
int PS4_SYSV_ABI sceNetShowRouteWithMemory();
int PS4_SYSV_ABI sceNetShutdown();
int PS4_SYSV_ABI sceNetSocket(const char* name, int family, int type, int protocol);
int PS4_SYSV_ABI sceNetSocketAbort();
int PS4_SYSV_ABI sceNetSocketClose();
int PS4_SYSV_ABI sceNetSyncCreate();
int PS4_SYSV_ABI sceNetSyncDestroy();
int PS4_SYSV_ABI sceNetSyncGet();
int PS4_SYSV_ABI sceNetSyncSignal();
int PS4_SYSV_ABI sceNetSyncWait();
int PS4_SYSV_ABI sceNetSysctl();
int PS4_SYSV_ABI sceNetTerm();
int PS4_SYSV_ABI sceNetThreadCreate();
int PS4_SYSV_ABI sceNetThreadExit();
int PS4_SYSV_ABI sceNetThreadJoin();
int PS4_SYSV_ABI sceNetUsleep();
int PS4_SYSV_ABI Func_0E707A589F751C68();
int PS4_SYSV_ABI sceNetEmulationGet();
int PS4_SYSV_ABI sceNetEmulationSet();

void RegisterlibSceNet(Core::Loader::SymbolsResolver* sym);
} // namespace Libraries::Net