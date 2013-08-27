/*
 *  SDMPluginHandler.h
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

#ifndef _SDM_HALO_PLUGIN_HANDLER_H_
#define _SDM_HALO_PLUGIN_HANDLER_H_

#ifdef _WIN32
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif

struct DataTypeFormat {
	char *name;
	uint32_t size;
} __attribute__ ((packed)) DataTypeFormat;

struct DataValue {
	char *name;
	uint32_t value;
} __attribute__ ((packed)) DataValue;

struct DataType {
	char *name;
	uint32_t offset;
	struct DataTypeFormat *format;
	struct DataType *properties;
	uint32_t propCount;
	struct DataValue *values;
	uint32_t valueCount;
	uint32_t size;
} __attribute__ ((packed)) DataType;

struct GeneratedTag {
	char *class;
	struct DataType *types;
	uint32_t count;
} __attribute__ ((packed)) GeneratedTag;

struct KnownTypes {
	struct GeneratedTag *tags;
	uint32_t count;
} __attribute__ ((packed)) KnownTypes;

struct KnownTypes* LoadPluginsAtPath(char *path);
struct GeneratedTag BuildTagFromPluginAtPath(char *path);

#endif
