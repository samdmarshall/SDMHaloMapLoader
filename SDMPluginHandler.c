/*
 *  SDMPluginHandler.c
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

#ifndef _SDM_HALO_PLUGIN_HANDLER_C_
#define _SDM_HALO_PLUGIN_HANDLER_C_

#include "SDMPluginHandler.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

enum DataTypeEnum {
	DTdependency = 0,
	DTstruct = 1,
	DTenum16 = 2,
	DTfloat = 3,
	DTbitmask32 = 4,
	DTshort = 5,
	DTstring32 = 6,
	DTchar = 7,
	DTcolorRGB = 8,
	DTbitmask8 = 9,
	DTcolorARGB = 10,
	DTfloatBounds = 11,
	DTangle = 12,
	DTRealV3d = 13,
	DTEuler2d = 14,
	DTindex = 15,
	DTlong = 16,
	DTbitmask16 = 17,
	DTcolorbyte = 18,
	DTcolorByteRGB = 19,
	DTloneID = 20,
	DTquaternion = 21,
	DTshortBounds = 22,
};

#define kKnownTypesCount 23
static char *kKnownDataTypes[kKnownTypesCount] = {"dependency", "struct", "enum16", "float", "bitmask32", "short", "string32", "char", "colorRGB", "bitmask8", "colorARGB", "floatBounds", "angle", "RealV3d", "Euler2d", "index", "long", "bitmask16", "colorbyte", "colorByteRGB", "loneID", "quaternion", "shortBounds"};
static uint32_t kKnownDataTypeSizes[kKnownTypesCount] = {0x10, 0x0, 0x2, 0x4, 0x4, 0x1, 0x20, 0x1, 0x12, 0x8, 0x8, 0x4, 0xc, 0x8, 0x2, 0x4, 0x2, 0x4, 0x12, 0x8, 0x30, 0x4};

uint32_t GetSizeForData(xmlAttr *node);
uint32_t GetOffsetForData(xmlAttr *node);
char* GetNameForData(xmlAttr *node);
struct DataType BuildDataType(xmlNode *node);
struct GeneratedTag GenerateTagFromPlugin(xmlNode *root);

uint32_t GetSizeForData(xmlAttr *node) {
	uint32_t offset = 0;
	xmlAttr *nodeAttr = NULL;
	for (nodeAttr = node; nodeAttr; nodeAttr = nodeAttr->next)
		if (strcmp((char *)nodeAttr->name, "size")==0)
			offset = strtol((char *)nodeAttr->children->content, NULL, 10);
	return offset;
}

uint32_t GetOffsetForData(xmlAttr *node) {
	uint32_t offset = 0;
	xmlAttr *nodeAttr = NULL;
	for (nodeAttr = node; nodeAttr; nodeAttr = nodeAttr->next)
		if (strcmp((char *)nodeAttr->name, "offset")==0)
			offset = strtol((char *)nodeAttr->children->content, NULL, 16);
	return offset;
}

char* GetNameForData(xmlAttr *node) {
	char *name;
	xmlAttr *nodeAttr = NULL;
	for (nodeAttr = node; nodeAttr; nodeAttr = nodeAttr->next)
		if (strcmp((char *)nodeAttr->name, "name")==0) {
			name = malloc(strlen((char *)nodeAttr->children->content));
			name = strncpy(name, (char *)nodeAttr->children->content, strlen((char *)nodeAttr->children->content));
		}
	return name;
}

struct DataType BuildDataType(xmlNode *node) {
	struct DataType type;
	uint32_t typeNum;
	for (typeNum = 0; typeNum < kKnownTypesCount; typeNum++) {
		if (strcmp((char*)node->name, kKnownDataTypes[typeNum])==0) break;
	}
	if (typeNum == DTstruct)
		type.sizeInBytes = GetSizeForData(node->properties);
	else
		type.sizeInBytes = kKnownDataTypeSizes[typeNum];
	
	type.format = typeNum;	
	type.offset = GetOffsetForData(node->properties);
	type.name = GetNameForData(node->properties);	
	return type;
}

struct GeneratedTag GenerateTagFromPlugin(xmlNode *root) {
	struct GeneratedTag tag;
	if (root->properties && root->properties->children) {
		tag.class = malloc(strlen((char*)root->properties->children->content));
		tag.class = strncpy(tag.class, (char*)root->properties->children->content, strlen((char*)root->properties->children->content));
		tag.types = malloc(sizeof(struct DataType));
		tag.count = 0;
		xmlNode *cur_node = NULL;
		for (cur_node = root->children; cur_node; cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
				tag.types = realloc(tag.types, sizeof(struct DataType)*(tag.count+1));
				tag.types[tag.count] = BuildDataType(cur_node);
				tag.count++;
			}
		}
	}
	return tag;
}

struct KnownTypes* LoadPluginsAtPath(char *path) {
	LIBXML_TEST_VERSION
	struct KnownTypes *loadedTypes = malloc(sizeof(struct KnownTypes));
	loadedTypes->count = 0;
	loadedTypes->tags = (struct GeneratedTag*)calloc(1, sizeof(struct GeneratedTag));
	char *full_file_name = malloc(strlen(path)+1);
	DIR *dir;
	struct dirent *ent;
	dir = opendir(path);
	while ((ent = readdir(dir)) != NULL) {
		char *file_name = ent->d_name;
		if (file_name[0] != '.') {
			full_file_name = realloc(full_file_name, strlen(path)+strlen(file_name)+2);
			strncpy(full_file_name, path, strlen(path));
			strncpy(&full_file_name[strlen(path)], file_name, strlen(file_name)+1);
			loadedTypes->tags = realloc(loadedTypes->tags, sizeof(struct GeneratedTag)*(loadedTypes->count+1));
			loadedTypes->tags[loadedTypes->count] = BuildTagFromPluginAtPath(full_file_name);
			loadedTypes->count++;			
		}
	}
	free(full_file_name);
	xmlCleanupParser();
	closedir(dir);
	return loadedTypes;
}

struct GeneratedTag BuildTagFromPluginAtPath(char *path) {
	struct GeneratedTag loadedTag = {0x0, 0x0};
	if (path) {
		xmlDoc *doc = NULL;
		doc = xmlReadFile(path, NULL, 0);
		if (doc != NULL) {
			xmlNode *root_element = xmlDocGetRootElement(doc);
			if (strncmp(&path[strlen(path)-3], "ent", 3)==0)
				loadedTag = GenerateTagFromPlugin(root_element);
		}
		xmlFreeDoc(doc);
	}
	return loadedTag;
}

#endif