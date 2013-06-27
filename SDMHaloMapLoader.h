/*
 *  SDMHaloMapLoader.h
 *  SDMHaloMapLoader
 *
 *  Copyright (c) 2013, Sam Marshall
 *  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *  3. All advertising materials mentioning features or use of this software must display the following acknowledgement:
 *  	This product includes software developed by the Sam Marshall.
 *  4. Neither the name of the Sam Marshall nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY Sam Marshall ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Sam Marshall BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDMPluginHandler.h"
#ifdef _WIN32
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif


#ifndef _SDM_HALO_MAP_LOADER_H_
#define _SDM_HALO_MAP_LOADER_H_

struct MapData {
	uint32_t mapMagic;
	uint32_t scenarioOffset;
	uint32_t globalsOffset;
	uint32_t modelCount;
	uint32_t bitmapCount;
	uint32_t machCount;
	uint32_t vehiCount;
	uint32_t scenCount;
	uint32_t itmcCount;
} __attribute__ ((packed)) MapData;

struct MemoryBuffer {
	char *data;
	uint32_t length;
	uint32_t offset;
} __attribute__ ((packed)) MemoryBuffer;

struct Header {
	uint32_t typeId;
	uint32_t version;
	uint32_t length;
	uint32_t zeros;
	uint32_t offset;
	uint32_t metaSize;
	char padding[0x8];
	char name[0x20];
	char builddate[0x20];
	uint32_t type;
} __attribute__ ((packed)) Header;

struct Index {
	uint32_t indexOffset;
	uint32_t baseId;
	uint32_t mapId;
	uint32_t tagCount;
	uint32_t vertexObjectCount;
	uint32_t vertexOffset;
	uint32_t indicesObjectCount;
	uint32_t vertexSize;
	uint32_t modelSize;
	uint32_t tagSignature;
} __attribute__ ((packed)) Index;

struct Tag {
	char classA[0x4];
	char classB[0x4];
	char classC[0x4];
	uint32_t identity;
	uint32_t stringOffset;
	uint32_t offset;
	char padding[0x8];
} __attribute__ ((packed)) Tag;

struct MapTag {
	uint32_t offset;
	char *name;
	struct GeneratedTag *plugin;
} __attribute__ ((packed)) MapTag;

struct Reflexive {
	uint32_t chunkCount;
	uint32_t offset;
	char padding[0x4];
} __attribute__ ((packed)) Reflexive;

struct ReflexiveRef {
	struct Reflexive type;
	struct Reflexive typeRef;
} __attribute__ ((packed)) ReflexiveRef;

struct ScenarioTag {
	char junkStr1[0x10];
	char junkStr2[0x10];
	char junkStr3[0x10];
	struct Reflexive skybox;
	char padding0[0x4];
	struct Reflexive children;
	uint32_t padding1[0x2e];
	uint32_t scenarioEditorSize;
	uint32_t padding2[0x2];
	uint32_t indexStartPointer;
	uint32_t padding3[0x2];
	uint32_t indexEndPointer;
	uint32_t padding4[0x39];
	struct Reflexive objectNames;
	struct ReflexiveRef scenery;
	struct ReflexiveRef biped;
	struct ReflexiveRef vehicle;
	struct ReflexiveRef equipment;
	struct ReflexiveRef weapon;
	struct Reflexive deviceGroups;
	struct ReflexiveRef machine;
	struct ReflexiveRef control;
	struct ReflexiveRef lightFixture;
	struct ReflexiveRef soundScenery;
	struct Reflexive unknownRef0[0x7];
	struct Reflexive startingProfiles;
	struct Reflexive spawns;
	struct Reflexive triggerVolumes;
	struct Reflexive animations;
	struct Reflexive mpFlags;
	struct Reflexive mpEquipment;
	struct Reflexive startingEquip;
	struct Reflexive bspSwitchTrigger;
	struct ReflexiveRef decals;
	struct Reflexive detailObjectRef;
	struct Reflexive unknownRef1[0x7];
	struct Reflexive actorVariant;
	struct Reflexive encounters;
	struct Reflexive commandLists;
	struct Reflexive unknownRef2;
	struct Reflexive startingLocations;
	struct Reflexive platoons;
	struct Reflexive aiConversations;
	uint32_t scriptDataSize;
	uint32_t unknown0;
	struct Reflexive scripts;
	struct Reflexive commands;
	struct Reflexive points;
	struct Reflexive aiAnimationRefs;
	struct Reflexive globals;
	struct Reflexive aiRecordingRefs;
	struct Reflexive unknownRef3;
	struct Reflexive participants;
	struct Reflexive lines;
	struct Reflexive scriptTriggers;
	struct Reflexive cutscences;
	struct Reflexive cutscenceTitles0;
	struct Reflexive sourceFiles;
	struct Reflexive cutsceneFlags;
	struct Reflexive cutsceneCameraPoints;
	struct Reflexive cutscenceTitles1;
	struct Reflexive unknownRef4[0x8];
	uint32_t unknown1[0x2];
	struct Reflexive bsp;
} __attribute__ ((packed)) ScenarioTag;

struct HaloMap {	
	struct MapData *mapData;
	struct MemoryBuffer *buffer;
	struct Header *header;
	struct Index *index;
	struct MapTag *tags;
	struct ScenarioTag *scenario;
	struct KnownTypes *plugins;
} __attribute__ ((packed)) HaloMap;

struct VehicleRef {
	char data[0x30];
} __attribute__ ((packed)) VehicleRef;
	
struct VehicleSpawn {
	char data[0x78];
} __attribute__ ((packed)) ;

struct BipedRef {
	char data[0x30];
} __attribute__ ((packed)) ;

struct SceneryRef {
	char data[0x30];
} __attribute__ ((packed)) ;

struct ScenerySpawn {
	char data[0x48];
} __attribute__ ((packed)) ;

struct MPEquipmentSpawn {
	char data[0x90];
} __attribute__ ((packed)) ;

struct PlayerSpawn {
	char data[0x34];
} __attribute__ ((packed)) ;

struct MPFlags {
	char data[0x94];
} __attribute__ ((packed)) ;

struct MachineRef {
	char data[0x40];
} __attribute__ ((packed)) ;

struct MachineSpawn {
	char data[0x30];
} __attribute__ ((packed)) ;

struct DeviceGroup {
	char data[0x34];
} __attribute__ ((packed)) ;

struct EncounterRef {
	char data[0xb0];
} __attribute__ ((packed)) ;

struct SkyboxRef {
	char data[0x24];
} __attribute__ ((packed)) ;

struct HaloMap* ParseHaloMapFromFileWithPlugins(char *mapPath, char *pluginsPath);
struct VehicleRef* ParseVehicleRefs(struct HaloMap *map);
struct VehicleSpawn* ParseVehicleSpawns(struct HaloMap *map);
struct BipedRef* ParseBipedRefs(struct HaloMap *map);
struct SceneryRef* ParseSceneryRefs(struct HaloMap *map);
struct ScenerySpawn* ParseScenerySpawns(struct HaloMap *map);
struct MPEquipmentSpawn* ParseMPEquipmentSpawns(struct HaloMap *map);
struct PlayerSpawn* ParsePlayerSpawns(struct HaloMap *map);
struct MPFlags* ParseMPFlags(struct HaloMap *map);
struct MachineRef* ParseMachineRefs(struct HaloMap *map);
struct MachineSpawn* ParseMachineSpawns(struct HaloMap *map);
struct DeviceGroup* ParseDeviceGroup(struct HaloMap *map);
struct EncounterRef* ParseEncounters(struct HaloMap *map);
struct SkyboxRef* ParseSkyboxRefs(struct HaloMap *map);

#endif