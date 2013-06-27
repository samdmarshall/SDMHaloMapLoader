/*
 *  SDMHaloMapLoader.c
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

#ifndef _SDM_HALO_MAP_LOADER_C_
#define _SDM_HALO_MAP_LOADER_C_

#include "SDMHaloMapLoader.h"

struct MemoryBuffer* MapFileToBuffer(char *path);

static inline char* TagType(char tag[4]) {
	char rev[4] = {tag[3], tag[2], tag[1], tag[0]};
	return rev;
}

struct VehicleRef* ParseVehicleRefs(struct HaloMap *map) {
	return (struct VehicleRef *)&map->buffer->data[map->scenario->vehicle.typeRef.offset];
}

struct VehicleSpawn* ParseVehicleSpawns(struct HaloMap *map) {
	return (struct VehicleSpawn *)&map->buffer->data[map->scenario->vehicle.type.offset];
}

struct BipedRef* ParseBipedRefs(struct HaloMap *map) {
	return (struct BipedRef *)&map->buffer->data[map->scenario->biped.typeRef.offset];
}

struct SceneryRef* ParseSceneryRefs(struct HaloMap *map) {
	return (struct SceneryRef *)&map->buffer->data[map->scenario->scenery.typeRef.offset];
}

struct ScenerySpawn* ParseScenerySpawns(struct HaloMap *map) {
	return (struct ScenerySpawn *)&map->buffer->data[map->scenario->scenery.type.offset];
}

struct MPEquipmentSpawn* ParseMPEquipmentSpawns(struct HaloMap *map) {
	return (struct MPEquipmentSpawn *)&map->buffer->data[map->scenario->mpEquipment.offset];
}

struct PlayerSpawn* ParsePlayerSpawns(struct HaloMap *map) {
	return (struct PlayerSpawn *)&map->buffer->data[map->scenario->spawns.offset];
}

struct MPFlags* ParseMPFlags(struct HaloMap *map) {
	return (struct MPFlags *)&map->buffer->data[map->scenario->mpFlags.offset];
}

struct MachineRef* ParseMachineRefs(struct HaloMap *map) {
	return (struct MachineRef *)&map->buffer->data[map->scenario->machine.typeRef.offset];
}

struct MachineSpawn* ParseMachineSpawns(struct HaloMap *map) {
	return (struct MachineSpawn *)&map->buffer->data[map->scenario->machine.type.offset];
}

struct DeviceGroup* ParseDeviceGroup(struct HaloMap *map) {
	return (struct DeviceGroup *)&map->buffer->data[map->scenario->deviceGroups.offset];
}

struct EncounterRef* ParseEncounters(struct HaloMap *map) {
	return (struct EncounterRef *)&map->buffer->data[map->scenario->encounters.offset];
}

struct SkyboxRef* ParseSkyboxRefs(struct HaloMap *map) {
	return (struct SkyboxRef *)&map->buffer->data[map->scenario->skybox.offset];
}

struct MemoryBuffer* MapFileToBuffer(char *path) {
	struct MemoryBuffer *buffer = calloc(sizeof(struct MemoryBuffer), 0x1);
	if (path) {
		FILE *mapFile = fopen(path, "r+");
		if (mapFile) {
			fseek(mapFile, 0x0, SEEK_END);
			buffer->length = ftell(mapFile);
			fseek(mapFile, 0x0, SEEK_SET);
			buffer->data = malloc(buffer->length);
			fread(buffer->data, buffer->length, 0x1, mapFile);
		}
		fclose(mapFile);
	}
	return buffer;
}

struct HaloMap* ParseHaloMapFromFileWithPlugins(char *mapPath, char *pluginsPath) {
	struct HaloMap *map = calloc(sizeof(struct HaloMap), 0x1);
	map->buffer = MapFileToBuffer(mapPath);
	map->plugins = LoadPluginsAtPath(pluginsPath);
	if (map->buffer) {
		map->mapData = malloc(sizeof(struct MapData));
		map->header = (struct Header *)&map->buffer->data[map->buffer->offset];
		map->buffer->offset += map->header->offset;
		map->index = (struct Index *)&map->buffer->data[map->buffer->offset];
		map->buffer->offset += sizeof(struct Index);
		map->mapData->mapMagic = (map->index->indexOffset - (map->header->offset+0x28));
		map->tags = malloc(sizeof(struct MapTag)*map->index->tagCount);
		for (uint32_t i = 0x0; i < map->index->tagCount; i++) {
			map->tags[i].offset = map->buffer->offset;
			struct Tag *tag = (struct Tag *)&map->buffer->data[map->buffer->offset];
			map->tags[i].name = &map->buffer->data[tag->stringOffset-map->mapData->mapMagic];
			for (uint32_t j = 0x0; j < map->plugins->count; j++) {
				if (memcmp(tag->classA, TagType(map->plugins->tags[j].class), 0x4)==0x0) {
					map->tags[i].plugin = &map->plugins->tags[j];
					break;
				}
			}
			if (memcmp(tag->classA, "rncs", 0x4) == 0x0)  
				map->mapData->scenarioOffset = map->tags[i].offset;
			else if (memcmp(tag->classA, "gtam", 0x4) == 0x0)
				map->mapData->globalsOffset = map->tags[i].offset;
			else if (memcmp(tag->classA, "2dom", 0x4) == 0x0)
				map->mapData->modelCount++;                    
			else if (memcmp(tag->classA, "mtib", 0x4) == 0x0)
				map->mapData->bitmapCount++;                   
			else if (memcmp(tag->classA, "hcam", 0x4) == 0x0)
				map->mapData->machCount++;                     
			else if (memcmp(tag->classA, "ihev", 0x4) == 0x0)
				map->mapData->vehiCount++;                     
			else if (memcmp(tag->classA, "necs", 0x4) == 0x0)
				map->mapData->scenCount++;                     
			else if (memcmp(tag->classA, "cmti", 0x4) == 0x0)
				map->mapData->itmcCount++;
			map->buffer->offset += sizeof(struct Tag);			
		}		
		map->scenario = (struct ScenarioTag *)&map->buffer->data[map->mapData->scenarioOffset];
		map->buffer->offset += (map->mapData->scenarioOffset + sizeof(struct ScenarioTag));
	}
	return map;
}
#endif