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

#define kKnownTypesCount 24

static struct DataTypeFormat KnownDataTypeFormats[kKnownTypesCount] = {
	{"dependency", 0x10},
	{"string128", 0x80},
	{"string32", 0x20},
	{"string4", 0x4},
	{"bitmask32", 0x4},
	{"bitmask16", 0x2},
	{"bitmask8", 0x1},
	{"colorARGB", 0x10},
	{"colorbyte", 0x4},
	{"colorRGB", 0xc},
	{"double", 0x8},
	{"short", 0x1},
	{"float", 0x4},
	{"char", 0x1},
	{"enum32", 0x4},
	{"enum16", 0x2},
	{"enum8", 0x1},
	{"index", 0x2},
	{"int32", 0x4},
	{"int16", 0x2},
	{"int8", 0x1},
	{"loneID", 0x4},
	{"struct", 0xc},
	{"long", 0x4},
};

uint32_t GetSizeForData(xmlAttr *node);
uint32_t GetValueForData(xmlAttr *node);
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

uint32_t GetValueForData(xmlAttr *node) {
	uint32_t offset = 0;
	xmlAttr *nodeAttr = NULL;
	for (nodeAttr = node; nodeAttr; nodeAttr = nodeAttr->next)
		if (strcmp((char *)nodeAttr->name, "value")==0)
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

bool HasValidType(xmlNode *node) {
	bool result = false;
	for (uint32_t typeNum = 0; typeNum < kKnownTypesCount; typeNum++) {
		if (strcmp((char*)node->name, KnownDataTypeFormats[typeNum].name)==0) {
			result = true;
			break;
		}
	}
	return result;
}

struct DataType BuildDataType(xmlNode *node) {
	struct DataType type;
	type.name = GetNameForData(node->properties);	
	type.offset = GetOffsetForData(node->properties);
	uint32_t typeNum;
	for (typeNum = 0; typeNum < kKnownTypesCount; typeNum++) {
		if (strcmp((char*)node->name, KnownDataTypeFormats[typeNum].name)==0) break;
	}
	type.format = &KnownDataTypeFormats[typeNum];
	type.properties = malloc(sizeof(struct DataType));
	type.propCount = 0;
	if (node->children && typeNum == 22) {
		xmlNode *props = NULL;
		for (props = node->children; props; props = props->next) {
			if (props->type == XML_ELEMENT_NODE) {
				if (HasValidType(props)) {
					type.properties = realloc(type.properties, sizeof(struct DataType)*(type.propCount+1));
					type.properties[type.propCount] = BuildDataType(props);
					type.propCount++;
				}
			}
		}
	}
	return type;
}

struct GeneratedTag GenerateTagFromPlugin(xmlNode *root) {
	struct GeneratedTag tag;
	if (root->properties && root->properties->children) {
		tag.class = malloc(strlen((char*)root->properties->children->content));
		tag.class = strncpy(tag.class, (char*)root->properties->children->content, strlen((char*)root->properties->children->content));
		tag.types = malloc(sizeof(struct DataType));
		tag.count = 0;
		xmlNode *cur_node = root->children;
		while (cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
				if (HasValidType(cur_node)) {
					tag.types = realloc(tag.types, sizeof(struct DataType)*(tag.count+1));
					tag.types[tag.count] = BuildDataType(cur_node);
					tag.count++;
				}
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